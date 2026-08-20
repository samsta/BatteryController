import os
import re
import time
import json
import shutil
import threading
import requests
from datetime import datetime
from collections import deque
from typing import Optional

from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler


# --------------------------------------------------------------------------------------
# CONFIG
URL = "http://jimster.ca/BatteryHikotron/BatteryHikotron-log-receiver.php"

ORIGINAL_FILE = "BatteryController.log"
SNAPSHOT_FILE = "BatteryControllerCOPY.log"
DIFF_FILE = "BatteryControllerDIFF.log"

DEBOUNCE_SECONDS = 10

QUEUE_DIR = "queue"
QUEUE_INDEX_FILE = "queue_index.json"

HTTP_TIMEOUT_SECONDS = 15
MAX_RETRIES = 5
BACKOFF_BASE_SECONDS = 2
BACKOFF_MAX_SECONDS = 60

SENDER_POLL_SECONDS = 2


# --------------------------------------------------------------------------------------
# PATHS
WATCH_DIR = os.path.abspath(".")
ORIGINAL_PATH = os.path.realpath(os.path.join(WATCH_DIR, ORIGINAL_FILE))
SNAPSHOT_PATH = os.path.realpath(os.path.join(WATCH_DIR, SNAPSHOT_FILE))
DIFF_PATH = os.path.realpath(os.path.join(WATCH_DIR, DIFF_FILE))

QUEUE_PATH = os.path.realpath(os.path.join(WATCH_DIR, QUEUE_DIR))
QUEUE_INDEX_PATH = os.path.realpath(os.path.join(WATCH_DIR, QUEUE_INDEX_FILE))


# --------------------------------------------------------------------------------------
# GLOBALS
_timer = None
lock = threading.Lock()
processing_lock = threading.Lock()

queue_lock = threading.Lock()
queue_wakeup = threading.Event()
stop_event = threading.Event()

# In-memory FIFO of queue filenames (persisted in queue_index.json)
send_queue = deque()


# --------------------------------------------------------------------------------------
def ts() -> str:
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def log(msg: str):
    print(f"{ts()} {msg}")


def ensure_dirs():
    os.makedirs(QUEUE_PATH, exist_ok=True)


def atomic_write_text(path: str, text: str):
    tmp = path + ".tmp"
    with open(tmp, "w", encoding="utf-8") as f:
        f.write(text)
    os.replace(tmp, path)


def atomic_write_bytes(path: str, data: bytes):
    tmp = path + ".tmp"
    with open(tmp, "wb") as f:
        f.write(data)
    os.replace(tmp, path)


def load_queue_index():
    global send_queue
    if not os.path.exists(QUEUE_INDEX_PATH):
        send_queue = deque()
        return

    try:
        with open(QUEUE_INDEX_PATH, "r", encoding="utf-8") as f:
            arr = json.load(f)
        # keep only files that still exist
        filtered = [x for x in arr if os.path.exists(os.path.join(QUEUE_PATH, x))]
        send_queue = deque(filtered)
    except Exception as e:
        log(f"queue index load error: {e}")
        send_queue = deque()


def save_queue_index():
    arr = list(send_queue)
    atomic_write_text(QUEUE_INDEX_PATH, json.dumps(arr, indent=2))


def next_queue_filename() -> str:
    # sortable timestamp + millis
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    return f"payload_{stamp}.log"


def enqueue_payload_bytes(data: bytes) -> Optional[str]:
    if not data:
        return None

    ensure_dirs()
    name = next_queue_filename()
    path = os.path.join(QUEUE_PATH, name)

    try:
        atomic_write_bytes(path, data)
        with queue_lock:
            send_queue.append(name)
            save_queue_index()
        queue_wakeup.set()
        log(f"queued {name} ({len(data)} bytes)")
        return name
    except Exception as e:
        log(f"enqueue error: {e}")
        return None


def post_with_retry(data: bytes) -> bool:
    headers = {
        "Content-Type": "text/plain",
        "User-Agent": "RPi-BatteryLogger/2.0"
    }

    delay = BACKOFF_BASE_SECONDS
    for attempt in range(1, MAX_RETRIES + 1):
        if stop_event.is_set():
            return False

        try:
            r = requests.post(URL, data=data, headers=headers, timeout=HTTP_TIMEOUT_SECONDS)
            if r.status_code == 200:
                return True

            # 4xx likely permanent except 408/429
            if 400 <= r.status_code < 500 and r.status_code not in (408, 429):
                log(f"POST permanent failure {r.status_code}, will keep payload for manual review")
                return False

            log(f"POST attempt {attempt}/{MAX_RETRIES} failed status={r.status_code}")
        except Exception as e:
            log(f"POST attempt {attempt}/{MAX_RETRIES} error: {e}")

        if attempt < MAX_RETRIES:
            sleep_for = min(delay, BACKOFF_MAX_SECONDS)
            time.sleep(sleep_for)
            delay *= 2

    return False


