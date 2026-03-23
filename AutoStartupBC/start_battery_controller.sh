#!/bin/sh
# start_battery_controller.sh

cd /home/pi/Documents/Projects/BatteryController/build/ || exit 1

echo 'sleep 30, then start battery controller Niall Deye'
sleep 30

/usr/bin/x-terminal-emulator -e ./testcan 4 candummy /dev/ttyACM0 usbdummy
