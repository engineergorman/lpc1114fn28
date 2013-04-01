#include <LPC11xx.h>

// LPC_FLASHCTRL->FLASHCFG
struct _FLASHCFG
{
	uint32_t FLASHTIM : 2;
	uint32_t reserved : 30;
};

#define FLASHTIM_1CLK 0x0
#define FLASHTIM_2CLK 0x1
#define FLASHTIM_3CLK 0x2

inline void FLASHCTRL_FLASHCFG_FLASHTIM(uint32_t val)
{
	((_FLASHCFG*)&LPC_FLASHCTRL->FLASHCFG)->FLASHTIM = val;
}

// LPC_SYSCON->SYSOSCCTRL
struct _SYSOSCCTRL
{
	uint32_t BYPASS : 1;
	uint32_t FREQRANGE : 1;
	uint32_t reserved : 30;
};

#define FREQRANGE_1_20_RANGE 0x0
#define FREQRANGE_15_25_RANGE 0x1

inline void SYSCON_SYSOSCCTRL_FREQRANGE(uint32_t val)
{
	((_SYSOSCCTRL*)&LPC_SYSCON->SYSOSCCTRL)->FREQRANGE = val;
}

// LPC_SYSCON->PDRUNCFG
struct _PDRUNCFG
{
	uint32_t IRCOUT : 1;
	uint32_t IRC : 1;
	uint32_t FLASH : 1;
	uint32_t BOD : 1;
	uint32_t ADC : 1;
	uint32_t SYSOSC : 1;
	uint32_t WDTOSC : 1;
	uint32_t SYSPLL : 1;
	uint32_t reserved : 24;
};
#define POWER_UP 0x0
#define POWER_DOWN 0x1

inline void SYSCON_PDRUNCFG_IRCOUT(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->IRCOUT = val;
}
inline void SYSCON_PDRUNCFG_SYSOSC(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->SYSOSC = val;
}
inline void SYSCON_PDRUNCFG_SYSPLL(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->SYSPLL = val;
}

// LPC_SYSCON->SYSPLLCLKSEL
struct _SYSPLLCLKSEL
{
	uint32_t SEL : 2;
	uint32_t reserved : 30;
};

#define SYSPLLCLKSEL_IRC_OSCILLATOR 0x0
#define SYSPLLCLKSEL_SYSTEM_OSCILLATOR 0x1

inline void SYSCON_SYSPLLCLKSEL(uint32_t val)
{
	((_SYSPLLCLKSEL*)&LPC_SYSCON->SYSPLLCLKSEL)->SEL = val;
}

// LPC_SYSCON->SYSPLLCLKUEN
struct _SYSPLLCLKUEN
{
	uint32_t ENA : 1;
	uint32_t reserved : 31;
};

inline void SYSCON_SYSPLLCLKUEN_TOGGLE()
{
	//Toggle the system PLL clock source update enable register
	_SYSPLLCLKUEN* pBits =(_SYSPLLCLKUEN*)&LPC_SYSCON->SYSPLLCLKUEN;
	pBits->ENA = 0;
	pBits->ENA = 1;
}

// LPC_SYSCON->SYSPLLCTRL
struct _SYSPLLCTRL
{
	uint32_t MSEL : 5;
	uint32_t PSEL : 2;
	uint32_t reserved : 25;
};

inline void SYSCON_SYSPLLCTRL(uint32_t m, uint32_t p)
{
	// System PLL control register
	_SYSPLLCTRL* pBits =(_SYSPLLCTRL*)&LPC_SYSCON->SYSPLLCTRL;
	pBits->MSEL = m;
	pBits->PSEL = p;
}

// LPC_SYSCON->SYSPLLSTAT (read only)
struct _SYSPLLSTAT
{
	uint32_t LOCK : 1;
	uint32_t reserved : 31;
};

#define PLL_NOTLOCKED 0x0
#define PLL_LOCKED 0x1

inline void WAIT_SYSCON_SYSPLLSTAT_LOCK()
{
	// wait for PLL lock
	while( PLL_NOTLOCKED == ((_SYSPLLSTAT*)&LPC_SYSCON->SYSPLLSTAT)->LOCK );
}


// LPC_SYSCON->MAINCLKSEL
struct _MAINCLKSEL
{
	uint32_t SEL : 2;
	uint32_t reserved : 30;
};

