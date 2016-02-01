#ifndef __UVISOR_DEBUG_BOX_H__
#define __UVISOR_DEBUG_BOX_H__

#include "mbed-drivers/mbed.h"

extern bool print_MriCore(uint32_t assign_val);
extern bool secure_mri_UART_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx);



#endif/*__UVISOR_DEBUG_BOX_CONTEXT_H__*/
