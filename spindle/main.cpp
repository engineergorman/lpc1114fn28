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

//
// When the LPC1114FN28 boots up, the pins start
// hi, then go lo for a short time, then hi again.
// This glitch could potentially turn both sides of the
// IGBT on at the same time and cause a very bad shoot-through 
// condition.  So we need to take a preventative measures...
// 
// The high side of the IGBT goes through one NAND gate
// and the inverted opto-isolator.  So hi output from the
// microprocessor is hi on the IGBT.  We need to get these
// signals low asap.  I use a cap-resistor analog timer on
// the B side of the NAND gate to keep it low during boot.
//
// The low side of the IGBT goes through 2 NAND gates and
// the inverted opto-isolator.  So a hi output from the 
// microprocessor is a low on the IGBT.  This inverted logic
// is a fail-safe just in case the uProc fails to boot.
// 

// 16B0
// MAT0 - AH  (PIO_0_8, pin 1)
// MAT1 - BH  (PIO_0_9, pin 2)
// MAT2 -
// MAT3 - 
// CAP0 - Hall0 (PIO_0_2, pin 25)

// 16B1
// MAT0 - CH  (PIO_1_9, pin 18)
// MAT1 - 
// MAT2 -
// MAT3 - 
// CAP0 - Hall1 (PIO_1_8, pin 17)

// 32B0
// MAT0 - 
// MAT1 - 
// MAT2 -
// MAT3 - 
// CAP0 - SpeedIn (PIO_1_5, pin 14)

// 32B1
// MAT0 - AL  (PIO_1_1, pin 10)
// MAT1 - BL  (PIO_1_2, pin 11)
// MAT2 -
// MAT3 - CL  (PIO_1_4, pin 13)
// CAP0 - Hall2 (PIO_1_0, pin 9)


extern "C" void SystemInit(void)
{
	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);
	__NOP();
	__NOP();

	// by default the chip's pins are set to inputs with pullup resistors.
	// we need to get the IGBT HI-side pins low before a capacitor-resistor timer
	// crosses the 0/1 threshold enabling driving of the IGBT devices.
	// if we dont do this we can get shoot-through condition and fry the IGBTs.
	LPC_GPIO0->DIR = (1<<8) |  (1<<9);
	LPC_GPIO0->DATA = 0;
	LPC_GPIO1->DIR = (1<<9);
	LPC_GPIO1->DATA = 0;
	
	// The IGBT LO-side pins are inverted logic, so they need to stay hi.
}

#define PWM_FREQ		(16*1000)		// 16 KHz
#define PWM_DUTY_CYCLE	(3000)	// 3000 levels


volatile uint64_t g_cycles;
volatile uint64_t g_curr;
volatile uint64_t g_prev;

#if 0
inline void GetTimer(uint32_t & timer, uint64_t & cycles)
{
	// check the reset interrupt and make sure that
	// g_cycles is consistent with current TC.
	cycles = g_cycles;
	timer = TMR16B0->TC;
	if (TMR16B0->IR.MR3)
	{
		// in the time that we sampled the TC and the interrupt register
		// it could have wrapped around to zero.  so we have to sample it
		// again to be consistent.
		timer = TMR16B0->TC;
		cycles += PWM_DUTY_CYCLE;
		g_cycles = cycles;
		TMR16B0->IR.MR3 = 0;
	}
}
#endif

inline void RecordEvent(uint32_t minor)
{
	// check the reset interrupt and make sure that
	// g_cycles is consistent with current TC.
	uint64_t cycles = g_cycles;
	uint32_t timer = TMR16B0->TC;
	if (TMR16B0->IR.MR3)
	{
		// in the time that we sampled the TC and the interrupt register
		// it could have wrapped around to zero.  so we have to sample it
		// again to be consistent.
		timer = TMR16B0->TC;
		cycles += PWM_DUTY_CYCLE;
		g_cycles = cycles;
		TMR16B0->IR.MR3 = 0;
	}
	
	g_prev = g_curr;
	if (timer < minor)
	{
		g_curr = cycles + ((int32_t)minor - (int32_t)PWM_DUTY_CYCLE);
	}
	else
	{
		g_curr = cycles + minor;
	}
}

extern "C" void TIMER16_0_IRQHandler(void)
{
	__disable_irq();
	if (TMR16B0->IR.CR0)
	{
		// disable capture
		LPC_TMR16B0->CCR = 0;		// disable rising & falling edge and interrupt.  
														// we use this instead of bit fields because it uses fewer cycles.
		RecordEvent(TMR16B0->CR0);
		LPC_TMR16B0->CCR = 0x7;		// enable rising & falling edge and interrupt.  
															// we use this instead of bit fields because it uses fewer cycles.
	}

	if (TMR16B0->IR.MR3)
	{
		g_cycles += PWM_DUTY_CYCLE;
	}
	LPC_TMR16B0->IR = 0x1F;		// clear all interrupts
	__NOP();__NOP();	// voodoo... not sure if this is required
	__enable_irq();
}

