import os
import shutil
import requests
import time
from datetime import datetime

original_file="BatteryController.log",
copy_file="BatteryControllerCOPY.log",
diff_file="BatteryControllerDIFF.log"

# ***********************************************************************************************
# Define the function to post the file
def post_file():
    url = "http://jimster.ca/BatteryOne/BatteryOne-log-receiver.php"

    file_path = diff_file

    # Check if the file exists
    if not os.path.exists(file_path):
        print(f"File '{file_path}' does not exist. Skipping this attempt.")
        return  # Exit the function if the file is missing

    # Get the size of the file in bytes
    file_size = os.path.getsize(file_path)
    print(f"Size of the file '{file_path}': {file_size} bytes")

    with open(file_path, 'r') as file:
        data = file.read()

    try:
        headers = {
            "Accept": "*/*",
            "Content-Type": "text/plain",
            "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
        }

        response = requests.post(url, data=data, headers=headers)

        if response.status_code == 200:
            print(f"File successfully posted at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
            os.remove(file_path)
            print(f"File '{file_path}' successfully deleted.")
        else:
            print(f"Failed to post the file. Status Code: {response.status_code}")
            print("Headers:", response.headers)
            print("Text:", response.text)

    except requests.exceptions.RequestException as e:
        print(f"Error posting file: {e}")
        # Ignore the exception and continue


# ***********************************************************************************************
def log_changed(original_file, copy_file, diff_file):
    # first run
    if not os.path.exists(copy_file):
        shutil.copy2(original_file, copy_file)
        return True

    # quick check
    if os.path.getsize(original_file) == os.path.getsize(copy_file):
        return False

    # file has grown, get new lines
    with open(original_file, "r", encoding="utf-8", errors="ignore") as f:
        original_lines = f.readlines()

    with open(copy_file, "r", encoding="utf-8", errors="ignore") as f:
        copy_lines = f.readlines()

    new_lines = original_lines[len(copy_lines):]

    with open(diff_file, "w", encoding="utf-8") as f:
        f.writelines(new_lines)

    # update snapshot
    shutil.copy2(original_file, copy_file)

    return True

# ***********************************************************************************************
# Function to sleep until 30 seconds after the top of the minute
def sleep_until_10s_after_the_minute():
    now = datetime.now()
    seconds_to_wait = 60 - now.second + 10  # Calculate wait time

    if seconds_to_wait >= 60:
        seconds_to_wait -= 60  # Adjust if it overflows
    if seconds_to_wait < 5:
        seconds_to_wait += 60  # Ensure a proper wait if very close to the mark

    print(f"Waiting for {seconds_to_wait} seconds to post the file...")
    time.sleep(seconds_to_wait)

# ***********************************************************************************************
# Main program
if __name__ == "__main__":
    post_file()
    while True:
         sleep_until_10s_after_the_minute()
         if log_changed(original_file, copy_file, diff_file):
            print(f" {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}: Log file has changed. Posting new data...")
            post_file()
         else:
            print(f" {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}: No changes detected in the log file.")

      
