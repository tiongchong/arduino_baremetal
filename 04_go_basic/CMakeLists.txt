cmake_minimum_required(VERSION 3.15)

# Automatically set project name to folder name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} C ASM)

# === CONFIGURABLE ===
set(MCU atmega328p)
set(F_CPU 16000000UL)
set(TARGET_NAME ${PROJECT_NAME})
set(BAUD 57600)

# === OS DETECTION ===
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(IS_WINDOWS TRUE)
    set(PORT COM19)
    set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-win32_x86_64)
    set(AVRDUDE_PATH ${CMAKE_SOURCE_DIR}/../../avrdude-v8.1-windows-x64/avrdude.exe)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(IS_LINUX TRUE)
    set(PORT /dev/ttyUSB0)
    set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-linux_x86_64)
    set(AVRDUDE_PATH avrdude)
else()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

# === Toolchain executables ===
set(COMPILER_PREFIX ${TOOLCHAIN_DIR}/bin/avr-)
set(OBJCOPY ${COMPILER_PREFIX}objcopy)
set(GCC ${COMPILER_PREFIX}gcc)

# Tell CMake to use the AVR GCC compilers explicitly
set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_ASM_COMPILER ${GCC})

# === Build output directory ===
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# === Sources ===
file(GLOB_RECURSE C_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB_RECURSE ASM_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.S")
set(SRC_FILES ${C_SOURCES} ${ASM_SOURCES})

# === Linker script ===
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld)
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU} -Wl,-T${LINKER_SCRIPT} -Wl,--gc-sections")

# === Compiler flags ===
set(COMMON_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os -ffreestanding -nostdlib -nostartfiles -Wall -Wextra")
set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS}")

# === Add executable target with .elf suffix explicitly ===
add_executable(${TARGET_NAME}.elf ${SRC_FILES})

# Set output file name explicitly to include .elf
set_target_properties(${TARGET_NAME}.elf PROPERTIES
    OUTPUT_NAME "${TARGET_NAME}.elf"
    RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}
)

# === Generate listing (.lst) and map files ===
target_compile_options(${TARGET_NAME}.elf PRIVATE
    "-Wa,-adhlns=${BUILD_DIR}/${TARGET_NAME}.lst"
)
set_target_properties(${TARGET_NAME}.elf PROPERTIES
    LINK_FLAGS "-Wl,-Map=${BUILD_DIR}/${TARGET_NAME}.map"
)

# === Post-build: generate .hex file from .elf ===
add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${OBJCOPY} -O ihex -R .eeprom
        ${BUILD_DIR}/${TARGET_NAME}.elf
        ${BUILD_DIR}/${TARGET_NAME}.hex
    WORKING_DIRECTORY ${BUILD_DIR}
    COMMENT "Generating HEX file"
)

# === Flash target ===
add_custom_target(flash
    COMMAND ${AVRDUDE_PATH}
        -c arduino
        -p ${MCU}
        -P ${PORT}
        -b ${BAUD}
        -U flash:w:${BUILD_DIR}/${TARGET_NAME}.hex
    DEPENDS ${TARGET_NAME}.elf
    WORKING_DIRECTORY ${BUILD_DIR}
    COMMENT "Flashing the device..."
)

# === Optional clean target (CMake built-in already handles this) ===
# You can run `cmake --build build --target clean` to clean

# === Optional all target (default build) ===
# Run `cmake --build build` to build all

