#include "mri_UART.h"

Serial mri_port(PB_10,PB_11);//USART3

void mri_Handler(void)
{
    mri_port.printf("mri_Handler\n\r");
    mriClearRxInterrupt(USART3);
}

void mri_UART_Init(void)
{
    mri_port.baud(230400);
    mri_port.printf("----- 230400 USART3 connected! -----\n\r");
    vIRQ_SetVector(USART3_IRQn,(uint32_t)&mri_Handler);
    vIRQ_SetPriority(USART3_IRQn,0);//highest priority in all external interrupts.
    vIRQ_EnableIRQ(USART3_IRQn);
    mriEnableUSART_RxInterrupt(USART3);//Enable USART RX interrupt.
}
