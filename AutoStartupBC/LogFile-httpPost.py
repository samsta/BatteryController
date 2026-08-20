import os
import time
import shutil
import threading
import requests
from datetime import datetime

from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler


# --------------------------------------------------------------------------------------
URL = "http://jimster.ca/BatteryHikotron/BatteryHikotron-log-receiver.php"

ORIGINAL_FILE = "BatteryController.log"
SNAPSHOT_FILE = "BatteryControllerCOPY.log"
DIFF_FILE = "BatteryControllerDIFF.log"

DEBOUNCE_SECONDS = 10


# --------------------------------------------------------------------------------------
# Resolve absolute, normalized paths once
WATCH_DIR = os.path.abspath(".")
ORIGINAL_PATH = os.path.realpath(os.path.join(WATCH_DIR, ORIGINAL_FILE))
SNAPSHOT_PATH = os.path.realpath(os.path.join(WATCH_DIR, SNAPSHOT_FILE))
DIFF_PATH = os.path.realpath(os.path.join(WATCH_DIR, DIFF_FILE))

_timer = None
lock = threading.Lock()
processing_lock = threading.Lock()


# --------------------------------------------------------------------------------------
def ts():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def post_file(file_path: str):
    if not os.path.exists(file_path):
        return

    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            data = f.read()

        if not data:
            return

        headers = {
            "Content-Type": "text/plain",
            "User-Agent": "RPi-BatteryLogger/1.1"
        }

        r = requests.post(URL, data=data, headers=headers, timeout=15)

        if r.status_code == 200:
            print(f"{ts()} uploaded {len(data)} bytes")
            os.remove(file_path)
        else:
            print(f"{ts()} POST failed {r.status_code}")

    except Exception as e:
        print(f"{ts()} post error: {e}")


# --------------------------------------------------------------------------------------
def process_diff():
    if not os.path.exists(ORIGINAL_PATH):
        return

    # first run snapshot
    if not os.path.exists(SNAPSHOT_PATH):
        shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)
        print(f"{ts()} snapshot initialized")
        return

    orig_size = os.path.getsize(ORIGINAL_PATH)
    snap_size = os.path.getsize(SNAPSHOT_PATH)

    # If file was rotated/truncated, reset snapshot baseline
    if orig_size < snap_size:
        shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)
        print(f"{ts()} file truncated/rotated, snapshot reset")
        return

    if orig_size == snap_size:
        return

    # read only appended data
    with open(ORIGINAL_PATH, "rb") as f:
        f.seek(snap_size)
        new_data = f.read()

    if not new_data:
        return

    tmp = DIFF_PATH + ".tmp"
    with open(tmp, "wb") as f:
        f.write(new_data)

    os.replace(tmp, DIFF_PATH)

    # update snapshot after extracting diff
    shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)

    post_file(DIFF_PATH)


# --------------------------------------------------------------------------------------
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
    def _is_target(path: str) -> bool:
        try:
            return os.path.realpath(path) == ORIGINAL_PATH
        except Exception:
            return False

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
            print(f"{ts()} event modified: {event.src_path}")
            self._schedule()

    def on_created(self, event):
        if event.is_directory:
            return
        if self._is_target(event.src_path):
            print(f"{ts()} event created: {event.src_path}")
            self._schedule()

    def on_moved(self, event):
        if event.is_directory:
            return
        # Some writers rotate via temp file + rename into place
        dest = getattr(event, "dest_path", "")
        if self._is_target(event.src_path) or self._is_target(dest):
            print(f"{ts()} event moved: {event.src_path} -> {dest}")
            self._schedule()


# --------------------------------------------------------------------------------------
def main():
    print(f"{ts()} Battery log watcher started (event-driven, {DEBOUNCE_SECONDS}s debounce)")
    print(f"{ts()} watching directory: {WATCH_DIR}")
    print(f"{ts()} target file: {ORIGINAL_PATH}")

    # Ensure snapshot exists if file exists at startup
    if os.path.exists(ORIGINAL_PATH) and not os.path.exists(SNAPSHOT_PATH):
        shutil.copy2(ORIGINAL_PATH, SNAPSHOT_PATH)
        print(f"{ts()} snapshot created at startup")

    handler = LogHandler()
    observer = Observer()
    observer.schedule(handler, path=WATCH_DIR, recursive=False)
    observer.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print(f"{ts()} stopping watcher...")
        observer.stop()

    observer.join()


# --------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()
