#!/bin/bash
set -e

AVRDUDE=avrdude
BUILD_DIR=build
MCU=atmega328p
PORT=/dev/ttyUSB0   # Update this to your Arduino serial port

# Find the HEX file in the build directory
HEX_FILE=$(find "$BUILD_DIR" -maxdepth 1 -name "*.hex" | head -n 1)

if [ -z "$HEX_FILE" ]; then
  echo "Error: No HEX file found in $BUILD_DIR"
  exit 1
fi

echo "Flashing $HEX_FILE to $MCU on $PORT..."

$AVRDUDE -v -p$MCU -c arduino -P $PORT -b 57600 -D -U flash:w:$HEX_FILE:i

