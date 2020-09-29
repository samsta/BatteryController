# BatteryController

[![Build Status](https://travis-ci.com/samsta/BatteryController.svg?token=G5p63QVxar7JscrjTLwf&branch=master)](https://travis-ci.com/samsta/BatteryController)

The battery controller is an interpreter between a HV(EV) battery and an inverter.
Currently it is being develped to interface between a Nissan Leaf Battery & an [SMA SBS 5 inverter](https://www.sma.de/en/products/battery-inverters/sunny-boy-storage-37-50-60.html) (affectionately known as The Altar of Satan or simply TAoS)

Logic:
1. Ensure all contactors are open (off)

2. Process Battery data
   - Receive data via CAN from (Leaf) battery
   - Determine if any values are at critical level. If so open contactors ASAP
   - Determine if any values are at warning level. If so take appropriate action (most likely reduce max current value for either charge, discharge or both)
   - If no critical or warning set variables to default limits
   - If no critical values move to analyse inverter 

3. Process Inverter data
   - Receive data via CAN from (SMA) Inverter
   - Look for 0x560 handshake request. When received respond. (If not received within 10 mins shutdown - this is how the LG behaves)
   - Monitor enable 12V line from (SMA) Inverter
   - Determine if shutdown requested. If so switch everything off ASAP
   - Determine if connect requested by CAN
   - Determine if connect requested by enable line
   - Initiate contactor enable sequence

4. Periodically broadcast battery state messages
   - Compare received messages from inverter of battery status to actual status & warn/shutdown if they differ too much?
