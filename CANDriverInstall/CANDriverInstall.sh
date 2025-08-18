cd /

# 250000 is for the Sinexel
#sudo /sbin/ip link set can0 up type can bitrate 250000
#sudo /sbin/ip link set can1 up type can bitrate 250000

# 500000 is for the TSUN
sudo /sbin/ip link set can0 up type can bitrate 500000
sudo /sbin/ip link set can1 up type can bitrate 500000

sudo ip link set can0 txqueuelen 1000
sudo ip link set can1 txqueuelen 1000