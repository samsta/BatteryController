import os
import time
import shutil
import requests
from datetime import datetime

URL = "http://jimster.ca/BatteryOne/BatteryOne-log-receiver.php"

ORIGINAL_FILE = "BatteryController.log"
SNAPSHOT_FILE = "BatteryControllerCOPY.log"
DIFF_FILE = "BatteryControllerDIFF.log"
PENDING_FILE = "BatteryControllerPENDING.log"


# --------------------------------------------------------------------------------------
def post_file(file_path: str) -> bool:
    if not os.path.exists(file_path):
        return True  # nothing to send, treat as success

    try:
        with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
            data = f.read()

        headers = {
            "Accept": "*/*",
            "Content-Type": "text/plain",
            "User-Agent": "RPi-BatteryLogger/1.0"
        }

        r = requests.post(URL, data=data, headers=headers, timeout=15)

        if r.status_code == 200:
            print(f"{datetime.now()} uploaded {len(data)} bytes")
            os.remove(file_path)
            return True

        print(f"POST failed: {r.status_code} {r.text}")
        return False

    except requests.RequestException as e:
        print(f"network error: {e}")
        return False


# --------------------------------------------------------------------------------------
def log_changed(original_file: str, snapshot_file: str, diff_file: str) -> bool:
    if not os.path.exists(original_file):
        return False

    # first run recovery
    if not os.path.exists(snapshot_file):
        shutil.copy2(original_file, snapshot_file)
        return False

    orig_size = os.path.getsize(original_file)
    snap_size = os.path.getsize(snapshot_file)

    if orig_size == snap_size:
        return False

    # append-only assumption: read only new bytes
    with open(original_file, "rb") as f:
        f.seek(snap_size)
        new_data = f.read()

    if not new_data:
        return False

    # write diff atomically
    tmp_diff = diff_file + ".tmp"
    with open(tmp_diff, "wb") as f:
        f.write(new_data)

    os.replace(tmp_diff, diff_file)

    # update snapshot ONLY after diff is safely written
    shutil.copy2(original_file, snapshot_file)

    return True


# --------------------------------------------------------------------------------------
def sleep_to_next_minute(offset_seconds: int = 10):
    now = time.time()
    next_tick = (int(now // 60) + 1) * 60 + offset_seconds
    time.sleep(max(1, next_tick - now))


# --------------------------------------------------------------------------------------
def main():
    print("Battery log forwarder started")

    # recovery: if crash happened with pending diff, resend it
    post_file(PENDING_FILE)

    while True:
        try:
            changed = log_changed(ORIGINAL_FILE, SNAPSHOT_FILE, DIFF_FILE)

            if changed:
                print(f"{datetime.now()} log updated")

                # move diff -> pending so we never lose it
                if os.path.exists(DIFF_FILE):
                    shutil.move(DIFF_FILE, PENDING_FILE)

                ok = post_file(PENDING_FILE)

                if not ok:
                    print("will retry pending data next cycle")

            else:
                print(f"{datetime.now()} no changes")

        except Exception as e:
            print(f"loop error: {e}")

        sleep_to_next_minute()


# --------------------------------------------------------------------------------------
if __name__ == "__main__":
    main()