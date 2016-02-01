#include "mri_UART.h"


Serial mri_port(PB_10,PB_11);//STM32F429i,USART3

void mri_Handler(void)
{
    mri_port.printf("mri_Handler\n\r");
    mriClearRxInterrupt(USART3);
}

void __mri_UART_Init(int baudrate,IRQn_Type USARTx_IRQn,USART_TypeDef *USARTx)
{
    mri_port.baud(baudrate);
    mri_port.printf("----- 230400 USART3 connected! -----\n\r");
    vIRQ_SetVector(USARTx_IRQn,(uint32_t)&mri_Handler);
    vIRQ_SetPriority(USARTx_IRQn,0);//highest priority in all external interrupts.
    vIRQ_EnableIRQ(USARTx_IRQn);
    mriEnableUSART_RxInterrupt(USARTx);//Enable USART RX interrupt.
    mri_port.printf("----- USART3 RX interrupt registered! -----\n\r");
}

void mri_PrintVal(uint32_t val)
{
    mri_port.printf("mri_Print:%d\n\r",val);
}
