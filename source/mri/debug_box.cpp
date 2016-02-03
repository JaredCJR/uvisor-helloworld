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


#include "mbed-drivers/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "debug_box_hw.h"
#include "include/buffer.h"
#include "include/packet.h"
#include "include/try_catch.h"

#define mriEnableUSART_RxInterrupt(UART_num) (UART_num)->CR1 |= USART_CR1_RXNEIE
#define mriClearRxInterrupt(UART_num) (UART_num)->SR &= ~USART_SR_RXNE

typedef struct
{
    mriPacket   packet;
    mriBuffer   buffer;
    uint32_t    flags;
    uint8_t     signalValue;
} MriCore;

typedef struct {
    /*USART instance*/
    RawSerial *mri_serial;
    uint8_t serial_buffer[sizeof(RawSerial)];

    /*core struct*/
    MriCore g_mri;

    /*test*/
    uint32_t val;
} Debug_Context;


/* create ACLs for secret data section */
DEBUG_BOX_ACL(g_debug_box_acl);


/* configure secure box compartment and reserve box context */
UVISOR_BOX_CONFIG(debug_box, g_debug_box_acl, UVISOR_BOX_STACK_SIZE,Debug_Context);


/*For test,the code dose not make any sense.*/
void volatile mri_Handler(void) 
{
    uint32_t registers[16];
    uvisor_ctx->mri_serial->printf("mri_Handler\n\r");

    /*Try to access registers*/
    asm volatile(
            "mov %[var_r0],r0\n\t"
            "mov %[var_r1],r1\n\t"
            "mov %[var_r2],r2\n\t"
            "mov %[var_r3],r3\n\t"
            "mov %[var_r4],r4\n\t"
            "mov %[var_r5],r5\n\t"
            "mov %[var_r6],r6\n\t"
            "mov %[var_r7],r7\n\t"
            "mov %[var_sp],sp\n\t"
            "mov %[var_lr],lr\n\t"
            "mov %[var_pc],pc\n\t"
            :[var_r0]"=r"(registers[0]),
             [var_r1]"=r"(registers[1]),
             [var_r2]"=r"(registers[2]),
             [var_r3]"=r"(registers[3]),
             [var_r4]"=r"(registers[4]),
             [var_r5]"=r"(registers[5]),
             [var_r6]"=r"(registers[6]),
             [var_r7]"=r"(registers[7]),
             [var_sp]"=r"(registers[13]),
             [var_lr]"=r"(registers[14]),
             [var_pc]"=r"(registers[15])
            :
            :);

    for(int i =0;i<16;i++)
    {
        uvisor_ctx->mri_serial->printf("r[%d]=0x%.8X\r\n",i,registers[i]);
        if(i==7)
        {
            i = 12;
        }
    }
    uvisor_ctx->mri_serial->printf("-------------------------------------\n\r");
    mriClearRxInterrupt(USART3);//USART3
}


static void __mri_UART_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    /*"placement new" operator in C++*/
    uvisor_ctx->mri_serial = ::new((void *) &uvisor_ctx->serial_buffer)RawSerial(PB_10,PB_11);
    uvisor_ctx->mri_serial->baud(baudrate);
    uvisor_ctx->mri_serial->printf("----- USART3 connected! -----\n\r");
    vIRQ_SetVector(USARTx_IRQn,(uint32_t)&mri_Handler);

    /*highest priority in all external interrupts.*/
    vIRQ_SetPriority(USARTx_IRQn,0);
    vIRQ_EnableIRQ(USARTx_IRQn);

    /*Enable USART RX interrupt.*/
    mriEnableUSART_RxInterrupt(USARTx);
    uvisor_ctx->mri_serial->printf("----- USART3 RX interrupt registered! -----\n\r");
}


static void clearCoreStructure(void)                                                                                              
{
    memset(&uvisor_ctx->g_mri, 0, sizeof(uvisor_ctx->g_mri));
}


UVISOR_EXTERN bool __mri_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    clearCoreStructure();

    __try
    {
    }
    __catch
    {
        //return false;
    }
    /*STM32F429 config*/
    __mri_UART_Init(baudrate,USARTx_IRQn,USARTx);
    return true;
}

bool mri_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    return secure_gateway(debug_box,__mri_Init,baudrate,USARTx_IRQn,USARTx);
}



static void mri_PrintVal(uint32_t val) 
{
    uvisor_ctx->mri_serial->printf("mri_Print:%d\n\r",val);
}


UVISOR_EXTERN bool __print_MriCore(uint32_t val)
{
    uvisor_ctx->val = val;
    mri_PrintVal(uvisor_ctx->val);
    return true;
}



bool print_MriCore(uint32_t assign_val)
{
    /* security transition happens here
     *   ensures that fn() will run with the privileges
     *   of the debug box */
    return secure_gateway(debug_box, __print_MriCore, assign_val);
}      
