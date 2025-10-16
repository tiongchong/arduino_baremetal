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
ifeq ($(OS),Windows_NT)
    # Windows
    HOST_OS := windows
    PORT := COM19
    TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-win32_x86_64
    CC := $(TOOLCHAIN_DIR)/bin/avr-gcc.exe
    OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy.exe
    AVRDUDE := ../../avrdude-v8.1-windows-x64/avrdude.exe
    MKDIR = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
    RM = del /Q
    RM_FILE = $(subst /,\,$1)
else
    # Linux
    HOST_OS := linux
    PORT := /dev/ttyUSB0
    TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-linux_x86_64
    CC := $(TOOLCHAIN_DIR)/bin/avr-gcc
    OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy
    AVRDUDE := avrdude
    MKDIR = mkdir -p $1
    RM = rm -f
    RM_FILE = $1
endif

# === Compilation Flags ===
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os

# === File Targets ===
TARGET = $(notdir $(CURDIR))
SRC := $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard *.c)
BUILD_DIR := build
ELF := $(BUILD_DIR)/$(TARGET).elf
HEX := $(BUILD_DIR)/$(TARGET).hex

# === Default Target ===
all: $(HEX)

# === Build Rules ===
$(BUILD_DIR):
	@$(call MKDIR,$@)

$(ELF): $(SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

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