extern "C" void TIMER16_1_IRQHandler(void)
{
	__disable_irq();
	if (TMR16B1->IR.CR0)
	{
		// disable capture
		LPC_TMR16B1->CCR = 0;		// disable rising & falling edge and interrupt.  
														// we use this instead of bit fields because it uses fewer cycles.
		RecordEvent(TMR16B0->CR0);
		LPC_TMR16B1->CCR = 0x7;		// enable rising & falling edge and interrupt.  
															// we use this instead of bit fields because it uses fewer cycles.
	}

	LPC_TMR16B1->IR = 0x1F;		// clear all interrupts
	__NOP();__NOP();	// voodoo... not sure if this is required
	__enable_irq();
}

extern "C" void TIMER32_0_IRQHandler(void)
{
	__disable_irq();
	if (TMR32B0->IR.CR0)
	{
		// disable capture
		LPC_TMR32B0->CCR = 0;		// disable rising & falling edge and interrupt.  
														// we use this instead of bit fields because it uses fewer cycles.
		LPC_TMR32B0->CCR = 0x7;		// enable rising & falling edge and interrupt.  
															// we use this instead of bit fields because it uses fewer cycles.
	}

	LPC_TMR32B0->IR = 0x1F;		// clear all interrupts
	__NOP();__NOP();	// voodoo... not sure if this is required
	__enable_irq();
}

extern "C" void TIMER32_1_IRQHandler(void)
{
	__disable_irq();
	if (TMR32B1->IR.CR0)
	{
		// disable capture
		LPC_TMR32B1->CCR = 0;		// disable rising & falling edge and interrupt.  
														// we use this instead of bit fields because it uses fewer cycles.
		uint32_t minor = TMR32B1->CR0;
		RecordEvent(TMR32B1->CR0);
		LPC_TMR32B1->CCR = 0x7;		// enable rising & falling edge and interrupt.  
															// we use this instead of bit fields because it uses fewer cycles.
	}

	LPC_TMR32B1->IR = 0x1F;		// clear all interrupts
	__NOP();__NOP();	// voodoo... not sure if this is required
	__enable_irq();
}

inline void GetHallSensorEvent(uint64_t & curr, uint64_t & prev)
{
	// try to do an atomic read of the two values
	uint64_t tmp = g_prev;
	curr = g_curr;
	prev = g_prev;
	if (prev != tmp)
	{
		// interrupt handler was called between reading g_curr and g_prev
		curr = g_curr;
	}
}

inline void AH(uint32_t duty)
{
	TMR16B0->MR0 = PWM_DUTY_CYCLE - duty;
}
inline void BH(uint32_t duty)
{
	TMR16B0->MR1 = PWM_DUTY_CYCLE - duty;
}
inline void CH(uint32_t duty)
{
	TMR16B1->MR0 = PWM_DUTY_CYCLE - duty;
}
inline void AL(uint32_t duty)
{
	TMR32B1->MR0 = duty;
}
inline void BL(uint32_t duty)
{
	TMR32B1->MR1 = duty;
}
inline void CL(uint32_t duty)
{
	TMR32B1->MR3 = duty;
}

