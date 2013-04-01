#include <LPC11xx.h>
#include "registers.h"

// configure the PLL to use a multiple of the IRC oscillator 
// crystal = speed of the IRC (normally 12MHz or 12000000ul)
// frequency = requested speed in Hz (normally 48000000ul)
// returns actual frequency attainable
uint32_t ConfigurePLL(uint32_t crystal, uint32_t frequency);

