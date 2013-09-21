#include <LPC11xx.h>
#include "registers.h"

#ifndef NO_EXTERN_SYSTEM_FREQUENCY
extern uint32_t SystemFrequency;
#endif

// configure the PLL to use a multiple of the IRC oscillator 
// crystal = speed of the IRC (normally 12MHz or 12000000ul)
// frequency = requested speed in Hz (normally 48000000ul)
// returns actual frequency attainable
uint32_t ConfigurePLL(uint32_t crystal, uint32_t frequency);

#define UART_BAUDRATE_115200 		115200
#define UART_BAUDRATE_9600   		9600
void uart_init (unsigned int baudrate);
void uart_open();
void uart_close();
int uart_read(void* buffer, int size);
void uart_write(const void* buffer, int size);
void uart_write_str(const void* buffer);
