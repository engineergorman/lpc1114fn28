#include "core.h"
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

// TODO: add license from rt library

// TODO: evaluate the need to implement the rt_hw_interrupt_enable() and disable.

#define IER_RBR         0x01
#define IER_THRE        0x02
#define IER_RLS         0x04

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01

#define LSR_RDR         0x01
#define LSR_OE          0x02
#define LSR_PE          0x04
#define LSR_FE          0x08
#define LSR_BI          0x10
#define LSR_THRE        0x20
#define LSR_TEMT        0x40
#define LSR_RXFE        0x80



typedef unsigned char rt_uint8_t;
typedef unsigned int rt_ubase_t;
typedef int rt_base_t;
typedef int rt_size_t;
typedef unsigned int rt_uint32_t;

#define RT_UART_RX_BUFFER_SIZE	64

struct rt_uart_lpc
{
	/* buffer for reception */
	rt_uint8_t read_index, save_index;
	rt_uint8_t rx_buffer[RT_UART_RX_BUFFER_SIZE];
} uart_device;

// TODO:  maybe we need to implement this?
rt_ubase_t rt_hw_interrupt_disable()
{
	return 0;
}

void rt_hw_interrupt_enable(rt_ubase_t x)
{
}

extern "C" void UART_IRQHandler(void)
{
	rt_ubase_t level, iir;
	rt_uart_lpc* uart = &uart_device;

	/* read IIR and clear it */
	iir = LPC_UART->IIR;

	iir >>= 1;                          /* skip pending bit in IIR */
	iir &= 0x07;                    /* check bit 1~3, interrupt identification */

	if (iir == IIR_RDA)         /* Receive Data Available */
	{
		/* Receive Data Available */
		uart->rx_buffer[uart->save_index] = LPC_UART->RBR;

		level = rt_hw_interrupt_disable();
		uart->save_index ++;
		if (uart->save_index >= RT_UART_RX_BUFFER_SIZE)
			uart->save_index = 0;
		rt_hw_interrupt_enable(level);
	}
}

void uart_init (unsigned int baudrate)
{
	/* device initialization */
	//memset(uart_device.rx_buffer, 0, sizeof(uart_device.rx_buffer));
	uart_device.read_index = uart_device.save_index = 0;

	rt_uint32_t Fdiv;
	volatile rt_uint32_t regVal;	// volatile keyword gets rid of warning

	/* Init UART Hardware */
	LPC_IOCON->PIO1_6 &= ~0x07;    /*  UART I/O config */
	LPC_IOCON->PIO1_6 |= 0x01;     /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;     /* UART TXD */

	/* Enable UART clock */
	//SYSCON_SYSAHBCLKCTRL_UART(SYSAHBCLKCTRL_ENABLE);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<12);
	LPC_SYSCON->UARTCLKDIV = 0x1;     /* divided by 1.  this also turns on the UART clock */

	// DLAB is the divisor latch access bit
	LPC_UART->LCR = 0x83;             /* DLAB = 1, 8 bits, no Parity, 1 Stop bit   (sec. 13.5.7) */

	/* set baudrate */
	//regVal = LPC_SYSCON->UARTCLKDIV;
	unsigned int  ahbClockDiv = LPC_SYSCON->SYSAHBCLKDIV;
	unsigned int  uartClockDiv = LPC_SYSCON->UARTCLKDIV;
	unsigned int pllclock = (SystemFrequency/ahbClockDiv)/uartClockDiv;
	Fdiv = pllclock / (16 * baudrate);

	LPC_UART->DLM = Fdiv / 256;
	LPC_UART->DLL = Fdiv % 256;
	LPC_UART->LCR = 0x03;           /* DLAB = 0 */
	LPC_UART->FCR = 0x07;           /* Enable and reset TX and RX FIFO. */  // (sec. 13.5.6)

	/* Read to clear the line status. */
	regVal = LPC_UART->LSR;

	/* Ensure a clean start, no data in either TX or RX FIFO. */
	while (( LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT) );
	while ( LPC_UART->LSR & LSR_RDR )
	{
			regVal = LPC_UART->RBR; /* Dump data from RX FIFO */
	}

	LPC_UART->IER = IER_RBR | IER_THRE | IER_RLS;   /* Enable UART interrupt */
}

void uart_open()
{
	/* Enable the UART Interrupt */
	NVIC_EnableIRQ(UART_IRQn);
}

void uart_close()
{
	/* Disable the UART Interrupt */
	NVIC_DisableIRQ(UART_IRQn);
}

int uart_read(void* buffer, int size)
{
	rt_uint8_t* ptr;
	struct rt_uart_lpc *uart = &uart_device;

	/* point to buffer */
	ptr = (rt_uint8_t*) buffer;
	while (size)
	{
		/* interrupt receive */
		rt_base_t level;

		/* disable interrupt */
		level = rt_hw_interrupt_disable();
		if (uart->read_index != uart->save_index)
		{
			*ptr = uart->rx_buffer[uart->read_index];

			uart->read_index ++;
			if (uart->read_index >= RT_UART_RX_BUFFER_SIZE)
				uart->read_index = 0;
		}
		else
		{
			/* no data in rx buffer */

			/* enable interrupt */
			rt_hw_interrupt_enable(level);
			break;
		}

		/* enable interrupt */
		rt_hw_interrupt_enable(level);

		ptr ++;
		size --;
	}

	return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}

void uart_write(const void* buffer, int size)
{
	char *ptr;
	ptr = (char*)buffer;

	while ( size != 0 )
	{
		/* THRE status, contain valid data */
		while ( !(LPC_UART->LSR & LSR_THRE) );

		/* write data */
		LPC_UART->THR = *ptr;

		ptr++;
		size--;
	}
}

void uart_write_str(const void* buffer)
{
	char *ptr;
	ptr = (char*)buffer;

	while ( *ptr )
	{
		/* THRE status, contain valid data */
		while ( !(LPC_UART->LSR & LSR_THRE) );

		/* write data */
		LPC_UART->THR = *ptr;

		ptr++;
	}
}
