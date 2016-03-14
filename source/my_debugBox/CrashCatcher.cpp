#include "CrashCatcher.h"
#include "mbed-drivers/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "CatcherDump.h"

static uint32_t getAddressOfExceptionStack(const CrashCatcherExceptionRegisters* pExceptionRegisters) 
{
    if (pExceptionRegisters->exceptionLR & LR_PSP)
        return pExceptionRegisters->psp;
    else
        return pExceptionRegisters->msp;
}


static const void* uint32AddressToPointer(uint32_t address)
{

    /* Test harness will define this value on 64-bit machine to provide upper 32-bits of pointer addresses. */
    uint64_t g_crashCatcherTestBaseAddress;

    if (sizeof(uint32_t*) == 8)
    {
        /* g_crashCatcherTestBaseAddress is currently unused */
        return (const void*)(unsigned long)((uint64_t)address | g_crashCatcherTestBaseAddress);
    }
    else
    {
        return (const void*)(unsigned long)address;
    }
}


static Object initStackPointers(const CrashCatcherExceptionRegisters* pExceptionRegisters,const CrashCatcherStackedRegisters* pStackedRegisters)
{
    Object object;
    object.pExceptionRegisters = pExceptionRegisters;
    object.sp = getAddressOfExceptionStack(pExceptionRegisters);
    object.pSP = (const CrashCatcherStackedRegisters*)uint32AddressToPointer((uint32_t)pStackedRegisters);
    object.flags = 0;
    return object;
}

static int areFloatingPointRegistersAutoStacked(const Object* pObject)
{
    return 0 == (pObject->pExceptionRegisters->exceptionLR & LR_FLOAT);
}

static void advanceStackPointerToValueBeforeException(Object* pObject)
{
    /* Cortex-M processor always push 8 integer registers on the stack. */
    pObject->sp += 8 * sizeof(uint32_t);
    /* ARMv7-M processors can also push 16 single-precision floating point registers, FPSCR and a padding word. */
    if (areFloatingPointRegistersAutoStacked(pObject))
    {
        pObject->sp += (16 + 1 + 1) * sizeof(uint32_t);
    }
    /* Cortex-M processor may also have had to force 8-byte alignment before auto stacking registers. */
    pObject->sp |= (pObject->pSP->psr & PSR_STACK_ALIGN) ? 4 : 0;
}

static int areFloatingPointCoprocessorsEnabled(void)
{
    /* The unit tests can point the core to a fake location for the Coprocessor Access Control Register. */
    /* uVisor does not support to access system registers right now! */
    /*
    uint32_t* g_pCrashCatcherCoprocessorAccessControlRegister = (uint32_t*)0xE000ED88;

    static const uint32_t coProcessor10and11EnabledBits = 5 << 20;
    uint32_t              coprocessorAccessControl = *g_pCrashCatcherCoprocessorAccessControlRegister;                                 

    return (coprocessorAccessControl & (coProcessor10and11EnabledBits)) == coProcessor10and11EnabledBits;
    */
    return 0;
}

static void initFloatingPointFlag(Object* pObject)                                                                                     
{
    if (areFloatingPointCoprocessorsEnabled())
    {
        pObject->flags |= CRASH_CATCHER_FLAGS_FLOATING_POINT;
    }
}

static void setStackSentinel(void)
{
    g_crashCatcherStack->stack[0] = CRASH_CATCHER_STACK_SENTINEL; 
}


static void dumpSignature(void)                                                                                    
{
    static const uint8_t signature[4] = {CRASH_CATCHER_SIGNATURE_BYTE0,
                                         CRASH_CATCHER_SIGNATURE_BYTE1,
                                         CRASH_CATCHER_VERSION_MAJOR,
                                         CRASH_CATCHER_VERSION_MINOR};
    CrashCatcher_DumpMemory(signature, CRASH_CATCHER_BYTE, sizeof(signature));
}


static void dumpFlags(const Object* pObject)                                                                                                
{
    CrashCatcher_DumpMemory(&pObject->flags, CRASH_CATCHER_BYTE, sizeof(pObject->flags));
}


static void dumpR0toR3(const Object* pObject)
{
    CrashCatcher_DumpMemory(&pObject->pSP->r0, CRASH_CATCHER_BYTE, 4 * sizeof(uint32_t));
}


static void dumpR4toR11(const Object* pObject)                                                                                              
{
    CrashCatcher_DumpMemory(&pObject->pExceptionRegisters->r4, CRASH_CATCHER_BYTE, (11 - 4 + 1) * sizeof(uint32_t));
}


static void dumpR12(const Object* pObject)
{
    CrashCatcher_DumpMemory(&pObject->pSP->r12, CRASH_CATCHER_BYTE, sizeof(uint32_t));
}

static void dumpSP(const Object* pObject)                                                                                                   
{
    CrashCatcher_DumpMemory(&pObject->sp, CRASH_CATCHER_BYTE, sizeof(uint32_t));
}


static void dumpLR_PC_PSR(const Object* pObject)
{
    CrashCatcher_DumpMemory(&pObject->pSP->lr, CRASH_CATCHER_BYTE, 3 * sizeof(uint32_t));
}


static void dumpExceptionPSR(const Object* pObject)                                                                                         
{
    CrashCatcher_DumpMemory(&pObject->pExceptionRegisters->exceptionPSR, CRASH_CATCHER_BYTE, sizeof(uint32_t));
}


void CrashCatcher_Entry(void)
{
    const CrashCatcherExceptionRegisters* pExceptionRegisters = 
          (const CrashCatcherExceptionRegisters*)(&g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-12]);
    const CrashCatcherStackedRegisters* pStackedRegisters = 
          (const CrashCatcherStackedRegisters*)(&g_crashCatcherStack->auto_stack[0]);

    Object object = initStackPointers(pExceptionRegisters,pStackedRegisters);
    advanceStackPointerToValueBeforeException(&object);
/*Floating Points resvent is not able to be accessed in uVisor now,so we assume that we does not use FPU.*/
    initFloatingPointFlag(&object);

    //do
    {
        setStackSentinel();
        CrashCatcher_DumpStart();
        dumpSignature();
        dumpFlags(&object);
        dumpR0toR3(&object);
        dumpR4toR11(&object);
        dumpR12(&object);
        dumpSP(&object);
        dumpLR_PC_PSR(&object);
        dumpExceptionPSR(&object);

/*Floating Points resvent is not able to be accessed in uVisor now,so we assume that we does not use FPU.*/
/*
        if (object.flags & CRASH_CATCHER_FLAGS_FLOATING_POINT)
        {
            dumpFloatingPointRegisters(&object);
        }
*/
/*
        dumpMemoryRegions(CrashCatcher_GetMemoryRegions());
        if (!isARMv6MDevice())
            dumpFaultStatusRegisters();
        checkStackSentinelForStackOverflow();
*/
    }
    //while (CrashCatcher_DumpEnd() == CRASH_CATCHER_TRY_AGAIN);

}
