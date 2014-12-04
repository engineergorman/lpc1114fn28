#include <LPC11xx.h>

//////////////////////////////////////////////////////////////////
//      Synchronous Serial Communication (SSP)                  //
//////////////////////////////////////////////////////////////////

// SPI read and write buffer size
#define SPI_FIFOSIZE 8 


struct _SSP_CR0
{
  __IO uint32_t DSS : 4;	// Data Size Select, number of bits transferred in each frame
	__IO uint32_t FRF : 2;	// Frame Format (SPI, TI, Microwire)
	__IO uint32_t CPOL : 1;	// Clock Out Polarity. This bit is only used in SPI mode.
	__IO uint32_t CPHA : 1;	// Clock Out Phase. This bit is only used in SPI mode.
	__IO uint32_t SCR : 8;	// Serial Clock Rate.   clock = PCLK / (CPSDVSR * [SCR+1])
};

#define SSP_CR0_DSS_4BIT	0x3
#define SSP_CR0_DSS_5BIT	0x4
#define SSP_CR0_DSS_6BIT	0x5
#define SSP_CR0_DSS_7BIT	0x6
#define SSP_CR0_DSS_8BIT	0x7
#define SSP_CR0_DSS_9BIT	0x8
#define SSP_CR0_DSS_10BIT	0x9
#define SSP_CR0_DSS_11BIT	0xA
#define SSP_CR0_DSS_12BIT	0xB
#define SSP_CR0_DSS_13BIT	0xC
#define SSP_CR0_DSS_14BIT	0xD
#define SSP_CR0_DSS_15BIT	0xE
#define SSP_CR0_DSS_16BIT	0xF

#define SSP_CR0_FRF_SPI	0x0
#define SSP_CR0_FRF_TI	0x1
#define SSP_CR0_FRF_MICROWIRE	0x2

#define SSP_CR0_CPOL_CLOCK_LOW	0x0		// SPI controller maintains the bus clock low between frames.
#define SSP_CR0_CPOL_CLOCK_HIGH	0x1		// SPI controller maintains the bus clock high between frames.

#define SSP_CR0_CPHA_AWAY_FROM	0x0		// SPI controller captures serial data on the first clock transition
																			//   of the frame, that is, the transition away from the inter-frame
																			//   state of the clock line.
#define SSP_CR0_CPHA_BACK_TO	0x1			// SPI controller captures serial data on the second clock
																			//   transition of the frame, that is, the transition back to the
																			//   inter-frame state of the clock line.

struct _SSP_CR1
{
  __IO uint32_t LBM : 1;	// Loop Back Mode  (ENABLE or DISABLE)
  __IO uint32_t SSE : 1;	// SPI Enable  (ENABLE or DISABLE)
  __IO uint32_t MS : 1;		// Master/Slave Mode.  This bit can only be written when the
													//   SSE bit is 0.
  __IO uint32_t SOD : 1;	// Slave Output Disable. This bit is relevant only in slave
													//   mode (MS = SSP_CR1_MS_SLAVE)
};

#define SSP_CR1_MS_MASTER	0x0
#define SSP_CR1_MS_SLAVE	0x1

#define SSP_CR1_SOD_OUTPUT_ENABLED	0x0
#define SSP_CR1_SOD_OUTPUT_DISABLED	0x1	// blocked from driving MISO line

// NOTE:  not sure if reading this register resets all bits.  if so, then
//  you need to read the register once and use this structure to access states.
struct _SSP_SR
{
  __I uint32_t TFE : 1;	// Transmit FIFO Empty. This bit is 1 is the Transmit FIFO is
												//   empty, 0 if not.
  __I uint32_t TNF : 1;	// Transmit FIFO Not Full. This bit is 0 if the Tx FIFO is full, 1 if not.
  __I uint32_t RNE : 1;	// Receive FIFO Not Empty. This bit is 0 if the Receive FIFO is
												//   empty, 1 if not.
  __I uint32_t RFF : 1;	// Receive FIFO Full. This bit is 1 if the Receive FIFO is full, 0 if not.
  __I uint32_t BSY : 1;	// Busy. This bit is 0 if the SPI controller is idle, 1 if it is currently
												//   sending/receiving a frame and/or the Tx FIFO is not empty.
};

struct _SSP_CPSR
{
  __IO uint32_t CPSDVSR : 8;	// Clock Prescale Register (divisor).  even value between 2 and 254
};

struct _SSP_IMSC
{
  __IO uint32_t RORIM : 1;	// Software should set this bit to enable interrupt when a Receive
														//   Overrun occurs, that is, when the Rx FIFO is full and another frame is
														//   completely received. The ARM spec implies that the preceding frame
														//   data is overwritten by the new frame data when this occurs.
  __IO uint32_t RTIM : 1;		// Software should set this bit to enable interrupt when a Receive
														//   Time-out condition occurs. A Receive Time-out occurs when the Rx
														//   FIFO is not empty, and no has not been read for a time-out period.
														//   The time-out period is the same for master and slave modes and is
														//   determined by the SSP bit rate: 32 bits at PCLK / (CPSDVSR*[SCR+1])
  __IO uint32_t RXIM : 1;		// Software should set this bit to enable interrupt when the Rx FIFO is at
														//   least half full.
  __IO uint32_t TXIM : 1;		// Software should set this bit to enable interrupt when the Tx FIFO is at
														//   least half empty.
};

struct _SSP_RIS
{
};

struct _SSP_MIS
{
};

struct _SSP_ICR
{
};

struct _SSP
{
  __IO _SSP_CR0 CR0;                    /*!< Offset: 0x000 Control Register 0 (R/W) */
  __IO _SSP_CR1 CR1;                    /*!< Offset: 0x004 Control Register 1 (R/W) */
  __IO uint32_t DR;                     /*!< Offset: 0x008 Data Register (R/W) */
  __I  _SSP_SR SR;                     /*!< Offset: 0x00C Status Registe (R/ ) */
  __IO _SSP_CPSR CPSR;                   /*!< Offset: 0x010 Clock Prescale Register (R/W) */
  __IO _SSP_IMSC IMSC;                   /*!< Offset: 0x014 Interrupt Mask Set and Clear Register (R/W) */
  __IO uint32_t RIS;                    /*!< Offset: 0x018 Raw Interrupt Status Register (R/W) */
  __IO uint32_t MIS;                    /*!< Offset: 0x01C Masked Interrupt Status Register (R/W) */
  __IO uint32_t ICR;                    /*!< Offset: 0x020 SSPICR Interrupt Clear Register (R/W) */
};

#define SSP0CON             ((_SSP  *) LPC_SSP0 )
#define SSP1CON             ((_SSP  *) LPC_SSP1 )
