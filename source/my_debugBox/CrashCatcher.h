#include "mbed-drivers/mbed.h"
#include "uvisor-lib/uvisor-lib.h"

#ifndef __CrashCatcher_H
#define __CrashCatcher_H


#define CRASH_CATCHER_STACK_WORD_COUNT 50
#define CRASH_CATCHER_AUTO_STACKED_WORD_COUNT 50


/* Fault handler will switch MSP to use this area as the stack while CrashCatcher code is running.
   NOTE: If you change the size of this buffer, it also needs to be changed when initializing the stack pointer. */
typedef struct
{
    volatile uint32_t stack[CRASH_CATCHER_STACK_WORD_COUNT];
    volatile uint32_t auto_stack[CRASH_CATCHER_AUTO_STACKED_WORD_COUNT];
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


/* The crash dump start with a four byte header.  The first two bytes are "cC", the third byte is the major version
   number, and the fourth bytes is the minor version number. */
#define CRASH_CATCHER_SIGNATURE_BYTE0 'c'                                                                                           
#define CRASH_CATCHER_SIGNATURE_BYTE1 'C'
#define CRASH_CATCHER_VERSION_MAJOR   2
#define CRASH_CATCHER_VERSION_MINOR   0

/* Supported element sizes to be used with CrashCatcher_DumpMemory calls. */
typedef enum
{   
    CRASH_CATCHER_BYTE = 1,
    CRASH_CATCHER_HALFWORD = 2,
    CRASH_CATCHER_WORD = 4
} CrashCatcherElementSizes;




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


/* An array of these structures is returned from CrashCatcher_GetMemoryRegions() to indicate what regions of memory
   should be dumped as part of the crash dump.  The last entry should contain a starting address of 0xFFFFFFFF to
   indicate that the end of the list has been encountered. */
typedef struct
{
    /* The first address of the element to be dumped for this region of memory. */
    /* The last region in the array return from CrashCatcher_GetMemoryRegions() must set this to 0xFFFFFFFF */
    uint32_t                 startAddress;
    /* Stop dumping the region once this address is encountered.  The dump isn't inclusive of this address. */
    /* It must be greater than startAddress. */
    uint32_t                 endAddress;
    /* This should be set to CRASH_CATCHER_BYTE except for peripheral registers which don't support 8-bit reads. */
    CrashCatcherElementSizes elementSize;
} CrashCatcherMemoryRegion;


#endif /* end of __CrashCatcher_H */
