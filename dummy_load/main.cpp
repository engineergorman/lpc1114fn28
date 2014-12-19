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


#define DISP_SELECT  0
#define DISP_DESELECT 1

inline void DispSelect(uint32_t value)
{
	GPIO1_DATA(8, value);
}

#define DISP_RESET 0
#define DISP_NO_RESET 1

inline void DispReset(uint32_t value)
{
	GPIO0_DATA(3, value);
}

#define DISP_CMD_MODE 0
#define DISP_DATA_MODE 1

uint32_t g_lastMode = 999;
inline void DispMode(uint32_t value)
{
	if (g_lastMode == value)
		return;
	g_lastMode = value;
	//Delay(1);
	while (SSP0CON->SR.TFE == 0);
	for (volatile int i = 0; i < 50; ++i);
	GPIO1_DATA(9, value);
}

//////////////////////////////////////////////////////



#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2 
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define  CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define  CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define  CMD_SET_BOOSTER_5  1
#define  CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0

void spiwrite(uint8_t c);

void setup(void);
void loop(void);

void st7565_init(void);
void st7565_command(uint8_t c);
void st7565_data(uint8_t c);
void st7565_set_brightness(uint8_t val);


void clear_screen(void);
void clear_buffer(uint8_t *buffer);
void write_buffer(uint8_t *buffer);

void testdrawbitmap(uint8_t *buff, const uint8_t *bitmap, uint8_t w, uint8_t h);



int pagemap[] = { 3, 2, 1, 0, 7, 6, 5, 4 };

uint8_t buffer[128*4];

struct FONT_CHAR_INFO
{
	uint16_t a;
	uint16_t b;
}	;

#include "fonts.h"

void test_display(void)
{
	st7565_init();

	int bofs = 0;
	int ofs = 1260;
	for (int i = 0; i < 15; ++i)
	{
		buffer[bofs++] = mSReferenceSansSerif_12ptBitmaps[ofs++];
	}
	bofs = 128 * 1;
	for (int i = 0; i < 15; ++i)
	{
		buffer[bofs++] = mSReferenceSansSerif_12ptBitmaps[ofs++];
	}

	bofs = 15;
	ofs = 2160;
	for (int i = 0; i < 15; ++i)
	{
		buffer[bofs++] = mSReferenceSansSerif_12ptBitmaps[ofs++];
	}
	bofs = (128 * 1) + 15;
	for (int i = 0; i < 15; ++i)
	{
		buffer[bofs++] = mSReferenceSansSerif_12ptBitmaps[ofs++];
	}


	bofs = 30;
	ofs = 2280;
	for (int i = 0; i < 15; ++i)
	{
		buffer[bofs++] = mSReferenceSansSerif_12ptBitmaps[ofs++];
	}
	bofs = (128 * 1) + 30;
	for (int i = 0; i < 15; ++i)
	{
		buffer[bofs++] = mSReferenceSansSerif_12ptBitmaps[ofs++];
	}
	
	write_buffer(buffer);
#if 0	
	Delay(3000);
	for (int j = 0; j < 3; ++j)
	{
		for (int i = 0; i < 128; ++i)
		{
			write_buffer(buffer + i);
			Delay(100);
		}
		for (int i = 127; i > 0; --i)
		{
			write_buffer(buffer+i);
		}
	}
	write_buffer(buffer);
#endif	
}


void st7565_init(void) 
{
	DispSelect(DISP_SELECT);	// assert /CS1
	
	DispReset(DISP_RESET);
	Delay(50);
	DispReset(DISP_NO_RESET);
	Delay(50);
	
  // LCD bias select
  st7565_command(CMD_SET_BIAS_9);
  // ADC select
  st7565_command(CMD_SET_ADC_NORMAL);		// do not flip x.  x reference from left.
  // SHL select
  st7565_command(CMD_SET_COM_NORMAL);		// do not flip y.  y referenced from bottom
  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);	// start at line 0

  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);
  Delay(50);

  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);
  Delay(50);

  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);
  Delay(50);

  // set lcd operating voltage (regulator resistor, ref voltage resistor)
	// with a 3.3 VDD we are getting approx 9v on VOUT with ratio = 001
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x1);
	st7565_set_brightness(26); // 0 - 63

	st7565_command(CMD_SET_ALLPTS_NORMAL);
	st7565_command(CMD_DISPLAY_ON);
}

inline void spiwrite(uint8_t c)
{
	while (SSP0CON->SR.TNF == 0);
	SSP0CON->DR = c;
//	Delay(1);
}

void st7565_command(uint8_t c)
{
	DispMode(DISP_CMD_MODE);
  spiwrite(c);
}

void st7565_data(uint8_t c) 
{
	DispMode(DISP_DATA_MODE);
  spiwrite(c);
}

