import os
import requests
import time
from datetime import datetime

# Define the function to post the file
def post_file():
    url = "http://jimster.ca/BatteryOne/BatteryOne-data-receiver-oems.php"
    file_path = 'MeterDataSDM630.txt'

    # Get the size of the file in bytes
    file_size = os.path.getsize(file_path)
    print(f"Size of the file '{file_path}': {file_size} bytes")

    with open(file_path, 'r') as file:
        data = file.read()

    try:
        response = requests.post(url, data=data)
        if response.status_code == 200:
            print(f"File successfully posted at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
            # Delete the file after successful upload
            os.remove(file_path)  # Remove the file
            print(f"File {file_path} successfully deleted.")
        else:
            print(f"Failed to post the file. Status Code: {response.status_code}")
            # Just ignore the failure and continue to the next attempt
    except requests.exceptions.RequestException as e:
        print(f"Error posting file: {e}")
        # Just ignore the exception and continue to the next attempt


# Function to sleep until 30 seconds after the top of the minute
def sleep_until_30s_after_the_minute():
    now = datetime.now()
    seconds_to_wait = 60 - now.second + 30  # Wait until the next 30 seconds past the minute
    if seconds_to_wait >= 60:
        seconds_to_wait -= 60
    if seconds_to_wait < 5:
        seconds_to_wait += 60

    print(f"Waiting for {seconds_to_wait} seconds to post the file...")
    time.sleep(seconds_to_wait)

# Main program
if __name__ == "__main__":
    while True:
        sleep_until_30s_after_the_minute() 
        post_file()  
