# Arduino baremetal

https://www.vyrian.com/blog/atmega328p-microcontroller-features-pinout-configuration-datasheets/


Datasheet : 8-bit AVR Microcontroller with 32K Bytes In-System Programmable Flash 

https://docs.arduino.cc/resources/datasheets/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf


DATASHEET COMPLETE

https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf


ATmega328P Memory

https://cookierobotics.com/043/

https://www.sharetechnote.com/html/Arduino_BasicInfo.html


AVR® Instruction Set Manual

https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/ReferenceManuals/AVR-InstructionSet-Manual-DS40002198.pdf

## to compile the project and flash using cmake build system
- cmake -S . -B build -G Ninja -D CMAKE_TOOLCHAIN_FILE="avr-toolchain.cmake"

or
- cmake -S . -B build -G "Unix Makefiles" -D CMAKE_TOOLCHAIN_FILE="avr-toolchain.cmake"

- cmake --build build --clean-first
- cmake --build build --target flash

## to compile the project using make build system (default : avr-gcc)
- make

or change to use llvm
- make TOOLCHAIN=clang 

## progress log
2025/10/18
- Update both CMake and Make build systems to support MacOS

2025/10/17
- Added breathing LED to PB5 of ATmega328P using software PWM and hardware timer.
- Added content covering core fundamentals of bare-metal C programming.

2025/10/16
- CMake and Make build systems have been verified to compile successfully on both Windows and Linux platforms.
- Added hardware timer for delay function.

2025/10/15 
- on this one fine day, I like to try out Arduino in the baremetal c approach.
- so i downloaded AVR toolchain - https://www.microchip.com/en-us/tools-resources/develop/microchip-studio/gcc-compilers
- and build avrdude from source - https://github.com/avrdudes/avrdude

- my first blinky project, tested working using Arduino Nano.

