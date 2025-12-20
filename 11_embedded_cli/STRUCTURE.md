# Project Structure Redesign

This document describes the redesigned directory structure for the ATmega328p baremetal project.

## Directory Structure

```
11_embedded_cli/
├── src/                          # Application source code
│   ├── core/
│   │   └── crt0.S               # Startup code (crt0)
│   └── main.c                    # Main application entry point
│
├── lib/                          # Library code
│   ├── avr/
│   │   └── io.h                  # AVR I/O register definitions
│   ├── std/                      # Custom standard library headers (no stdlib dependency)
│   │   ├── stdbool.h             # Boolean type definitions
│   │   ├── stdint.h              # Integer type definitions
│   │   ├── stdarg.h              # Variable argument support
│   │   ├── stddef.h              # Standard definitions (NULL, size_t, etc.)
│   │   ├── stdlib.h              # Memory allocation (malloc, free, etc.)
│   │   ├── string.h              # String manipulation functions
│   │   ├── pgmspace.h            # Program memory access macros
│   │   ├── stdlib.c              # Implementation of stdlib functions
│   │   └── string.c              # Implementation of string functions
│   ├── config.h                  # Project configuration (F_CPU, etc.)
│   └── embedded_cli/             # Embedded CLI submodule location
│       ├── embedded_cli.h        # Embedded CLI header
│       └── src/
│           └── embedded_cli.c    # Embedded CLI implementation
│
├── drivers/                      # Hardware driver layer
│   ├── include/
│   │   ├── uart.h                # UART driver interface
│   │   └── output.h              # Common output interface for printf redirection
│   └── src/
│       ├── uart/
│       │   └── uart.c            # UART driver implementation
│       ├── vga/                  # Future: VGA driver
│       │   └── vga.c
│       └── output.c              # Output interface implementation
│
├── sys/                          # System-level code
│   ├── include/
│   │   └── stdio.h               # printf interface
│   └── src/
│       └── stdio.c               # printf implementation (redirectable)
│
├── linker.ld                     # Linker script
├── CMakeLists.txt                # CMake build configuration
├── Makefile                      # Makefile build configuration
└── avr-toolchain.cmake           # AVR toolchain configuration
```

## Key Design Decisions

### 1. No Standard Library Dependencies
- All standard library headers are custom implementations in `lib/std/`
- No dependency on system `<stdbool.h>`, `<stdint.h>`, etc.
- Custom implementations of `malloc`, `free`, `string` functions, etc.

### 2. Printf Redirection Support
- `printf` uses a function pointer (`output_putc_t`) to output characters
- Output can be redirected by calling `output_set_putc(uart_putc)` or `output_set_putc(vga_putc)`
- Common interface defined in `drivers/include/output.h`
- Implementation in `drivers/src/output.c`

### 3. Driver Organization
- UART driver in `drivers/src/uart/uart.c`
- Future VGA driver can be added in `drivers/src/vga/vga.c`
- Both can be used with the same printf interface

### 4. Embedded CLI Location
- Embedded CLI submodule should be placed in `lib/embedded_cli/`
- Header: `lib/embedded_cli/embedded_cli.h`
- Source: `lib/embedded_cli/src/embedded_cli.c`
- Note: The embedded_cli code may need to be updated to use custom std headers instead of system headers

## Usage Example

```c
#include "stdio.h"
#include "uart.h"
#include "output.h"

int main(void) {
    // Initialize UART
    uart_init();
    
    // Redirect printf to UART
    output_set_putc(uart_putc);
    
    // Now printf will output to UART
    printf("Hello, World!\n");
    
    // Later, can redirect to VGA:
    // output_set_putc(vga_putc);
    // printf("This goes to VGA\n");
    
    return 0;
}
```

## Build System

The CMakeLists.txt has been updated to:
- Include all source files from the new directory structure
- Set up proper include paths for all directories
- Compile custom standard library implementations
- Support the embedded_cli submodule

## Migration Notes

1. **Include Paths**: All includes now use relative paths (e.g., `"std/stdint.h"` instead of `<stdint.h>`)
2. **Printf**: Must call `output_set_putc()` before using `printf()`
3. **Embedded CLI**: May need to update its includes to use custom std headers

