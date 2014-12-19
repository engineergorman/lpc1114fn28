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

/////////////////////////////////////////////////
//	LPC_FLASHCTRL Section
/////////////////////////////////////////////////

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

/////////////////////////////////////////////////
//	LPC_SYSCON Section
/////////////////////////////////////////////////

#include "syscon.h"


/////////////////////////////////////////////////
//	LPC_GPIOn Section
/////////////////////////////////////////////////

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

inline void GPIO1_DIR(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIODIR*)&LPC_GPIO1->DIR)->IO |= 1 << pin;
	else
		((_GPIODIR*)&LPC_GPIO1->DIR)->IO &= ~ (1 << pin);
}

// LPC_GPIO0->IS
// Interrupt sense Register (R/W)
struct _GPIOIS
{
	uint32_t ISENSE : 12;	// Selects interrupt on pin x as level or edge sensitive
	uint32_t reserved : 20;	
};

#define GPIO_EDGE_SENSITIVE	0x0
#define GPIO_LEVEL_SENSITIVE	0x1

inline void GPIO0_IS(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIS*)&LPC_GPIO0->IS)->ISENSE |= 1 << pin;
	else
		((_GPIOIS*)&LPC_GPIO0->IS)->ISENSE &= ~ (1 << pin);
}

inline void GPIO1_IS(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIS*)&LPC_GPIO1->IS)->ISENSE |= 1 << pin;
	else
		((_GPIOIS*)&LPC_GPIO1->IS)->ISENSE &= ~ (1 << pin);
}

// LPC_GPIO0->IBE
// Interrupt both edges Register (R/W)
struct _GPIOIBE
{
	uint32_t IBE : 12;	// Selects interrupt on pin x to be triggered on both edges
	uint32_t reserved : 20;	
};

#define GPIO_SINGLE_EDGE	0x0
#define GPIO_BOTH_EDGES 0x1

inline void GPIO0_IBE(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIBE*)&LPC_GPIO0->IBE)->IBE |= 1 << pin;
	else
		((_GPIOIBE*)&LPC_GPIO0->IBE)->IBE &= ~ (1 << pin);
}

inline void GPIO1_IBE(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIBE*)&LPC_GPIO1->IBE)->IBE |= 1 << pin;
	else
		((_GPIOIBE*)&LPC_GPIO1->IBE)->IBE &= ~ (1 << pin);
}

// LPC_GPIO0->IEV
// Interrupt Event Register  (R/W)
struct _GPIOIEV
{
	uint32_t IEV : 12;	// Selects interrupt on pin x to be triggered rising or falling edges
	uint32_t reserved : 20;	
};

// If GPIO_EDGE_SENSITIVE set, then:
#define GPIO_FALLING_EDGE	0x0
#define GPIO_RISING_EDGES 0x1
// If GPIO_LEVEL_SENSITIVE set, then:
#define GPIO_LOW_LEVEL 0x0
#define GPIO_HIGH_LEVEL 0x1

inline void GPIO0_IEV(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIEV*)&LPC_GPIO0->IEV)->IEV |= 1 << pin;
	else
		((_GPIOIEV*)&LPC_GPIO0->IEV)->IEV &= ~ (1 << pin);
}

inline void GPIO1_IEV(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIEV*)&LPC_GPIO1->IEV)->IEV |= 1 << pin;
	else
		((_GPIOIEV*)&LPC_GPIO1->IEV)->IEV &= ~ (1 << pin);
}

// LPC_GPIO0->IE
// Interrupt mask Register (R/W) 
struct _GPIOIE
{
	uint32_t MASK : 12;	// Selects interrupt on pin x to be masked
	uint32_t reserved : 20;	
};

#define GPIO_INTERRUPT_DISABLE 0x0
#define GPIO_INTERRUPT_ENABLE 0x1

inline void GPIO0_IE(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIE*)&LPC_GPIO0->IE)->MASK |= 1 << pin;
	else
		((_GPIOIE*)&LPC_GPIO0->IE)->MASK &= ~ (1 << pin);
}

inline void GPIO1_IE(uint32_t pin, uint32_t dir)
{
	if (dir)
		((_GPIOIE*)&LPC_GPIO1->IE)->MASK |= 1 << pin;
	else
		((_GPIOIE*)&LPC_GPIO1->IE)->MASK &= ~ (1 << pin);
}

