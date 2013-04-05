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


volatile int32_t g_motorPos[2];
volatile int32_t g_motorInc[2];

extern "C" void PIOINT1_IRQHandler(void)
{
	// TODO: noise filtering
	if (GPIO1_MIS(0))
	{
		g_motorPos[0] += g_motorInc[0];	
		GPIO1_IC(0);
		__NOP();__NOP();
	}
	if (GPIO1_MIS(1))
	{
		g_motorPos[1] += g_motorInc[1];	
		GPIO1_IC(1);
		__NOP();__NOP();
	}

}

int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);

	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);
		
	// set direction on port 0_7 (pin 28) to output
	GPIO0_DIR(7, GPIO_OUTPUT);	
	GPIO0_DATA(7, 1);  // turn on diagnostic led

	for (int i = 0; i < 2; ++i)
	{
		g_motorPos[i] = 0;
		g_motorInc[i] = 1;
	}
	
	// setup interrupts on PIO1_0
	IOCON_PIO1_0_MODE(PIO1_0_MODE_NO_RESISTOR);
	GPIO1_DIR(0, GPIO_INPUT);	
	GPIO1_IS(0, GPIO_EDGE_SENSITIVE);
	GPIO1_IBE(0, GPIO_BOTH_EDGES);
	GPIO1_IE(0, GPIO_INTERRUPT_ENABLE);

	// setup interrupts on PIO1_1
	IOCON_PIO1_1_MODE(PIO1_1_MODE_NO_RESISTOR);
	GPIO1_DIR(1, GPIO_INPUT);	
	GPIO1_IS(1, GPIO_EDGE_SENSITIVE);
	GPIO1_IBE(1, GPIO_BOTH_EDGES);
	GPIO1_IE(1, GPIO_INTERRUPT_ENABLE);

  NVIC_EnableIRQ(EINT1_IRQn);

	while (1)
	{
		__NOP();
	}
}
