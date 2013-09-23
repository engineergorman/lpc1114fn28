#include <LPC11xx.h>


struct LPC_TMR
{
  //__IO uint32_t IR;                     /*!< Offset: 0x000 Interrupt Register (R/W) */
	struct _IR
	{
		__IO uint32_t MR0 : 1;		// MR0 interrupt
		__IO uint32_t MR1 : 1;		// MR1 interrupt
		__IO uint32_t MR2 : 1;		// MR2 interrupt
		__IO uint32_t MR3 : 1;		// MR3 interrupt
		__IO uint32_t CR0 : 1;		// CR0 interrupt
		__IO uint32_t reserved : 27;	
	} IR;
	
  //__IO uint32_t TCR;                    /*!< Offset: 0x004 Timer Control Register (R/W) */
	struct _TCR
	{
		__IO uint32_t CEn : 1;		// Counter Enable
		__IO uint32_t CRst : 1;		// Counter Reset
		__IO uint32_t reserved : 30;	
	} TCR;	

  __IO uint32_t TC;                     /*!< Offset: 0x008 Timer Counter Register (R/W) */
  __IO uint32_t PR;                     /*!< Offset: 0x00C Prescale Register (R/W) */
  __IO uint32_t PC;                     /*!< Offset: 0x010 Prescale Counter Register (R/W) */

  //__IO uint32_t MCR;                    /*!< Offset: 0x014 Match Control Register (R/W) */
	struct _MCR
	{
		__IO uint32_t MR0I : 1;	// Interrupt on MR0: an interrupt is generated when MR0 matches the value in the TC
		__IO uint32_t MR0R : 1; 	// Reset on MR0: the TC will be reset if MR0 matches it.
		__IO uint32_t MR0S : 1; 	// Stop on MR0: the TC and PC will be stopped and TCR[0] will be set to 0 if MR0 matches the TC

		__IO uint32_t MR1I : 1;
		__IO uint32_t MR1R : 1;
		__IO uint32_t MR1S : 1;

		__IO uint32_t MR2I : 1;
		__IO uint32_t MR2R : 1;
		__IO uint32_t MR2S : 1;
		
		__IO uint32_t MR3I : 1;
		__IO uint32_t MR3R : 1;
		__IO uint32_t MR3S : 1;
		__IO uint32_t reserved : 20;	
	} MCR;

  __IO uint32_t MR0;                    /*!< Offset: 0x018 Match Register 0 (R/W) */
  __IO uint32_t MR1;                    /*!< Offset: 0x01C Match Register 1 (R/W) */
  __IO uint32_t MR2;                    /*!< Offset: 0x020 Match Register 2 (R/W) */
  __IO uint32_t MR3;                    /*!< Offset: 0x024 Match Register 3 (R/W) */
  //__IO uint32_t CCR;                    /*!< Offset: 0x028 Capture Control Register (R/W) */
	struct _CCR
	{
		__IO uint32_t RISING_EDGE : 1; 	// load CR0 with TC on rising edge
		__IO uint32_t FALLING_EDGE : 1; 	// load CR0 with TC on falling edge
		__IO uint32_t INTERRUPT : 1; 	// generate interrupt on capture event
		__IO uint32_t reserved : 29;
	} CCR;

  __I  uint32_t CR0;                    /*!< Offset: 0x02C Capture Register 0 (R/ ) */
  __I  uint32_t CR1;                    /*!< Offset: 0x030 Capture Register 1 (R/ ) */
       uint32_t RESERVED1[2];
  __IO uint32_t EMR;                    /*!< Offset: 0x03C External Match Register (R/W) */
       uint32_t RESERVED2[12];
  //__IO uint32_t CTCR;                   /*!< Offset: 0x070 Count Control Register (R/W) */
	struct _CTCR
	{
		__IO uint32_t CTM : 2;	// Counter/Timer Mode
		__IO uint32_t CIS : 2;	// Count Input Select
		__IO uint32_t reserved : 28;
	} CTCR;
	
