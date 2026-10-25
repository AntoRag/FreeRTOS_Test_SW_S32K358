/*
 * stm_callbacks.c
 *
 * Thin adapters retaining the callback names referenced by generated RTD
 * configuration while routing all behavior through the common timebase.
 */

#include "Mcal.h"
#include "bsp_timebase.h"
#include "stm_test.h"

void App_Core0Stm10msCallback(uint8 channel)
{
    (void)channel;
    BspTimebase_HandleInterrupt(APP_TASK_RATE_10MS);
}

void App_Core0Stm100msCallback(uint8 channel)
{
    (void)channel;
    BspTimebase_HandleInterrupt(APP_TASK_RATE_100MS);
}

void App_Core0Stm1000msCallback(uint8 channel)
{
    (void)channel;
    BspTimebase_HandleInterrupt(APP_TASK_RATE_1000MS);
}

void App_Core0StmTestCallback(uint8 channel)
{
    /* This callback is bound to STM_0_CH_3 by the generated .mex config. */
    StmTest_NotifyFromISR(channel);
}
