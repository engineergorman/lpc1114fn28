#include "core.h"

// configure the PLL to use a multiple of the IRC oscillator 
// crystal = speed of the IRC (normally 12MHz or 12000000ul)
// frequency = requested speed in Hz (normally 48000000ul)
// returns actual frequency attainable
uint32_t ConfigurePLL(uint32_t crystal, uint32_t frequency)
{
	// calculate PLL parameters
	uint32_t m = frequency / crystal;		// M is the PLL multiplier
	uint32_t fcco = m * crystal * 2;		// FCCO is the internal PLL frequency
	frequency = crystal * m;						// compute actual frequency

	uint32_t p = 0;
	while (fcco < 156000000)
	{
		fcco *= 2;
		p++;		// find P which gives FCCO in the allowed range (over 156MHz)
	}

	// configure flash access time. allows the chip to run at higher speeds.
	uint32_t val;
	if (frequency < 20000000ul)
		val = FLASHTIM_1CLK;
	else if (frequency < 40000000ul)
		val = FLASHTIM_2CLK;
	else
		val = FLASHTIM_3CLK;
	FLASHCTRL_FLASHCFG_FLASHTIM(val);

	if (crystal < 17500000)					// divide the ranges on 17.5MHz then
		val = FREQRANGE_1_20_RANGE; 	// "lower speed" crystals
	else
		val = FREQRANGE_15_25_RANGE;	// "higher speed" crystals
	SYSCON_SYSOSCCTRL_FREQRANGE(val);

	// power-up main oscillator
	SYSCON_PDRUNCFG_SYSOSC(POWER_UP);
	int i;	// short delay
  for (i = 0; i < 200; i++) __NOP();

	// select main oscillator as the input clock for PLL
	SYSCON_SYSPLLCLKSEL(SYSPLLCLKSEL_IRC_OSCILLATOR);
	// confirm the change of PLL input clock by toggling the ENA bit in LPC_SYSCON->SYSPLLCLKUEN register
	SYSCON_SYSPLLCLKUEN_TOGGLE();
	// configure PLL
	SYSCON_SYSPLLCTRL(m - 1, p);
	// power-up PLL
	SYSCON_PDRUNCFG_SYSPLL(POWER_UP);
	// wait for PLL lock
	WAIT_SYSCON_SYSPLLSTAT_LOCK();
	// select PLL output as the main clock
	SYSCON_MAINCLKSEL(MAINCLKSEL_SYS_PLL_CLOCK_OUT);
	// confirm the change of main clock by toggling the ENA bit in LPC_SYSCON->MAINCLKUEN register
	SYSCON_MAINCLKUEN_TOGGLE();
	
	// set AHB clock divider to 1
	SYSCON_SYSAHBCLKDIV(1);

	return frequency;
}