#define MAINCLKSEL_IRC_OSCILATOR 0x0
#define MAINCLKSEL_INPUT_CLOCK_SYS_PLL 0x1
#define MAINCLKSEL_WDT_OSCILATOR 0x2
#define MAINCLKSEL_SYS_PLL_CLOCK_OUT 0x3

inline void SYSCON_MAINCLKSEL(uint32_t val)
{
	((_MAINCLKSEL*)&LPC_SYSCON->MAINCLKSEL)->SEL = val;
}

// LPC_SYSCON->MAINCLKUEN
struct _MAINCLKUEN
{
	uint32_t ENA : 1;
	uint32_t reserved : 31;
};

inline void SYSCON_MAINCLKUEN_TOGGLE()
{
	//Toggle main clock source update enable register
	_MAINCLKUEN* pBits =(_MAINCLKUEN*)&LPC_SYSCON->MAINCLKUEN;
	pBits->ENA = 0;
	pBits->ENA = 1;
}

// LPC_SYSCON->SYSAHBCLKCTRL
struct _SYSAHBCLKCTRL
{
	uint32_t SYS : 1;
	uint32_t ROM : 1;
	uint32_t RAM : 1;
	uint32_t FLASHREG : 1;
	uint32_t FLASHARRAY : 1;
	uint32_t I2C : 1;
	uint32_t GPIO : 1;
	uint32_t CT16B0 : 1;
	uint32_t CT16B1 : 1;
	uint32_t CT32B0 : 1;
	uint32_t CT32B1 : 1;
	uint32_t SSP0 : 1;
	uint32_t UART : 1;
	uint32_t ADC : 1;
	uint32_t reserved0 : 1;
	uint32_t WDT : 1;
	uint32_t IOCON : 1;
	uint32_t CAN : 1;
	uint32_t SSP1 : 1;
	uint32_t reserved1 : 13;
};
#define SYSAHBCLKCTRL_DISABLE 0x0
#define SYSAHBCLKCTRL_ENABLE 0x1

inline void SYSCON_SYSAHBCLKCTRL_I2C(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->I2C = val;
}

inline void SYSCON_SYSAHBCLKCTRL_GPIO(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->GPIO = val;
}

inline void SYSCON_SYSAHBCLKCTRL_CT16B0(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->CT16B0 = val;
}

inline void SYSCON_SYSAHBCLKCTRL_CT16B1(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->CT16B1 = val;
}

inline void SYSCON_SYSAHBCLKCTRL_CT32B0(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->CT32B0 = val;
}

inline void SYSCON_SYSAHBCLKCTRL_CT32B1(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->CT32B1 = val;
}

inline void SYSCON_SYSAHBCLKCTRL_SSP0(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->SSP0 = val;
}

inline void SYSCON_SYSAHBCLKCTRL_UART(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->UART = val;
}

inline void SYSCON_SYSAHBCLKCTRL_ADC(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->ADC = val;
}

inline void SYSCON_SYSAHBCLKCTRL_IOCON(uint32_t val)
{
	((_SYSAHBCLKCTRL*)&LPC_SYSCON->SYSAHBCLKCTRL)->IOCON = val;
}

// LPC_SYSCON->SYSAHBCLKDIV
struct _SYSAHBCLKDIV
{
	uint32_t DIV : 8;
	uint32_t reserved : 24;
};

inline void SYSCON_SYSAHBCLKDIV(uint32_t val)
{
	((_SYSAHBCLKDIV*)&LPC_SYSCON->SYSAHBCLKDIV)->DIV = val;
}

// LPC_GPIO0->DIR
struct _GPIODIR
{
	uint32_t IO : 12;
	uint32_t reserved : 20;	
};

#define GPIO_INPUT 0x0
#define GPIO_OUTPUT 0x1

inline void GPIO0_DIR(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIODIR*)&LPC_GPIO0->DIR)->IO |= 1 << pin;
	else
		((_GPIODIR*)&LPC_GPIO0->DIR)->IO &= ~ (1 << pin);
}

inline void GPIO0_DATA( uint32_t pin, uint32_t val )
{
  LPC_GPIO0->MASKED_ACCESS[(1 << pin)] = (val << pin);
}
