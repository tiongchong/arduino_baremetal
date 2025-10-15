cmake_minimum_required(VERSION 3.15)
project(blinky C)

# === CONFIGURABLE ===
set(MCU atmega328p)
set(F_CPU 16000000UL)
set(BAUD 115200)
set(AVR_TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-linux_x86_64)

# === TOOLCHAIN ===
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_C_COMPILER ${AVR_TOOLCHAIN_DIR}/bin/avr-gcc)
set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")

# === FIND ALL .c FILES RECURSIVELY ===
# Recursively find all C sources, but exclude build directory
file(GLOB_RECURSE ALL_C_FILES CONFIGURE_DEPENDS *.c)

# Filter out files inside build/ folder (adjust if build is inside your source dir)
list(FILTER ALL_C_FILES EXCLUDE REGEX ".*/build/.*")

set(SRC_FILES ${ALL_C_FILES})

# === BUILD TARGET ===
add_executable(blinky.elf ${SRC_FILES})

# === GENERATE HEX AFTER BUILD ===
add_custom_command(TARGET blinky.elf POST_BUILD
    COMMAND ${AVR_TOOLCHAIN_DIR}/bin/avr-objcopy -O ihex -R .eeprom blinky.elf blinky.hex
    COMMENT "Generating HEX file"
)