void SetupPWM16B0()
{
	// MAT0 - AH  (PIO_0_8, pin 1)
	// MAT1 - BH  (PIO_0_9, pin 2)
	// MAT2 -
	// MAT3 - 
	// CAP0 - Hall0 (PIO_0_2, pin 25)
	
	// Enables clock for counter/timer
	SYSCON_SYSAHBCLKCTRL_CT16B0(SYSAHBCLKCTRL_ENABLE);

	// set timer prescaler
	TMR16B0->PR = 0;	// TC incremented on every PCLK
	// set match register 3 to reset TC
	TMR16B0->MCR.MR3R = MCR_ENABLE;
	// set match register 3 to generate interrupt
	TMR16B0->MCR.MR3I = MCR_ENABLE;
	// set PWM duty cycle
	TMR16B0->MR3 = PWM_DUTY_CYCLE - 1;	// TC will take on values from 0.. DUTY_CYCLE-1
	// set pulse width
	AH(0);	// turn off
	BH(0);	// turn off
	
	// enable PWM mode on match registers 0,1 (MR0, MR1)
	TMR16B0->PWMC.PWMEN0 = PWMC_ENABLE;
	TMR16B0->PWMC.PWMEN1 = PWMC_ENABLE;

	// setup capture control register
	TMR16B0->CCR.RISING_EDGE = CCR_ENABLE;
	TMR16B0->CCR.FALLING_EDGE = CCR_ENABLE;
	TMR16B0->CCR.INTERRUPT = CCR_ENABLE;

	// setup pin 25 for timer capture input
	IOCON->PIO0_2.FUNC = PIO0_2_FUNC_TIMER;
	IOCON->PIO0_2.HYS = HYS_ENABLE;

	// setup pins 1,2 for PWM output
	IOCON->PIO0_8.FUNC = PIO0_8_FUNC_TIMER;
	IOCON->PIO0_9.FUNC = PIO0_9_FUNC_TIMER;

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

	// Enables clock for counter/timer
	SYSCON_SYSAHBCLKCTRL_CT16B1(SYSAHBCLKCTRL_ENABLE);

	// set timer prescaler
	TMR16B1->PR = 0;
	// set match register 3 to reset TC
	TMR16B1->MCR.MR3R = MCR_ENABLE;
	// set PWM duty cycle
	TMR16B1->MR3 = PWM_DUTY_CYCLE - 1;	// TC will take on values from 0.. DUTY_CYCLE-1
	// set pulse width
	CH(0);
	// enable PWM mode on match register 0 (MR0)
	TMR16B1->PWMC.PWMEN0 = PWMC_ENABLE;

	// setup capture control register
	TMR16B1->CCR.RISING_EDGE = CCR_ENABLE;
	TMR16B1->CCR.FALLING_EDGE = CCR_ENABLE;
	TMR16B1->CCR.INTERRUPT = CCR_ENABLE;

	// setup pin 17 for timer capture input
	IOCON->PIO1_8.FUNC = PIO1_8_FUNC_TIMER;
	IOCON->PIO1_8.HYS = HYS_ENABLE;

	// setup pin 18 for PWM output
	IOCON->PIO1_9.FUNC = PIO1_9_FUNC_TIMER;

	// Enable interrupt
	NVIC_EnableIRQ(TIMER_16_1_IRQn);
}

void SetupPWM32B0()
{
	// MAT0 - 
	// MAT1 - 
	// MAT2 -
	// MAT3 - 
	// CAP0 - SpeedIn (PIO_1_5, pin 14)

	// Enables clock for counter/timer
	SYSCON_SYSAHBCLKCTRL_CT32B0(SYSAHBCLKCTRL_ENABLE);

	// set timer prescaler
	TMR32B0->PR = 0;	// TC incremented on every PCLK
	// set match register 3 to reset TC
	TMR32B0->MCR.MR3R = MCR_ENABLE;
	// set match register 3 to generate interrupt
	TMR32B0->MCR.MR3I = MCR_ENABLE;
	// set PWM duty cycle
	TMR32B0->MR3 = 0xFFFFFFFF - 1;	// TC will take on values from 0.. 0xFFFFFFFF-1

	// setup capture control register
	TMR32B0->CCR.RISING_EDGE = CCR_ENABLE;
	TMR32B0->CCR.FALLING_EDGE = CCR_ENABLE;
	TMR32B0->CCR.INTERRUPT = CCR_ENABLE;

	// setup pin 14 for timer capture input
	IOCON->PIO1_5.FUNC = PIO1_5_FUNC_TIMER;
	IOCON->PIO1_5.HYS = HYS_ENABLE;

	// Enable interrupt
	NVIC_EnableIRQ(TIMER_32_0_IRQn);
}

