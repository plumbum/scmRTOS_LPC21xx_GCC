        .global main                    // int main(void)
        .global sysInit

        .global _etext                  // -> .data initial values in ROM
        .global _data                   // -> .data area in RAM
        .global _edata                  // end of .data area
        .global __bss_start             // -> .bss area in RAM
        .global __bss_end__             // end of .bss area
        .global _stack                 // top of stack

// Stack Sizes
        .set  UND_STACK_SIZE, 0x00000004
        .set  ABT_STACK_SIZE, 0x00000004
        .set  FIQ_STACK_SIZE, 0x00000004
        .set  IRQ_STACK_SIZE, 0X00000080
        .set  SVC_STACK_SIZE, 0x00000004

// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
        .set  MODE_USR, 0x10            // User Mode
        .set  MODE_FIQ, 0x11            // FIQ Mode
        .set  MODE_IRQ, 0x12            // IRQ Mode
        .set  MODE_SVC, 0x13            // Supervisor Mode
        .set  MODE_ABT, 0x17            // Abort Mode
        .set  MODE_UND, 0x1B            // Undefined Mode
        .set  MODE_SYS, 0x1F            // System Mode

        .equ  I_BIT, 0x80               // when I bit is set, IRQ is disabled
        .equ  F_BIT, 0x40               // when F bit is set, FIQ is disabled


#include <OS_Target_core.h>
#include <scmRTOS_CONFIG.h>

#define VAL_PLLCFG_MSEL  ((PLL_MUL - 1) << 0)
#if (PLL_DIV == 1)
#define PLL_DIV_VALUE 0x00
#elif (PLL_DIV == 2)
#define PLL_DIV_VALUE 0x01
#elif (PLL_DIV == 4)
#define PLL_DIV_VALUE 0x10
#elif (PLL_DIV == 8)
#define PLL_DIV_VALUE 0x11
#endif
#define VAL_PLLCFG_PSEL  (PLL_DIV_VALUE << 5)
#define VAL_PLLCFG       (VAL_PLLCFG_MSEL | VAL_PLLCFG_PSEL)

# Phase Locked Loop (PLL) definitions
        .equ    PLL_BASE,       0xE01FC080  /* PLL Base Address */
        .equ    PLLCON_OFS,     0x00        /* PLL Control Offset*/
        .equ    PLLCFG_OFS,     0x04        /* PLL Configuration Offset */
        .equ    PLLSTAT_OFS,    0x08        /* PLL Status Offset */
        .equ    PLLFEED_OFS,    0x0C        /* PLL Feed Offset */

        .equ    PLLCONPLLE,    0x01        /* PLL Enable */
        .equ    PLLCONPLLC,    (1<<1)      /* PLL Connect */
        .equ    PLLSTAT_PLOCK,  (1<<10)     /* PLL Lock Status */


# Startup code
        .text
        .code 32
        .align 2

        .global _boot
        .func   _boot
_boot:

// Runtime Interrupt Vectors
// -------------------------
Vectors:
        b     _start                    // reset - _start
        ldr   pc,_undf                  // undefined - _undf
        ldr   pc,_swi                   // SWI - _swi
        ldr   pc,_pabt                  // program abort - _pabt
        ldr   pc,_dabt                  // data abort - _dabt
        nop                             // reserved
#if (scmRTOS_CONTEXT_SWITCH_SCHEME == 0) || defined(GCC_IRQ_PATCH_REQUIRED)
        ldr   pc, _irq         /* IRQ interrupt         */
