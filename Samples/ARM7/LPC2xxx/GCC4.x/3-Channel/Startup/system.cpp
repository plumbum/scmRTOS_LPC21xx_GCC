#include "device.h"

#include <scmRTOS.h>

extern "C" void ContextSwitcher_ISR();

extern "C" void sysInit(void);
void sysInit(void)
{
//  lowInit();                            // setup clocks and processor port pins
    // setup the parallel port pin
    PINSEL0 = 0x00000000;
    PINSEL1 = 0x00000000;

#if (USE_FIO == 1)
    SCS |= 0x01; // Enable Fast GPIO
    FIO0MASK = 0x00000000;
    FIO0SET = 0x00000000;       //Initialize pins to high level
    FIO0CLR = 0xFFFFFFFF;       //Initialize pins to low level
    FIO0DIR = 0x00000000;       //Set pin direction
#else
    IOSET = 0x00000000;       //Initialize pins to high level
    IOCLR = 0xFFFFFFFF;       //Initialize pins to low level
    IODIR = 0x00000000;       //Set pin direction
#endif


    // setup & enable the MAM
    MAMTIM = MAMTIM_CYCLES;
    MAMCR = MAMCR_FULL;

    // set the peripheral bus speed
    // value computed from config.h
    VPBDIV = VPBDIV_VALUE;                // set the peripheral bus clock speed

    // set the interrupt controller defaults
#if defined(RAM_RUN)
    MEMMAP = MEMMAP_SRAM;                 // map interrupt vectors space into SRAM
#elif defined(ROM_RUN)
    MEMMAP = MEMMAP_FLASH;                // map interrupt vectors space into FLASH
#else
#error RUN_MODE not defined!
#endif

    VICIntEnClear = 0xFFFFFFFF;           // clear all interrupts
    VICIntSelect = 0x00000000;            // clear all FIQ selections
    VICDefVectAddr = (uint32_t)reset;     // point unvectored IRQs to reset()

    VICVectAddr0   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr1   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr2   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr3   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr4   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr5   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr6   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr7   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr8   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr9   = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr10  = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr11  = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr12  = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr13  = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr14  = (unsigned int)0;      /* Set the vector address                             */
    VICVectAddr15  = (unsigned int)0;      /* Set the vector address                             */

// ***************************************************************************
// ** TIMER0
// ***************************************************************************
//          RTOS system timer
    T0IR = 0x000000FF;                      // clear int requests
    T0TCR = (1<<1);                         // reset timer
    T0PR = 0;
    T0MR0 = PCLK / RTOS_TICK_RATE;          // int at TICK_RATE
    T0MCR = TMCR_MR0_I | TMCR_MR0_R;
    T0TCR = (1<<0);                         // CE = 1, enable timer

    // Set system timer interrupt vector with little bit higher priority than context switcher
    VICVectAddr14 = (dword)OS::SystemTimer_ISR;
    VICVectCntl14 = (1<<5) | SYSTEM_TIMER_INT;

#if scmRTOS_CONTEXT_SWITCH_SCHEME == 1
    // Set context switcher interrupt handler with lowest possible priority
    VICVectAddr15 = (dword)ContextSwitcher_ISR;
    VICVectCntl15 = (1<<5) | CONTEXT_SWITCH_INT;

    VICIntEnable =  (1UL<<SYSTEM_TIMER_INT) | (1UL<<CONTEXT_SWITCH_INT);
#else
    VICIntEnable =  (1UL<<SYSTEM_TIMER_INT);
#endif

    VICVectAddr = 0;    // Reset VIC logic

}


