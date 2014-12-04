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

#define PWM_DUTY_CYCLE	(1000)	// 1024 levels

#define DUTY_CYCLE_VOLT_DBLR		1024

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
inline void DH(uint32_t duty)
{
	TMR32B1->MR0 = DUTY_CYCLE_VOLT_DBLR - duty;
}

void SetupPWM16B0()
{
	// MAT0 - AH  (PIO_0_8, pin 1)
	// MAT1 - BH  (PIO_0_9, pin 2)
	// MAT2 -
	// MAT3 - 
	// CAP0 - 
	
	// Enables clock for counter/timer
	SYSCON->SYSAHBCLKCTRL.CT16B0 = SYSAHBCLKCTRL_ENABLE;

	// set timer prescaler
	TMR16B0->PR = 0;	// TC incremented on every PCLK
	// set match register 3 to reset TC
	TMR16B0->MCR.MR3R = MCR_ENABLE;
	// set PWM duty cycle
	TMR16B0->MR3 = PWM_DUTY_CYCLE - 1;	// TC will take on values from 0.. DUTY_CYCLE-1
	// set pulse width
	AH(0);	// turn off
	BH(0);	// turn off
	
	// enable PWM mode on match registers 0,1 (MR0, MR1)
	TMR16B0->PWMC.PWMEN0 = PWMC_ENABLE;
	TMR16B0->PWMC.PWMEN1 = PWMC_ENABLE;

	// setup pins 1,2 for PWM output
	IOCON->PIO0_8.FUNC = PIO0_8_FUNC_TIMER;
	IOCON->PIO0_9.FUNC = PIO0_9_FUNC_TIMER;
}

void SetupPWM16B1()
{
	// MAT0 - CL  (PIO_1_9, pin 18)
	// MAT1 - 
	// MAT2 -
	// MAT3 - 
	// CAP0 - 

	// Enables clock for counter/timer
	SYSCON->SYSAHBCLKCTRL.CT16B1 = SYSAHBCLKCTRL_ENABLE;

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

	// setup pin 18 for PWM output
	IOCON->PIO1_9.FUNC = PIO1_9_FUNC_TIMER;
}


void SetupPWM32B1()
{
	// MAT0 - (PIO1_1, pin 10)
	// MAT1 - 
	// MAT2 - 
	// MAT3 - 
	// CAP0 - 

	// Enables clock for counter/timer
	SYSCON->SYSAHBCLKCTRL.CT32B1 = SYSAHBCLKCTRL_ENABLE;

	// set timer prescaler
	TMR32B1->PR = 0;
	// set match register 3 to reset TC
	TMR32B1->MCR.MR3R = MCR_ENABLE;
	// set PWM duty cycle
	TMR32B1->MR3 = DUTY_CYCLE_VOLT_DBLR - 1;	// TC will take on values from 0.. DUTY_CYCLE-1
	// set pulse width
	DH(DUTY_CYCLE_VOLT_DBLR/2);
	// enable PWM mode on match register 0 (MR0)
	TMR32B1->PWMC.PWMEN0 = PWMC_ENABLE;

	// setup pin 18 for PWM output
	IOCON->PIO1_1.FUNC = PIO1_1_FUNC_TIMER;
}

void SetupADC()
{
	SYSCON->PDRUNCFG.ADC = POWER_UP;
	SYSCON->SYSAHBCLKCTRL.ADC = SYSAHBCLKCTRL_ENABLE;
	IOCON->PIO1_0.FUNC = PIO1_0_FUNC_ADC;
	IOCON->PIO1_0.ADMODE = ADMODE_ANALOG_INPUT;
	
	ADC_CR_SEL(1, ADC_ENABLE);	// enable sampling on AD1 (pin 9)
	ADC_CR_CLKDIV(SystemFrequency / 4500000);		// ADC clock must be <= 4.5Mhz
	ADC_CR_START(ADC_START_START_NOW);	// take a sample
}


void EnableTimers()
{
	// from looking at the assembly to enable the counters, 
	//	it takes 4 cycles to enable each clock
	TMR16B0->TC = 0;
	TMR16B1->TC = 4;
	TMR32B1->TC = 8;
	
	// NOTE: don't use bitfields for this because timing is critical
	LPC_TMR16B0->TCR = 1;
	LPC_TMR16B1->TCR = 1;
	LPC_TMR32B1->TCR = 1;

	// all clocks should be in sync at this point
}

struct Color
{
	int red;
	int grn;
	int blu;
};

Color m_colors[] = { 
{1024, 0, 0} , 
{0, 1024, 0} ,
{0, 0, 1024} ,
{1024, 1024, 0} ,
{0, 1024, 1024} ,
{1024, 0, 1024} ,
{1024, 1024, 1024} ,
};

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

	SetupPWM16B0();
	SetupPWM16B1();
	SetupPWM32B1();
	EnableTimers();
#if 1
	AH((PWM_DUTY_CYCLE*100)/1000);
	BH(0);
	CH(0);
	
	while(1)
	{
	}
#endif	
	// PWM through the colors
	int fade = 5;
	int hold = 5000;

	int ci = 0;
	while (1)
	{
		Color ca = m_colors[ci];
		ci = ( ci + 1 ) % 7;
		Color cb = m_colors[ci];
		cb.red -= ca.red;
		cb.grn -= ca.grn;
		cb.blu -= ca.blu;
		
		for (int i = 0; i < 512; ++i)
		{
			int red = ca.red + ((i * cb.red) / 512);
			int grn = ca.grn + ((i * cb.grn) / 512);
			int blu = ca.blu + ((i * cb.blu) / 512);
			
			AH(red * 2 / 4);
			BH(grn * 2 / 4);
			CH(blu * 2 / 4);

			Delay(fade);
		}

		Delay(hold);
	}
}
