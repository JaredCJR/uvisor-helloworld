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


#ifndef __UVISOR_DEBUG_BOX_HW_H__
#define __UVISOR_DEBUG_BOX_HW_H__



#if defined(TARGET_LIKE_STM32F429I_DISCO)

#define mriEnableUSART_RxInterrupt(UART_num) (UART_num)->CR1 |= USART_CR1_RXNEIE
#define mriClearRxInterrupt(UART_num) (UART_num)->SR &= ~USART_SR_RXNE


#define DEBUG_BOX_ACL(acl_list_name)                            \
    static const UvisorBoxAclItem acl_list_name[] = {           \
        /*USART3 related ACLs*/                                 \
        {GPIOB,     sizeof(*GPIOB),     UVISOR_TACLDEF_PERIPH}, \
        {RCC,       sizeof(*RCC),       UVISOR_TACLDEF_PERIPH}, \
        {USART3,    sizeof(*USART3),    UVISOR_TACLDEF_PERIPH}, \
    }


#else

#define mriEnableUSART_RxInterrupt(UART_num)
#define mriClearRxInterrupt(UART_num)

#define DEBUG_BOX_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {}
    
#endif




#endif/*__UVISOR_DEBUG_BOX_CONTEXT_H__*/
