#!/bin/sh
# start_battery_controller.sh

cd /home/pi/Documents/Projects/BatteryController/build/ || exit 1

#echo 'sleep 30, then start battery controller'
#sleep 30

/usr/bin/x-terminal-emulator -e ./testcan 4 can0 /dev/ttyUSB0 dummy
