//******************************************************************************
//*
//*     FULLNAME:  Single-Chip Microcontroller Real-Time Operating System
//*
//*     NICKNAME:  scmRTOS
//*
//*     PROCESSOR: LPC2xxx (NXP)
//*
//*     TOOLKIT:   arm-elf-gcc (GNU)
//*
//*     PURPOSE:   Target Dependent Stuff Header. Declarations And Definitions
//*
//*     Version: 3.10
//*
//*     $Revision: $
//*     $Date:: 2010-03-24 #$
//*
//*     Copyright (c) 2003-2010, Harry E. Zhurov
//*
//*     Permission is hereby granted, free of charge, to any person
//*     obtaining  a copy of this software and associated documentation
//*     files (the "Software"), to deal in the Software without restriction,
//*     including without limitation the rights to use, copy, modify, merge,
//*     publish, distribute, sublicense, and/or sell copies of the Software,
//*     and to permit persons to whom the Software is furnished to do so,
//*     subject to the following conditions:
//*
//*     The above copyright notice and this permission notice shall be included
//*     in all copies or substantial portions of the Software.
//*
//*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//*     EXPRESS  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//*     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//*     IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//*     CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//*     TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
//*     THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//*
//*     =================================================================
//*     See http://scmrtos.sourceforge.net for documentation, latest
//*     information, license and contact details.
//*     =================================================================
//*
//******************************************************************************
//*     ARM port by Sergey A. Borshch, Copyright (c) 2006-2010
//*     Adapt to GCC by Ivan A-R <ivan@tuxotronic.org>

#ifndef TARGET_LPC2XXX_H__
#define TARGET_LPC2XXX_H__

#ifdef __ASSEMBLER__

#if scmRTOS_CONTEXT_SWITCH_SCHEME == 1 && !defined(GCC_IRQ_PATCH_REQUIRED)
#define OS_INTERRUPT   __attribute__((interrupt("IRQ")))
#else
#define OS_INTERRUPT
#endif

#if scmRTOS_CONTEXT_SWITCH_SCHEME == 1
INLINE inline void RaiseContextSwitch()
{
    do {
        VICSoftInt = (1<<CONTEXT_SWITCH_INT);
    } while (0) // set flag and enable interrupt
}
#endif

INLINE inline void IRQ_DONE()
{
   do { VICVectAddr = 0; } while(0)    // Reset VIC logic
}

#define SYSTEM_TIMER_HANDLER()  \
    do                          \
    {                           \
        RESET_SYSTIMER_INT();   \
        SystemTimer_Handler();  \
    }                           \
    while(0)

#else   // __ASSEMBLER__

#if scmRTOS_CONTEXT_SWITCH_SCHEME == 0
        .macro  IRQ_SWITCH  ; call interrupt handler
        LDR     R1,=VICVectAddr
        MOV     LR, PC
        LDR     PC,[R1]
        .endm
#else   // scmRTOS_CONTEXT_SWITCH_SCHEME == 1
        .macro  IRQ_SWITCH  ; call interrupt handler
        LDR     PC, VICVectAddr
        .endm

        .macro  IRQ_DONE    ; reset interrupt controller
        LDR     R1, =VICVectAddr
        MOV     R2, #(1 << CONTEXT_SWITCH_INT)
        STR     R2, [R1, #VICSoftIntClear - VICVectAddr]        ; clear soft int request
        STR     R1, [R1]                                        ; reset VIC
        .endm
#endif  // scmRTOS_CONTEXT_SWITCH_SCHEME

#endif  // __ASSEMBLER__

#endif  /* TARGET_LPC2XXX_H__ */

