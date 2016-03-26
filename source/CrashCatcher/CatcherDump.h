/*                                                                                                                                 
 * Copyright (c) 2016, Jia-Rung Chang, All Rights Reserved                                                                         
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
void CrashCatcher_Entry(CatcherStack_TypeDef *g_crashCatcherStack);


#endif /* end of CatcherDump */