  //__IO uint32_t PWMC;                   /*!< Offset: 0x074 PWM Control Register (R/W) */
	struct _PWMC
	{
		__IO uint32_t PWMEN0 : 1; 	// PWM channel 0 enable
		__IO uint32_t PWMEN1 : 1; 	// PWM channel 1 enable
		__IO uint32_t PWMEN2 : 1; 	// PWM channel 2 enable
		__IO uint32_t PWMEN3 : 1; 	// PWM channel 3 enable;  preferred PWM cycle register
		__IO uint32_t reserved : 28;
	} PWMC;
};

#define TMR16B0           ((LPC_TMR    *) LPC_CT16B0_BASE)
#define TMR16B1           ((LPC_TMR    *) LPC_CT16B1_BASE)
#define TMR32B0           ((LPC_TMR    *) LPC_CT32B0_BASE)
#define TMR32B1           ((LPC_TMR    *) LPC_CT32B1_BASE)


/////////////////////////////////////////////////
//	LPC_TMR16Bn Section
//    16-bit Timers
/////////////////////////////////////////////////

//
//
//  Timer 16B0
//
//

// Timer interrupt register.  Read to determine which interrupt is pending.
// Write a 1 to clear.  Writing a 0 has no effect.
struct _TMR_IR
{
	uint32_t MR0 : 1;		// MR0 interrupt
	uint32_t MR1 : 1;		// MR1 interrupt
	uint32_t MR2 : 1;		// MR2 interrupt
	uint32_t MR3 : 1;		// MR3 interrupt
	uint32_t CR0 : 1;		// CR0 interrupt
	uint32_t reserved : 27;	
};

inline uint32_t TMR16B0_IR_MR0()
{
	return ((_TMR_IR*)&LPC_TMR16B0->IR)->MR0;
}

inline void TMR16B0_IR_MR0_Clear()
{
	((_TMR_IR*)&LPC_TMR16B0->IR)->MR0 = 1;
}

inline uint32_t TMR16B0_IR_MR1()
{
	return ((_TMR_IR*)&LPC_TMR16B0->IR)->MR1;
}

inline void TMR16B0_IR_MR1_Clear()
{
	((_TMR_IR*)&LPC_TMR16B0->IR)->MR1 = 1;
}

inline uint32_t TMR16B0_IR_MR2()
{
	return ((_TMR_IR*)&LPC_TMR16B0->IR)->MR2;
}

inline void TMR16B0_IR_MR2_Clear()
{
	((_TMR_IR*)&LPC_TMR16B0->IR)->MR2 = 1;
}

inline uint32_t TMR16B0_IR_MR3()
{
	return ((_TMR_IR*)&LPC_TMR16B0->IR)->MR3;
}

inline void TMR16B0_IR_MR3_Clear()
{
	((_TMR_IR*)&LPC_TMR16B0->IR)->MR3 = 1;
}

inline uint32_t TMR16B0_IR_CR0()
{
	return ((_TMR_IR*)&LPC_TMR16B0->IR)->CR0;
}

inline void TMR16B0_IR_CR0_Clear()
{
	((_TMR_IR*)&LPC_TMR16B0->IR)->CR0 = 1;
}

// LPC_TMR16B0->TCR
// The Timer Control Register (TCR) is used to control the operation of the counter/timer.
struct _TCR
{
	uint32_t CEn : 1;		// Counter Enable
	uint32_t CRst : 1;		// Counter Reset
	uint32_t reserved : 30;	
};

#define TCR_DISABLE 0x0
#define TCR_ENABLE 0x1
inline void TMR16B0_TCR_CEn(uint32_t val)
{
	((_TCR*)&LPC_TMR16B0->TCR)->CEn = val;
}

#define TCR_NOT_RESET 0x0
#define TCR_RESET 0x1
inline void TMR16B0_TCR_CRst(uint32_t val)
{
	((_TCR*)&LPC_TMR16B0->TCR)->CRst = val;
}

