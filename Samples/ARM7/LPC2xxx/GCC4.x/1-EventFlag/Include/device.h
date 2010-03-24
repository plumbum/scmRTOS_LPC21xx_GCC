/******************************************************************************
 *
 * $RCSfile: $
 * $Revision: $
 *
 * This module provides information about the project configuration
 * Copyright 2004, R O SoftWare
 * No guarantees, warrantees, or promises, implied or otherwise.
 * May be used for hobby or commercial purposes provided copyright
 * notice remains intact.
 *
 *****************************************************************************/
#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "LPC2103.h"

#ifndef __ASSEMBLER__
#include <inttypes.h>


// declare functions and values from crt0.S & the linker control file
extern "C" void reset(void);
// extern void exit(void);
extern "C" void abort(void);
// maybe add interrupt vector addresses

#endif /* __ASSEMBLER__ */

#define USE_FIO 1
#define USE_PLL 1


#define RTOS_TICK_RATE 1000 // Hz


#define VIC_SW 1
#define VIC_TIMER3 27


// some handy DEFINES
#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif

#define HOST_BAUD           (115200)

#define WDOG()

// PLL setup values are computed within the LPC include file
// It relies upon the following defines
#define FOSC                (14745600UL)  // Master Oscillator Freq.
#define PLL_MUL             (4)         // PLL Multiplier
#define CCLK                (FOSC * PLL_MUL) // CPU Clock Freq.

// Pheripheral Bus Speed Divider
#define PBSD                1           // MUST BE 1, 2, or 4
#define PCLK                (CCLK / PBSD) // Pheripheal Bus Clock Freq.


///////////////////////////////////////////////////////////////////////////////
// MAM defines
#define MAMCR_OFF     0
#define MAMCR_PART    1
#define MAMCR_FULL    2

#define MAMTIM_CYCLES (((CCLK) + 19999999) / 20000000)

///////////////////////////////////////////////////////////////////////////////
// MEMMAP defines
#define MEMMAP_BBLK   0                 // Interrupt Vectors in Boot Block
#define MEMMAP_FLASH  1                 // Interrupt Vectors in Flash
#define MEMMAP_SRAM   2                 // Interrupt Vectors in SRAM

///////////////////////////////////////////////////////////////////////////////
// PLL defines & computations
// Compute the value of PLL_DIV and test range validity
// FOSC & PLL_MUL should be defined in project configuration file (config.h)
#ifndef CCLK
#define CCLK          (FOSC * PLL_MUL)  // CPU Clock Freq.
#endif

#define FCCO_MAX      (320000000)       // Max CC Osc Freq.
#define PLL_DIV       (FCCO_MAX / (2 * CCLK)) // PLL Divider
#define FCCO          (FOSC * PLL_MUL * 2 * PLL_DIV) // CC Osc. Freq.

// PLLCON Register Bit Definitions
#define PLLCON_PLLE   (1 << 0)          // PLL Enable
#define PLLCON_PLLC   (1 << 1)          // PLL Connect

// PLLCFG Register Bit Definitions
#define PLLCFG_MSEL   ((PLL_MUL - 1) << 0) // PLL Multiplier
#define PLLCFG_PSEL   ((PLL_DIV - 1) << 5) // PLL Divider

// PLLSTAT Register Bit Definitions
#define PLLSTAT_LOCK  (1 << 10)         // PLL Lock Status Bit

///////////////////////////////////////////////////////////////////////////////
// VPBDIV defines & computations
#define VPBDIV_VALUE  (PBSD & 0x03)     // VPBDIV value


// Do some value range testing
#if ((FOSC < 10000000) || (FOSC > 25000000))
#error Fosc out of range (10MHz-25MHz)
#error correct and recompile
#endif

#if ((CCLK < 10000000) || (CCLK > 60000000))
#error cclk out of range (10MHz-60MHz)
#error correct PLL_MUL and recompile
#endif

#if ((FCCO < 150000000) || (FCCO > 320000000))
#error Fcco out of range (156MHz-320MHz)
#error internal algorithm error
#endif

#if ((PBSD != 1) && (PBSD != 2) && (PBSD != 4))
#error Pheripheal Bus Speed Divider (PBSD) illegal value (1, 2, or 4)
#endif


#endif
