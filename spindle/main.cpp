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

#define PWM_FREQ		(16*1000)		// 16 KHz
#define PWM_DUTY_CYCLE	(3000)	// 3000 levels


struct HallSensorTime
{
	uint32_t cycle_count;
	uint32_t subcycle_count;
};

struct HallTimers
{
	uint64_t cycles;

	uint64_t curr;
	uint64_t prev;
};

HallTimers g_hallTimers;

extern "C" void TIMER16_0_IRQHandler(void)
{
	__disable_irq();
	if (TMR16B0->IR.CR0)
	{
		uint32_t timerCount = TMR16B0->TC;
		uint32_t cycleResetPending = TMR16B0->IR.MR3;
		uint64_t major = g_hallTimers.cycles;
		uint32_t minor = TMR16B0->CR0;
		if (timerCount < minor && !cycleResetPending)
		{
			// timer counter has wrapped around.  we need to back up
			// time unless the cycle reset interrupt is still pending.
			major -= PWM_DUTY_CYCLE;
		}
		major += minor;
		g_hallTimers.prev = g_hallTimers.curr;
		g_hallTimers.curr = g_hallTimers.cycles + major;
		
		TMR16B0->IR.CR0 = 1;	// clear the interrupt
	}

	if (TMR16B0->IR.MR3)
	{
		g_hallTimers.cycles += PWM_DUTY_CYCLE;
		TMR16B0->IR.MR3 = 1;	// clear the interrupt
	}
	__NOP();__NOP();
	__enable_irq();
}

extern "C" void TIMER16_1_IRQHandler(void)
{
	__disable_irq();
	if (TMR16B1->IR.CR0)
	{
		uint32_t timerCount = TMR16B0->TC;	// not a typo, we want timer 16_0
		uint32_t cycleResetPending = TMR16B0->IR.MR3;	// not a typo, we want timer 16_0
		uint64_t major = g_hallTimers.cycles;
		uint32_t minor = TMR16B1->CR0;
		if (timerCount < minor && !cycleResetPending)
		{
			// timer counter has wrapped around.  we need to back up
			// time unless the cycle reset interrupt is still pending.
			major -= PWM_DUTY_CYCLE;
		}
		major += minor;
		g_hallTimers.prev = g_hallTimers.curr;
		g_hallTimers.curr = g_hallTimers.cycles + major;
		
		TMR16B1->IR.CR0 = 1;	// clear the interrupt
	}

	__NOP();__NOP();
	__enable_irq();
}

void SetupPWM16B0()
{
	// MAT0 - AL  (PIO_0_8, pin 1)
	// MAT1 - BL  (PIO_0_9, pin 2)
	// MAT2 -
	// MAT3 - 
	// CAP0 - Hall0 (PIO_0_2, pin 25)
	
	// Enables clock for 16-bit counter/timer 0
	SYSCON_SYSAHBCLKCTRL_CT16B0(SYSAHBCLKCTRL_ENABLE);

	// setup pins 1,2 for PWM output
	IOCON_PIO0_8_FUNC(PIO0_8_FUNC_TIMER);
	IOCON_PIO0_9_FUNC(PIO0_9_FUNC_TIMER);

	// disable pullup resistor pins 1,2
	IOCON_PIO0_8_MODE(PIO0_8_MODE_NO_RESISTOR);
	IOCON_PIO0_9_MODE(PIO0_9_MODE_NO_RESISTOR);

	// setup pin 25 for timer capture input
	IOCON_PIO0_2_FUNC(PIO0_2_FUNC_TIMER);

	// set timer prescaler
	TMR16B0->PR = 0;	// TC incremented on every PCLK
	// set match register 3 to reset TC
	TMR16B0->MCR.MR3R = MCR_ENABLE;
	// set match register 3 to generate interrupt
	TMR16B0->MCR.MR3I = MCR_ENABLE;
	// set PWM duty cycle
	TMR16B0->MR3 = PWM_DUTY_CYCLE - 1;	// TC will take on values from 0.. DUTY_CYCLE-1
	// set pulse width
	TMR16B0->MR0 = PWM_DUTY_CYCLE;	// this essentially turns off the pin
	// enable PWM mode on match registers 0,1 (MR0, MR1)
	TMR16B0->PWMC.PWMEN0 = PWMC_ENABLE;
	TMR16B0->PWMC.PWMEN1 = PWMC_ENABLE;

	// setup capture control register
	TMR16B0->CCR.RISING_EDGE = CCR_ENABLE;
	TMR16B0->CCR.FALLING_EDGE = CCR_ENABLE;
	TMR16B0->CCR.INTERRUPT = CCR_ENABLE;
	
	// Enable interrupt
	NVIC_EnableIRQ(TIMER_16_0_IRQn);
}

