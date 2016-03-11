#include "CrashCatcher.h"
#include "CatcherDump.h"

void CrashCatcher_DumpStart(void) 
{
    cC_printf("\r\n\r\nCRASH ENCOUNTERED\r\n"
                 "Enable logging and start dump.\r\n");
}

