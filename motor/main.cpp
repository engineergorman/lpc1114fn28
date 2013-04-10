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

typedef uint64_t U64;
typedef int64_t I64;
typedef uint32_t U32;
typedef int32_t I32;

#define TRUE 1
#define FALSE 0
typedef int BOOL;

uint32_t SystemFrequency;


extern "C" void SystemInit(void)
{
	// if you initialize clock here beware that the C++ runtime 
	// initialization code runs next...  and writes to globals
	// do not persist.  so the SystemFrequency needs to be set
	// in the main() function, or just initialize the clock there
	// for simplicity.
}


volatile U64 SysTickCnt;      // SysTick Counter

// SysTick Interrupt Handler (1ms)
extern "C" void SysTick_Handler(void) 
{           
  SysTickCnt++;
}


void Delay(U64 tick)
{
  U64 systickcnt;

  systickcnt = SysTickCnt;
	// TODO: handle when SysTickCnt overflows
  while ((SysTickCnt - systickcnt) < tick);
}

volatile uint32_t* g_pMotorPWMArray[2];

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
	
	g_pMotorPWMArray[0] = &LPC_TMR16B0->MR0;
	g_pMotorPWMArray[1] = &LPC_TMR16B0->MR1;
}


inline void MotorPower(int index, uint32_t speed)
{
	*(g_pMotorPWMArray[index]) = 100 - speed;  // as a percentage
}

void MotorDir(int index, int dir)
{
	int a, b;
	if (dir == -1)	// backwards
	{
		a = 0;
		b = 1;
	}
	else if (dir == 0)	// braking
	{
		a = 1;
		b = 1;
	}
	else	// (dir == 1)  forwards
	{
		a = 1;
		b = 0;
	}
	
	GPIO1_DATA(4, a); // turn on 1A on H-Bridge (this controls motor direction)
	GPIO1_DATA(5, b); // turn on 2A on H-Bridge (this controls motor direction)
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
inline int abs(int a)
{
	return (a < 0) ? -a : a;
}

volatile int g_motorPos;
volatile int g_motorStop;
volatile U64 g_motorTime;
volatile int g_motorInc;

extern "C" void PIOINT1_IRQHandler(void)
{
	U64 systickcnt = SysTickCnt;

	if (GPIO1_MIS(0))
	{
		g_motorPos += g_motorInc;
		g_motorTime = systickcnt;
		if (g_motorPos == g_motorStop)
		{
			MotorDir(0, 0);	// brake motor
			MotorPower(0, 100);	// stop the motor
		}
		GPIO1_IC(0);	// clear the interrupt flag
		__NOP();__NOP();	// required after clearing interrupt flags
	}
}

void FlashLED()
{
	for (int i = 0; i < 10; ++i)
	{
		Delay(1000);
		GPIO0_DATA(7, i & 0x1);  // turn on diagnostic led
	}
}

int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);

	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);
	
  SysTick_Config(SystemFrequency/10000 - 1); // Generate interrupt each .1 ms
	
	// set direction on port 0_7 (pin 28) to output
	GPIO0_DIR(7, GPIO_OUTPUT);	
	GPIO0_DATA(7, 1);  // turn on diagnostic led

	GPIO1_DIR(4, GPIO_OUTPUT);	// configure port 1_5 (pin 14) for output
	GPIO1_DIR(5, GPIO_OUTPUT);	// configure port 1_5 (pin 14) for output

	// PWM output (pin 1) is tied to 1,2EN on H-bridge (SN754410 Quad Half H-bridge)	// 
	SetupPWM();

	MotorDir(0, 0);	// turn on braking at 100%
	MotorPower(0, 100);

	// motor details:
	// 120:1 gearbox
	// 8 teeth on motor gear
	// 32 teeth on gear with optical wheel
	// 4 edges per rot of optical wheel
	// 8/32*120*4 = 106.666 edges per rev
	// At 5.5V max speed with just 2 gears,
	// pulse width is avg 9.5 ms.
	//
	// a full stop should be no edge for about 150ms
	// and motor on brake.
	//
	
	g_motorPos = 0;
	g_motorStop = 0;
	g_motorTime = 0;
	g_motorInc = 0;
	
	// setup interrupts on PIO1_0
	IOCON_PIO1_0_MODE(PIO1_0_MODE_PULLDOWN_RESISTOR);
	GPIO1_DIR(0, GPIO_INPUT);	
	GPIO1_IS(0, GPIO_EDGE_SENSITIVE);
	GPIO1_IBE(0, GPIO_BOTH_EDGES);
	GPIO1_IE(0, GPIO_INTERRUPT_ENABLE);

  NVIC_EnableIRQ(EINT1_IRQn);

	U64 time1, time2, oldTime;
	int pos, oldPos;
	oldTime = 0xFFFFFFFFFFFFFFFF;
	U64 systickcnt;
	int desiredMotorInc;		// g_motorInc can only be changed when in a stopped state

	while (1)
	{
		FlashLED();
		desiredMotorInc = g_motorInc = 1;
		g_motorStop = 100;
		MotorDir(0, 1);
		MotorPower(0, 100);

		while (1)
		{
			do {
				time1 = g_motorTime;
				pos = g_motorPos;
				time2 = g_motorTime;
			} while (time1 != time2);

			if (pos > g_motorStop)
			{
				desiredMotorInc = -1;
			}
			else if (pos < g_motorStop)
			{
				desiredMotorInc = 1;
			}

			systickcnt = SysTickCnt;
			if ((systickcnt - time2) > 200 /*ms*/)
			{
				// stopped
				if (pos ==  g_motorStop)
				{
					break;	//achieved goal pos and we are stopped
				}
				
				if (desiredMotorInc != g_motorInc)
				{
					g_motorInc = desiredMotorInc;		// g_motorInc must always be 1 or -1, otherwise we lose track of position
					// change direction and turn power back on
					MotorDir(0, desiredMotorInc);
					MotorPower(0, 20);
				}
			}
		}

		FlashLED();
		desiredMotorInc = g_motorInc = -1;
		g_motorStop = 0;
		MotorDir(0, -1);
		MotorPower(0, 100);

		while (1)
		{
			do {
				time1 = g_motorTime;
				pos = g_motorPos;
				time2 = g_motorTime;
			} while (time1 != time2);

			if (pos > g_motorStop)
			{
				desiredMotorInc = -1;
			}
			else if (pos < g_motorStop)
			{
				desiredMotorInc = 1;
			}

			systickcnt = SysTickCnt;
			if ((systickcnt - time2) > 200 /*ms*/)
			{
				// stopped
				if (pos ==  g_motorStop)
				{
					break;	//achieved goal pos and we are stopped
				}
				
				if (desiredMotorInc != g_motorInc)
				{
					g_motorInc = desiredMotorInc;		// g_motorInc must always be 1 or -1, otherwise we lose track of position
					// change direction and turn power back on
					MotorDir(0, desiredMotorInc);
					MotorPower(0, 20);
				}
			}
		}

	}
}
