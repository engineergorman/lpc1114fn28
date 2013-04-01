#include <LPC11xx.h>
#include "../core/core.h"

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

int main(void)
{
	// Set system frequency to 48MHz
	SystemFrequency = ConfigurePLL(12000000UL, 48000000UL);

	// Enable clock to IO Configuration block.  Needed for UART, SPI, I2C, etc...
	SYSCON_SYSAHBCLKCTRL_IOCON(SYSAHBCLKCTRL_ENABLE);
	
  SysTick_Config(SystemFrequency/1000 - 1); // Generate interrupt each 1 ms
	
	// set direction on port 0_7 (pin 28) to output
	GPIO0_DIR(7, GPIO_OUTPUT);
	
	while (1)
	{
		int j;
		for (j = 0; j < 10; ++j)
		{
			Delay(50);
			
			GPIO0_DATA(7, 1); // turn on LED on pin 28

			Delay(50);

			GPIO0_DATA(7, 0);		// turn off LED
		}

		Delay(1000);	
	}
}
