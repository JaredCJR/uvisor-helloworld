/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/mbed.h"
#include "minar/minar.h"
#include "core-util/FunctionPointer.h"
#include "uvisor-lib/uvisor-lib.h"
#include "main-hw.h"
#include "box-challenge.h"
#include "box-debug.h"
#include "btn.h"
#include "CrashCatcher/CatcherDump.h"

using mbed::util::FunctionPointer0;


/* Create ACLs for main box. */
MAIN_ACL(g_main_acl);

/* Enable uVisor. */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);

DigitalOut led(MAIN_LED);
Serial pc(STDIO_UART_TX, STDIO_UART_RX);


/*CrashCatcher storage*/
ACLtoCrashCatcherMemoryRegion ACLs_warehouse_CrashCatcher;
CatcherStack_TypeDef * pg_crashCatcherStack = (CatcherStack_TypeDef *)CatcherStack_Base;

uint8_t g_challenge[CHALLENGE_SIZE];
minar::Scheduler *g_scheduler;
minar::callback_handle_t g_event = NULL;

static void toggle_led(void)
{
    led = !led;
}

static void retry_secret(void)
{
    /* Check the secret. */
    pc.printf("Verifying secret... ");
    bool verified = challenge_verify(g_challenge, sizeof(g_challenge));
    pc.printf("%s\r\n", verified ? "Match" : "Mismatch");

    /* Cancel the previous event for LED blinking. */
    g_scheduler->cancelCallback(g_event);

    /* Schedule the new LED blinking pattern. */
    /* The blinking frequency will be faster if the secret has been verified. */
    g_event = minar::Scheduler::postCallback(FunctionPointer0<void>(toggle_led).bind())
                .period(minar::milliseconds(verified ? 100 : 500))
                .tolerance(minar::milliseconds(1))
                .getHandle();
}

static void CrashCatcher_init(CatcherStack_TypeDef *g_crashCatcherStack)
{
    /* clear CrashCatcher stack*/
    for(int i =0;i<CRASH_CATCHER_STACK_WORD_COUNT;i++)
    {
        g_crashCatcherStack->stack[i] = 0;
    }
    for(int i =0;i<CRASH_CATCHER_AUTO_STACKED_WORD_COUNT;i++)
    {
        g_crashCatcherStack->auto_stack[i] = 0;
    }
    for(int i =0;i<CRASH_CATCHER_FAULT_REGISTERS_WORD_COUNT;i++)
    {
        g_crashCatcherStack->fault_status[i] = 0;
    }

    /*register the ACLs for CrashCatcher*/
    targetACLs_register((UvisorBoxAclItem *)&g_main_acl, 12 ,&ACLs_warehouse_CrashCatcher);

}


void app_start(int, char *[])
{
    CrashCatcher_init(pg_crashCatcherStack);

    /* Set the console baud-rate. */
    pc.baud(115200);

    pc.printf("\r\n***** uvisor-helloworld example *****\r\n");

    /* Initialize the debug box. */
    box_debug::init();

    /* Reset the challenge. */
    memset(&g_challenge, 0, sizeof(g_challenge));

    /* Turn the LED off. */
    led = LED_OFF;

    /* Initialize the challenge. */
    challenge_init();

    /* Configure the pushbutton. */
    btn_init();

    /* Get a handle to the scheduler. */
    g_scheduler = minar::Scheduler::instance();

    /* Schedule an event to periodically check for the secret. */
    minar::Scheduler::postCallback(FunctionPointer0<void>(retry_secret).bind())
        .period(minar::milliseconds(1000))
        .tolerance(minar::milliseconds(100));

    pc.printf("Main unprivileged box configured\r\n");

    pc.printf("CrashCatcher stack:\n\r");
    pc.printf("lr=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-1]);
    pc.printf("r11=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-2]);
    pc.printf("r10=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-3]);
    pc.printf("r9=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-4]);
    pc.printf("r8=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-5]);
    pc.printf("r7=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-6]);
    pc.printf("r6=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-7]);
    pc.printf("r5=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-8]);
    pc.printf("r4=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-9]);
    pc.printf("msp=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-10]);
    pc.printf("psp=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-11]);
    pc.printf("xPSR=%p\n\r",pg_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-12]);

    pc.printf("PSR=%p\n\r",pg_crashCatcherStack->auto_stack[7]);
    pc.printf("pc=%p\n\r",pg_crashCatcherStack->auto_stack[6]);
    pc.printf("lr=%p\n\r",pg_crashCatcherStack->auto_stack[5]);
    pc.printf("r12=%p\n\r",pg_crashCatcherStack->auto_stack[4]);
    pc.printf("r3=%p\n\r",pg_crashCatcherStack->auto_stack[3]);
    pc.printf("r2=%p\n\r",pg_crashCatcherStack->auto_stack[2]);
    pc.printf("r1=%p\n\r",pg_crashCatcherStack->auto_stack[1]);
    pc.printf("r0=%p\n\r",pg_crashCatcherStack->auto_stack[0]);

    CrashCatcher_Entry(pg_crashCatcherStack);
}

void cC_printf(const char * format,...)
{    
    va_list args;
    va_start(args, format);
    pc.vprintf(format, args);
    va_end(args);
}

/* Let CrashCatcher know what RAM contents should be part of crash dump.
 * The last "regions" must end with "{0xFFFFFFFF, 0xFFFFFFFF}"
 */
extern "C" const CrashCatcherMemoryRegion* CrashCatcher_GetMemoryRegions(void) 
{ 
    static const CrashCatcherMemoryRegion regions[] = { 
                                                        {0xFFFFFFFF, 0xFFFFFFFF, CRASH_CATCHER_BYTE}
                                                      };
    return regions;
}