// LPC_TMR16B0->TC
// Timer Counter  
// The 16-bit Timer Counter is incremented when 
// the Prescale Counter reaches its terminal count.
inline void TMR16B0_TC_Set(uint32_t val)
{
	LPC_TMR16B0->TC = val & 0x0000FFFF;
}
inline uint32_t TMR16B0_TC_Get()
{
	return LPC_TMR16B0->TC;
}

// LPC_TMR16B0->PR
// Prescale Register
// The 16-bit Prescale Register specifies the maximum value for the Prescale Counter.
inline void TMR16B0_PR_Set(uint32_t val)
{
	LPC_TMR16B0->PR = val & 0x0000FFFF;
}
inline uint32_t TMR16B0_PR_Get()
{
	return LPC_TMR16B0->PR;
}

// LPC_TMR16B0->PC
// Prescale Counter
// The Prescale Counter is incremented on every PCLK.
// When it reaches the value stored in the Prescale Register, 
// the Timer Counter is incremented, and the Prescale Counter is 
// reset on the next PCLK.  This causes the TC to increment on 
// every PCLK when PR = 0, every 2 PCLKs when PR = 1, etc
inline void TMR16B0_PC_Set(uint32_t val)
{
	LPC_TMR16B0->PC = val & 0x0000FFFF;
}
inline uint32_t TMR16B0_PC_Get()
{
	return LPC_TMR16B0->PC;
}

// LPC_TMR16B0->MCR
// Match Control Register
// The Match Control Register is used to control what operations are 
// performed when one of the Match Registers matches the Timer Counter.
struct _MCR
{
	uint32_t MR0I : 1;	// Interrupt on MR0: an interrupt is generated when MR0 matches the value in the TC
	uint32_t MR0R : 1; 	// Reset on MR0: the TC will be reset if MR0 matches it.
	uint32_t MR0S : 1; 	// Stop on MR0: the TC and PC will be stopped and TCR[0] will be set to 0 if MR0 matches the TC

	uint32_t MR1I : 1;
	uint32_t MR1R : 1;
	uint32_t MR1S : 1;

	uint32_t MR2I : 1;
	uint32_t MR2R : 1;
	uint32_t MR2S : 1;
	
	uint32_t MR3I : 1;
	uint32_t MR3R : 1;
	uint32_t MR3S : 1;
	uint32_t reserved : 20;	
};

#define MCR_DISABLE 0x0
#define MCR_ENABLE 0x1

// MR0
inline void TMR16B0_MCR_MR0I(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR0I = val;
}

inline void TMR16B0_MCR_MR0R(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR0R = val;
}

inline void TMR16B0_MCR_MR0S(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR0S = val;
}
// MR1
inline void TMR16B0_MCR_MR1I(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR1I = val;
}

inline void TMR16B0_MCR_MR1R(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR1R = val;
}

inline void TMR16B0_MCR_MR1S(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR1S = val;
}
// MR2
inline void TMR16B0_MCR_MR2I(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR2I = val;
}

inline void TMR16B0_MCR_MR2R(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR2R = val;
}

inline void TMR16B0_MCR_MR2S(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR2S = val;
}
// MR3
inline void TMR16B0_MCR_MR3I(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR3I = val;
}

inline void TMR16B0_MCR_MR3R(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR3R = val;
}

inline void TMR16B0_MCR_MR3S(uint32_t val)
{
	((_MCR*)&LPC_TMR16B0->MCR)->MR3S = val;
}

// LPC_TMR16B0->MR0
// Match Register 0
// The Match register values are continuously compared to the Timer Counter value. When 
// the two values are equal, actions can be triggered automatically. The action possibilities 
// are to generate an interrupt, reset the Timer Counter, or stop the timer. Actions are 
// controlled by the settings in the MCR register
inline void TMR16B0_MR0_Set(uint32_t val)
{
	LPC_TMR16B0->MR0 = val & 0x0000FFFF;
}

inline uint32_t TMR16B0_MR0_Get()
{
	return LPC_TMR16B0->MR0;
}

// LPC_TMR16B0->MR1
// Match Register 1
inline void TMR16B0_MR1_Set(uint32_t val)
{
	LPC_TMR16B0->MR1 = val & 0x0000FFFF;
}

