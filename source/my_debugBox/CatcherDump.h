#include "CrashCatcher.h"
#include <stdarg.h>

#ifndef __CatcherDump_H
#define __CatcherDump_H


typedef struct
{
    UvisorBoxAclItem * ACLs;
    uint32_t sizeofACLs;
}ACLtoCrashCatcherMemoryRegion;

extern ACLtoCrashCatcherMemoryRegion ACLs_warehouse_CrashCatcher;

void cC_printf(const char * format,...);
void CrashCatcher_DumpStart(void);
void CrashCatcher_DumpMemory(const void* pvMemory, CrashCatcherElementSizes elementSize, size_t elementCount);
extern "C" const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void);
void targetACLs_register(UvisorBoxAclItem *items, uint32_t ACL_size,ACLtoCrashCatcherMemoryRegion* warehouse);
void CrashCatcher_Entry(void);


#endif /* end of CatcherDump */
