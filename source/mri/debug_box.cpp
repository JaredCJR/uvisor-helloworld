#include "mbed-drivers/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "debug_box_hw.h"
#include "mri_UART.h"

typedef struct {
    Serial mri_port(PB_10,PB_11);//USART3 pins in STM32F429
    uint32_t val;
} MriCore;


/* create ACLs for secret data section */
DEBUG_BOX_ACL(g_debug_box_acl);


/* configure secure box compartment and reserve box context */
UVISOR_BOX_CONFIG(debug_box, g_debug_box_acl, UVISOR_BOX_STACK_SIZE, MriCore);

UVISOR_EXTERN bool __mri_UART_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    //memcpy(uvisor_ctx->mri_port,new Serial(PB_10,PB_11), sizeof(Serial));//USART3 pins in STM32F429
    uvisor_ctx->mri_port.baud(baudrate);
    uvisor_ctx->mri_port.printf("----- USART3 connected! -----\n\r");
    vIRQ_SetVector(USARTx_IRQn,(uint32_t)&mri_Handler);
    vIRQ_SetPriority(USARTx_IRQn,0);//highest priority in all external interrupts.
    vIRQ_EnableIRQ(USARTx_IRQn);
    mriEnableUSART_RxInterrupt(USARTx);//Enable USART RX interrupt.
    uvisor_ctx->mri_port.printf("----- USART3 RX interrupt registered! -----\n\r");
    return 1;
}



bool secure_mri_UART_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    return secure_gateway(debug_box,__mri_UART_Init,baudrate,USARTx_IRQn,USARTx);
}

static void mri_PrintVal(uint32_t val) 
{
    uvisor_ctx->mri_port.printf("mri_Print:%d\n\r",val);
}


UVISOR_EXTERN bool __print_MriCore(uint32_t val)
{
    uvisor_ctx->val = val;
    mri_PrintVal(uvisor_ctx->val);
    return 1;
}



bool print_MriCore(uint32_t assign_val)
{
    /* security transition happens here
     *   ensures that fn() will run with the privileges
     *   of the debug box */
    return secure_gateway(debug_box, __print_MriCore, assign_val);
}      
