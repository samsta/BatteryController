#!/bin/bash
#---------------------------------------------
# -s0	10 kbit/s
# -s1	20 kbit/s
# -s2	50 kbit/s
# -s3	100 kbit/s
# -s4	125 kbit/s
# -s5	250 kbit/s
# -s6	500 kbit/s
# -s7	800 kbit/s
# -s8	1 Mbit/s

#sudo slcand -o -c -s6 -t hw /dev/ttyACM0 can0
#sudo ifconfig can0 txqueuelen 1000
#sudo ifconfig can0 up

# int rcvbuf = 4 * 1024 * 1024; // 4 MB
# setsockopt(s, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

#---------------------------------------------
# start_vcan.sh
# Creates two virtual CAN interfaces (vcan0, vcan1)
# and adjusts buffer size
#---------------------------------------------

set -e

##echo "Loading vcan kernel module..."
sudo modprobe vcan

# for IFACE in vcan0 vcan1; do
#   if ip link show "$IFACE" &>/dev/null; then
#     echo "Removing existing $IFACE..."
#     sudo ip link delete "$IFACE" type vcan || true
#   fi
# done

##echo "Creating vcan0..."
sudo ip link add dev can0 type vcan
# sudo ip link add dev vcan1 type vcan

# increase transmit queue length (buffer size)
sudo ip link set can0 txqueuelen 1000
# sudo ip link set vcan1 txqueuelen 1000

sudo ip link set up can0
# sudo ip link set up vcan1

##echo
##echo "Active vCAN interfaces with queue settings:"
##ip -details link show type vcan

##echo
##echo "? can0 is up with txqueuelen=1000"



