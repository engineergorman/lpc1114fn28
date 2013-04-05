// The engineergorman\lpc1114fn28 framework is licensed under the MIT license:
// 
// Copyright (C) 2013 by Chris Gorman
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include <LPC11xx.h>
#include "../core/core.h"
#include "stdio.h"

uint32_t SystemFrequency;


extern "C" void SystemInit(void)
{
	// if you initialize clock here beware that the C++ runtime 
	// initialization code runs next...  and writes to globals
	// do not persist.  so the SystemFrequency needs to be set
	// in the main() function, or just initialize the clock there
	// for simplicity.
}


int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);

	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);
	
  SysTick_Config(SystemFrequency/1000 - 1); // Generate interrupt each 1 ms
	
	// set direction on port 0_7 (pin 28) to output
	GPIO0_DIR(7, GPIO_OUTPUT);	
	GPIO0_DATA(7, 1);  // turn on diagnostic led
	
	#if 0
	// Work In Progress... not complete
	
	SYSCON_PDRUNCFG_ADC(POWER_UP);
	SYSCON_SYSAHBCLKCTRL_ADC(SYSAHBCLKCTRL_ENABLE);
	IOCON_PIO1_0_FUNC(PIO1_0_FUNC_ADC);
	//IOCON_PIO1_0_MODE(PIO1_0_MODE_NO_RESISTOR);
	IOCON_PIO1_0_ADMODE(PIO1_0_ADMODE_ANALOG_INPUT);
	
	ADC_CR_SEL(1, ADC_ENABLE);	// enable sampling on AD1 (pin 9)
	ADC_CR_CLKDIV(SystemFrequency / 4500000);		// ADC clock must be <= 4.5Mhz
	ADC_CR_BURST(ADC_BURST_ENABLE);	// enable burst mode
	ADC_CR_CLKS(ADC_CLKS_5);		// 4 bits accuracy
	ADC_CR_START(ADC_START_STOP);	// 000 bits in START field is required

	// TODO: setup ADC interrupts and handlers to capture the reoccuring samples
	#endif
}
