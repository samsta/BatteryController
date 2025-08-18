#!/bin/sh
# launcher.sh
# navigate to home directory, then to this directory, then execute python script, then back home

cd /
cd home/pi/Documents/Projects/BatteryController/build/
#echo 'sleep 30, then start battery controller'
#sleep 30
/usr/bin/x-terminal-emulator -e ./testcan 4 can0 /dev/ttyTSY11576900 /dev/ttyTSY12508710
