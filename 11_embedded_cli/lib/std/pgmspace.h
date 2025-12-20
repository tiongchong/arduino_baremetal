#ifndef PROGMEM_H
#define PROGMEM_H

#include "std/stdint.h"

// Program memory macros for AVR
#define PROGMEM __attribute__((section(".progmem.data")))
#define PSTR(s) ((const char*)(s))

// Low-level program memory read macros
#define __LPM(addr) \
    (__extension__({ \
        uint16_t __addr16 = (uint16_t)(addr); \
        uint8_t __result; \
        __asm__ __volatile__ ( \
            "lpm %0, Z" "\n\t" \
            : "=r" (__result) \
            : "z" (__addr16) \
        ); \
        __result; \
    }))

#define __LPM_word(addr) \
    (__extension__({ \
        uint16_t __addr16 = (uint16_t)(addr); \
        uint16_t __result; \
        __asm__ __volatile__ ( \
            "lpm %A0, Z+" "\n\t" \
            "lpm %B0, Z" "\n\t" \
            : "=r" (__result), "=z" (__addr16) \
            : "1" (__addr16) \
        ); \
        __result; \
    }))

#define __LPM_dword(addr) \
    (__extension__({ \
        uint16_t __addr16 = (uint16_t)(addr); \
        uint32_t __result; \
        __asm__ __volatile__ ( \
            "lpm %A0, Z+" "\n\t" \
            "lpm %B0, Z+" "\n\t" \
            "lpm %C0, Z+" "\n\t" \
            "lpm %D0, Z" "\n\t" \
            : "=r" (__result), "=z" (__addr16) \
            : "1" (__addr16) \
        ); \
        __result; \
    }))

#define pgm_read_byte(addr) __LPM((uint16_t)(addr))
#define pgm_read_word(addr) __LPM_word((uint16_t)(addr))
#define pgm_read_dword(addr) __LPM_dword((uint16_t)(addr))

#endif // PROGMEM_H

