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
#include "stdlib.h"
#include "stdio.h"


extern "C" void SystemInit(void)
{
	// if you initialize clock here beware that the C++ runtime 
	// initialization code runs next...  and writes to globals
	// do not persist.  so the SystemFrequency needs to be set
	// in the main() function, or just initialize the clock there
	// for simplicity.
}



void SetupPWM()
{
	// Enables clock for 16-bit counter/timer 0
	SYSCON_SYSAHBCLKCTRL_CT16B0(SYSAHBCLKCTRL_ENABLE);
	// setup pin 1 for PWM output
	IOCON_PIO0_8_FUNC(PIO0_8_FUNC_TIMER);
	// disable pullup resistor pin 1
	IOCON_PIO0_8_MODE(PIO0_8_MODE_NO_RESISTOR);
	// set timer prescaler
	TMR16B0_PR_Set(SystemFrequency/2500000);	// TC = 2.5Mhz
	// set match register 3 to reset TC
	TMR16B0_MCR_MR3R(MCR_ENABLE);
	// set PWM duty cycle
	TMR16B0_MR3_Set(100);	// 25KHz (PWM freq for SanAce B97 server fan)
	// set pulse width
	TMR16B0_MR0_Set(0);	// 0%
	// enable PWM mode on match register 0 (MR0)
	TMR16B0_PWMC_PWMEN0(PWMC_ENABLE);
	// enable counter
	TMR16B0_TCR_CEn(TCR_ENABLE);	
}

inline bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);
		
	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);

	SetupPWM();

	// set pulse width
	TMR16B0_MR0_Set(0);
	
	uart_init(UART_BAUDRATE_115200);
	uart_open();
	
	char cmdbuff[3];
	int pos = 0;
	
	char buffer[64];
	char str[] = "fan control: fXX, where XX is a number from 0-99.\r\n";
	char ack[] = "\r\nok.\r\n";
	uart_write_str(str);
	
	while (1)
	{
		int sz = uart_read(buffer, 64);
		uart_write(buffer, sz);

		if (sz > 0)
		{
			for (int i = 0; i < sz; ++i)
			{
				char c  = buffer[i];
				if ( ( c == 'f' && pos == 0)
					|| (isDigit(c) && (pos == 1 || pos == 2)) )
				{
					cmdbuff[pos++] = buffer[i];
				}
				if (c == '\r' || c == '\n')
				{
					if (pos == 3)
					{
						int val = (cmdbuff[1] - '0') * 10 + (cmdbuff[2] - '0');
						TMR16B0_MR0_Set(val);
						uart_write_str(ack);
					}
					pos = 0;
				}
			}
		}		
	}
}