// LPC_GPIO0->MIS
// GPIO masked interrupt status register (R)
struct _GPIOMIS
{
	uint32_t MASK : 12;	// interrupt status bits
	uint32_t reserved : 20;	
};

// returns 1 if interrupt on GPIO0_x; 0 if no interrupt (or was masked)
inline uint32_t GPIO0_MIS(uint32_t pin)
{
	return (((_GPIOMIS*)&LPC_GPIO0->MIS)->MASK >> pin) & 0x1;
}

inline uint32_t GPIO1_MIS(uint32_t pin)
{
	return (((_GPIOMIS*)&LPC_GPIO1->MIS)->MASK >> pin) & 0x1;
}

// LPC_GPIO0->IC
// GPIO interrupt clear register (W)
struct _GPIOIC
{
	uint32_t CLR : 12;	// clears the interrupt edge detection logic
	uint32_t reserved : 20;	
};

// clears interrupt on pin x
//		Note:  It is recommended to add two NOPs after the clear of the interrupt edge 
//		detection logic before the exit of the interrupt service routine, ex: __NOP();__NOP();

inline void GPIO0_IC(uint32_t pin)
{
	((_GPIOIC*)&LPC_GPIO0->IC)->CLR |= 1 << pin;
}

inline void GPIO1_IC(uint32_t pin)
{
	((_GPIOIC*)&LPC_GPIO1->IC)->CLR |= 1 << pin;
}

// LPC_GPIO0->MASKED_ACCESS
// write value at GPIOn, pin = n
inline void GPIO0_DATA( uint32_t pin, uint32_t val )
{
  LPC_GPIO0->MASKED_ACCESS[(1 << pin)] = (val << pin);
}
// read value at GPIOn, pin = n
inline uint32_t GPIO0_DATA( uint32_t pin )
{
  return LPC_GPIO0->MASKED_ACCESS[(1 << pin)];
}

inline void GPIO1_DATA( uint32_t pin, uint32_t val )
{
  LPC_GPIO1->MASKED_ACCESS[(1 << pin)] = (val << pin);
}

inline uint32_t GPIO1_DATA( uint32_t pin )
{
  return LPC_GPIO1->MASKED_ACCESS[(1 << pin)];
}

#include "timers.h"

/////////////////////////////////////////////////
//	LPC_IOCON Section
/////////////////////////////////////////////////

// many pins have the same config structure
struct IOSTD
{
	__IO uint32_t FUNC : 3;	// pin function
	__IO uint32_t MODE : 2;	// pull up/down resistor mode
	__IO uint32_t HYS : 1;		// Hysteresis.
	__IO uint32_t reserved0 : 4;
	__IO uint32_t OD : 1;		// Selects pseudo open-drain mode
	__IO uint32_t reserved1 : 21;
};

struct IOI2C
{
	__IO uint32_t FUNC : 3;	// pin function
	__IO uint32_t reserved0 : 5;
	__IO uint32_t I2CMODE : 2;		// Selects I2C mode
	__IO uint32_t reserved1 : 22;
};

struct IOAD
{
	__IO uint32_t FUNC : 3;	// pin function
	__IO uint32_t MODE : 2;	// pull up/down resistor mode
	__IO uint32_t HYS : 1;		// Hysteresis.
	__IO uint32_t reserved0 : 1;
	__IO uint32_t ADMODE : 1;		// Selects Analog/Digital mode
	__IO uint32_t reserved1 : 2;
	__IO uint32_t OD : 1;		// Selects pseudo open-drain mode
	__IO uint32_t reserved2 : 21;
};

struct IOSCK0_LOC
{
	__IO uint32_t SCKLOC : 2;	// SCK0 location
};

struct _IOCON
{
  __IO uint32_t PIO2_6;                 /*!< Offset: 0x000 I/O configuration for pin PIO2_6 (R/W) */
       uint32_t RESERVED0[1];
  __IO uint32_t PIO2_0;                 /*!< Offset: 0x008 I/O configuration for pin PIO2_0/DTR/SSEL1 (R/W) */
  __IO IOSTD PIO0_0;           /*!< Offset: 0x00C I/O configuration for pin RESET/PIO0_0  (R/W) */
  __IO IOSTD PIO0_1;                 /*!< Offset: 0x010 I/O configuration for pin PIO0_1/CLKOUT/CT32B0_MAT2 (R/W) */
  __IO IOSTD PIO1_8;                 /*!< Offset: 0x014 I/O configuration for pin PIO1_8/CT16B1_CAP0 (R/W) */
  __IO uint32_t SSEL1_LOC;              /*!< Offset: 0x018 IOCON SSEL1 location register (IOCON_SSEL1_LOC, address 0x4004 4018) */
  __IO IOSTD PIO0_2;                 /*!< Offset: 0x01C I/O configuration for pin PIO0_2/SSEL0/CT16B0_CAP0 (R/W) */

