#include "mbed-drivers/mbed.h"
#include "uvisor-lib/uvisor-lib.h"
#include "debug_box_hw.h"
#include "mri_UART.h"

typedef struct {
    uint32_t val;
} MriCore;


/* create ACLs for secret data section */
DEBUG_BOX_ACL(g_debug_box_acl);


/* configure secure box compartnent and reserve box context */
UVISOR_BOX_CONFIG(debug_box, g_debug_box_acl, UVISOR_BOX_STACK_SIZE, MriCore);

bool secure_mri_UART_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    return secure_gateway(debug_box,__mri_UART_Init,baudrate,USARTx_IRQn,USARTx);
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
