# === Default Target ===
default:
	$(MAKE) clean
	$(MAKE) flash
	$(MAKE) clean

# === Configuration ===
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 57600

# === OS Detection ===
UNAME_S := $(shell uname -s)

ifeq ($(OS),Windows_NT)
    HOST_OS := windows
else ifeq ($(UNAME_S),Linux)
    HOST_OS := linux
else ifeq ($(UNAME_S),Darwin)
    HOST_OS := macos
else
    $(error Unsupported OS)
endif

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
    TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-darwin_universal
    CC := $(TOOLCHAIN_DIR)/bin/avr-gcc
    OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy
    AVRDUDE := avrdude
    MKDIR = mkdir -p $1
    RM = rm -f
    RM_FILE = $1
endif

# === Paths & Files ===
TARGET = $(notdir $(CURDIR))
SRC_DIR = src
LIB_DIR = lib

SRC_C := $(wildcard $(SRC_DIR)/*.c)
SRC_S := $(wildcard $(SRC_DIR)/*.S)
SRC := $(SRC_C) $(SRC_S)

INCLUDES = -I$(LIB_DIR)

BUILD_DIR := build
ELF := $(BUILD_DIR)/$(TARGET).elf
HEX := $(BUILD_DIR)/$(TARGET).hex
LINKER_SCRIPT := linker.ld

# === Compilation Flags ===
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -std=gnu11 -Wall -Wextra -ffreestanding -nostdlib -nostartfiles $(INCLUDES)
LDFLAGS = -mmcu=$(MCU) -Wl,-T$(LINKER_SCRIPT) -Wl,--gc-sections

# === Targets ===
all: $(HEX)

$(BUILD_DIR):
	@$(call MKDIR,$@)

$(ELF): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# === Flash ===
flash: $(HEX)
	$(AVRDUDE) -c arduino -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$<

# === Clean ===
clean:
ifeq ($(HOST_OS),windows)
	@if exist $(subst /,\,$(BUILD_DIR)) rmdir /S /Q $(subst /,\,$(BUILD_DIR))
else
	@rm -rf $(BUILD_DIR)
endif

.PHONY: default all flash clean