  __IO uint32_t PIO2_7;                 /*!< Offset: 0x020 I/O configuration for pin PIO2_7 (R/W) */
  __IO uint32_t PIO2_8;                 /*!< Offset: 0x024 I/O configuration for pin PIO2_8 (R/W) */
  __IO uint32_t PIO2_1;                 /*!< Offset: 0x028 I/O configuration for pin PIO2_1/nDSR/SCK1 (R/W) */
  __IO IOSTD PIO0_3;                 /*!< Offset: 0x02C I/O configuration for pin PIO0_3 (R/W) */
  __IO IOI2C PIO0_4;                 /*!< Offset: 0x030 I/O configuration for pin PIO0_4/SCL (R/W) */
  __IO IOI2C PIO0_5;                 /*!< Offset: 0x034 I/O configuration for pin PIO0_5/SDA (R/W) */
  __IO IOSTD PIO1_9;                 /*!< Offset: 0x038 I/O configuration for pin PIO1_9/CT16B1_MAT0 (R/W) */
  __IO uint32_t PIO3_4;                 /*!< Offset: 0x03C I/O configuration for pin PIO3_4 (R/W) */

  __IO uint32_t PIO2_4;                 /*!< Offset: 0x040 I/O configuration for pin PIO2_4 (R/W) */
  __IO uint32_t PIO2_5;                 /*!< Offset: 0x044 I/O configuration for pin PIO2_5 (R/W) */
  __IO uint32_t PIO3_5;                 /*!< Offset: 0x048 I/O configuration for pin PIO3_5 (R/W) */
  __IO IOSTD PIO0_6;                 /*!< Offset: 0x04C I/O configuration for pin PIO0_6/SCK0 (R/W) */
  __IO IOSTD PIO0_7;                 /*!< Offset: 0x050 I/O configuration for pin PIO0_7/nCTS (R/W) */
  __IO uint32_t PIO2_9;                 /*!< Offset: 0x054 I/O configuration for pin PIO2_9 (R/W) */
  __IO uint32_t PIO2_10;                /*!< Offset: 0x058 I/O configuration for pin PIO2_10 (R/W) */
  __IO uint32_t PIO2_2;                 /*!< Offset: 0x05C I/O configuration for pin PIO2_2/DCD/MISO1 (R/W) */

  __IO IOSTD PIO0_8;                 /*!< Offset: 0x060 I/O configuration for pin PIO0_8/MISO0/CT16B0_MAT0 (R/W) */
  __IO IOSTD PIO0_9;                 /*!< Offset: 0x064 I/O configuration for pin PIO0_9/MOSI0/CT16B0_MAT1 (R/W) */
  __IO IOSTD PIO0_10;          /*!< Offset: 0x068 I/O configuration for pin SWCLK/PIO0_10/SCK0/CT16B0_MAT2 (R/W) */
  __IO uint32_t PIO1_10;                /*!< Offset: 0x06C I/O configuration for pin PIO1_10/AD6/CT16B1_MAT1 (R/W) */
  __IO uint32_t PIO2_11;                /*!< Offset: 0x070 I/O configuration for pin PIO2_11/SCK0 (R/W) */
  __IO IOAD PIO0_11;              /*!< Offset: 0x074 I/O configuration for pin TDI/PIO0_11/AD0/CT32B0_MAT3 (R/W) */
  __IO IOAD PIO1_0;               /*!< Offset: 0x078 I/O configuration for pin TMS/PIO1_0/AD1/CT32B1_CAP0 (R/W) */
  __IO IOAD PIO1_1;               /*!< Offset: 0x07C I/O configuration for pin TDO/PIO1_1/AD2/CT32B1_MAT0 (R/W) */

