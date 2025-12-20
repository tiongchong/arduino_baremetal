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
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    set(IS_MAC TRUE)
    set(PORT /dev/tty.usbserial-1420)
    set(TOOLCHAIN_DIR ${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-darwin_universal)
    set(AVRDUDE_PATH avrdude)
else()
    message(FATAL_ERROR "Unsupported OS: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

# === Toolchain executables ===
set(COMPILER_PREFIX ${TOOLCHAIN_DIR}/bin/avr-)
set(GCC ${COMPILER_PREFIX}gcc)
set(OBJCOPY ${COMPILER_PREFIX}objcopy)
set(OBJDUMP ${COMPILER_PREFIX}objdump)

# Tell CMake to use the AVR GCC compilers explicitly
set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_ASM_COMPILER ${GCC})

# === Build output directory ===
set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

# === Include directories ===
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/lib/avr
    ${CMAKE_SOURCE_DIR}/lib/std
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/drivers/include
    ${CMAKE_SOURCE_DIR}/sys/include
    ${CMAKE_SOURCE_DIR}/lib/embedded_cli
    ${CMAKE_SOURCE_DIR}/src
)

# Convert include dirs to -I flags
set(INCLUDE_FLAGS "")
foreach(DIR ${INCLUDE_DIRS})
    set(INCLUDE_FLAGS "${INCLUDE_FLAGS} -I${DIR}")
endforeach()

# === Sources ===
# Main application sources
file(GLOB SRC_MAIN "${CMAKE_SOURCE_DIR}/src/*.c")
file(GLOB SRC_S "${CMAKE_SOURCE_DIR}/src/*.S")
list(REMOVE_ITEM SRC_S "${CMAKE_SOURCE_DIR}/src/core/crt0.S")
set(CRT0_SRC "${CMAKE_SOURCE_DIR}/src/core/crt0.S")

# System sources (stdio, etc.)
file(GLOB SRC_SYS "${CMAKE_SOURCE_DIR}/sys/src/*.c")

# Driver sources
file(GLOB SRC_DRIVERS "${CMAKE_SOURCE_DIR}/drivers/src/*/*.c")
file(GLOB SRC_DRIVERS_DIRECT "${CMAKE_SOURCE_DIR}/drivers/src/*.c")

# Standard library sources
file(GLOB SRC_STD "${CMAKE_SOURCE_DIR}/lib/std/*.c")

# Embedded CLI sources (submodule)
file(GLOB SRC_EMBEDDED_CLI "${CMAKE_SOURCE_DIR}/lib/embedded_cli/src/*.c")

# Combine all C sources
set(SRC_C ${SRC_MAIN} ${SRC_SYS} ${SRC_DRIVERS} ${SRC_DRIVERS_DIRECT} ${SRC_STD} ${SRC_EMBEDDED_CLI})

# Create object targets for C sources
set(OBJ_C "")
foreach(SRC ${SRC_C})
    get_filename_component(FNAME ${SRC} NAME_WE)
    get_filename_component(DIR ${SRC} DIRECTORY)
    file(RELATIVE_PATH REL_DIR ${CMAKE_SOURCE_DIR} ${DIR})
    string(REPLACE "/" "_" OBJ_NAME "${REL_DIR}_${FNAME}")
    set(OBJ ${BUILD_DIR}/${OBJ_NAME}.o)
    add_custom_command(
        OUTPUT ${OBJ}
        COMMAND ${GCC} -mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -Wextra -ffunction-sections -fdata-sections -nostdlib -nostartfiles ${INCLUDE_FLAGS} -c ${SRC} -o ${OBJ}
        DEPENDS ${SRC}
    )
    list(APPEND OBJ_C ${OBJ})
endforeach()

# Compile other assembly sources
set(OBJ_S "")
foreach(SRC ${SRC_S})
    get_filename_component(FNAME ${SRC} NAME_WE)
    get_filename_component(DIR ${SRC} DIRECTORY)
    file(RELATIVE_PATH REL_DIR ${CMAKE_SOURCE_DIR} ${DIR})
    string(REPLACE "/" "_" OBJ_NAME "${REL_DIR}_${FNAME}")
    set(OBJ ${BUILD_DIR}/${OBJ_NAME}.o)
    add_custom_command(
        OUTPUT ${OBJ}
        COMMAND ${GCC} -mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -ffunction-sections -fdata-sections -nostdlib ${INCLUDE_FLAGS} -c ${SRC} -o ${OBJ}
        DEPENDS ${SRC}
    )
    list(APPEND OBJ_S ${OBJ})
endforeach()

# Compile CRT0 separately
set(CRT0_OBJ "${BUILD_DIR}/crt0.o")
add_custom_command(
    OUTPUT ${CRT0_OBJ}
    COMMAND ${GCC} -mmcu=${MCU} -DF_CPU=${F_CPU} -Os -Wall -ffunction-sections -fdata-sections -nostdlib ${INCLUDE_FLAGS} -c ${CRT0_SRC} -o ${CRT0_OBJ}
    DEPENDS ${CRT0_SRC}
)

# Combine all objects
set(OBJS ${OBJ_C} ${OBJ_S} ${CRT0_OBJ})

# === Linker script ===
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/linker.ld)
add_custom_target(${TARGET_NAME}.elf ALL
    DEPENDS ${OBJS}
)
add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${GCC} -mmcu=${MCU} -nostartfiles ${OBJS} -Wl,-T${LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map=${BUILD_DIR}/${TARGET_NAME}.map -o ${BUILD_DIR}/${TARGET_NAME}.elf
    COMMENT "Linking ELF..."
)

# === Generate .hex file ===
add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${OBJCOPY} -O ihex ${BUILD_DIR}/${TARGET_NAME}.elf ${BUILD_DIR}/${TARGET_NAME}.hex
    COMMENT "Generating HEX file..."
)

# === Generate listing (.lst) ===
if(IS_WINDOWS)
    add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
        COMMAND powershell -Command "& { & '${OBJDUMP}' -d -S '${BUILD_DIR}/${TARGET_NAME}.elf' | Out-File -Encoding ASCII '${BUILD_DIR}/${TARGET_NAME}.lst' }"
        COMMENT "Generating listing file..."
    )
else()
    add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
        COMMAND ${OBJDUMP} -d -S ${BUILD_DIR}/${TARGET_NAME}.elf > ${BUILD_DIR}/${TARGET_NAME}.lst
        COMMENT "Generating listing file..."
    )
endif()

# === Flash target ===
add_custom_target(flash
    COMMAND ${AVRDUDE_PATH} -c arduino -p ${MCU} -P ${PORT} -b ${BAUD} -U flash:w:${BUILD_DIR}/${TARGET_NAME}.hex
    DEPENDS ${TARGET_NAME}.elf
    WORKING_DIRECTORY ${BUILD_DIR}
    COMMENT "Flashing the device..."
)

# === Clean target ===
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUILD_DIR}
    COMMENT "Cleaning build directory..."
)
