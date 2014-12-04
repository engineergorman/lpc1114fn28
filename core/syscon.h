#include <LPC11xx.h>

/////////////////////////////////////////////////
//	SYSCON Section
/////////////////////////////////////////////////

// Peripheral reset control register
// SYSCON->PRESETCTRL
struct _PRESETCTRL
{
	uint32_t SSP0_RST_N : 1;	// SPI0 reset control
	uint32_t I2C_RST_N : 1;	// I2C reset control
	uint32_t SSP1_RST_N : 1;	// SPI1 reset control
	uint32_t CAN_RST_N : 1;	// CAN reset control
};

// SYSCON->SYSOSCCTRL
struct _SYSOSCCTRL
{
	uint32_t BYPASS : 1;
	uint32_t FREQRANGE : 1;		// FREQRANGE_1_20_RANGE, FREQRANGE_15_25_RANGE
	uint32_t reserved : 30;
};

#define FREQRANGE_1_20_RANGE 0x0
#define FREQRANGE_15_25_RANGE 0x1

// SYSCON->PDRUNCFG
struct _PDRUNCFG
{
	uint32_t IRCOUT : 1;		// POWER_UP, POWER_DOWN
	uint32_t IRC : 1;				// POWER_UP, POWER_DOWN
	uint32_t FLASH : 1;			// POWER_UP, POWER_DOWN
	uint32_t BOD : 1;				// POWER_UP, POWER_DOWN
	uint32_t ADC : 1;				// POWER_UP, POWER_DOWN
	uint32_t SYSOSC : 1;		// POWER_UP, POWER_DOWN
	uint32_t WDTOSC : 1;		// POWER_UP, POWER_DOWN
	uint32_t SYSPLL : 1;		// POWER_UP, POWER_DOWN
	uint32_t reserved : 24;
};

#define POWER_UP 0x0
#define POWER_DOWN 0x1

// SYSCON->SYSPLLCLKSEL
struct _SYSPLLCLKSEL
{
	uint32_t SEL : 2;		// SYSPLLCLKSEL_IRC_OSCILLATOR, SYSPLLCLKSEL_SYSTEM_OSCILLATOR
	uint32_t reserved : 30;
};

#define SYSPLLCLKSEL_IRC_OSCILLATOR 0x0
#define SYSPLLCLKSEL_SYSTEM_OSCILLATOR 0x1

// SYSCON->SYSPLLCLKUEN
struct _SYSPLLCLKUEN
{
	uint32_t ENA : 1;						// NOTE:  use SYSCON_SYSPLLCLKUEN_TOGGLE()
	uint32_t reserved : 31;
};

// SYSCON->SYSPLLCTRL
struct _SYSPLLCTRL
{
	uint32_t MSEL : 5;
	uint32_t PSEL : 2;
	uint32_t reserved : 25;
};

// SYSCON->SYSPLLSTAT (read only)
struct _SYSPLLSTAT
{
	uint32_t LOCK : 1;					// NOTE:  use WAIT_SYSCON_SYSPLLSTAT_LOCK()
	uint32_t reserved : 31;
};

#define PLL_NOTLOCKED 0x0
#define PLL_LOCKED 0x1

// SYSCON->MAINCLKSEL
struct _MAINCLKSEL
{
	uint32_t SEL : 2;		// MAINCLKSEL_IRC_OSCILATOR, MAINCLKSEL_INPUT_CLOCK_SYS_PLL,
											// MAINCLKSEL_WDT_OSCILATOR, MAINCLKSEL_SYS_PLL_CLOCK_OUT
	uint32_t reserved : 30;
};

#define MAINCLKSEL_IRC_OSCILATOR 0x0
#define MAINCLKSEL_INPUT_CLOCK_SYS_PLL 0x1
#define MAINCLKSEL_WDT_OSCILATOR 0x2
#define MAINCLKSEL_SYS_PLL_CLOCK_OUT 0x3

// SYSCON->MAINCLKUEN
struct _MAINCLKUEN
{
	uint32_t ENA : 1;						// NOTE: use SYSCON_MAINCLKUEN_TOGGLE()
	uint32_t reserved : 31;
};

