#include "stdio.h"
#include "stddef.h"
#include "uart.h"
#include "output.h"

#define EMBEDDED_CLI_IMPL
#include "embedded_cli.h"

// 164 bytes is minimum size for this params on Arduino Nano
#define CLI_BUFFER_SIZE 166
#define CLI_RX_BUFFER_SIZE 16
#define CLI_CMD_BUFFER_SIZE 32
#define CLI_HISTORY_SIZE 32
#define CLI_BINDING_COUNT 3

EmbeddedCli *cli;

CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

void onCommand(EmbeddedCli *embeddedCli, CliCommand *command);

void writeChar(EmbeddedCli *embeddedCli, char c);

void onHello(EmbeddedCli *cli, char *args, void *context);

void onLed(EmbeddedCli *cli, char *args, void *context);

void onAdc(EmbeddedCli *cli, char *args, void *context);


// --- Main program ---
int main(void) {
    uart_init();
    
    // Set printf output to UART
    // Later, this can be changed to vga_putc for VGA output
    output_set_putc(uart_putc);

    EmbeddedCliConfig *config = embeddedCliDefaultConfig();
    config->cliBuffer = cliBuffer;
    config->cliBufferSize = CLI_BUFFER_SIZE;
    config->rxBufferSize = CLI_RX_BUFFER_SIZE;
    config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
    config->historyBufferSize = CLI_HISTORY_SIZE;
    config->maxBindingCount = CLI_BINDING_COUNT;
    cli = embeddedCliNew(config);

    if (cli == NULL) {
        printf("Cli was not created. Check sizes!\n");
        return -1;
    }
    printf("Cli has started. Enter your commands.\n");

 


    // --- Test Cases ---
    printf("Char: %c, String: %s\n", 'A', "Hello World");
    printf("Int: %+05d, UInt: %u\n", -123, 456);
    long big_num = 2147483648L;
    printf("Long: %ld\n", big_num);
    int val = 255;
    printf("Hex: %#x, Octal: %#o, Pointer: %p\n", val, val, &val);

    double pi = 3.14159265;
    double small = 0.0001234;
    printf("Float: %.4f, Sci: %.3e\n", pi, small);

    while (1) { }
    return 0;
}
