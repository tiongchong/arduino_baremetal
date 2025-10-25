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

# === Compilation Flags ===
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os
# For listing
ASMFLAGS = -Wa,-adhln=$(BUILD_DIR)/$(TARGET).lst
# For map file
LDFLAGS = -Wl,-Map=$(BUILD_DIR)/$(TARGET).map

# === File Targets ===
TARGET = $(notdir $(CURDIR))
SRC := $(wildcard src/**/*.c) $(wildcard src/*.c) $(wildcard *.c)
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
