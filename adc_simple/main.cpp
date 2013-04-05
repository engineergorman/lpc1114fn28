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
		
	// set direction on port 0_7 (pin 28) to output
	GPIO0_DIR(7, GPIO_OUTPUT);	
	GPIO0_DATA(7, 1);  // turn on diagnostic led
	
	// Setup ADC
	// My geared motor has an optical wheel,
	// and sensor with analog output.  this
	// code is just a test to see if the values
	// coming from the ADC are good... and
	// it works!
	
	SYSCON_PDRUNCFG_ADC(POWER_UP);
	SYSCON_SYSAHBCLKCTRL_ADC(SYSAHBCLKCTRL_ENABLE);
	IOCON_PIO1_0_FUNC(PIO1_0_FUNC_ADC);
	IOCON_PIO1_0_ADMODE(PIO1_0_ADMODE_ANALOG_INPUT);
	
	ADC_CR_SEL(1, ADC_ENABLE);	// enable sampling on AD1 (pin 9)
	ADC_CR_CLKDIV(SystemFrequency / 4500000);		// ADC clock must be <= 4.5Mhz
	ADC_CR_START(ADC_START_START_NOW);	// take a sample
	
	// sample the ADC and count the edge transitions
	int revs = 0;
	bool high = false;
	uint32_t ledtoggle = 1;
	while (1)
	{
		revs = 0;
		high = false;
		while (revs < 10)
		{
			_ADC_GDR sample = ADC_GDR_Sample();	// get a sample
			if (sample.DONE)
			{
				int v = sample.V_VREF;
				ADC_CR_START(ADC_START_START_NOW);	// trigger another sampling
				if (high && v < 0x1FF)
				{
					high = false;
					++revs;
				}
				else if (!high && v >= 0x1FF)
				{
					high = true;
					++revs;
				}
			}
		}
		
		// we just counted 10 transitions, lets toggle the led
		ledtoggle = (~ ledtoggle) & 0x1;
		GPIO0_DATA(7, ledtoggle);  // toggle diagnostic led
	}
}
