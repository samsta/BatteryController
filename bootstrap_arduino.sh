#!/bin/bash

src_base=$(dirname $(readlink -f $0))

ARDUINO_BOARD=${ARDUINO_BOARD-diecimila}
ARDUINO_CPU=${ARDUINO_CPU-atmega328}
ARDUINO_SDK_VERSION=${ARDUINO_SDK_VERSION-1.8.9}
ARDUINO_SDK_BASE_PATH=${ARDUINO_SDK_BASE_PATH-$src_base}

export ARDUINO_SDK_PATH=${ARDUINO_SDK_BASE_PATH}/arduino-sdk/arduino-${ARDUINO_SDK_VERSION}


echo "- Will build for board $ARDUINO_BOARD with CPU $ARDUINO_CPU"
echo "  using Arduino SDK version $ARDUINO_SDK_VERSION."

if [ ! -f "$ARDUINO_SDK_PATH/arduino" ]; then
   rm -rf "$ARDUINO_SDK_PATH"
   mkdir -p "$ARDUINO_SDK_PATH"
   sdk_file=arduino-$ARDUINO_SDK_VERSION-linux64.tar.xz
   echo "- Downloading and installing $sdk_file..."
   (
      cd "$ARDUINO_SDK_BASE_PATH"
      wget "https://downloads.arduino.cc/$sdk_file" -O "$sdk_file"
      tar -xf "$sdk_file" -C arduino-sdk
      rm "$sdk_file"
   )
fi    

echo "- Arduino SDK is located at $ARDUINO_SDK_PATH."
cmake -DCMAKE_TOOLCHAIN_FILE="$src_base/Arduino-CMake-NG/cmake/Arduino-Toolchain.cmake" \
      -DARDUINO_BOARD=$ARDUINO_BOARD \
      -DARDUINO_CPU=$ARDUINO_CPU \
      "$src_base"
