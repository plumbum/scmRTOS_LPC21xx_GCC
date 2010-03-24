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
    FIO0DIR |= (1<<2) | (1<<7) | (1<<16);
    FIO0CLR = (1<<2) | (1<<7);
    FIO0SET = (1<<16);

    while(1)
    {
        volatile int i;
        FIO0SET = (1<<2);
        for(i=0; i<2000; i++) ;
        FIO0CLR = (1<<2);
        for(i=0; i<2000; i++) ;
    }
    OS::Run();
}
//---------------------------------------------------------------------------
namespace OS
{
    template<>
    OS_PROCESS void TProc1::Exec()
    {
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
            //AT91C_BASE_PIOA->PIO_CODR = (1 << 0);
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
void OS::SystemTimerUserHook() { }
//---------------------------------------------------------------------------
void OS::IdleProcessUserHook() { }
//---------------------------------------------------------------------------
OS_INTERRUPT void Timer_ISR()
{
    OS::TISRW ISRW;
    //AT91C_BASE_TCB->TCB_TC0.TC_SR;   // read to clear int flag

    //AT91C_BASE_PIOA->PIO_SODR = (1 << 0);
    Timer_Ovf.SignalISR();

    //AT91C_BASE_AIC->AIC_EOICR = 0;
}
//-----------------------------------------------------------------------------

