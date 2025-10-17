# Arduino baremetal

## to compile the project and flash using cmake build system
- cmake -S . -B build -G Ninja -D CMAKE_TOOLCHAIN_FILE="avr-toolchain.cmake"
- cmake --build build --clean-first
- cmake --build build --target flash

## to compile the project using make build system
- make

## progress log
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

