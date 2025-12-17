# === Default Target ===
default:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) flash
	
# === Configuration ===
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 57600
TARGET = $(notdir $(CURDIR))
	
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
	OBJDUMP := $(TOOLCHAIN_DIR)/bin/avr-objdump.exe
	AVRDUDE := ../../avrdude-v8.1-windows-x64/avrdude.exe
	MKDIR = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
	RM = del /Q
	RM_FILE = $(subst /,\,$1)
	
else ifeq ($(HOST_OS),linux)
	PORT := /dev/ttyUSB0
	TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-linux_x86_64
	CC := $(TOOLCHAIN_DIR)/bin/avr-gcc
	OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy
	OBJDUMP := $(TOOLCHAIN_DIR)/bin/avr-objdump
	AVRDUDE := avrdude
	MKDIR = mkdir -p $1
	RM = rm -f
	RM_FILE = $1
	
else ifeq ($(HOST_OS),macos)
	PORT := /dev/tty.usbserial-1420
	TOOLCHAIN_DIR := ../../avr8-gnu-toolchain-darwin_universal
	CC := $(TOOLCHAIN_DIR)/bin/avr-gcc
	OBJCOPY := $(TOOLCHAIN_DIR)/bin/avr-objcopy
	OBJDUMP := $(TOOLCHAIN_DIR)/bin/avr-objdump
	AVRDUDE := avrdude
	MKDIR = mkdir -p $1
	RM = rm -f
	RM_FILE = $1
endif
	
# === Paths & Files ===
SRC_DIR := src
LIB_DIR := lib/avr
BUILD_DIR := build
LINKER_SCRIPT := linker.ld
	
SRC_C := $(wildcard $(SRC_DIR)/*.c)
SRC_S := $(wildcard $(SRC_DIR)/*.S)
CRT0 := $(BUILD_DIR)/crt0.o
CRT0_SRC := $(SRC_DIR)/crt0.S
	
OBJ_C := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_C))
OBJ_S := $(patsubst $(SRC_DIR)/%.S,$(BUILD_DIR)/%.o,$(SRC_S))
OBJS := $(OBJ_C) $(OBJ_S)
	
ELF := $(BUILD_DIR)/$(TARGET).elf
HEX := $(BUILD_DIR)/$(TARGET).hex
LST := $(BUILD_DIR)/$(TARGET).lst
MAP := $(BUILD_DIR)/$(TARGET).map
	
# === Compilation Flags ===
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -ffunction-sections -fdata-sections -nostdlib -nostartfiles -I$(LIB_DIR)
ASFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -ffunction-sections -fdata-sections -nostdlib
LDFLAGS = -mmcu=$(MCU) -Wl,-T$(LINKER_SCRIPT) -Wl,--gc-sections -Wl,-Map=$(MAP)
	
# === Build Targets ===
all: $(HEX) $(LST)
	
# Create build directories
$(BUILD_DIR):
	@$(call MKDIR,$@)
	
# Compile crt0.S
$(CRT0): $(CRT0_SRC) | $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@
	
# Compile C sources
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	
# Compile assembly sources
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S | $(BUILD_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@
	
# Link
$(ELF): $(OBJS)
	$(CC) -nostartfiles $(OBJS) $(LDFLAGS) -o $@
	
# Convert to HEX
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@
	
# Create disassembly listing
$(LST): $(ELF)
ifeq ($(HOST_OS),windows)
	@powershell -Command "& { & '$(OBJDUMP)' -d -S '$(ELF)' | Out-File -Encoding ASCII '$(LST)' }"
else
	$(OBJDUMP) -d -S $< > $@
endif
	
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
	@rm -f $(ELF) $(HEX) $(LST) $(MAP)
	
.PHONY: default all flash clean
