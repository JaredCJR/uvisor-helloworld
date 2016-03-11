#include "CrashCatcher.h"
#include <stdarg.h>

#ifndef __CatcherDump_H
#define __CatcherDump_H

void cC_printf(const char * format,...);
void CrashCatcher_DumpStart(void);
void CrashCatcher_DumpMemory(const void* pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount);



#endif /* end of CatcherDump */
