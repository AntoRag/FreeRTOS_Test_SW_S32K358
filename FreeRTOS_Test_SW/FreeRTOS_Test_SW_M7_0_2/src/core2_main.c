/*
 * core2_main.c
 *
 * Created on: 24 ott 2026
 *     Author: anton
 *
 * CM7_2 application entrypoint.
 */

#include "core2_main.h"

volatile uint32_t g_Core2MainEntered = 0U;

void Core2_Main(void)
{
    /*
     * Reaching this assignment proves that:
     *
     * 1. CM7_2 executed Core2_Boot();
     * 2. the shared mailbox was recognized;
     * 3. CM7_0 published SYSTEM_READY;
     * 4. the protocol version matched.
     */
    g_Core2MainEntered = 1U;

    /*
     * Temporary application placeholder.
     *
     * The CM7_2 FreeRTOS scheduler and TaskB set will replace this loop
     * after the boot synchronization path is independently proven.
     */
    for (;;)
    {
        /* Intentionally empty. */
    }
}