inline uint32_t TMR16B0_MR1_Get()
{
	return LPC_TMR16B0->MR1;
}

// LPC_TMR16B0->MR2
// Match Register 2
inline void TMR16B0_MR2_Set(uint32_t val)
{
	LPC_TMR16B0->MR2 = val & 0x0000FFFF;
}

inline uint32_t TMR16B0_MR2_Get()
{
	return LPC_TMR16B0->MR2;
}

// LPC_TMR16B0->MR3
// Match Register 3
inline void TMR16B0_MR3_Set(uint32_t val)
{
	LPC_TMR16B0->MR3 = val & 0x0000FFFF;
}

inline uint32_t TMR16B0_MR3_Get()
{
	return LPC_TMR16B0->MR3;
}

// LPC_TMR16B0->CR0
// Capture Register 0
inline void TMR16B0_CR0_Set(uint32_t val)
{
	LPC_TMR16B0->CR0 = val & 0x0000FFFF;
}

inline uint32_t TMR16B0_CR0_Get()
{
	return LPC_TMR16B0->CR0;
}

// LPC_TMR16B0->CCR
// Capture Control Register 
struct _CCR
{
	uint32_t RISING_EDGE : 1; 	// load CR0 with TC on rising edge
	uint32_t FALLING_EDGE : 1; 	// load CR0 with TC on falling edge
	uint32_t INTERRUPT : 1; 	// generate interrupt on capture event
	uint32_t reserved : 29;
};

#define CCR_DISABLE 0x0
#define CCR_ENABLE 0x1

inline void TMR16B0_CCR_RISING_EDGE(uint32_t val)
{
	((_CCR*)&LPC_TMR16B0->CCR)->RISING_EDGE = val;
}

inline void TMR16B0_CCR_FALLING_EDGE(uint32_t val)
{
	((_CCR*)&LPC_TMR16B0->CCR)->FALLING_EDGE = val;
}

inline void TMR16B0_CCR_INTERRUPT(uint32_t val)
{
	((_CCR*)&LPC_TMR16B0->CCR)->INTERRUPT = val;
}

// LPC_TMR16B0->PWMC
// PWM Control register
// For timer 0, three single-edge controlled PWM outputs can be selected on the 
// CT16B0_MAT[2:0] outputs. For timer 1, two single-edged PWM outputs can be selected 
// on the CT16B1_Mat[1:0] outputs. One additional match register determines the PWM 
// cycle length. When a match occurs in any of the other match registers, the PWM output is 
// set to HIGH. The timer is reset by the match register that is configured to set the PWM 
// cycle length. When the timer is reset to zero, all currently HIGH match outputs configured 
// as PWM outputs are cleared.
// Note: It is recommended to use match channel 3 to set 
// the PWM cycle because it is not pinned out.
struct _PWMC
{
	uint32_t PWMEN0 : 1; 	// PWM channel 0 enable
	uint32_t PWMEN1 : 1; 	// PWM channel 1 enable
	uint32_t PWMEN2 : 1; 	// PWM channel 2 enable
	uint32_t PWMEN3 : 1; 	// PWM channel 3 enable;  preferred PWM cycle register
	uint32_t reserved : 28;
};

#define PWMC_DISABLE 0x0
#define PWMC_ENABLE 0x1

inline void TMR16B0_PWMC_PWMEN0(uint32_t val)
{
	((_PWMC*)&LPC_TMR16B0->PWMC)->PWMEN0 = val;
}

inline void TMR16B0_PWMC_PWMEN1(uint32_t val)
{
	((_PWMC*)&LPC_TMR16B0->PWMC)->PWMEN1 = val;
}

inline void TMR16B0_PWMC_PWMEN2(uint32_t val)
{
	((_PWMC*)&LPC_TMR16B0->PWMC)->PWMEN2 = val;
}

inline void TMR16B0_PWMC_PWMEN3(uint32_t val)
{
	((_PWMC*)&LPC_TMR16B0->PWMC)->PWMEN3 = val;
}

