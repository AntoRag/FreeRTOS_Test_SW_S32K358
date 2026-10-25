/*
 * bsp_timebase.c
 *
 * Shared STM initialization, start and interrupt-to-task dispatch.
 */

#include <stddef.h>

#include "bsp_timebase.h"

#include "Stm_Ip.h"

volatile uint32_t
    g_BspTimebaseInterruptCount[APP_TASK_RATE_COUNT] = { 0U };

static bool BspTimebase_IsConfigValid(
    const BspTimebaseConfigType *config
)
{
    uint32_t rateIndex;

    if ((config == NULL) || (config->instanceConfig == NULL))
    {
        return false;
    }

    for (rateIndex = 0U;
         rateIndex < (uint32_t)APP_TASK_RATE_COUNT;
         rateIndex++)
    {
        if ((config->channel[rateIndex].channelConfig == NULL) ||
            (config->channel[rateIndex].periodTicks == 0U))
        {
            return false;
        }
    }

    return true;
}

bool BspTimebase_Init(void)
{
    const BspTimebaseConfigType *config;
    uint32_t rateIndex;

    config = BspTimebase_GetConfig();

    if (!BspTimebase_IsConfigValid(config))
    {
        return false;
    }

    Stm_Ip_Init(config->instance, config->instanceConfig);

    for (rateIndex = 0U;
         rateIndex < (uint32_t)APP_TASK_RATE_COUNT;
         rateIndex++)
    {
        Stm_Ip_InitChannel(
            config->instance,
            config->channel[rateIndex].channelConfig
        );
    }

    return true;
}

bool BspTimebase_Start(void)
{
    const BspTimebaseConfigType *config;
    uint32_t rateIndex;

    config = BspTimebase_GetConfig();

    if (!BspTimebase_IsConfigValid(config))
    {
        return false;
    }

    for (rateIndex = 0U;
         rateIndex < (uint32_t)APP_TASK_RATE_COUNT;
         rateIndex++)
    {
        Stm_Ip_StartCounting(
            config->instance,
            config->channel[rateIndex].channel,
            config->channel[rateIndex].periodTicks
        );
    }

    return true;
}

void BspTimebase_HandleInterrupt(AppTaskRateType rate)
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;

    if (rate < APP_TASK_RATE_COUNT)
    {
        g_BspTimebaseInterruptCount[rate]++;
        AppTasks_ReleaseFromISR(rate, &higherPriorityTaskWoken);
        portYIELD_FROM_ISR(higherPriorityTaskWoken);
    }
}
