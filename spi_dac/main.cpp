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



extern "C" void SystemInit(void)
{
	// if you initialize clock here beware that the C++ runtime 
	// initialization code runs next...  and writes to globals
	// do not persist.  so the SystemFrequency needs to be set
	// in the main() function, or just initialize the clock there
	// for simplicity.
}


volatile unsigned long SysTickCnt;      // SysTick Counter

// SysTick Interrupt Handler (1ms)
extern "C" void SysTick_Handler(void) 
{           
  SysTickCnt++;
}

void Delay(unsigned long tick)
{
  unsigned long systickcnt;

  systickcnt = SysTickCnt;
	// TODO: handle when SysTickCnt overflows
  while ((SysTickCnt - systickcnt) < tick);
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

	
	
	//
	// Configure SPI 0
	//
	
	// Power on SSP 0
  SYSCON->SYSAHBCLKCTRL.SSP0 = 1;

	// reset the device
	SYSCON->PRESETCTRL.SSP0_RST_N = 0;	// assert reset
	SYSCON->PRESETCTRL.SSP0_RST_N = 1;	// de-assert reset

	// setup clock divisor for SSP0 device
	SYSCON->SSP0CLKDIV = 2;
	
	// pin 1 - PIO0_8, set to MISO
	// pin 2 - PIO0_9, set to MOSI
	// pin 6 - PIO0_6, set to SCK0
	// pin 25 - PIO0_2, set to SSEL0
	
	IOCON->PIO0_8.FUNC = PIO0_8_FUNC_MISO;
	IOCON->PIO0_9.FUNC = PIO0_9_FUNC_MOSI;
	IOCON->SCK_LOC.SCKLOC = SCK0_LOC_PIO0_6;
	IOCON->PIO0_6.FUNC = PIO0_6_FUNC_SCK0;
	IOCON->PIO0_2.FUNC = PIO0_2_FUNC_SSEL;

	// setup data clock rate to 500Khz
	// Serial Clock Rate = PCLK / (CPSDVSR * [SCR+1])
	SSP0CON->CPSR.CPSDVSR = 2;
	SSP0CON->CR0.SCR = 23;

	SSP0CON->CR0.DSS = SSP_CR0_DSS_8BIT;
	SSP0CON->CR0.FRF = SSP_CR0_FRF_SPI;
	SSP0CON->CR0.CPOL = SSP_CR0_CPOL_CLOCK_LOW;
	SSP0CON->CR0.CPHA = SSP_CR0_CPHA_AWAY_FROM;
	
	SSP0CON->CR1.MS = SSP_CR1_MS_MASTER;
	
	// empty out FIFO
	uint32_t data;
	for (int i = 0; i < SPI_FIFOSIZE; ++i)
	{
		data = SSP0CON->DR;
	}

	// enable SPI controller
	SSP0CON->CR1.SSE = 1;

	while (1)
	{
		for (int j = 0; j < 4096; ++j)
		{
			Delay(5);
			SSP0CON->DR = j >> 8;
			SSP0CON->DR = j & 0xFF;
		}
	}
	
}
