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
	// reset timers
	//TMR16B0_TCR_CRst(TCR_RESET);
	
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

int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);

	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);
	
  SysTick_Config(SystemFrequency/1000 - 1); // Generate interrupt each 1 ms
	
	// set direction on port 0_7 (pin 28) to output
	GPIO0_DIR(7, GPIO_OUTPUT);	
	GPIO1_DATA(7, 1);  // turn on diagnostic led

	GPIO1_DIR(5, GPIO_OUTPUT);	// configure port 1_5 (pin 14) for output
	GPIO1_DATA(5, 1); // turn on 1A on H-Bridge (this controls motor direction)

	// PWM output (pin 1) is tied to 1,2EN on H-bridge (SN754410 Quad Half H-bridge)	// 
	SetupPWM();

	// Setup ADC
	// My geared motor has a optical wheel,
	// and sensor with analog output.  this
	// code is just a test to see if the values
	// coming from the ADC are good... and
	// it works!
	
	SYSCON_PDRUNCFG_ADC(POWER_UP);
	SYSCON_SYSAHBCLKCTRL_ADC(SYSAHBCLKCTRL_ENABLE);
	IOCON_PIO1_0_FUNC(PIO1_0_FUNC_ADC);
	//IOCON_PIO1_0_MODE(PIO1_0_MODE_NO_RESISTOR);
	IOCON_PIO1_0_ADMODE(PIO1_0_ADMODE_ANALOG_INPUT);
	
	ADC_CR_SEL(1, ADC_ENABLE);	// enable sampling on AD1 (pin 9)
	ADC_CR_CLKDIV(SystemFrequency / 4500000);		// ADC clock must be <= 4.5Mhz
	ADC_CR_START(ADC_START_START_NOW);	// take a sample
	
	// sample the ADC and count the edge transitions
	// when 50 edges are detected, reverse the direction
	// on the motor.
	int revs = 0;
	bool high = false;
	bool dir = false;
	while (1)
	{
		revs = 0;
		high = false;
		while (revs < 50)
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
		dir = !dir;
		if (dir)
		{
			TMR16B0_MR0_Set(100);	// turn off motor
			Delay(700);	// wait for gears to stop spinning
			GPIO1_DATA(5, 0);	// change direction
			TMR16B0_MR0_Set(0);	// turn on motor
		}
		else
		{
			TMR16B0_MR0_Set(100);
			Delay(700);
			GPIO1_DATA(5, 1);
			TMR16B0_MR0_Set(0);
		}
	}
	
#if 0
	// old PWM test
	int fade = 25;
	int hold = 1000;
	int maxamt = 100;
	int minamt = 25;
	
	while (1)
	{
		int j;
		for (j = maxamt; j >= minamt; --j)
		{
			Delay(fade);
			// set pulse width (this controls speed of motor)
			TMR16B0_MR0_Set(j);
		}

		Delay(hold);	

		for (j = minamt; j <= maxamt; ++j)
		{
			Delay(fade);
			// set pulse width
			TMR16B0_MR0_Set(j);
		}

		Delay(hold);	
	}
#endif	
}