void SetupPWM16B1()
{
	// MAT0 - CL  (PIO_1_9, pin 18)
	// MAT1 - 
	// MAT2 -
	// MAT3 - 
	// CAP0 - Hall1 (PIO_1_8, pin 17)

	// Enables clock for 16-bit counter/timer 0
	SYSCON_SYSAHBCLKCTRL_CT16B1(SYSAHBCLKCTRL_ENABLE);

	// setup pin 18 for PWM output
	IOCON_PIO1_9_FUNC(PIO1_9_FUNC_TIMER);

	// disable pullup resistor pin 18
	IOCON_PIO1_9_MODE(PIO1_9_MODE_NO_RESISTOR);

	// setup pin 17 for timer capture input
	IOCON_PIO1_8_FUNC(PIO1_8_FUNC_TIMER);

	// set timer prescaler
	TMR16B1->PR = 0;
	// set match register 3 to reset TC
	TMR16B1->MCR.MR3R = MCR_ENABLE;
	// set PWM duty cycle
	TMR16B1->MR3 = PWM_DUTY_CYCLE;
	// set pulse width
	TMR16B1->MR0 = PWM_DUTY_CYCLE;
	// enable PWM mode on match register 0 (MR0)
	TMR16B1->PWMC.PWMEN0 = PWMC_ENABLE;
	// setup capture control register
	TMR16B1->CCR.RISING_EDGE = CCR_ENABLE;
	TMR16B1->CCR.FALLING_EDGE = CCR_ENABLE;
	TMR16B1->CCR.INTERRUPT = CCR_ENABLE;

	// Enable interrupt
	NVIC_EnableIRQ(TIMER_16_1_IRQn);
}

void EnableTimers()
{
	// from looking at the assembly to enable the counters, 
	//	it takes 4 cycles to enable each clock
	TMR16B0->TC = 0;
	TMR16B1->TC = 4;
	TMR32B0->TC = 8;
	TMR32B1->TC = 12;
	
	// NOTE: don't use bitfields for this because timing is critical
	LPC_TMR16B0->TCR = 1;
	LPC_TMR16B1->TCR = 1;
	LPC_TMR32B0->TCR = 1;
	LPC_TMR32B1->TCR = 1;

	// all four clocks should be in sync at this point
}

inline bool isDigit(char c)
{
	return c >= '0' && c <= '9';
}

// EnableDrivers() - if enable is 'true' then we drive the 
// B side of the NAND gates to HI which allows the PWM outputs to 
// to switch on the IGBT drivers.
void EnableDrivers(bool enable)
{
	if (enable)
	{
		GPIO0_DATA(4, 1);
		GPIO0_DATA(5, 1);
	}
	else
	{
		GPIO0_DATA(4, 0);
		GPIO0_DATA(5, 0);
	}
}

int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);
		
	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);

	EnableTimers();
	
	// ports 0_4 (pin 27) and 0_5 (pin 5) default to Input with Open Drain.
	// pins have an external pulldown resistor and connect to the B inputs of the
	// NAND gates.  this turns off all IGBT modules by default.
	IOCON_PIO0_4_FUNC(PIO0_4_FUNC_GPIO);
	IOCON_PIO0_4_I2CMODE(PIO0_4_I2CMODE_GPIO);
	GPIO0_DATA(4, 0);
	GPIO0_DIR(4, GPIO_OUTPUT);		// (pin 27) to NAND for IGBT HI

	IOCON_PIO0_5_FUNC(PIO0_5_FUNC_GPIO);
	IOCON_PIO0_5_I2CMODE(PIO0_5_I2CMODE_GPIO);
	GPIO0_DATA(5, 0);
	GPIO0_DIR(5, GPIO_OUTPUT);		// (pin 5) to NAND for IGBT LO

	uart_init(UART_BAUDRATE_115200);
	uart_open();
	uart_write_str("CNC spindle control.\r\n");

	SetupPWM16B0();
	SetupPWM16B1();
	EnableTimers();
	
	for (int i = 0; i < 100; ++i)
		__NOP();
		
	uint32_t a = TMR16B0->TC;
	uint32_t b = TMR16B1->TC;
	uint32_t c = TMR32B0->TC;
	uint32_t d = TMR32B1->TC;		

	__NOP();
}
