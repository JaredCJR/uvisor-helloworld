#include "CrashCatcher.h"
#include "CatcherDump.h"

void CrashCatcher_DumpStart(void) 
{
    cC_printf("\r\n\r\nCRASH ENCOUNTERED\r\n"
                 "Enable logging and start dump.\r\n");
    cC_printf("\r\n");
}


static void dumpHexDigit(uint8_t nibble)
{
    static const char hexToASCII[] = "0123456789ABCDEF";
    assert( nibble < 16 );
    cC_printf("%c",hexToASCII[nibble]);
}

static void dumpByteAsHex(uint8_t byte)                                                                                             
{
    dumpHexDigit(byte >> 4);
    dumpHexDigit(byte & 0xF);
}

static void dumpBytes(const uint8_t* pMemory, size_t elementCount)
{
    size_t i;
    for (i = 0 ; i < elementCount ; i++)
    {
        /* Only dump 16 bytes to a single line before introducing a line break. */
        if (i != 0 && (i & 0xF) == 0)
        {
            cC_printf("\r\n");
        }
        dumpByteAsHex(*pMemory++);
    }                                                                                                                               
}

static void dumpHalfwords(const uint16_t* pMemory, size_t elementCount)
{
    size_t i;
    for (i = 0 ; i < elementCount ; i++)
    {
        uint16_t val = *pMemory++;
        /* Only dump 8 halfwords to a single line before introducing a line break. */
        if (i != 0 && (i & 0x7) == 0)
        {
            cC_printf("\r\n");
        }
        dumpBytes((uint8_t*)&val, sizeof(val));
    }
}

static void dumpWords(const uint32_t* pMemory, size_t elementCount)
{
    size_t i;
    for (i = 0 ; i < elementCount ; i++)
    {
        uint32_t val = *pMemory++;
        /* Only dump 4 words to a single line before introducing a line break. */
        if (i != 0 && (i & 0x3) == 0)
        {
            cC_printf("\r\n");
        }
        dumpBytes((uint8_t*)&val, sizeof(val));
    }
}


void CrashCatcher_DumpMemory(const void* pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount)
{
    switch (elementSize)
    {
    case CRASH_CATCHER_BYTE:
        dumpBytes( (const uint8_t*)pvMemory, elementCount);
        break;
    case CRASH_CATCHER_HALFWORD:
        dumpHalfwords( (const uint16_t*)pvMemory, elementCount);
        break;
    case CRASH_CATCHER_WORD:
        dumpWords( (const uint32_t*)pvMemory, elementCount);
        break;
    }                                                                                                                               
    cC_printf("\r\n");
}