  __IO IOAD PIO1_2;               /*!< Offset: 0x080 I/O configuration for pin nTRST/PIO1_2/AD3/CT32B1_MAT1 (R/W) */
  __IO uint32_t PIO3_0;                 /*!< Offset: 0x084 I/O configuration for pin PIO3_0/nDTR (R/W) */
  __IO uint32_t PIO3_1;                 /*!< Offset: 0x088 I/O configuration for pin PIO3_1/nDSR (R/W) */
  __IO uint32_t PIO2_3;                 /*!< Offset: 0x08C I/O configuration for pin PIO2_3/RI/MOSI1 (R/W) */
  __IO IOAD PIO1_3;           /*!< Offset: 0x090 I/O configuration for pin SWDIO/PIO1_3/AD4/CT32B1_MAT2 (R/W) */
  __IO IOAD PIO1_4;                 /*!< Offset: 0x094 I/O configuration for pin PIO1_4/AD5/CT32B1_MAT3 (R/W) */
  __IO uint32_t PIO1_11;                /*!< Offset: 0x098 I/O configuration for pin PIO1_11/AD7 (R/W) */
  __IO uint32_t PIO3_2;                 /*!< Offset: 0x09C I/O configuration for pin PIO3_2/nDCD (R/W) */

  __IO IOSTD PIO1_5;                 /*!< Offset: 0x0A0 I/O configuration for pin PIO1_5/nRTS/CT32B0_CAP0 (R/W) */
  __IO IOSTD PIO1_6;                 /*!< Offset: 0x0A4 I/O configuration for pin PIO1_6/RXD/CT32B0_MAT0 (R/W) */
  __IO IOSTD PIO1_7;                 /*!< Offset: 0x0A8 I/O configuration for pin PIO1_7/TXD/CT32B0_MAT1 (R/W) */
  __IO uint32_t PIO3_3;                 /*!< Offset: 0x0AC I/O configuration for pin PIO3_3/nRI (R/W) */
  __IO IOSCK0_LOC SCK_LOC;                /*!< Offset: 0x0B0 SCK pin location select Register (R/W) */
  __IO uint32_t DSR_LOC;                /*!< Offset: 0x0B4 DSR pin location select Register (R/W) */
  __IO uint32_t DCD_LOC;                /*!< Offset: 0x0B8 DCD pin location select Register (R/W) */
  __IO uint32_t RI_LOC;                 /*!< Offset: 0x0BC RI pin location Register (R/W) */

  __IO uint32_t CT16B0_CAP0_LOC;        /*!< Offset: 0x0C0 IOCON CT16B0_CAP0 location register (IOCON_CT16B0_CAP0_LOC, address 0x4004 40C0) */
  __IO uint32_t SCK1_LOC;               /*!< Offset: 0x0C4 IOCON SCK1 location register (IOCON_SCK1_LOC, address 0x4004 40C4) */
  __IO uint32_t MISO1_LOC;              /*!< Offset: 0x0C8 IOCON MISO1 location register (IOCON_MISO1_LOC, address 0x4004 40C8) */
  __IO uint32_t MOSI1_LOC;              /*!< Offset: 0x0CC IOCON MOSI1 location register (IOCON_MOSI1_LOC, address 0x4004 40CC) */
  __IO uint32_t CT32B0_CAP0_LOC;        /*!< Offset: 0x0D0 IOCON CT32B0_CAP0 location register (IOCON_CT32B0_CAP0_LOC, address 0x4004 40D0) */
  __IO uint32_t RXD_LOC;                /*!< Offset: 0x0D4 IOCON RXD location register (IOCON_RXD_LOC, address 0x4004 40D4) */
};

#define IOCON             ((_IOCON  *) LPC_IOCON_BASE )


#define MODE_NO_RESISTOR 0x0
#define MODE_PULLDOWN_RESISTOR 0x1
#define MODE_PULLUP_RESISTOR 0x2
#define MODE_REPEATER 0x3

#define I2CMODE_FAST 0x0
#define I2CMODE_GPIO 0x1
#define I2CMODE_FAST_PLUS 0x2

#define ADMODE_ANALOG_INPUT 0x0
#define ADMODE_DIGITAL 0x1

#define HYS_DISABLE 0x0
#define HYS_ENABLE 0x1

#define PIO0_1_FUNC_GPIO 0x0
#define PIO0_1_FUNC_CLKOUT 0x1
#define PIO0_1_FUNC_TIMER 0x2

