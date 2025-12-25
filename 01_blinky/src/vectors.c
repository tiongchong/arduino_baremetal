#include <stdint.h>

/* Forward declaration of reset handler */
void reset_handler(void);

/* AVR vector table symbol */
__attribute__((section(".vectors")))
void (*const __vectors[])(void) = {
    reset_handler
};
