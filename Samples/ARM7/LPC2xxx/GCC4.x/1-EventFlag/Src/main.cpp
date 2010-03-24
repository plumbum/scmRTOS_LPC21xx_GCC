//******************************************************************************
//*
//*     FULLNAME:  Single-Chip Microcontroller Real-Time Operating System
//*
//*     NICKNAME:  scmRTOS
//*
//*     PROCESSOR: AT91SAM7xxx (Atmel)
//*
//*     TOOLKIT:   arm-elf-gcc (GNU)
//*
//*     PURPOSE:   Port Test File
//*
//*     Version: 3.10
//*
//*     $Revision: 260 $
//*     $Date:: 2010-01-26 #$
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
#include <stdint.h>
#include <scmRTOS.h>


void halInit(void);

//---------------------------------------------------------------------------
//
//      Process types
//
typedef OS::process<OS::pr0, 200> TProc1;
typedef OS::process<OS::pr1, 200> TProc2;
typedef OS::process<OS::pr2, 200> TProc3;
//---------------------------------------------------------------------------
//
//      Process objects
//
TProc1 Proc1;
TProc2 Proc2;
TProc3 Proc3;
//---------------------------------------------------------------------------
                                 //
OS::TEventFlag ef;               //
OS::TEventFlag Timer_Ovf;
//---------------------------------------------------------------------------
int main()
{
    halInit();
   //enable interrupts (both IRQ and FIQ) 
#if 0
   asm volatile ("mrs r3, cpsr       \n\t"                          
                 "bic r3, r3, #0xC0  \n\t"
                 "msr cpsr, r3       \n\t"
                 :
                 :
                 : "r3" );
#endif

#if 0
    while(1)
    {
        volatile int i;
        for(i=0; i<1000; i++) ;
        FIO0SET = (1<<6);
        for(i=0; i<1000; i++) ;
        FIO0CLR = (1<<6);
    }
#endif

    OS::Run();
}
//---------------------------------------------------------------------------
namespace OS
{
    template<>
    OS_PROCESS void TProc1::Exec()
    {
        FIO0SET = (1<<4);
        for(;;)
        {
            Sleep(2);
            ef.Wait();
        }
    }
//---------------------------------------------------------------------------
    template<>
    OS_PROCESS void TProc2::Exec()
    {
        for(;;)
        {
            Timer_Ovf.Wait();
            FIO0CLR = (1<<2);
        }
    }
//---------------------------------------------------------------------------
    template<>
    OS_PROCESS void TProc3::Exec()
    {
        for(;;)
        {
            Sleep(1);
            ef.Signal();
        }
    }
}   // namespace OS
//---------------------------------------------------------------------------
void OS::SystemTimerUserHook() { FIO0SET = (1<<4); }
//---------------------------------------------------------------------------
void OS::IdleProcessUserHook() { }
//---------------------------------------------------------------------------
//extern "C" void Timer_ISR() __attribute__((interrupt("IRQ")));

OS_INTERRUPT void Timer_ISR()
{
    OS::TISRW ISRW;
    T1IR = 0xFF; // T1IR;                    // clear int flag

    FIO0SET = (1<<2);
    static int st = 0;
    if(st)
        FIO0SET = (1<<3);
    else
        FIO0CLR = (1<<3);
    st = ~st;
    Timer_Ovf.SignalISR();
}
//-----------------------------------------------------------------------------

void halInit(void)
{
    FIO0DIR |= 0xFC | (1<<16);
    FIO0CLR = 0xFC;

    //FIO0SET = (1<<16);

// ***************************************************************************
// ** TIMER1
// ***************************************************************************
//          EventFlag test timer
    T1IR = 0x000000FF;                      // clear int requests
    T1TCR = (1<<1);
    T1PR = 0;
    T1MR0 = PCLK / 350;                    // int at 10Hz
    T1MCR = TMCR_MR0_I | TMCR_MR0_R;        // MR0INT = 1, MR0RES = 1
    T1TCR = (1<<0);                         // CE = 1, enable timer

    // Set periodical timer interrupt with highest priority
    VICVectAddr0 = (dword)Timer_ISR;
    VICVectCntl0 = 0x20 | VIC_TIMER1;
    VICIntEnable |= (1UL<<VIC_TIMER1);
    VICVectAddr = 0;    // Reset VIC logic

#if scmRTOS_CONTEXT_SWITCH_SCHEME == 1
    FIO0SET = (1<<7);
#endif
}