// SYSCON->SYSAHBCLKCTRL
struct _SYSAHBCLKCTRL
{
	uint32_t SYS : 1;			// SYSAHBCLKCTRL_DISABLE, SYSAHBCLKCTRL_ENABLE
	uint32_t ROM : 1;			// (all registers use the same as above)
	uint32_t RAM : 1;			// ...
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

// SYSCON->SYSAHBCLKDIV
struct _SYSAHBCLKDIV
{
	uint32_t DIV : 8;
	uint32_t reserved : 24;
};

struct _SYSCON
{
  __IO uint32_t SYSMEMREMAP;            /*!< Offset: 0x000 System memory remap (R/W) */
  __IO _PRESETCTRL PRESETCTRL;             /*!< Offset: 0x004 Peripheral reset control (R/W) */
  __IO _SYSPLLCTRL SYSPLLCTRL;             /*!< Offset: 0x008 System PLL control (R/W) */
  __IO _SYSPLLSTAT SYSPLLSTAT;             /*!< Offset: 0x00C System PLL status (R/W ) */
       uint32_t RESERVED0[4];

  __IO _SYSOSCCTRL SYSOSCCTRL;             /*!< Offset: 0x020 System oscillator control (R/W) */
  __IO uint32_t WDTOSCCTRL;             /*!< Offset: 0x024 Watchdog oscillator control (R/W) */
  __IO uint32_t IRCCTRL;                /*!< Offset: 0x028 IRC control (R/W) */
       uint32_t RESERVED1[1];
  __IO uint32_t SYSRSTSTAT;             /*!< Offset: 0x030 System reset status Register (R/ ) */
       uint32_t RESERVED2[3];
  __IO _SYSPLLCLKSEL SYSPLLCLKSEL;           /*!< Offset: 0x040 System PLL clock source select (R/W) */
  __IO _SYSPLLCLKUEN SYSPLLCLKUEN;           /*!< Offset: 0x044 System PLL clock source update enable (R/W) */
       uint32_t RESERVED3[10];

  __IO _MAINCLKSEL MAINCLKSEL;             /*!< Offset: 0x070 Main clock source select (R/W) */
  __IO _MAINCLKUEN MAINCLKUEN;             /*!< Offset: 0x074 Main clock source update enable (R/W) */
  __IO _SYSAHBCLKDIV SYSAHBCLKDIV;           /*!< Offset: 0x078 System AHB clock divider (R/W) */
       uint32_t RESERVED4[1];

  __IO _SYSAHBCLKCTRL SYSAHBCLKCTRL;          /*!< Offset: 0x080 System AHB clock control (R/W) */
       uint32_t RESERVED5[4];
  __IO uint32_t SSP0CLKDIV;             /*!< Offset: 0x094 SSP0 clock divider (R/W) */
  __IO uint32_t UARTCLKDIV;             /*!< Offset: 0x098 UART clock divider (R/W) */
  __IO uint32_t SSP1CLKDIV;             /*!< Offset: 0x09C SSP1 clock divider (R/W) */
       uint32_t RESERVED6[12];

  __IO uint32_t WDTCLKSEL;              /*!< Offset: 0x0D0 WDT clock source select (R/W) */
  __IO uint32_t WDTCLKUEN;              /*!< Offset: 0x0D4 WDT clock source update enable (R/W) */
  __IO uint32_t WDTCLKDIV;              /*!< Offset: 0x0D8 WDT clock divider (R/W) */
       uint32_t RESERVED8[1];
  __IO uint32_t CLKOUTCLKSEL;           /*!< Offset: 0x0E0 CLKOUT clock source select (R/W) */
  __IO uint32_t CLKOUTUEN;              /*!< Offset: 0x0E4 CLKOUT clock source update enable (R/W) */
  __IO uint32_t CLKOUTDIV;              /*!< Offset: 0x0E8 CLKOUT clock divider (R/W) */
       uint32_t RESERVED9[5];

  __IO uint32_t PIOPORCAP0;             /*!< Offset: 0x100 POR captured PIO status 0 (R/ ) */
  __IO uint32_t PIOPORCAP1;             /*!< Offset: 0x104 POR captured PIO status 1 (R/ ) */
       uint32_t RESERVED10[18];
  __IO uint32_t BODCTRL;                /*!< Offset: 0x150 BOD control (R/W) */
  __IO uint32_t SYSTCKCAL;              /*!< Offset: 0x154 System tick counter calibration (R/W) */

       uint32_t RESERVED13[7];
  __IO uint32_t NMISRC;                 /*!< Offset: 0x174 NMI source selection register (R/W) */
       uint32_t RESERVED14[34];

