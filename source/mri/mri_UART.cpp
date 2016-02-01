#include "mri_UART.h"


//Serial mri_port(PB_10,PB_11);//STM32F429i,USART3

void mri_Handler(void)
{
    //uvisor_ctx->mri_port.printf("mri_Handler\n\r");
    mriClearRxInterrupt(USART3);//USART3
}


