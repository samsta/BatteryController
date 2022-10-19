# BatteryController
BatteryController is an interpreter package to integrate EV batteries & inverters. Specifically (at a high level):

•	Translate communications

•	Control power connections

•	Report status to other systems

It is written in C++ 
# Operation
On initial commissioning the battery’s voltages are manually matched and then connected to the 400V DC bus.
# Central Controller
The Central Controller receives battery information from the Module Controllers. It aggregates & processes this information then passes it to the inverter. If the aggregated status is OK and the inverter requests it, then it connects the inverter to the 400V DC bus via a pre-charge process.
If a battery is not OK the Central Controller commands that battery to be disconnected by communicating with the appropriate Module Controller. The system can continue running with batteries offline
If necessary the Central Controller can command a power cycle to an individual Battery’s computer.
# Module Controller
The Module  Controller receives battery information from up to three batteries. It passes this information on to the Central Controller.
If commanded by the Central Controller it can disconnect a battery from the 400V DC bus &/or power cycle an individual battery’s computer.


# Building the Code

This is a [CMake](https://cmake.org/) project. CMake is great when it comes to cross-compiling and compiling code on different platforms. Currently, we can build on the Raspberry Pi (on the command line) or Linux and macOS (on the command line or using an IDE like Eclipse or Xcode). In theory, we could build under Windows too (e.g. using Visual Studio) but I have not bothered providing the support in the source code which I'm happy to leave as an exercise to an interested party. The unit tests and all units should just work on Windows, but the top-level files stitching it all together will need to be different as Windows' way to deal with files and the CAN bus etc. are different - unless you choose to use [Cygwin](https://www.cygwin.com/).

## Eclipse on Ubuntu

Currently, we can only build for the native platform under Eclipes (i.e. no cross-compiling to create code for the Pi). This is good for unit tests, but you can also run the other applications given you provide either real or virtual CAN interfaces.

First, let's install the prerequisites. In a terminal, run:

~~~
sudo apt -y install build-essential cmake git libgtk-3-0 libsocketcan-dev libgpiod-dev can-utils
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
   - navigate to Projects/BatteryController-build and click Open
   - It should now list under Projects:
      - BatteryController@BatteryController-build
   - click Finish

You have now imported the project into Eclipse. You should not have to do that again, it should still be there the next time you open Eclipse.

Some useful views to Open:

- Window > Show View > Project Explorer. You'll find the code in the newly opened view under '[Source Directory]
- Window > Show View > Build Targets. You can double click the 'all' target, and once that's completed, you can run the tests by double clicking the 'check` target. You can run the test by building the `test` target but it won't give you as much useful output when a test fails.

Note: If you add source files to Eclipse, you have to add them to the CMakeLists.txt in the same directory too. This is so we can build stuff outside of Eclipse, e.g. on the Pi where we just use `CMake` and `make` and no IDE.



Rough structure and description of key softare objects:

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


