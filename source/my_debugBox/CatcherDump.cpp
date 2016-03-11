#include "CrashCatcher.h"
#include "CatcherDump.h"

Serial uart(STDIO_UART_TX, STDIO_UART_RX);


void CrashCatcher_DumpStart(void) 
{
    uart.baud(115200);
    uart.printf("\r\n\r\nCRASH ENCOUNTERED\r\n"
                 "Enable logging and then press any key to start dump.\r\n");
}
