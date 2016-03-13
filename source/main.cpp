/*
 * Copyright (c) 2013-2015, ARM Limited, All Rights Reserved
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
#include "my_debugBox/CrashCatcher.h"
#include "my_debugBox/CatcherDump.h"


using mbed::util::FunctionPointer0;

/* create ACLs for main box */
MAIN_ACL(g_main_acl);

/* enable uvisor */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);

DigitalOut led(MAIN_LED);
Serial pc(STDIO_UART_TX, STDIO_UART_RX);

uint8_t g_challenge[CHALLENGE_SIZE];
minar::Scheduler *g_scheduler;
minar::callback_handle_t g_event = NULL;

static void toggle_led(void)
{
    led = !led;
}

static void retry_secret(void)
{
    /* check secret */
    pc.printf("verifying secret...");
    bool verified = verify_secret(g_challenge, sizeof(g_challenge));
    pc.printf(" done\n\r");

    /* cancel previous event for LED blinking */
    g_scheduler->cancelCallback(g_event);

    /* schedule new LED blinking pattern */
    /* the blinking frequency will be faster if the secret has been verified */
    g_event = minar::Scheduler::postCallback(FunctionPointer0<void>(toggle_led).bind())
                .period(minar::milliseconds(verified ? 100 : 500))
                .tolerance(minar::milliseconds(1))
                .getHandle();
}

void app_start(int, char *[])
{
    /* clear CrashCatcher stack*/
    for(int i =0;i<CRASH_CATCHER_STACK_WORD_COUNT;i++)
    {
        g_crashCatcherStack->stack[i] = 0;
        g_crashCatcherStack->auto_stack[i] = 0;
    }

    /* set the console baud-rate */
    pc.baud(115200);

    pc.printf("***** uvisor-helloworld example *****\n\r");

    /* Initialize the debug box. */
    box_debug::init();

    /* reset challenge */
    memset(&g_challenge, 0, sizeof(g_challenge));

    /* turn LED off */
    led = LED_OFF;

    /* configure pushbutton */
    btn_init();

    /* get handle of scheduler */
    g_scheduler = minar::Scheduler::instance();

    /* schedule event to periodically check for secret */
    minar::Scheduler::postCallback(FunctionPointer0<void>(retry_secret).bind())
        .period(minar::milliseconds(1000))
        .tolerance(minar::milliseconds(100));

    pc.printf("main unprivileged box configured\n\r");

    pc.printf("CrashCatcher stack:\n\r");
    pc.printf("lr=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-1]);
    pc.printf("r11=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-2]);
    pc.printf("r10=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-3]);
    pc.printf("r9=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-4]);
    pc.printf("r8=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-5]);
    pc.printf("r7=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-6]);
    pc.printf("r6=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-7]);
    pc.printf("r5=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-8]);
    pc.printf("r4=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-9]);
    pc.printf("msp=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-10]);
    pc.printf("psp=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-11]);
    pc.printf("xPSR=%p\n\r",g_crashCatcherStack->stack[CRASH_CATCHER_STACK_WORD_COUNT-12]);

    pc.printf("PSR=%p\n\r",g_crashCatcherStack->auto_stack[7]);
    pc.printf("pc=%p\n\r",g_crashCatcherStack->auto_stack[6]);
    pc.printf("lr=%p\n\r",g_crashCatcherStack->auto_stack[5]);
    pc.printf("r12=%p\n\r",g_crashCatcherStack->auto_stack[4]);
    pc.printf("r3=%p\n\r",g_crashCatcherStack->auto_stack[3]);
    pc.printf("r2=%p\n\r",g_crashCatcherStack->auto_stack[2]);
    pc.printf("r1=%p\n\r",g_crashCatcherStack->auto_stack[1]);
    pc.printf("r0=%p\n\r",g_crashCatcherStack->auto_stack[0]);

    //CrashCatcher_Entry();
}

void cC_printf(const char * format,...)
{    
    va_list args;
    va_start(args, format);
    pc.vprintf(format, args);
    va_end(args);
}
