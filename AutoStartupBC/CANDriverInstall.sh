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

sudo slcand -o -c -s6 -t hw /dev/ttyCANABLE can0
sudo ifconfig can0 txqueuelen 1000
sudo ifconfig can0 up


# int rcvbuf = 4 * 1024 * 1024; // 4 MB
# setsockopt(s, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf));