#define PIO0_2_FUNC_GPIO 0x0
#define PIO0_2_FUNC_SSEL 0x1
#define PIO0_2_FUNC_TIMER 0x2

#define PIO0_3_FUNC_GPIO 0x0

#define PIO0_4_FUNC_GPIO 0x0
#define PIO0_4_FUNC_I2C_SCL 0x1

#define PIO0_5_FUNC_GPIO 0x0
#define PIO0_5_FUNC_I2C_SDA 0x1

#define PIO0_6_FUNC_GPIO 0x0
#define PIO0_6_FUNC_R 0x1		// reserved
#define PIO0_6_FUNC_SCK0 0x2	// NOTE: pin must be selected in IOCON->SCK_LOC location register

#define SCK0_LOC_PIO0_10 0x0
#define SCK0_LOC_PIO2_11 0x1
#define SCK0_LOC_PIO0_6 0x2

#define PIO0_8_FUNC_GPIO 0x0
#define PIO0_8_FUNC_MISO 0x1
#define PIO0_8_FUNC_TIMER 0x2

#define PIO0_9_FUNC_GPIO 0x0
#define PIO0_9_FUNC_MOSI 0x1
#define PIO0_9_FUNC_TIMER 0x2

#define PIO0_11_FUNC_R 0x0		// reserved
#define PIO0_11_FUNC_GPIO 0x1
#define PIO0_11_FUNC_ADC 0x2
#define PIO0_11_FUNC_TIMER 0x3

#define PIO1_0_FUNC_R 0x0		// reserved
#define PIO1_0_FUNC_GPIO 0x1
#define PIO1_0_FUNC_ADC 0x2
#define PIO1_0_FUNC_TIMER 0x3

#define PIO1_1_FUNC_R 0x0		// reserved
#define PIO1_1_FUNC_GPIO 0x1
#define PIO1_1_FUNC_ADC 0x2
#define PIO1_1_FUNC_TIMER 0x3

#define PIO1_2_FUNC_R 0x0		// reserved
#define PIO1_2_FUNC_GPIO 0x1
#define PIO1_2_FUNC_ADC 0x2
#define PIO1_2_FUNC_TIMER 0x3

#define PIO1_4_FUNC_GPIO 0x0
#define PIO1_4_FUNC_ADC 0x1
#define PIO1_4_FUNC_TIMER 0x2

#define PIO1_5_FUNC_GPIO 0x0
#define PIO1_5_FUNC_RTS 0x1
#define PIO1_5_FUNC_TIMER 0x2

#define PIO1_8_FUNC_GPIO 0x0
#define PIO1_8_FUNC_TIMER 0x1

#define PIO1_9_FUNC_GPIO 0x0
#define PIO1_9_FUNC_TIMER 0x1

//
//  HERE IS THE FUNCTIONAL METHOD THAT I AM RETIRING SOON
//

// LPC_IOCON->PIO0_2
// I/O configuration for pin (25) PIO0_2/SSEL0/CT16B0_CAP0 (R/W)
struct _PIO0_2
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 4;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved1 : 21;
};

inline void IOCON_PIO0_2_FUNC(uint32_t val)
{
	((_PIO0_2*)&LPC_IOCON->PIO0_2)->FUNC = val;
}

#define PIO0_2_MODE_NO_RESISTOR 0x0
#define PIO0_2_MODE_PULLDOWN_RESISTOR 0x1
#define PIO0_2_MODE_PULLUP_RESISTOR 0x2
#define PIO0_2_MODE_REPEATER 0x3
inline void IOCON_PIO0_2_MODE(uint32_t val)
{
	((_PIO0_2*)&LPC_IOCON->PIO0_2)->MODE = val;
}


// LPC_IOCON->PIO0_8
// I/O configuration for pin (1) PIO0_8/MISO0/CT16B0_MAT0 (R/W)
struct _PIO0_8
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 4;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved1 : 21;
};

inline void IOCON_PIO0_8_FUNC(uint32_t val)
{
	((_PIO0_8*)&LPC_IOCON->PIO0_8)->FUNC = val;
}

#define PIO0_8_MODE_NO_RESISTOR 0x0
#define PIO0_8_MODE_PULLDOWN_RESISTOR 0x1
#define PIO0_8_MODE_PULLUP_RESISTOR 0x2
#define PIO0_8_MODE_REPEATER 0x3
inline void IOCON_PIO0_8_MODE(uint32_t val)
{
	((_PIO0_8*)&LPC_IOCON->PIO0_8)->MODE = val;
}


