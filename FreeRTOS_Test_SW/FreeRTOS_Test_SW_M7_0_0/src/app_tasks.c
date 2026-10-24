/*
 * app_tasks.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */
#include <app_tasks.h>


volatile uint32_t g_Core0TaskA10msExecutionCount	=	0;
volatile uint32_t g_Core0TaskA100msExecutionCount	=	0;
volatile uint32_t g_Core0TaskA1000msExecutionCount	=	0;

static TaskHandle_t s_Core0TaskA10msHandle 				= 	NULL;
static TaskHandle_t s_Core0TaskA100msHandle 			= 	NULL;
static TaskHandle_t s_Core0TaskA1000msHandle 			= 	NULL;



/* Initial task used to validate notification blocking. */
static void App_TaskA10ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /*
         * Block until STM0 CH0 releases this task.
         * pdFALSE consumes one pending notification at a time.
         */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_Core0TaskA10msExecutionCount++;

        /* Future 10 ms application workload goes here. */
    }
}

static void App_TaskA100ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /*
         * Block until STM0 CH1 releases this task.
         * pdFALSE consumes one pending notification at a time.
         */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_Core0TaskA100msExecutionCount++;

        /* Future 100 ms application workload goes here. */
    }
}

static void App_TaskA1000ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /*
         * Block until STM0 CH2 releases this task.
         * pdFALSE consumes one pending notification at a time.
         */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_Core0TaskA1000msExecutionCount++;

        /* Future 1000 ms application workload goes here. */
    }
}


void AppTasks_ReleaseA10msFromISR(BaseType_t *higherPriorityTaskWoken)
{
	vTaskNotifyGiveFromISR(s_Core0TaskA10msHandle,higherPriorityTaskWoken);
}

void AppTasks_ReleaseA100msFromISR(BaseType_t *higherPriorityTaskWoken)
{
	vTaskNotifyGiveFromISR(s_Core0TaskA100msHandle,higherPriorityTaskWoken);
}

void AppTasks_ReleaseA1000msFromISR(BaseType_t *higherPriorityTaskWoken)
{
	vTaskNotifyGiveFromISR(s_Core0TaskA1000msHandle,higherPriorityTaskWoken);
}

BaseType_t AppTasks_CreateA(void)
{
    BaseType_t status;

    status = xTaskCreate(
        App_TaskA10ms,
        "TaskA_10ms",
        APP_TASK_A_10MS_STACK_DEPTH,
        NULL,
        APP_TASK_A_10MS_PRIORITY,
        &s_Core0TaskA10msHandle
    );

    if (status != pdPASS)
    {
        return pdFAIL;
    }

    status = xTaskCreate(
        App_TaskA100ms,
        "TaskA_100ms",
        APP_TASK_A_100MS_STACK_DEPTH,
        NULL,
        APP_TASK_A_100MS_PRIORITY,
        &s_Core0TaskA100msHandle
    );

    if (status != pdPASS)
    {
        return pdFAIL;
    }

    status = xTaskCreate(
        App_TaskA1000ms,
        "TaskA_1000ms",
        APP_TASK_A_1000MS_STACK_DEPTH,
        NULL,
        APP_TASK_A_1000MS_PRIORITY,
        &s_Core0TaskA1000msHandle
    );

    return status;
}