def sender_loop():
    log("sender thread started")
    while not stop_event.is_set():
        item = None

        with queue_lock:
            if send_queue:
                item = send_queue[0]

        if not item:
            queue_wakeup.wait(timeout=SENDER_POLL_SECONDS)
            queue_wakeup.clear()
            continue

        path = os.path.join(QUEUE_PATH, item)
        if not os.path.exists(path):
            # stale index entry
            with queue_lock:
                if send_queue and send_queue[0] == item:
                    send_queue.popleft()
                    save_queue_index()
            continue

        try:
            with open(path, "rb") as f:
                data = f.read()

            if not data:
                log(f"empty payload {item}, dropping")
                with queue_lock:
                    if send_queue and send_queue[0] == item:
                        send_queue.popleft()
                        save_queue_index()
                os.remove(path)
                continue

            ok = post_with_retry(data)
            if ok:
                log(f"uploaded {item} ({len(data)} bytes)")
                with queue_lock:
                    if send_queue and send_queue[0] == item:
                        send_queue.popleft()
                        save_queue_index()
                os.remove(path)
            else:
                # keep at head, retry later
                time.sleep(5)

        except Exception as e:
            log(f"sender error for {item}: {e}")
            time.sleep(5)

    log("sender thread stopped")


# --------------------------------------------------------------------------------------
def process_diff():
    if not os.path.exists(ORIGINAL_PATH):
        return

    # first run snapshot
    if not os.path.exists(SNAPSHOT_PATH):
        shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)
        log("snapshot initialized")
        return

    orig_size = os.path.getsize(ORIGINAL_PATH)
    snap_size = os.path.getsize(SNAPSHOT_PATH)

    # if truncated/rotated, reset baseline
    if orig_size < snap_size:
        shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)
        log("file truncated/rotated, snapshot reset")
        return

    if orig_size == snap_size:
        return

    with open(ORIGINAL_PATH, "rb") as f:
        f.seek(snap_size)
        new_data = f.read()

    if not new_data:
        return

    # optional local debug artifact (latest diff)
    try:
        atomic_write_bytes(DIFF_PATH, new_data)
    except Exception as e:
        log(f"warning: could not update diff file: {e}")

    # update snapshot before enqueue so we don't duplicate on restart
    shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)

    enqueue_payload_bytes(new_data)


def trigger_processing():
    global _timer

    with lock:
        _timer = None

    if not processing_lock.acquire(blocking=False):
        return

    try:
        process_diff()
    finally:
        processing_lock.release()


# --------------------------------------------------------------------------------------
class LogHandler(FileSystemEventHandler):
    @staticmethod
    def _norm(path: str) -> str:
        try:
            return os.path.realpath(path)
        except Exception:
            return path

    def _is_target(self, path: str) -> bool:
        return self._norm(path) == ORIGINAL_PATH

    def _schedule(self):
        global _timer
        with lock:
            if _timer is not None:
                _timer.cancel()
            _timer = threading.Timer(DEBOUNCE_SECONDS, trigger_processing)
            _timer.daemon = True
            _timer.start()

    def on_modified(self, event):
        if event.is_directory:
            return
        if self._is_target(event.src_path):
            log(f"event modified: {event.src_path}")
            self._schedule()

    def on_created(self, event):
        if event.is_directory:
            return
        if self._is_target(event.src_path):
            log(f"event created: {event.src_path}")
            self._schedule()

    def on_moved(self, event):
        if event.is_directory:
            return
        dest = getattr(event, "dest_path", "")
        if self._is_target(event.src_path) or self._is_target(dest):
            log(f"event moved: {event.src_path} -> {dest}")
            self._schedule()


# --------------------------------------------------------------------------------------
def recover_orphan_queue_files():
    """
    If queue dir has files not in queue_index (e.g., crash between file write and index save),
    re-add them in timestamp order.
    """
    ensure_dirs()
    pattern = re.compile(r"^payload_\d{8}_\d{6}_\d{6}\.log$")
    files = [f for f in os.listdir(QUEUE_PATH) if pattern.match(f)]
    files.sort()

    with queue_lock:
        known = set(send_queue)
        added = 0
        for f in files:
            if f not in known:
                send_queue.append(f)
                added += 1
        if added:
            save_queue_index()
            log(f"recovered {added} orphan payload(s)")


def initial_snapshot_setup():
    if os.path.exists(ORIGINAL_PATH) and not os.path.exists(SNAPSHOT_PATH):
        shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)
        log("snapshot created at startup")


def main():
    log(f"Battery log watcher started (debounce={DEBOUNCE_SECONDS}s)")
    log(f"watch dir: {WATCH_DIR}")
    log(f"target file: {ORIGINAL_PATH}")
    log(f"queue dir: {QUEUE_PATH}")

    ensure_dirs()
    load_queue_index()
    recover_orphan_queue_files()
    initial_snapshot_setup()

    # sender worker
    sender = threading.Thread(target=sender_loop, daemon=True)
    sender.start()

    handler = LogHandler()
    observer = Observer()
    observer.schedule(handler, path=WATCH_DIR, recursive=False)
    observer.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        log("shutdown requested")
    finally:
        stop_event.set()
        queue_wakeup.set()
        observer.stop()
        observer.join(timeout=10)
        sender.join(timeout=10)
        log("stopped cleanly")


# --------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