// LPC_IOCON->PIO0_9
// I/O configuration for pin (2) PIO0_9/MOSI0/CT16B0_MAT1 (R/W)
struct _PIO0_9
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 4;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved1 : 21;
};

inline void IOCON_PIO0_9_FUNC(uint32_t val)
{
	((_PIO0_9*)&LPC_IOCON->PIO0_9)->FUNC = val;
}

#define PIO0_9_MODE_NO_RESISTOR 0x0
#define PIO0_9_MODE_PULLDOWN_RESISTOR 0x1
#define PIO0_9_MODE_PULLUP_RESISTOR 0x2
#define PIO0_9_MODE_REPEATER 0x3
inline void IOCON_PIO0_9_MODE(uint32_t val)
{
	((_PIO0_9*)&LPC_IOCON->PIO0_9)->MODE = val;
}


// LPC_IOCON->R_PIO0_11
// I/O configuration for pin (4) R/PIO0_11/AD0/CT32B0_MAT3 (R/W)
struct _PIO0_11
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 1;
	uint32_t ADMODE : 1;		// Selects Analog/Digital mode
	uint32_t reserved1 : 2;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved2 : 21;
};

inline void IOCON_PIO0_11_FUNC(uint32_t val)
{
	((_PIO0_11*)&LPC_IOCON->R_PIO0_11)->FUNC = val;
}

#define PIO0_11_MODE_NO_RESISTOR 0x0
#define PIO0_11_MODE_PULLDOWN_RESISTOR 0x1
#define PIO0_11_MODE_PULLUP_RESISTOR 0x2
#define PIO0_11_MODE_REPEATER 0x3
inline void IOCON_PIO0_11_MODE(uint32_t val)
{
	((_PIO0_11*)&LPC_IOCON->R_PIO0_11)->MODE = val;
}

#define PIO0_11_ADMODE_ANALOG_INPUT 0x0
#define PIO0_11_ADMODE_DIGITAL 0x1
inline void IOCON_PIO0_11_ADMODE(uint32_t val)
{
	((_PIO0_11*)&LPC_IOCON->R_PIO0_11)->ADMODE = val;
}


// LPC_IOCON->PIO0_5
// I/O configuration for pin (5) PIO0_5/SDA (R/W)
struct _PIO0_5
{
	uint32_t FUNC : 3;	// pin function
	uint32_t reserved0 : 5;
	uint32_t I2CMODE : 2;		// Selects I2C mode
	uint32_t reserved1 : 22;
};

inline void IOCON_PIO0_5_FUNC(uint32_t val)
{
	((_PIO0_5*)&LPC_IOCON->PIO0_5)->FUNC = val;
}

#define PIO0_5_I2CMODE_FAST 0x0
#define PIO0_5_I2CMODE_GPIO 0x1
#define PIO0_5_I2CMODE_FAST_PLUS 0x2
inline void IOCON_PIO0_5_I2CMODE(uint32_t val)
{
	((_PIO0_5*)&LPC_IOCON->PIO0_5)->I2CMODE = val;
}


// LPC_IOCON->PIO0_4
// I/O configuration for pin (27) PIO0_4/SCL (R/W)
struct _PIO0_4
{
	uint32_t FUNC : 3;	// pin function
	uint32_t reserved0 : 5;
	uint32_t I2CMODE : 2;		// Selects I2C mode
	uint32_t reserved1 : 22;
};

inline void IOCON_PIO0_4_FUNC(uint32_t val)
{
	((_PIO0_4*)&LPC_IOCON->PIO0_4)->FUNC = val;
}

#define PIO0_4_I2CMODE_FAST 0x0
#define PIO0_4_I2CMODE_GPIO 0x1
#define PIO0_4_I2CMODE_FAST_PLUS 0x2
inline void IOCON_PIO0_4_I2CMODE(uint32_t val)
{
	((_PIO0_4*)&LPC_IOCON->PIO0_4)->I2CMODE = val;
}


// LPC_IOCON->R_PIO1_0
// I/O configuration for pin R_PIO1_0/AD1/CT32B1_CAP0 (R/W)
struct _PIO1_0
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 1;
	uint32_t ADMODE : 1;		// Selects Analog/Digital mode
	uint32_t reserved1 : 2;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved2 : 21;
};

