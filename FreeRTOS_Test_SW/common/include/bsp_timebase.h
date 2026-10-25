/*
 * bsp_timebase.h
 *
 * Common STM timebase implementation with per-image RTD bindings.
 */

#ifndef BSP_TIMEBASE_H_
#define BSP_TIMEBASE_H_

#include <stdbool.h>
#include <stdint.h>

#include "Stm_Ip_Types.h"
#include "app_tasks.h"

typedef struct
{
    const Stm_Ip_ChannelConfigType *channelConfig;
    uint8_t channel;
    uint32_t periodTicks;
} BspTimebaseChannelConfigType;

typedef struct
{
    uint8_t instance;
    const Stm_Ip_InstanceConfigType *instanceConfig;
    BspTimebaseChannelConfigType channel[APP_TASK_RATE_COUNT];
} BspTimebaseConfigType;

/* Implemented once per ELF by the project-local RTD binding file. */
const BspTimebaseConfigType *BspTimebase_GetConfig(void);

extern volatile uint32_t
    g_BspTimebaseInterruptCount[APP_TASK_RATE_COUNT];

bool BspTimebase_Init(void);
bool BspTimebase_Start(void);

void BspTimebase_HandleInterrupt(AppTaskRateType rate);

#endif /* BSP_TIMEBASE_H_ */
