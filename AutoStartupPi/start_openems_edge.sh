#!/bin/sh
# launcher.sh
# navigate to home directory, then to this directory, then execute python script, then back home

cd /
cd home/pi/openems
echo 'sleep 40, then start openems edge'
sleep 40
/usr/bin/x-terminal-emulator -e "source '$HOME/.sdkman/bin/sdkman-init.sh' && java -version && java -jar openems-edge-release.jar | tee openems-edge-log.txt"

