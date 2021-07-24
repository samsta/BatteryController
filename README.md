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


Rough structure and description of key blocks:

## Monitor

- receives decoded CAN messages from the Battery
- retains measurement and identity data and exposes methods to query the data
- determines current limits and exposes methods to query them
- determines whether it is safe to operate the contactor
   - it uses a reference to a contactor instance to do that
  
## Contactor

- is told (by Monitor) whether it is safe to be operated
- is requested to open or close by Inverter
   - however, the request is only actioned when safe to operate
- exposes a method to query whether it is actually opened

## Inverter (SunnyBoyStorage)

- periodically queries measurement and limit data from Monitor, and the contactor state and sends it out to CAN
   - it uses a reference to a Timer instance to register its periodic callback
   - it uses a reference to a Monitor instance to query the data from
   - it uses a reference to a Contactor instance to query its state
   - it uses a reference to a FrameSink to send the CAN messages
- responds with identity data as queried from Monitor when a handshake is received
   - it uses a reference to a FrameSink to send the CAN messages
- closes and opens the inverter in response to messages received
   - it uses a reference to a Contactor instance to open and close the contactor
   

Auxiliary blocks:

## FrameAggregator

- assembles the diagnostic message groups received from the battery into complete messages

## GroupPoller

- polls the diagnostic message groups from the battery

And a few more bits and bobs that need to be extracted from the horrible testcan.cpp and turned into proper classes with defined responsibilities.

# Building the Code

## Eclipse on Ubuntu

Currently, we can only build for the native platform under Eclipes (i.e. no cross-compiling to create code for the Pi). This is good for unit tests, but you can also run the other applications given you provide either real or virtual CAN interfaces.

First, let's install the prerequisites. In a terminal, run:

~~~
sudo apt -y install cmake libgtk-3-0 libsocketcan-dev libgpiod-dev can-utils
~~~

Download the installer from https://www.eclipse.org/downloads/download.php?file=/oomph/epp/2020-12/R/eclipse-inst-jre-linux64.tar.gz , then unpack and run eclipse-inst. Note that your browser might download it to a different directory, but we'll assume the default here:

~~~
cd ~/Downloads
tar xvf eclipse-inst-jre-linux64.tar.gz  
cd eclipse-installer/  
./eclipse-inst  
~~~

Then check the 'C/C++ Development Tools' are installed: 
- Help > About Eclipse > Installation Details > Installed Software

If you can't find the C/C++ Development Tools in there let me know and I'll dig up some instructions.

Then download the CMake Editor plugin: 

- Help > Eclipse Marketplace > Search
   - search for 'CMake Editor'
   - install it; follow on-screen instructions

Then, go to a a directory of your choice (I'll assume `~/Projects`, which is short for `/home/<username>/Projects`):

~~~
cd ~/Projects
git clone --recurse-submodules https://github.com/samsta/BatteryController.git
mkdir BatteryController-build
cd BatteryController-build
cmake ../BatteryController -G 'Eclipse CDT4 - Unix Makefiles'
~~~

Do not despair - this only needs to be done once to create the `.project` file Eclipse wants.

Now import the project into Eclipse:

- File > Import > General > Existing Projects into Workspace > Next
   - Choose 'Select root directory', then click 'Browse'
   - click 'Home'
   - navigate to Projects/NatteryController-build and click Open
   - It should now list under Projects:
      - BatteryController@BatteryController-build
   - click Finish

You have now imported the project into Eclipse. You should not have to do that again, it should still be there the next time you open Eclipse.

Some useful views to Open:

- Window > Show View > Project Explorer. You'll find the code in the newly opened view under '[Source Directory]
- Window > Show View > Build Targets. You can double click the 'all' target, and once that's completed, you can run the tests by double clicking the 'test` target.

I will add a few more targets that build and run individual tests as that's much more convenient.

Note: If you add source files to Eclipse, you have to add them to the CMakeLists.txt in the same directory too. This is so we can build stuff outside of Eclipse, e.g. on the Pi where we just use `CMake` and `make` and no IDE.

