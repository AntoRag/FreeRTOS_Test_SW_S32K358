/*==================================================================================================
* Project : RTD AUTOSAR 4.9
* Platform : CORTEXM
* Peripheral : S32K3XX
* Dependencies : none
*
* Autosar Version : 4.9.0
* Autosar Revision : ASR_REL_4_9_REV_0000
* Autosar Conf.Variant :
* SW Version : 7.0.0
* Build Version : S32K3_RTD_7_0_0_QLP03_D2512_ASR_REL_4_9_REV_0000_20251210
*
* Copyright 2020 - 2025 NXP
*
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file main.c
*
*   @addtogroup main_module main module documentation
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif

/* Including necessary configuration files. */
#include "Mcal.h"


/* FreeRTOS kernel interfaces. */
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>

/* Scheduler-smoke configuration. */
#define APP_TASK_10MS_PRIORITY       (tskIDLE_PRIORITY + 3U)
#define APP_TASK_10MS_STACK_DEPTH    (256U)

#define APP_FATAL_NONE               (0U)
#define APP_FATAL_TASK_CREATE        (1U)
#define APP_FATAL_SCHEDULER_RETURNED (2U)

static TaskHandle_t s_Core0TaskA10msHandle = NULL;

volatile uint32_t g_Core0TaskA10msStarted        = 0U;
volatile uint32_t g_Core0TaskA10msReleaseCount   = 0U;
volatile uint32_t g_Core0TaskA10msExecutionCount = 0U;
volatile uint32_t g_Core0FatalError              = APP_FATAL_NONE;


/* Initial task used to validate notification blocking. */
static void App_TaskA10ms(void *pvParameters)
{
    uint32_t notificationCount;

    (void)pvParameters;

    g_Core0TaskA10msStarted = 1U;

    for (;;)
    {
        notificationCount = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        g_Core0TaskA10msReleaseCount += notificationCount;
        g_Core0TaskA10msExecutionCount++;
    }
}


int main(void)
{
    BaseType_t taskCreateResult;

    taskCreateResult = xTaskCreate(
		App_TaskA10ms,
			"C0_TaskA10ms",
			APP_TASK_10MS_STACK_DEPTH,
			NULL, APP_TASK_10MS_PRIORITY,
			&s_Core0TaskA10msHandle
			);

    if (taskCreateResult != pdPASS)
    {
		g_Core0FatalError = APP_FATAL_TASK_CREATE;
        for(;;)
        {
			/* Task creation failed, inspect g_Core0FatalError */
        }
    }

    vTaskStartScheduler();

    /*
    * A correctly started scheduler never returns.
    * Reaching this point is always an error.
    */
    g_Core0FatalError = APP_FATAL_SCHEDULER_RETURNED;

    for (;;)
    {
        /* Fatal state. */
    }

}

#ifdef __cplusplus
}
#endif