inline void IOCON_PIO1_0_FUNC(uint32_t val)
{
	((_PIO1_0*)&LPC_IOCON->R_PIO1_0)->FUNC = val;
}

#define PIO1_0_MODE_NO_RESISTOR 0x0
#define PIO1_0_MODE_PULLDOWN_RESISTOR 0x1
#define PIO1_0_MODE_PULLUP_RESISTOR 0x2
#define PIO1_0_MODE_REPEATER 0x3
inline void IOCON_PIO1_0_MODE(uint32_t val)
{
	((_PIO1_0*)&LPC_IOCON->R_PIO1_0)->MODE = val;
}

#define PIO1_0_ADMODE_ANALOG_INPUT 0x0
#define PIO1_0_ADMODE_DIGITAL 0x1
inline void IOCON_PIO1_0_ADMODE(uint32_t val)
{
	((_PIO1_0*)&LPC_IOCON->R_PIO1_0)->ADMODE = val;
}


// LPC_IOCON->R_PIO1_1
// I/O configuration for pin R_PIO1_1/AD2/CT32B1_MAT0 (R/W)
struct _PIO1_1
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 1;
	uint32_t ADMODE : 1;		// Selects Analog/Digital mode
	uint32_t reserved1 : 2;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved2 : 21;
};

inline void IOCON_PIO1_1_FUNC(uint32_t val)
{
	((_PIO1_1*)&LPC_IOCON->R_PIO1_1)->FUNC = val;
}

#define PIO1_1_MODE_NO_RESISTOR 0x0
#define PIO1_1_MODE_PULLDOWN_RESISTOR 0x1
#define PIO1_1_MODE_PULLUP_RESISTOR 0x2
#define PIO1_1_MODE_REPEATER 0x3
inline void IOCON_PIO1_1_MODE(uint32_t val)
{
	((_PIO1_1*)&LPC_IOCON->R_PIO1_1)->MODE = val;
}

#define PIO1_1_ADMODE_ANALOG_INPUT 0x0
#define PIO1_1_ADMODE_DIGITAL 0x1
inline void IOCON_PIO1_1_ADMODE(uint32_t val)
{
	((_PIO1_1*)&LPC_IOCON->R_PIO1_1)->ADMODE = val;
}


// LPC_IOCON->PIO1_8
// I/O configuration for pin (17) PIO1_8/CT16B1_CAP0 (R/W)
struct _PIO1_8
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 4;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved1 : 21;
};

inline void IOCON_PIO1_8_FUNC(uint32_t val)
{
	((_PIO1_8*)&LPC_IOCON->PIO1_8)->FUNC = val;
}

#define PIO1_8_MODE_NO_RESISTOR 0x0
#define PIO1_8_MODE_PULLDOWN_RESISTOR 0x1
#define PIO1_8_MODE_PULLUP_RESISTOR 0x2
#define PIO1_8_MODE_REPEATER 0x3
inline void IOCON_PIO1_8_MODE(uint32_t val)
{
	((_PIO1_8*)&LPC_IOCON->PIO1_8)->MODE = val;
}


// LPC_IOCON->PIO1_9
// I/O configuration for pin (18) PIO1_9/CT16B1_MAT0 (R/W)
struct _PIO1_9
{
	uint32_t FUNC : 3;	// pin function
	uint32_t MODE : 2;	// pull up/down resistor mode
	uint32_t HYS : 1;		// Hysteresis.
	uint32_t reserved0 : 4;
	uint32_t OD : 1;		// Selects pseudo open-drain mode
	uint32_t reserved1 : 21;
};

inline void IOCON_PIO1_9_FUNC(uint32_t val)
{
	((_PIO1_9*)&LPC_IOCON->PIO1_9)->FUNC = val;
}

#define PIO1_9_MODE_NO_RESISTOR 0x0
#define PIO1_9_MODE_PULLDOWN_RESISTOR 0x1
#define PIO1_9_MODE_PULLUP_RESISTOR 0x2
#define PIO1_9_MODE_REPEATER 0x3
inline void IOCON_PIO1_9_MODE(uint32_t val)
{
	((_PIO1_9*)&LPC_IOCON->PIO1_9)->MODE = val;
}


