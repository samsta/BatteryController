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
_timer = None
lock = threading.Lock()
processing_lock = threading.Lock()


# --------------------------------------------------------------------------------------
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
            "User-Agent": "RPi-BatteryLogger/1.0"
        }

        r = requests.post(URL, data=data, headers=headers, timeout=15)

        if r.status_code == 200:
            print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S')} uploaded {len(data)} bytes")
            os.remove(file_path)
        else:
            print(f"{datetime.now().strftime('%Y-%m-%d %H:%M:%S')} POST failed {r.status_code}")

    except Exception as e:
        print(f"post error: {e}")


# --------------------------------------------------------------------------------------
def process_diff():
    if not os.path.exists(ORIGINAL_FILE):
        return

    # first run snapshot
    if not os.path.exists(SNAPSHOT_FILE):
        shutil.copy2(ORIGINAL_FILE, SNAPSHOT_FILE)
        return

    orig_size = os.path.getsize(ORIGINAL_FILE)
    snap_size = os.path.getsize(SNAPSHOT_FILE)

    if orig_size <= snap_size:
        return

    # read only appended data
    with open(ORIGINAL_FILE, "rb") as f:
        f.seek(snap_size)
        new_data = f.read()

    if not new_data:
        return

    tmp = DIFF_FILE + ".tmp"
    with open(tmp, "wb") as f:
        f.write(new_data)

    os.replace(tmp, DIFF_FILE)

    shutil.copy2(ORIGINAL_FILE, SNAPSHOT_FILE)

    post_file(DIFF_FILE)


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
    def on_modified(self, event):
        global _timer

        if not event.src_path.endswith(ORIGINAL_FILE):
            return

        with lock:
            if _timer is not None:
                _timer.cancel()

            _timer = threading.Timer(DEBOUNCE_SECONDS, trigger_processing)
            _timer.start()


# --------------------------------------------------------------------------------------
def main():
    print("Battery log watcher started (event-driven, 10s debounce)")

    # ensure snapshot exists
    if os.path.exists(ORIGINAL_FILE) and not os.path.exists(SNAPSHOT_FILE):
        shutil.copy2(ORIGINAL_FILE, SNAPSHOT_FILE)

    handler = LogHandler()
    observer = Observer()
    observer.schedule(handler, path=".", recursive=False)
    observer.start()

    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        observer.stop()

    observer.join()


# --------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()