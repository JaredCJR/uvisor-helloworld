/*
 * Copyright 2016 Chang,Jia-Rung (https://github.com/JaredCJR)
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


#ifndef __UVISOR_DEBUG_CONTEXT_H__
#define __UVISOR_DEBUG_CONTEXT_H__

#include "mbed-drivers/mbed.h"
#include "mri/include/buffer.h"
#include "mri/include/packet.h"
#include "mri/include/try_catch.h"
#include "mri/architectures/armv7-m/armv7-m.h"
#include "mri/architectures/armv7-m/debug_cm3.h"

typedef struct
{
    mriPacket   packet; 
    mriBuffer   buffer; 
    uint32_t    flags;  
    uint8_t     signalValue;
} MriCore;
    
typedef struct {
    /*USART instance*/
    RawSerial       *mri_serial;
    uint8_t         serial_buffer[sizeof(RawSerial)];

    /*Core state*/
    MriCore g_mri;

    /*ARMv7 Cortex-M State*/
    CortexMState    __mriCortexMState;
    const uint32_t  __mriCortexMFakeStack[8];

    /*test*/
    uint32_t val;
} Debug_Context;



#endif/*__UVISOR_DEBUG_CONTEXT_H__*/
