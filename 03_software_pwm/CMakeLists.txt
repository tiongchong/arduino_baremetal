cmake_minimum_required(VERSION 3.15)

# Automatically set the project name to the folder name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} C)

# === CONFIGURABLE ===
set(MCU atmega328p)
set(F_CPU 16000000UL)
set(TARGET_NAME ${PROJECT_NAME})
set(BAUD 57600)

# === OS DETECTION ===
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(IS_WINDOWS TRUE)
    set(PORT COM19)
    set(TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-win32_x86_64")
    set(AVRDUDE_PATH "${CMAKE_SOURCE_DIR}/../../avrdude-v8.1-windows-x64/avrdude.exe")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(IS_LINUX TRUE)
    set(PORT /dev/ttyUSB0)
    set(TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-linux_x86_64")
    set(AVRDUDE_PATH "avrdude")  # Assumes avrdude is in PATH
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(IS_MAC TRUE)
    set(PORT /dev/tty.usbserial-1420)
    set(TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-darwin_universal")
    set(AVRDUDE_PATH "avrdude")  # Assumes avrdude is in PATH
else()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

# === Toolchain Binaries ===
set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}/bin/avr-gcc")
set(OBJCOPY "${TOOLCHAIN_DIR}/bin/avr-objcopy")

# === COMPILER FLAGS ===
set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU}")

# === SOURCES ===
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
list(FILTER SRC_FILES EXCLUDE REGEX ".*/build/.*")

# === TARGET ===
add_executable(${TARGET_NAME}.elf ${SRC_FILES})

# === HEX GENERATION ===
add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${OBJCOPY} -O ihex -R .eeprom ${TARGET_NAME}.elf ${TARGET_NAME}.hex
    COMMENT "Generating HEX file"
)

# === FLASH TARGET ===
add_custom_target(flash
    COMMAND ${AVRDUDE_PATH}
        -c arduino
        -p ${MCU}
        -P ${PORT}
        -b ${BAUD}
        -U flash:w:${TARGET_NAME}.hex
    DEPENDS ${TARGET_NAME}.hex
    COMMENT "Flashing the device..."
)
