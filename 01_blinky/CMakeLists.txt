cmake_minimum_required(VERSION 3.15)

# Automatically set the project name to the folder name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECT_NAME} C)

# === CONFIGURABLE ===
set(MCU atmega328p)
set(F_CPU 16000000UL)
set(TARGET_NAME ${PROJECT_NAME})
set(BAUD 57600)

# === TOOLCHAIN SELECTION ===
# Options: gcc (default), clang
if(NOT DEFINED TOOLCHAIN)
    set(TOOLCHAIN gcc)
endif()

# === OS DETECTION ===
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(PORT COM19)
    set(GCC_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-win32_x86_64")
    set(LLVM_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../llvm-avr/bin")
    set(AVRDUDE_PATH "${CMAKE_SOURCE_DIR}/../../avrdude-v8.1-windows-x64/avrdude.exe")

elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    set(PORT /dev/ttyUSB0)
    set(GCC_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-linux_x86_64")
    set(LLVM_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../llvm-avr/bin")
    set(AVRDUDE_PATH avrdude)

elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(PORT /dev/tty.usbserial-1420)
    set(GCC_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-darwin_universal")
    set(LLVM_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../llvm-avr/bin")
    set(AVRDUDE_PATH avrdude)

else()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

# === TOOLCHAIN SELECTION ===
if(${TOOLCHAIN} STREQUAL "clang")
    set(CMAKE_C_COMPILER "${LLVM_TOOLCHAIN_DIR}/clang")
    set(OBJCOPY "${LLVM_TOOLCHAIN_DIR}/llvm-objcopy")
    set(OBJDUMP "${LLVM_TOOLCHAIN_DIR}/llvm-objdump")
else()
    set(CMAKE_C_COMPILER "${GCC_TOOLCHAIN_DIR}/bin/avr-gcc")
    set(OBJCOPY "${GCC_TOOLCHAIN_DIR}/bin/avr-objcopy")
    set(OBJDUMP "${GCC_TOOLCHAIN_DIR}/bin/avr-objdump")
endif()

# === COMMON FLAGS ===
set(COMMON_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")
set(AVR_LIBC_INC "${GCC_TOOLCHAIN_DIR}/avr/include")
set(LDSCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")

# === TOOLCHAIN-SPECIFIC FLAGS ===
if(${TOOLCHAIN} STREQUAL "clang")
    set(CMAKE_C_FLAGS
        "${COMMON_FLAGS} --target=avr -ffreestanding -fno-exceptions -fno-rtti -nostdlib -I${AVR_LIBC_INC}"
    )
    set(CMAKE_EXE_LINKER_FLAGS
        "-Wl,-T,${LDSCRIPT} -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map -fuse-ld=lld"
    )
else()
    set(CMAKE_C_FLAGS "${COMMON_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS
        "-Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET_NAME}.map"
    )
endif()

# === SOURCES ===
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.c")

# Match Makefile behavior
if(NOT ${TOOLCHAIN} STREQUAL "clang")
    list(FILTER SRC_FILES EXCLUDE REGEX ".*/crt0.c")
    list(FILTER SRC_FILES EXCLUDE REGEX ".*/vectors.c")
endif()

# === TARGET ===
add_executable(${TARGET_NAME}.elf ${SRC_FILES})

# === LST GENERATION ===
if(${TOOLCHAIN} STREQUAL "clang")
    add_custom_command(
        TARGET ${TARGET_NAME}.elf POST_BUILD
        COMMAND ${OBJDUMP} -d -S --demangle
                ${TARGET_NAME}.elf
                > ${TARGET_NAME}.lst
        COMMENT "Generating LST file (clang)"
    )
else()
    target_compile_options(${TARGET_NAME}.elf PRIVATE
        "-Wa,-adhln=${CMAKE_BINARY_DIR}/${TARGET_NAME}.lst"
    )
endif()

# === HEX GENERATION ===
add_custom_command(
    TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${OBJCOPY} -O ihex -R .eeprom
            ${TARGET_NAME}.elf
            ${TARGET_NAME}.hex
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
