#include "CrashCatcher.h"
#include "CatcherDump.h"
#include <stdarg.h>

Serial uart(STDIO_UART_TX, STDIO_UART_RX);

static void cC_printf(const char * format,...)
{    
    va_list args;
    va_start(args, format);

    uart.vprintf(format, args);

    va_end(args);
}


void CrashCatcher_DumpStart(void) 
{
    uart.baud(115200);
    cC_printf("\r\n\r\nCRASH ENCOUNTERED\r\n"
                 "Enable logging and then press any key to start dump.\r\n");
}

