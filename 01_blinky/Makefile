# === Toolchain Selection ===
# Options: gcc (default), clang
TOOLCHAIN ?= gcc

# === Default Target ===
default:
	$(MAKE) clean
	$(MAKE) flash

# === Configuration ===
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 57600

# === OS Detection ===
ifeq ($(OS),Windows_NT)
    HOST_OS := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        HOST_OS := linux
    else ifeq ($(UNAME_S),Darwin)
        HOST_OS := macos
    else
        $(error Unsupported OS)
    endif
endif

# === OS-Specific Configuration ===
# === OS-Specific Configuration ===
ifeq ($(HOST_OS),windows)
    PORT := COM19
    TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-win32_x86_64
    CC := $(TOOLCHAIN_DIR)/bin/avr-gcc.exe
    OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy.exe
    AVRDUDE := ../../avrdude-v8.1-windows-x64/avrdude.exe
    MKDIR = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
    RM = del /Q
    RM_FILE = $(subst /,\,$1)

else ifeq ($(HOST_OS),linux)
    PORT := /dev/ttyUSB0
    TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-linux_x86_64
    CC := $(TOOLCHAIN_DIR)/bin/avr-gcc
    OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy
    AVRDUDE := avrdude
    MKDIR = mkdir -p $1
    RM = rm -f
    RM_FILE = $1

else ifeq ($(HOST_OS),macos)
    PORT := /dev/tty.usbserial-1420
    AVRDUDE := avrdude
    MKDIR = mkdir -p $1
    RM = rm -f
    RM_FILE = $1

    # --- GNU AVR toolchain (default) ---
    GCC_TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-darwin_universal

    # --- LLVM AVR toolchain ---
    LLVM_TOOLCHAIN_DIR := ../../llvm-avr/bin

    ifeq ($(TOOLCHAIN),clang)
        CC := $(LLVM_TOOLCHAIN_DIR)/clang
        OBJCOPY := $(LLVM_TOOLCHAIN_DIR)/llvm-objcopy
        LD := $(LLVM_TOOLCHAIN_DIR)/ld.lld
    else
        CC := $(GCC_TOOLCHAIN_DIR)/bin/avr-gcc
        OBJCOPY := $(GCC_TOOLCHAIN_DIR)/bin/avr-objcopy
    endif
endif


# === Source Files ===
SRC_ALL := $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard *.c)

# === Common Compilation Flags ===
COMMON_FLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os
AVR_LIBC_INC := ../../avr8-gnu-toolchain-darwin_universal/avr/include
LDSCRIPT := $(CURDIR)/linker.ld

# === Toolchain-Specific Flags ===
ifeq ($(TOOLCHAIN),clang)
    CC := ../../llvm-avr/bin/clang
    LD := ../../avr8-gnu-toolchain-darwin_universal/bin/avr-gcc
    SRC := $(SRC_ALL)
    ASMFLAGS =
    CFLAGS = $(COMMON_FLAGS) \
             --target=avr \
             -ffreestanding \
             -fno-exceptions \
             -fno-rtti \
             -nostdlib \
             -I $(AVR_LIBC_INC)
    LDFLAGS = \
        -Wl,-T,$(LDSCRIPT) \
        -Wl,-Map=$(BUILD_DIR)/$(TARGET).map \
        -fuse-ld=lld
else
    SRC := $(filter-out %/crt0.c %/vectors.c,$(SRC_ALL))
    ASMFLAGS = -Wa,-adhln=$(BUILD_DIR)/$(TARGET).lst
    CFLAGS = $(COMMON_FLAGS)
    LDFLAGS = -Wl,-Map=$(BUILD_DIR)/$(TARGET).map
endif


# === File Targets ===
TARGET = $(notdir $(CURDIR))

BUILD_DIR := build
ELF := $(BUILD_DIR)/$(TARGET).elf
HEX := $(BUILD_DIR)/$(TARGET).hex
LST := $(BUILD_DIR)/$(TARGET).lst
MAP := $(BUILD_DIR)/$(TARGET).map

# === Default Target ===
all: $(HEX)

# === Build Rules ===
$(BUILD_DIR):
	@$(call MKDIR,$@)

$(ELF): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(ASMFLAGS) -o $@ $^ $(LDFLAGS)

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# === Flash the device ===
flash: $(HEX)
	$(AVRDUDE) -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<

# === Clean up build files ===
clean:
ifeq ($(HOST_OS),windows)
	@if exist $(subst /,\,$(BUILD_DIR)) rmdir /S /Q $(subst /,\,$(BUILD_DIR))
else
	@rm -rf $(BUILD_DIR)
endif

.PHONY: default all flash clean