void st7565_set_brightness(uint8_t val)
{
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}

void clear_screen(void) 
{
  uint8_t p, c;
  for(p = 0; p < 4; p++) 
	{
    st7565_command(CMD_SET_PAGE | p);
		st7565_command(CMD_SET_COLUMN_UPPER);
		st7565_command(CMD_SET_COLUMN_LOWER);
    for(c = 0; c < 128; c++) 
		{
      st7565_data(0x0);
    }     
  }
}

void write_buffer(uint8_t *buf) 
{
  //uint8_t c, p;
	unsigned p;
	uint8_t * cs;
	uint8_t * ce;


  for(p = 0; p < 4; p++) 
	{
    st7565_command(CMD_SET_PAGE | p);
    st7565_command(CMD_SET_COLUMN_UPPER);
    st7565_command(CMD_SET_COLUMN_LOWER);

		DispMode(DISP_DATA_MODE);
    
		cs = buf + (128 * p);
		ce = cs + 128;
		while (cs < ce)
		{
			while (SSP0CON->SR.TNF == 0);
			SSP0CON->DR = *cs;
			++cs;
		}
		
//     for(c = 0; c < 128; c++) 
// 		{
//       st7565_data(buffer[(128*p)+c]);
//     }
  }
}


//////////////////////////////////////////////////////

volatile uint32_t g_encoder;

