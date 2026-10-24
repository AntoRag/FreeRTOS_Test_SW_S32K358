/*
 * bsp_timebase.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#include "bsp_timebase.h"

#include "FreeRTOS.h"
#include "task.h"

/* Stm interfaces. */
#include "Stm_Ip.h"
#include "Stm_Ip_Cfg.h"

#include "app_tasks.h"


volatile uint32_t g_Core0Stm10msInterruptCount		=	0;
volatile uint32_t g_Core0Stm100msInterruptCount		=	0;
volatile uint32_t g_Core0Stm1000msInterruptCount	=	0;



/* ISR callbacks from Stm */
void App_Core0Stm10msCallback(uint8 channel)
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;

    (void)channel;

    g_Core0Stm10msInterruptCount++;

    AppTasks_ReleaseA10msFromISR(&higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

void App_Core0Stm100msCallback(uint8 channel)
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;
    (void)channel;
    g_Core0Stm100msInterruptCount++;

    AppTasks_ReleaseA100msFromISR(&higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}

void App_Core0Stm1000msCallback(uint8 channel)
{
	BaseType_t higherPriorityTaskWoken = pdFALSE;
    (void)channel;
    g_Core0Stm1000msInterruptCount++;

    AppTasks_ReleaseA1000msFromISR(&higherPriorityTaskWoken);

    portYIELD_FROM_ISR(higherPriorityTaskWoken);
}


void BspTimebase_Init(void)
{
	/* Initialise STM*/
	Stm_Ip_Init(APP_STM_INSTANCE,&STM_0_InitConfig_PB);

	Stm_Ip_InitChannel(APP_STM_INSTANCE, STM_0_CH_0);
	Stm_Ip_InitChannel(APP_STM_INSTANCE, STM_0_CH_1);
	Stm_Ip_InitChannel(APP_STM_INSTANCE, STM_0_CH_2);
}

void BspTimebase_Start(void)
{
    /* Start the periodic STM compare channels. */
    Stm_Ip_StartCounting(
        APP_STM_INSTANCE,
        APP_STM_10MS_CHANNEL,
        APP_STM_10MS_TICKS
    );

    Stm_Ip_StartCounting(
        APP_STM_INSTANCE,
        APP_STM_100MS_CHANNEL,
        APP_STM_100MS_TICKS
    );

    Stm_Ip_StartCounting(
        APP_STM_INSTANCE,
        APP_STM_1000MS_CHANNEL,
        APP_STM_1000MS_TICKS
    );
}
