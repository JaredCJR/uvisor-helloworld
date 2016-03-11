#include "mbed-drivers/mbed.h"
#include "uvisor-lib/uvisor-lib.h"

#ifndef __CrashCatcher_H
#define __CrashCatcher_H


#define CRASH_CATCHER_STACK_WORD_COUNT 100

/* Fault handler will switch MSP to use this area as the stack while CrashCatcher code is running.
   NOTE: If you change the size of this buffer, it also needs to be changed in the HardFault_Handler (in
         FaultHandler_arm*.S) when initializing the stack pointer. */
typedef struct
{
    volatile uint32_t stack[CRASH_CATCHER_STACK_WORD_COUNT];
}CatcherStack_TypeDef;

#define CatcherStack_Base     ((uint32_t)0x20000000)
#define g_crashCatcherStack   ((CatcherStack_TypeDef *)CatcherStack_Base)


/* Bit in LR to indicate whether PSP was used for automatic stacking of registers during exception entry. */
#define LR_PSP (1 << 2)

/* Bit in LR set to 0 when automatic stacking of floating point registers occurs during exception handling. */
#define LR_FLOAT (1 << 4)

/* Bit in auto stacked xPSR which indicates whether stack was force 8-byte aligned. */
#define PSR_STACK_ALIGN (1 << 9)

/* The second word of the dump contains flags.  These are the allowed flags. */
/* Flag to indicate that 16 single-precision floating point registers and FPSCR will follow integer registers. */
#define CRASH_CATCHER_FLAGS_FLOATING_POINT (1 << 0)

/* This magic value will be found as the last word in a crash dump if the fault handler overflowed the stack while
   generating the crash dump. */
#define CRASH_CATCHER_STACK_SENTINEL 0xACCE55ED


/* This structure contains the integer registers that are automatically stacked by Cortex-M processor when it enters
   an exception handler. */
typedef struct
{
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;
    uint32_t pc;
    uint32_t psr;
    /* The following floating point registers are only stacked when the LR_FLOAT bit is set in exceptionLR. */
    uint32_t floats[16];
    uint32_t fpscr;
    uint32_t reserved; /* keeps 8-byte alignment */
} CrashCatcherStackedRegisters;


/* This structure is filled in by the Hard Fault exception handler (or unit test) and then passed in as a parameter to
   CrashCatcher_Entry(). */
typedef struct
{
    uint32_t exceptionPSR;
    uint32_t psp;
    uint32_t msp;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t exceptionLR;
} CrashCatcherExceptionRegisters;

typedef struct
{
    const CrashCatcherExceptionRegisters* pExceptionRegisters;
    const CrashCatcherStackedRegisters*   pSP;
    uint32_t                              sp;
    uint32_t                              flags; 
} Object;

void CrashCatcher_Entry(void);

#endif /* end of __CrashCatcher_H */
