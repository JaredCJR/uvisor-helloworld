#include "CrashCatcher.h"


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
        return (const void*)(unsigned long)((uint64_t)address | g_crashCatcherTestBaseAddress);
    else
        return (const void*)(unsigned long)address;
}


static Object initStackPointers(const CrashCatcherExceptionRegisters* pExceptionRegisters)  
{
    Object object;
    object.pExceptionRegisters = pExceptionRegisters;
    object.sp = getAddressOfExceptionStack(pExceptionRegisters);
    object.pSP = (const CrashCatcherStackedRegisters*)uint32AddressToPointer(object.sp);
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
        pObject->sp += (16 + 1 + 1) * sizeof(uint32_t);
    /* Cortex-M processor may also have had to force 8-byte alignment before auto stacking registers. */
    pObject->sp |= (pObject->pSP->psr & PSR_STACK_ALIGN) ? 4 : 0;
}

static int areFloatingPointCoprocessorsEnabled(void)
{
    /* The unit tests can point the core to a fake location for the Coprocessor Access Control Register. */
    /*May Cause MPU fault , but uVisor does not protecting FPU currently! */
    uint32_t* g_pCrashCatcherCoprocessorAccessControlRegister = (uint32_t*)0xE000ED88;

    static const uint32_t coProcessor10and11EnabledBits = 5 << 20;
    uint32_t              coprocessorAccessControl = *g_pCrashCatcherCoprocessorAccessControlRegister;                                 

    return (coprocessorAccessControl & (coProcessor10and11EnabledBits)) == coProcessor10and11EnabledBits;
}

static void initFloatingPointFlag(Object* pObject)                                                                                     
{
    if (areFloatingPointCoprocessorsEnabled())
        pObject->flags |= CRASH_CATCHER_FLAGS_FLOATING_POINT;
}

void CrashCatcher_Entry()
{
    const CrashCatcherExceptionRegisters* pExceptionRegisters = (const CrashCatcherExceptionRegisters*)g_crashCatcherStack;

    Object object = initStackPointers(pExceptionRegisters);
    advanceStackPointerToValueBeforeException(&object);
    initFloatingPointFlag(&object);
/*
    do
    {
        setStackSentinel();
        CrashCatcher_DumpStart();
        dumpSignature(&object);
        dumpFlags(&object);
        dumpR0toR3(&object);
        dumpR4toR11(&object);
        dumpR12(&object);
        dumpSP(&object);
        dumpLR_PC_PSR(&object);
        dumpExceptionPSR(&object);
        if (object.flags & CRASH_CATCHER_FLAGS_FLOATING_POINT)
            dumpFloatingPointRegisters(&object);
        dumpMemoryRegions(CrashCatcher_GetMemoryRegions());
        if (!isARMv6MDevice())
            dumpFaultStatusRegisters();
        checkStackSentinelForStackOverflow();
    }
    while (CrashCatcher_DumpEnd() == CRASH_CATCHER_TRY_AGAIN);
*/
}