  __IO uint32_t STARTAPRP0;             /*!< Offset: 0x200 Start logic edge control Register 0 (R/W) */
  __IO uint32_t STARTERP0;              /*!< Offset: 0x204 Start logic signal enable Register 0 (R/W) */
  __O  uint32_t STARTRSRP0CLR;          /*!< Offset: 0x208 Start logic reset Register 0  ( /W) */
  __IO uint32_t STARTSRP0;              /*!< Offset: 0x20C Start logic status Register 0 (R/W) */
  __IO uint32_t STARTAPRP1;             /*!< Offset: 0x210 Start logic edge control Register 0 (R/W). (LPC11UXX only) */
  __IO uint32_t STARTERP1;              /*!< Offset: 0x214 Start logic signal enable Register 0 (R/W). (LPC11UXX only) */
  __O  uint32_t STARTRSRP1CLR;          /*!< Offset: 0x218 Start logic reset Register 0  ( /W). (LPC11UXX only) */
  __IO uint32_t STARTSRP1;              /*!< Offset: 0x21C Start logic status Register 0 (R/W). (LPC11UXX only) */
       uint32_t RESERVED17[4];

  __IO uint32_t PDSLEEPCFG;             /*!< Offset: 0x230 Power-down states in Deep-sleep mode (R/W) */
  __IO uint32_t PDAWAKECFG;             /*!< Offset: 0x234 Power-down states after wake-up (R/W) */
  __IO _PDRUNCFG PDRUNCFG;               /*!< Offset: 0x238 Power-down configuration Register (R/W) */
       uint32_t RESERVED15[110];
  __I  uint32_t DEVICE_ID;              /*!< Offset: 0x3F4 Device ID (R/ ) */
};

#define SYSCON             ((_SYSCON  *) LPC_SYSCON_BASE )

inline void SYSCON_SYSPLLCLKUEN_TOGGLE()
{
	//Toggle the system PLL clock source update enable register
	_SYSPLLCLKUEN* pBits =(_SYSPLLCLKUEN*)&LPC_SYSCON->SYSPLLCLKUEN;
	pBits->ENA = 0;
	pBits->ENA = 1;
}

inline void WAIT_SYSCON_SYSPLLSTAT_LOCK()
{
	// wait for PLL lock
	while( PLL_NOTLOCKED == ((_SYSPLLSTAT*)&LPC_SYSCON->SYSPLLSTAT)->LOCK );
}

inline void SYSCON_MAINCLKUEN_TOGGLE()
{
	//Toggle main clock source update enable register
	_MAINCLKUEN* pBits =(_MAINCLKUEN*)&LPC_SYSCON->MAINCLKUEN;
	pBits->ENA = 0;
	pBits->ENA = 1;
}


//
// The following methods are deprecated.  Use the SYSCON structure
// and access the registers directly.  For example, 
// old method:  
//   SYSCON_SYSAHBCLKCTRL_CT16B0(SYSAHBCLKCTRL_ENABLE)
// new method: 
//   SYSCON->SYSAHBCLKCTRL.CT16B0 = SYSAHBCLKCTRL_ENABLE
//

inline void SYSCON_SYSOSCCTRL_FREQRANGE(uint32_t val)
{
	((_SYSOSCCTRL*)&LPC_SYSCON->SYSOSCCTRL)->FREQRANGE = val;
}

inline void SYSCON_PDRUNCFG_IRCOUT(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->IRCOUT = val;
}
inline void SYSCON_PDRUNCFG_SYSOSC(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->SYSOSC = val;
}
inline void SYSCON_PDRUNCFG_ADC(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->ADC = val;
}
inline void SYSCON_PDRUNCFG_SYSPLL(uint32_t val)
{
	((_PDRUNCFG*)&LPC_SYSCON->PDRUNCFG)->SYSPLL = val;
}

inline void SYSCON_SYSPLLCLKSEL(uint32_t val)
{
	((_SYSPLLCLKSEL*)&LPC_SYSCON->SYSPLLCLKSEL)->SEL = val;
}

inline void SYSCON_SYSPLLCTRL(uint32_t m, uint32_t p)
{
	// System PLL control register
	_SYSPLLCTRL* pBits =(_SYSPLLCTRL*)&LPC_SYSCON->SYSPLLCTRL;
	pBits->MSEL = m;
	pBits->PSEL = p;
}

inline void SYSCON_MAINCLKSEL(uint32_t val)
{
	((_MAINCLKSEL*)&LPC_SYSCON->MAINCLKSEL)->SEL = val;
}

inline void SYSCON_SYSAHBCLKDIV(uint32_t val)
{
	((_SYSAHBCLKDIV*)&LPC_SYSCON->SYSAHBCLKDIV)->DIV = val;
}

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
