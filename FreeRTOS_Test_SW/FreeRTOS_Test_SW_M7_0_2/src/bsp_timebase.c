/*
 * bsp_timebase.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#include <app_tasks.h>
#include <bsp_timebase.h>
#include "FreeRTOS.h"
#include "task.h"

/* Stm interfaces. */
#include "Stm_Ip.h"
#include "Stm_Ip_Cfg.h"



volatile uint32_t g_Core2Stm10msInterruptCount		=	0;
volatile uint32_t g_Core2Stm100msInterruptCount		=	0;
volatile uint32_t g_Core2Stm1000msInterruptCount	=	0;



/* ISR callbacks from Stm */
void App_Core2Stm10msCallback(uint8 channel)
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;

    (void)channel;

    g_Core2Stm10msInterruptCount++;

    AppTasks_ReleaseB10msFromISR(&higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

void App_Core2Stm100msCallback(uint8 channel)
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;
    (void)channel;
    g_Core2Stm100msInterruptCount++;

    AppTasks_ReleaseB100msFromISR(&higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

void App_Core2Stm1000msCallback(uint8 channel)
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;
    (void)channel;
    g_Core2Stm1000msInterruptCount++;

    AppTasks_ReleaseB1000msFromISR(&higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}


void BspTimebase_Init(void)
{
	Stm_Ip_Init(APP_B_STM_INSTANCE,&STM_2_InitConfig_PB);
	/* Initialise STM*/
	Stm_Ip_InitChannel(APP_B_STM_INSTANCE, STM_2_CH_0);
	Stm_Ip_InitChannel(APP_B_STM_INSTANCE, STM_2_CH_1);
	Stm_Ip_InitChannel(APP_B_STM_INSTANCE, STM_2_CH_2);
}

void BspTimebase_Start(void)
{
    /* Start the periodic STM compare channels. */
    Stm_Ip_StartCounting(
        APP_B_STM_INSTANCE,
        APP_B_STM_10MS_CHANNEL,
        APP_B_STM_10MS_TICKS
    );

    Stm_Ip_StartCounting(
        APP_B_STM_INSTANCE,
        APP_B_STM_100MS_CHANNEL,
        APP_B_STM_100MS_TICKS
    );

    Stm_Ip_StartCounting(
        APP_B_STM_INSTANCE,
        APP_B_STM_1000MS_CHANNEL,
        APP_B_STM_1000MS_TICKS
    );
}
