# avr-toolchain.cmake

# Tell CMake this is a cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# === Toolchain Directory Detection ===

if(WIN32)
    set(AVR_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-win32_x86_64")
    set(CMAKE_C_COMPILER "${AVR_TOOLCHAIN_DIR}/bin/avr-gcc.exe")
    set(AVRDUDE_PATH "${CMAKE_SOURCE_DIR}/../../avrdude-v8.1-windows-x64/avrdude.exe")

elseif(UNIX AND NOT APPLE)
    set(AVR_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-linux_x86_64")
    set(CMAKE_C_COMPILER "${AVR_TOOLCHAIN_DIR}/bin/avr-gcc")
    set(AVRDUDE_PATH "avrdude")  # Assume it's in PATH

elseif(APPLE)
    # Use same toolchain path as in Makefile
    set(AVR_TOOLCHAIN_DIR "${CMAKE_SOURCE_DIR}/../../avr8-gnu-toolchain-darwin_universal")
    set(CMAKE_C_COMPILER "${AVR_TOOLCHAIN_DIR}/bin/avr-gcc")
    set(AVRDUDE_PATH "avrdude")  # Assumes installed via brew or similar

else()
    message(FATAL_ERROR "Unsupported host OS")
endif()

# Set OBJCOPY based on toolchain path
set(OBJCOPY "${AVR_TOOLCHAIN_DIR}/bin/avr-objcopy")

# Avoid checking host compiler capabilities
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