#else
        ldr   pc,[pc,#-0xFF0]           // IRQ - read the VIC
#endif
        ldr   pc,_fiq                   // FIQ - _fiq

#if 0
// Use this group for production
_undf:  .word _reset                    // undefined - _reset
_swi:   .word _reset                    // SWI - _reset
_pabt:  .word _reset                    // program abort - _reset
_dabt:  .word _reset                    // data abort - _reset

#if (scmRTOS_CONTEXT_SWITCH_SCHEME == 0) || defined(GCC_IRQ_PATCH_REQUIRED)
_irq:   .word IRQHandler
#else
_irq:   .word _reset                    // IRQ - _reset
#endif

_fiq:   .word _reset                    // FIQ - _reset

#else
// Use this group for development
_undf:  .word __undf                    // undefined
_swi:   .word __swi                     // SWI
_pabt:  .word __pabt                    // program abort
_dabt:  .word __dabt                    // data abort

#if (scmRTOS_CONTEXT_SWITCH_SCHEME == 0) || defined(GCC_IRQ_PATCH_REQUIRED)
_irq:   .word IRQHandler
#else
_irq:   .word __irq                     // IRQ
#endif

_fiq:   .word __fiq                     // FIQ

__undf: b     .                         // undefined
__swi:  b     .                         // SWI
__pabt: b     .                         // program abort
__dabt: b     .                         // data abort
__irq:  b     .                         // IRQ
__fiq:  b     .                         // FIQ
#endif
        .size _boot, . - _boot
        .endfunc


// Setup the operating mode & stack.
// ---------------------------------
        .global _start, start, _mainCRTStartup
        .func   _start

_start:
start:
_mainCRTStartup:

#if (USE_PLL == 1)
        ldr     r0, =PLL_BASE
        mov     r1, #0xAA
        mov     r2, #0x55

# Disable PLL before programming (in case it was enabled before)
        mov     r3, #0
        str     r3, [R0, #PLLCON_OFS]
        str     r1, [R0, #PLLFEED_OFS]
        str     r2, [R0, #PLLFEED_OFS]

# Wait for 		
# Configure and Enable PLL
        mov     r3, #VAL_PLLCFG
        str     r3, [R0, #PLLCFG_OFS] 
        mov     r3, #PLLCONPLLE
        str     r3, [R0, #PLLCON_OFS]
        str     r1, [R0, #PLLFEED_OFS]
        str     r2, [R0, #PLLFEED_OFS]

# Wait until PLL Locked
PLL_Loop:
        ldr     r3, [R0, #PLLSTAT_OFS]
        ands    r3, R3, #PLLSTAT_PLOCK
        beq     PLL_Loop

# Switch to PLL Clock
        mov     r3, #(PLLCONPLLE | PLLCONPLLC)
        str     r3, [R0, #PLLCON_OFS]
        str     r1, [R0, #PLLFEED_OFS]
        str     r2, [R0, #PLLFEED_OFS]
#endif

// Initialize Interrupt System
// - Set stack location for each mode
// - Leave in System Mode with Interrupts Disabled
// -----------------------------------------------
        ldr   r0,=_stack
        msr   CPSR_c,#MODE_UND|I_BIT|F_BIT // Undefined Instruction Mode
        mov   sp,r0
        sub   r0,r0,#UND_STACK_SIZE
        msr   CPSR_c,#MODE_ABT|I_BIT|F_BIT // Abort Mode
        mov   sp,r0
        sub   r0,r0,#ABT_STACK_SIZE
        msr   CPSR_c,#MODE_FIQ|I_BIT|F_BIT // FIQ Mode
        mov   sp,r0
        sub   r0,r0,#FIQ_STACK_SIZE
        msr   CPSR_c,#MODE_IRQ|I_BIT|F_BIT // IRQ Mode
        mov   sp,r0
        sub   r0,r0,#IRQ_STACK_SIZE
        msr   CPSR_c,#MODE_SVC|I_BIT|F_BIT // Supervisor Mode
        mov   sp,r0
        sub   r0,r0,#SVC_STACK_SIZE
        msr   CPSR_c,#MODE_SYS|I_BIT|F_BIT // System Mode
        mov   sp,r0

// Copy initialized data to its execution address in RAM
// -----------------------------------------------------
#ifdef ROM_RUN
        ldr   r1,=_etext                // -> ROM data start
        ldr   r2,=_data                 // -> data start
        ldr   r3,=_edata                // -> end of data
1:      cmp   r2,r3                     // check if data to move
        ldrlo r0,[r1],#4                // copy it
        strlo r0,[r2],#4
        blo   1b                        // loop until done
#endif
// Clear .bss
// ----------
        mov   r0,#0                     // get a zero
        ldr   r1,=__bss_start           // -> bss start
        ldr   r2,=__bss_end__           // -> bss end
2:      cmp   r1,r2                     // check if data to clear
        strlo r0,[r1],#4                // clear 4 bytes
        blo   2b                        // loop until done

// Call sysInit()
        ldr   r10,=sysInit
        mov   lr,pc
        bx    r10                       // enter sysInit()

// Call constructors
//-----------------------------------------------------------------------------
//     Call C++ constructors
        ldr     r0, =__ctors_start
        ldr     r1, =__ctors_end
ctor_loop:
        cmp     r0, r1
        beq     ctor_end
        ldr     r2, [r0], #4
        stmfd   sp!, {r0,r1}
        mov     lr, pc
        bx      r2                      // some constructors can be in thumb mode
        ldmfd   sp!, {r0,r1}
        b       ctor_loop
ctor_end:

// Call main program: main(0)
// --------------------------
        mov   r0,#0                     // no arguments (argc = 0)
        mov   r1,r0
        mov   r2,r0
        mov   fp,r0                     // null frame pointer
        mov   r7,r0                     // null frame pointer for thumb
        ldr   r10,=main
        mov   lr,pc
        bx    r10                       // enter main()

        .size   _start, . - _start
        .endfunc

        .global _reset, reset, exit, abort
        .func   _reset
_reset:
reset:
exit:
abort:
#if 0
// Disable interrupts, then force a hardware reset by driving P23 low
// -------------------------------------------------------------------
        mrs   r0,cpsr                   // get PSR
        orr   r0,r0,#I_BIT|F_BIT        // disable IRQ and FIQ
        msr   cpsr,r0                   // set up status register

        ldr   r1,=(PS_BASE)             // PS Base Address
        ldr   r0,=(PS_PIO)              // PIO Module
        str   r0,[r1,#PS_PCER_OFF]      // enable its clock
        ldr   r1,=(PIO_BASE)            // PIO Base Address
        ldr   r0,=(1<<23)               // P23
        str   r0,[r1,#PIO_PER_OFF]      // make sure pin is contolled by PIO
        str   r0,[r1,#PIO_CODR_OFF]     // set the pin low
        str   r0,[r1,#PIO_OER_OFF]      // make it an output
#endif
        b     .                         // loop until reset

        .size _reset, . - _reset
        .endfunc

        .end
