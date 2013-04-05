# Code Samples for NXP's LPC1114FN28/102 Microprocessor
The LPC1114FN28/102 is a low cost (about $1.50 - $2.50 each)
yet highly capable microprocessor in a DIP28 form factor.  This makes
the microprocessor easy for prototyping with a breadboard.
The chip is a 32-bit ARM Cortex M0 running up to 48Mhz with the 
internal oscillator.  Features: 32K flash, 4K RAM, 22 GPIO, UART,
SPI, I2C, ADC, DAC, PWM, nested interrupts, etc.  The full datasheet is
here:  http://www.nxp.com/documents/data_sheet/LPC111X.pdf

This project is exploring the use of C++ to create a highy readable thin hardware abstraction
layer for setting up the chip's core features and perhipherals.  Current
examples include:
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
to set the hardware configuration registers.  This makes the code very hard to read and also error 
prone.  My preference is to use C++ and overlay the hardware register with a 
structure containing named bitfields. The bitfields are written to look like 
the LPC11xx User Manual, so there is less room for error. For example:

```C++
    // LPC_IOCON->PIO0_8
    // I/O configuration for pin PIO0_8/MISO0/CT16B0_MAT0 (R/W)
    struct _PIO0_8
    {
        uint32_t FUNC : 3;	// pin function
        uint32_t MODE : 2;	// pull up/down resistor mode
        uint32_t HYS : 1;		// Hysteresis.
        uint32_t reserved0 : 4;
        uint32_t OD : 1;		// Selects pseudo open-drain mode
        uint32_t reserved1 : 21;
    };
```

I then use inline functions to cast the ptr to register to the structure and then set/get the bit field values.  

```C++
    #define PIO0_8_FUNC_GPIO 0x0
    #define PIO0_8_FUNC_MISO 0x1
    #define PIO0_8_FUNC_TIMER 0x2
    inline void IOCON_PIO0_8_FUNC(uint32_t val)
    {
        ((_PIO0_8*)&LPC_IOCON->PIO0_8)->FUNC = val;
    }
```

The end result is code that reads like the LPC11xx User Manual and is 
just as efficient as C implementations because of the inlines.   
And best of all I let the compiler figure out all of the masks and bit shifts for me.  
Here is what programming the 16-bit timer for PWM looks like with 
these inline functions:

```C++
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
```

Of course this function could be wrapped in another abstraction layer, 
but at least the code interfacing with the hardware is easy to read.  

If you want to contribute to this project please let me know.

Cheers,
Chris Gorman
engineergorman@gmail.com
http://engineergorman.com/

# Requirements
I purchased my LPC1114FN28/102 ($2.50) from Mouser along with an ARM-JTAG-COOCOX 
programmer/SWD debugger ($38).  You can use any development environment 
suitable for LPC11xx.  There are some excellent open source solutions,
such as the one at CooCox.org.

My prefernce is to use the free version of Keil's MDK-ARM IDE with Olimex's
ARM-JTAG-COOCOX programmer/SWD debugger.

# License
The engineergorman\lpc1114fn28 framework is licensed under the MIT license:

Copyright (C) 2013 by Chris Gorman

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
