/*
 * Copyright 2016 Chang,Jia-Rung (https://github.com/JaredCJR)
 * Copyright (c) 2013-2015, ARM Limited
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


#ifndef __UVISOR_DEBUG_BOX_H__
#define __UVISOR_DEBUG_BOX_H__

#include "mbed-drivers/mbed.h"

extern bool print_MriCore(uint32_t assign_val);
extern bool mri_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx);



#endif/*__UVISOR_DEBUG_BOX_CONTEXT_H__*/