extern "C" void PIOINT1_IRQHandler(void)
{
	// TODO: noise filtering
	if (GPIO1_MIS(0))
	{
		g_encoder = SysTickCnt;
		GPIO1_IC(0);
		__NOP();__NOP();
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

	// pin 17 (1_8) is connected to /CS1 on display
	IOCON->PIO1_8.FUNC = PIO1_8_FUNC_GPIO;
	GPIO1_DIR(8, GPIO_OUTPUT);	
	
	// pin 18 (1_9) is connected to A0 on display
	IOCON->PIO1_9.FUNC = PIO1_9_FUNC_GPIO;
	GPIO1_DIR(9, GPIO_OUTPUT);	
	
	// pin 26 (0_3) is connected to /RST on display
	IOCON->PIO0_3.FUNC = PIO0_3_FUNC_GPIO;
	//IOCON->PIO0_3.MODE = MODE_PULLUP_RESISTOR;
	GPIO0_DIR(3, GPIO_OUTPUT);	

	// configure display lines
	DispReset(DISP_RESET);
	DispSelect(DISP_DESELECT);
	//DispMode(DISP_CMD_MODE);
	
	//
	// Configure SPI 0
	//
	
	// Power on SSP 0
  SYSCON->SYSAHBCLKCTRL.SSP0 = 1;

	// reset the device
	SYSCON->PRESETCTRL.SSP0_RST_N = 0;	// assert reset
	SYSCON->PRESETCTRL.SSP0_RST_N = 1;	// de-assert reset

	// setup clock divisor for SSP0 device
	SYSCON->SSP0CLKDIV = 2;
	
	// pin 1 - PIO0_8, set to MISO
	// pin 2 - PIO0_9, set to MOSI
	// pin 6 - PIO0_6, set to SCK0
	// pin 25 - PIO0_2, set to SSEL0
	
	IOCON->PIO0_8.FUNC = PIO0_8_FUNC_MISO;
	IOCON->PIO0_9.FUNC = PIO0_9_FUNC_MOSI;
	IOCON->SCK_LOC.SCKLOC = SCK0_LOC_PIO0_6;
	IOCON->PIO0_6.FUNC = PIO0_6_FUNC_SCK0;
	IOCON->PIO0_2.FUNC = PIO0_2_FUNC_SSEL;

	// setup data clock rate (CPDVSR=2):
	//   SCR=23 = 500Khz
	//   SCR=11 = 1Mhz
	//   SCR=5  = 2Mhz
	// Serial Clock Rate = PCLK / (CPSDVSR * [SCR+1])
	SSP0CON->CPSR.CPSDVSR = 2;
	SSP0CON->CR0.SCR = 11;

	SSP0CON->CR0.DSS = SSP_CR0_DSS_8BIT;
	SSP0CON->CR0.FRF = SSP_CR0_FRF_SPI;
	SSP0CON->CR0.CPOL = SSP_CR0_CPOL_CLOCK_LOW;
	SSP0CON->CR0.CPHA = SSP_CR0_CPHA_AWAY_FROM;
	
	SSP0CON->CR1.MS = SSP_CR1_MS_MASTER;
	
	// empty out FIFO
	volatile uint32_t data;
	for (int i = 0; i < SPI_FIFOSIZE; ++i)
	{
		data = SSP0CON->DR;
	}

	// enable SPI controller
	SSP0CON->CR1.SSE = 1;
	
	//
	//  setup the rotary encoder interrupt
	//
	
	// setup interrupts on PIO1_0
	IOCON_PIO1_0_MODE(PIO1_0_MODE_NO_RESISTOR);
	GPIO1_DIR(0, GPIO_INPUT);	
	GPIO1_IS(0, GPIO_EDGE_SENSITIVE);
	GPIO1_IBE(0, GPIO_SINGLE_EDGE);
	GPIO1_IEV(0, GPIO_FALLING_EDGE);
	GPIO1_IE(0, GPIO_INTERRUPT_ENABLE);

	IOCON_PIO1_1_MODE(PIO1_1_MODE_NO_RESISTOR);
	GPIO1_DIR(1, GPIO_INPUT);	

	NVIC_EnableIRQ(EINT1_IRQn);

	
	//test_display();

	// pin 4 (0_11) is connected to ~SEL on DAC
	IOCON->PIO0_11.FUNC = PIO0_11_FUNC_GPIO;
	GPIO0_DIR(11, GPIO_OUTPUT);
	GPIO0_DATA(11, 1);	// set to hi to deselect the DAC

	// pin 24 (0_1) is connected to FET gating 100 Ohm current measurement 
	IOCON->PIO0_1.FUNC = PIO0_1_FUNC_GPIO;
	GPIO0_DIR(1, GPIO_OUTPUT);
	GPIO0_DATA(1, 1);	// set to high to select 

	int dacVal = -1;
	int updateDacVal = 0;
	g_encoder = 0;
	int onDownCycle = 1;
	int lastUpdate = 0;
	int accel = 1;
	while (1)
	{
		if (g_encoder)
		{
			uint32_t currTime = SysTickCnt;
			uint32_t encoderTime = g_encoder;
			
			uint32_t delta;
			if (currTime  < encoderTime)
			{
				delta = (0xFFFFFFFF - encoderTime) + currTime;
			}
			else
			{
				delta = currTime - encoderTime;
			}
			
			if (delta > 2)
			{
				int b = GPIO1_DATA(1);
				int a = GPIO1_DATA(0);
				
				int doTransition = 1;
				if (onDownCycle)
				{
					if (0 == a)
					{
						// transition to up cycle
						doTransition = 0;
						onDownCycle = 0;
						GPIO1_IEV(0, GPIO_RISING_EDGES);
					}
				}
				else
				{
					// looking for the up transition
					if (1 == a)
					{
						// we have completed one full cycyle
						if (lastUpdate < currTime)
						{
							delta = currTime - lastUpdate;
						}
						else
						{
							delta = (0xFFFFFFFF - lastUpdate) + currTime;
						}
						if (delta < 250)	// 25 msec between clicks
						{
							++accel;
						}
						else
						{
							--accel;
						}
						accel = (accel < 1) ? 1 : accel;
						accel = (accel > 100) ? 100 : accel;
						lastUpdate = currTime;
						
						updateDacVal += (b ? -1 : 1) * accel;
					}
				}
				if (doTransition)
				{
					onDownCycle = 1;
					GPIO1_IEV(0, GPIO_FALLING_EDGE);
				}
				g_encoder = 0;
			}
		}
		else
		{
			// need to bring down accel if we have fewer clicks
			uint32_t currTime = SysTickCnt;
			uint32_t delta;
			if (lastUpdate < currTime)
			{
				delta = currTime - lastUpdate;
			}
			else
			{
				delta = (0xFFFFFFFF - lastUpdate) + currTime;
			}
			if (delta > 250)
			{
				--accel;
			}
			accel = (accel < 1) ? 1 : accel;			
		}
		
		if (updateDacVal != dacVal)
		{
			updateDacVal = (updateDacVal < 0) ? 0 : updateDacVal;
			updateDacVal = (updateDacVal > 4095) ? 4095 : updateDacVal;
			dacVal = updateDacVal;
			
			Delay(1);
			GPIO0_DATA(11, 0);	// set to low to select the DAC
			Delay(1);
			SSP0CON->DR = dacVal >> 8;
			SSP0CON->DR = dacVal & 0xFF;
			Delay(1);
			GPIO0_DATA(11, 1);	// set to hi
			Delay(1);
		}
	}
	
#if 0
	while (1)
	{
		for (int j = 0; j < 4096; ++j)
		{
			Delay(1);
			GPIO0_DATA(11, 0);	// set to low to select the DAC
			Delay(1);
			SSP0CON->DR = j >> 8;

// 			Delay(1);
// 			GPIO0_DATA(11, 1);	// set to hi
// 			Delay(1);
// 			GPIO0_DATA(11, 0);	// set to low to select the DAC
// 			Delay(1);

			SSP0CON->DR = j & 0xFF;
			Delay(1);
			GPIO0_DATA(11, 1);	// set to hi
			Delay(1);
		}
	}
#endif
}