/////////////////////////////////////////////////
//	LPC_ADC Section
/////////////////////////////////////////////////

// LPC_ADC->CR
struct _ADC_CR
{
	uint32_t SEL : 8;	// Selects which of the AD7:0 pins is sampled and converted.
	uint32_t CLKDIV : 8;	// PCLK divided by CLKDIV +1 to produce the ADC clock (<= 4.5MHz)
	uint32_t BURST : 1;		// Burst mode.
	uint32_t CLKS : 3;	// number of clocks used for each conversion (Burst mode)
	uint32_t reserved0 : 4;
	uint32_t START : 3;	// control whether/when A/D conversion is started (Burst mode)	uint32_t CLKS : 3;
	uint32_t EDGE : 1;
	uint32_t reserved1 : 4;
};

#define ADC_DISABLE 0x0
#define ADC_ENABLE 0x1
// first param is the ADn index, second param is enable/disable
inline void ADC_CR_SEL(uint32_t idx, uint32_t val)
{
	if (val)
		((_ADC_CR*)&LPC_ADC->CR)->SEL |= (1 << idx);
	else
		((_ADC_CR*)&LPC_ADC->CR)->SEL &= ~(1<< idx);
}

inline void ADC_CR_CLKDIV(uint32_t val)
{
	((_ADC_CR*)&LPC_ADC->CR)->CLKDIV = val;
}

#define ADC_BURST_DISABLE 0x0
#define ADC_BURST_ENABLE 0x1
inline void ADC_CR_BURST(uint32_t val)
{
	((_ADC_CR*)&LPC_ADC->CR)->BURST = val;
}

#define ADC_CLKS_11	0x0		// 10 bits accuracy
#define ADC_CLKS_10	0x1		// 9 bits accuracy
#define ADC_CLKS_9	0x2		// 8 bits accuracy
#define ADC_CLKS_8	0x3		// 7 bits accuracy
#define ADC_CLKS_7	0x4		// 6 bits accuracy
#define ADC_CLKS_6	0x5		// 5 bits accuracy
#define ADC_CLKS_5	0x6		// 4 bits accuracy
#define ADC_CLKS_4	0x7		// 3 bits accuracy
inline void ADC_CR_CLKS(uint32_t val)
{
	((_ADC_CR*)&LPC_ADC->CR)->CLKS = val;
}

// When the BURST bit is 0, these bits control whether and when an A/D conversion is started
#define ADC_START_STOP 0x0	// No start (this value should be used when clearing PDN to 0).
#define ADC_START_START_NOW 0x1	// Start conversion now.
#define ADC_START_ON_EDGE_PIO0_2 0x2		// Start conversion when edge on PIO0_2/SSEL/CT16B0_CAP0
#define ADC_START_ON_EDGE_PIO1_5 0x3		// Start conversion when edge on PIO1_5/DIR/CT32B0_CAP0
#define ADC_START_ON_EDGE_PIO1_6 0x4		// Start conversion when edge on CT32B0_MAT0
#define ADC_START_ON_EDGE_PIO1_7 0x5		// Start conversion when edge on CT32B0_MAT1
#define ADC_START_ON_EDGE_PIO0_8 0x6		// Start conversion when edge on CT16B0_MAT0
#define ADC_START_ON_EDGE_PIO0_9 0x7		// Start conversion when edge on CT16B0_MAT1
inline void ADC_CR_START(uint32_t val)
{
	((_ADC_CR*)&LPC_ADC->CR)->START = val;
}

// LPC_ADC->GDR
// A/D Global Data Register
// contains the result of the most recent A/D conversion
// Note: Reading this register resets the DONE bit to zero
struct _ADC_GDR
{
	uint32_t reserved0 : 6;
	uint32_t V_VREF : 10;	// voltage sample
	uint32_t reserved1 : 8;
	uint32_t CHN : 3;	// channel from which the result bits V_VREF were converted.
	uint32_t reserved2 : 3;
	uint32_t OVERRUN : 1;
	uint32_t DONE : 1;	// This bit is set to 1 when an A/D conversion completes.
};

#define ADC_GDR_DONE	0x1

inline _ADC_GDR ADC_GDR_Sample()
{
	return (_ADC_GDR&)LPC_ADC->GDR;
}


/////////////////////////////////////////////////
//	LPC_UART Section
/////////////////////////////////////////////////
