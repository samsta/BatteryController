import os
import requests
import time
from datetime import datetime

# Define the function to post the file
def post_file():
    url = "http://jimster.ca/BatteryOne/BatteryOne-data-receiver-oems.php"
    file_path = 'MeterDataSDM630.txt'

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
        response = requests.post(url, data=data)
        if response.status_code == 200:
            print(f"File successfully posted at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
            os.remove(file_path)  # Delete the file after successful upload
            print(f"File '{file_path}' successfully deleted.")
        else:
            print(f"Failed to post the file. Status Code: {response.status_code}")
            # Ignore the failure and continue

    except requests.exceptions.RequestException as e:
        print(f"Error posting file: {e}")
        # Ignore the exception and continue

# Function to sleep until 30 seconds after the top of the minute
def sleep_until_30s_after_the_minute():
    now = datetime.now()
    seconds_to_wait = 60 - now.second + 30  # Calculate wait time

    if seconds_to_wait >= 60:
        seconds_to_wait -= 60  # Adjust if it overflows
    if seconds_to_wait < 5:
        seconds_to_wait += 60  # Ensure a proper wait if very close to the mark

    print(f"Waiting for {seconds_to_wait} seconds to post the file...")
    time.sleep(seconds_to_wait)

# Main program
if __name__ == "__main__":
    post_file()
    while True:
        sleep_until_30s_after_the_minute()
        post_file()
