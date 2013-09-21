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

volatile uint32_t* pMotorPWMArray[2];

void SetupPWM()
{
	// Enables clock for 16-bit counter/timer 0
	SYSCON_SYSAHBCLKCTRL_CT16B0(SYSAHBCLKCTRL_ENABLE);
	// setup pin 1 for PWM output
	IOCON_PIO0_8_FUNC(PIO0_8_FUNC_TIMER);
	// disable pullup resistor pin 1
	IOCON_PIO0_8_MODE(PIO0_8_MODE_NO_RESISTOR);
	// set timer prescaler
	TMR16B0_PR_Set(SystemFrequency/10000);	// TC = 10Khz
	// set match register 3 to reset TC
	TMR16B0_MCR_MR3R(MCR_ENABLE);
	// set PWM duty cycle
	TMR16B0_MR3_Set(100);	// 100Hz
	// set pulse width
	TMR16B0_MR0_Set(0);	// 0%
	// enable PWM mode on match register 0 (MR0)
	TMR16B0_PWMC_PWMEN0(PWMC_ENABLE);
	// enable counter
	TMR16B0_TCR_CEn(TCR_ENABLE);	
	
	pMotorPWMArray[0] = &LPC_TMR16B0->MR0;
	pMotorPWMArray[1] = &LPC_TMR16B0->MR1;
}


inline void SetMotorSpeed(int index, uint32_t speed)
{
	*(pMotorPWMArray[index]) = speed;  // as a percentage
}

inline int min(int a, int b)
{
	if (a < b) 
		return a; 
	else 
		return b;
}
inline int max(int a, int b)
{
	if (a > b) 
		return a; 
	else 
		return b;
}

volatile int32_t g_motorPos;
volatile unsigned long g_motorTime;

volatile int g_transitionTime;	// time stamp of recent interrupt
volatile bool g_inTransition;	// edge transition occured, waiting for elasped time

volatile int32_t g_motorInc;

// interrupt should only be valid if the previous interrupt occurred 
// a number of system ticks ago.

// TODO:  not sure if the logic is correct.  if the transition was noisy
//	then the condition below is not satisfied until the next noisy transition.

// it's almost like when we have a transition, we need to schedule a timer interrupt
//  that occurs in q number of cycles.  if another edge comes in before the timer
//  then the timer is cleared.  we are looking for both a transition and a delay.
//  it would be simplest to do this without schduling a timer... just use history.

extern "C" void PIOINT1_IRQHandler(void)
{
	unsigned long systickcnt = SysTickCnt;

	if (GPIO1_MIS(0))
	{
		if (g_inTransition)
		{
			unsigned long elaspedTime = g_transitionTime - systickcnt;
			if (elaspedTime > 2 /*ms*/)
			{
				g_motorPos += g_motorInc;
				g_motorTime = systickcnt;
				// we are still in transition, we just need to reset the transition timer
				g_transitionTime = systickcnt;
			}
			else
			{
				g_inTransition = false;
			}
		}
		else
		{
			g_inTransition = true;
			g_transitionTime = systickcnt;
		}
		
		GPIO1_IC(0);	// clear the interrupt flag
	}
	
	__NOP();__NOP();	// required after clearing interrupt flags

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

	GPIO1_DIR(6, GPIO_OUTPUT);
	GPIO1_DATA(6, 0);

	GPIO1_DIR(7, GPIO_OUTPUT);
	GPIO1_DATA(7, 0);
	
	GPIO1_DIR(8, GPIO_OUTPUT);
	GPIO1_DATA(8, 0);

	GPIO1_DIR(9, GPIO_OUTPUT);
	GPIO1_DATA(9, 0);
	
	uint32_t pattern[4];
	pattern[0] = 1;
	pattern[1] = 0;
	pattern[2] = 0;
	pattern[3] = 0;

	while (1)
	{
		for (int i = 0; i < 25; ++i)
		{
			Delay(50);
			
			// shift
			uint32_t tmp = pattern[3];
			pattern[3] = pattern[2];
			pattern[2] = pattern[1];
			pattern[1] = pattern[0];
			pattern[0] = tmp;
			GPIO1_DATA(9, pattern[0]);
			GPIO1_DATA(8, pattern[1]);
			GPIO1_DATA(7, pattern[2]);
			GPIO1_DATA(6, pattern[3]);
			Delay(20);
			GPIO1_DATA(9, 0);
			GPIO1_DATA(8, 0);
			GPIO1_DATA(7, 0);
			GPIO1_DATA(6, 0);
		}

		Delay(60);

		for (int i = 0; i < 25; ++i)
		{
			Delay(50);
			
			// shift
			uint32_t tmp = pattern[0];
			pattern[0] = pattern[1];
			pattern[1] = pattern[2];
			pattern[2] = pattern[3];
			pattern[3] = tmp;
			GPIO1_DATA(9, pattern[0]);
			GPIO1_DATA(8, pattern[1]);
			GPIO1_DATA(7, pattern[2]);
			GPIO1_DATA(6, pattern[3]);
#if 0
			Delay(20);
			GPIO1_DATA(9, 0);
			GPIO1_DATA(8, 0);
			GPIO1_DATA(7, 0);
			GPIO1_DATA(6, 0);
#endif			
		}

		Delay(60);
	}
	
}