void SetupPWM32B1()
{
	// MAT0 - AL  (PIO_1_1, pin 10)
	// MAT1 - BL  (PIO_1_2, pin 11)
	// MAT2 -
	// MAT3 - CL  (PIO_1_4, pin 13)
	// CAP0 - Hall2 (PIO_1_0, pin 9)
	
	// Enables clock for counter/timer
	SYSCON_SYSAHBCLKCTRL_CT32B1(SYSAHBCLKCTRL_ENABLE);

	// set timer prescaler
	TMR32B1->PR = 0;
	// set match register 2 to reset TC
	TMR32B1->MCR.MR2R = MCR_ENABLE;
	// set PWM duty cycle
	TMR32B1->MR2 = PWM_DUTY_CYCLE - 1;	// TC will take on values from 0.. DUTY_CYCLE-1
	// set pulse width
	AL(0);
	BL(0);
	CL(0);
	// enable PWM mode on match register 0,1,3 (MR0,1,3)
	TMR32B1->PWMC.PWMEN0 = PWMC_ENABLE;
	TMR32B1->PWMC.PWMEN1 = PWMC_ENABLE;
	TMR32B1->PWMC.PWMEN3 = PWMC_ENABLE;

	// setup capture control register
	TMR32B1->CCR.RISING_EDGE = CCR_ENABLE;
	TMR32B1->CCR.FALLING_EDGE = CCR_ENABLE;
	TMR32B1->CCR.INTERRUPT = CCR_ENABLE;

	// setup pin 19 for timer capture input
	IOCON->PIO1_0.FUNC = PIO1_0_FUNC_TIMER;
	IOCON->PIO1_0.HYS = HYS_ENABLE;

	// setup pins 10, 11, 13 for PWM output
	IOCON->PIO1_1.FUNC = PIO1_1_FUNC_TIMER;
	IOCON->PIO1_2.FUNC = PIO1_2_FUNC_TIMER;
	IOCON->PIO1_4.FUNC = PIO1_4_FUNC_TIMER;
	
	// Enable interrupt
	NVIC_EnableIRQ(TIMER_32_1_IRQn);
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

struct Phase
{
	uint8_t ah : 1;
	uint8_t bh : 1;
	uint8_t ch : 1;

	uint8_t al : 1;
	uint8_t bl : 1;
	uint8_t cl : 1;
	
	void Engage(uint32_t amount)
	{
		// TODO:  need a way to reliably set the MR outputs now and not wait until end of PWM cycle for it to happen
		
		if (ah)
			AH(amount);
		else
			AH(0);
			
		if (bh)
			BH(amount);
		else
			BH(0);
			
		if (ch)
			CH(amount);
		else
			CH(0);

		if (al)
			AL(PWM_DUTY_CYCLE);
		else
			AL(0);
			
		if (bl)
			BL(PWM_DUTY_CYCLE);
		else
			BL(0);
			
		if (cl)
			CL(PWM_DUTY_CYCLE);
		else
			CL(0);
	}
};

// the hall sensor inputs form a 3-bit number that is used as an
// index into this table for commutating forward.
Phase g_phases[] = {
	{ 0,0,0, 0,0,0 },
	{ 1,0,0, 0,0,1 },
	{ 0,1,0, 1,0,0 },
	{ 0,1,0, 0,0,1 },
	{ 0,0,1, 0,1,0 },
	{ 1,0,0, 0,1,0 },
	{ 0,0,1, 1,0,0 },
	{ 0,0,0, 0,0,0 },
};

struct Hall
{
	uint8_t h0 : 1;
	uint8_t h1 : 1;
	uint8_t h2 : 1;
	
	inline uint8_t ToIndex()
	{
		return *(uint8_t*)this;
	}
};

Hall g_hallSequence[] = {
	{ 1,0,1 },
	{ 1,0,0 },
	{ 1,1,0 },
	{ 0,1,0 },
	{ 0,1,1 },
	{ 0,0,1 }
};


int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);
	

	uart_init(UART_BAUDRATE_115200);
	uart_open();
	uart_write_str("CNC spindle control.\r\n");

	g_cycles = 0;
	g_curr = 0;
	g_prev = 0;

	for (int j = 0; j < 200000; ++j)  __NOP();
	
	SetupPWM16B0();
	SetupPWM16B1();
	SetupPWM32B0();
	SetupPWM32B1();
	EnableTimers();
	
	uint64_t curr;
	uint64_t prev;

	uint64_t last_curr = 0;
	uint64_t last_prev = 0;
	char buff[64];
	sprintf(buff, ".");

	Hall sensorIn;
#if 1
	while (1)
	{
		GetHallSensorEvent(curr, prev);
		if (curr != last_curr)
		{
			sensorIn.h0 = GPIO0_DATA(2) >> 2;
			sensorIn.h1 = GPIO1_DATA(8) >> 8;
			sensorIn.h2 = GPIO1_DATA(0);
			int idx = sensorIn.ToIndex();
			Phase & phase = g_phases[idx];
			phase.Engage(1000);

// 			int64_t delta = (int64_t)curr - (int64_t)prev;
// 			sprintf(buff, "%lld   %d %d %d  %d\r\n", delta, sensorIn.h0, sensorIn.h1, sensorIn.h2, idx );
// 			uart_write_str(buff);
 			//uart_write_str(buff);

			
			last_curr = curr;
			last_prev = prev;
		}
	}

#else

	while (1)
	{
		for (int i = 0; i < 6; ++i)
		{
			for (int j = 0; j < 200000; ++j)  __NOP();
			sensorIn = g_hallSequence[i];
			int idx = sensorIn.ToIndex();
			Phase & phase = g_phases[idx];
			phase.Engage(500);
		}
	}
#endif	
}
