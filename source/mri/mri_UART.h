#ifndef __UVISOR_MRI_UART_H__
#define __UVISOR_MRI_UART_H__

#include "mbed-drivers/mbed.h"                                                                                                                                           
#include "uvisor-lib/uvisor-lib.h"

#define mriEnableUSART_RxInterrupt(UART_num) (UART_num)->CR1 |= USART_CR1_RXNEIE
#define mriClearRxInterrupt(UART_num) (UART_num)->SR &= ~USART_SR_RXNE

extern void mri_Handler(void);
extern void mri_UART_Init(void);

#endif/*__UVISOR_MRI_UART_H__*/
