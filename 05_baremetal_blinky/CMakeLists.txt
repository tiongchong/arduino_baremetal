cmake_minimum_required(VERSION 3.20)
project(AVR_Project C ASM)

# === CONFIGURABLE ===
set(MCU atmega328p)
set(F_CPU 16000000UL)
set(BAUD 57600)
set(TARGET_NAME ${PROJECT_NAME})

# === OS DETECTION ===
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(OS_NAME "Windows")
    set(PORT COM19)
    set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-win32_x86_64)
    set(AVR_GCC ${TOOLCHAIN_DIR}/bin/avr-gcc.exe)
    set(AVR_OBJCOPY ${TOOLCHAIN_DIR}/bin/avr-objcopy.exe)
    set(AVR_OBJDUMP ${TOOLCHAIN_DIR}/bin/avr-objdump.exe)
    set(AVRDUDE ${CMAKE_SOURCE_DIR}/../../avrdude-v8.1-windows-x64/avrdude.exe)

elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(OS_NAME "Linux")
    set(PORT /dev/ttyUSB0)
    set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-linux_x86_64)
    set(AVR_GCC ${TOOLCHAIN_DIR}/bin/avr-gcc)
    set(AVR_OBJCOPY ${TOOLCHAIN_DIR}/bin/avr-objcopy)
    set(AVR_OBJDUMP ${TOOLCHAIN_DIR}/bin/avr-objdump)
    set(AVRDUDE avrdude)

elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(OS_NAME "macOS")
    set(PORT /dev/tty.usbserial-1420)
    set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-darwin_universal)
    set(AVR_GCC ${TOOLCHAIN_DIR}/bin/avr-gcc)
    set(AVR_OBJCOPY ${TOOLCHAIN_DIR}/bin/avr-objcopy)
    set(AVR_OBJDUMP ${TOOLCHAIN_DIR}/bin/avr-objdump)
    set(AVRDUDE avrdude)
else()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

message(STATUS "Detected OS: ${OS_NAME}")
message(STATUS "Using AVR-GCC: ${AVR_GCC}")

# === Toolchain Setup ===
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER ${AVR_GCC})
set(CMAKE_ASM_COMPILER ${AVR_GCC})

# === Paths & Sources ===
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

set(SOURCES
    ${SRC_DIR}/startup.S
    ${SRC_DIR}/crt0.S
    ${SRC_DIR}/baremetal_blinky.c
)

set(BUILD_DIR ${CMAKE_BINARY_DIR})
file(MAKE_DIRECTORY ${BUILD_DIR})

set(ELF ${BUILD_DIR}/${PROJECT_NAME}.elf)
set(HEX ${BUILD_DIR}/${PROJECT_NAME}.hex)
set(LST ${BUILD_DIR}/${PROJECT_NAME}.lst)
set(MAP ${BUILD_DIR}/${PROJECT_NAME}.map)
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld)

# === Flags ===
set(CFLAGS
    -mmcu=${MCU}
    -DF_CPU=${F_CPU}
    -Os
    -std=gnu11
    -Wall
    -Wextra
    -ffreestanding
    -nostdlib
    -nostartfiles
    -I${LIB_DIR}
)

set(LDFLAGS
    -mmcu=${MCU}
    -nostdlib
    -nostartfiles
    -Wl,-T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,-Map=${MAP}
)

# === Target ===
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}.elf
    RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE ${CFLAGS})
target_link_options(${PROJECT_NAME} PRIVATE ${LDFLAGS})

# === Post Build ===
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${AVR_OBJCOPY} -O ihex -R .eeprom ${ELF} ${HEX}
    COMMAND ${AVR_OBJDUMP} -d -S ${ELF} > ${LST}
    COMMENT "Generating HEX and LST files..."
)

# === Flash Target ===
add_custom_target(
    flash
    COMMAND ${AVRDUDE} -c arduino -p ${MCU} -P ${PORT} -b ${BAUD} -U flash:w:${HEX}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Flashing ${HEX} to ${MCU}..."
)

# === Clean Target ===
add_custom_target(
    clean-all
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${BUILD_DIR}
    COMMENT "Cleaning all build artifacts..."
)
