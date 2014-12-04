# Code Samples for NXP's LPC1114FN28/102 Microprocessor
The LPC1114FN28/102 is a low cost (about $1.50 - $2.50 each)
yet highly capable microprocessor in a DIP28 form factor.  This makes
the microprocessor easy for prototyping with a breadboard.
The chip is a 32-bit ARM Cortex M0 running up to 48Mhz with the 
internal oscillator.  Features: 32K flash, 4K RAM, 22 GPIO, UART,
SPI, I2C, ADC, DAC, PWM, nested interrupts, etc.  The full datasheet is
here:  http://www.nxp.com/documents/data_sheet/LPC111X.pdf

UPDATE (November 2014):  The hardware abstraction layer (HAL) has been evolving.  I am slowly replacing all 
inline functions with code that accesses structures with nicely labeled bitfields instead.
This saves a lot of time and effort in maintaining and extending the HAL.  In addition to the bitfields there are #defines
that abstract the actual values and make the code more readible.  The better examples are the newer ones, such spi_dac, 
spi_display, pwm_rgbLED, and spindle.  The older samples, such as blinky, still use the older method of inline functions.  Eventually
I will refactor the code to remove all of the inline functions.

This project is exploring the use of C++ to create a highy readable, yet very-thin hardware abstraction
layer for setting up the chip's core features and perhipherals.  

Current examples include:
* spi_dac:  uses SPI to communicate with a 12-bit precision DAC, the MAX538BCSA+
* spi_display:  uses SPI to communicate with a 128x32 graphic Chip On Glass LCD, the NHD-C12832A1Z-NSW-BBW-3V3 ($11 on Mouser)
* pwm_rgbLED: uses 3 PWM channels to drive a Red Green Blue LED
* spindle:  work-in-progress.  PWM controller for a BLDC 3-phase motor (spindle on a CNC).  this is probably my best example on
how to use the timer interrupts.
* pwm_fan:  uses PWM to control fan speed on a typical 4-wire 12v CPU/case cooling fan.  use Putty to send fan speed commands from your PC.
* uart_interrupt:  can't remember what i was doing with this.  i think i was rewriting the code in core\uart.cpp so that
it uses the more readible structures/bitfields instead of bit twiddling.  use with caution.  the code in core\uart.cpp seems to
be working reliably, so you may want to start off with that one instead if you need uart functionality.

Older examples (need to be rewritten using the newer methods): 
* blinky:  Configure the PLL for 48Mhz operation using the built in IRC
* pwm:  Configure 16-bit Timer 0 for PWM operation
* adc_simple:  Simple (software mode) Analog to Digital conversion 
* adc_burstmode:  Continuous sample (burst mode) Analog to Digital conversion (started but not finished)
* gpio_interrupt:  GPIO external interrupt
* motor:  advanced motor control (a work in progress). uses PWM to control motor speed, 
GPIO ports to control direction, external interrupt to sense motor speed (via IR sensor and optical wheel)

# Example
Some people may find that the ARM Cortex uProc is difficult to program when compared to simpler 8-bit microprocessors.
With more capability there is usually more complexity.  But with the right 
abstraction layer(s) in place, this complexity is easily mitigated.  For example, 
most of the samples on the web use standard bit shifting and masking 
to set the hardware configuration registers.  This makes the code very hard to read and is also error 
prone.  My preference is to use C++ and overlay the hardware register with a 
structure containing named bitfields. The bitfields are written to look like 
the LPC11xx User Manual, so there is less room for error. For example, this code initializes 
the Pulse Width Modulation(PWM) hardware with 2 outputs:

```C++
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
```

The goal is to let the compiler figure out all of the masks and bit shifts for us.  
The end result is code that reads like the LPC11xx User Manual and is 
nearly as efficient as the direct bit-masking implementations.

NOTE:  Any register that clears a status immediately when read
must be read into a local variable first.  Then you can use the 
structures/bitfields to acccess the sub fields.


If you want to contribute to this project please let me know.

Cheers,
Chris Gorman
engineergorman:at:gmail+com+ (figure it out, no spam please)
http://engineergorman.com/

# Requirements
I purchased my LPC1114FN28/102 ($2.50) from Mouser along with an ARM-JTAG-COOCOX 
programmer/SWD debugger ($38).  You can use any development environment 
suitable for LPC11xx.  There are some excellent open source solutions,
such as the one at CooCox.org.

My preference is to use the free version of Keil's MDK-ARM IDE (MDK-Lite version 4.7) with Olimex's
ARM-JTAG-COOCOX programmer/SWD debugger.

NOTE:  The latest version of Keil's MDK-Lite doesn't work with the ARM-JTAG-COOCOX programmer.
Use MDK-Lite Version 4.7 if you can get it.  Otherwise you may need to consider a different
programmer/SWD debugger device that is compatible with the latest version of MDK-Lite.

# License
The engineergorman\lpc1114fn28 framework is licensed under the MIT license:

Copyright (C) 2014 by Chris Gorman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
