/*
 * app_tasks.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */
#include <app_tasks.h>


volatile uint32_t g_Core2TaskB10msExecutionCount	=	0;
volatile uint32_t g_Core2TaskB100msExecutionCount	=	0;
volatile uint32_t g_Core2TaskB1000msExecutionCount	=	0;

static TaskHandle_t s_Core2TaskB10msHandle 				= 	NULL;
static TaskHandle_t s_Core2TaskB100msHandle 			= 	NULL;
static TaskHandle_t s_Core2TaskB1000msHandle 			= 	NULL;



/* Initial task used to validate notification blocking. */
static void App_TaskB10ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /*
         * Block until STM0 CH0 releases this task.
         * pdFALSE consumes one pending notification at a time.
         */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_Core2TaskB10msExecutionCount++;

        /* Future 10 ms application workload goes here. */
    }
}

static void App_TaskB100ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /*
         * Block until STM0 CH1 releases this task.
         * pdFALSE consumes one pending notification at a time.
         */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_Core2TaskB100msExecutionCount++;

        /* Future 100 ms application workload goes here. */
    }
}

static void App_TaskB1000ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /*
         * Block until STM0 CH2 releases this task.
         * pdFALSE consumes one pending notification at a time.
         */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

        g_Core2TaskB1000msExecutionCount++;

        /* Future 1000 ms application workload goes here. */
    }
}


void AppTasks_ReleaseB10msFromISR(BaseType_t *higherPriorityTaskWoken)
{
	vTaskNotifyGiveFromISR(s_Core2TaskB10msHandle,higherPriorityTaskWoken);
}

void AppTasks_ReleaseB100msFromISR(BaseType_t *higherPriorityTaskWoken)
{
	vTaskNotifyGiveFromISR(s_Core2TaskB100msHandle,higherPriorityTaskWoken);
}

void AppTasks_ReleaseB1000msFromISR(BaseType_t *higherPriorityTaskWoken)
{
	vTaskNotifyGiveFromISR(s_Core2TaskB1000msHandle,higherPriorityTaskWoken);
}

BaseType_t AppTasks_CreateB(void)
{
    BaseType_t status;

    status = xTaskCreate(
        App_TaskB10ms,
        "TaskB_10ms",
        APP_TASK_B_10MS_STACK_DEPTH,
        NULL,
        APP_TASK_B_10MS_PRIORITY,
        &s_Core2TaskB10msHandle
    );

    if (status != pdPASS)
    {
        return pdFAIL;
    }

    status = xTaskCreate(
        App_TaskB100ms,
        "TaskB_100ms",
        APP_TASK_B_100MS_STACK_DEPTH,
        NULL,
        APP_TASK_B_100MS_PRIORITY,
        &s_Core2TaskB100msHandle
    );

    if (status != pdPASS)
    {
        return pdFAIL;
    }

    status = xTaskCreate(
        App_TaskB1000ms,
        "TaskB_1000ms",
        APP_TASK_B_1000MS_STACK_DEPTH,
        NULL,
        APP_TASK_B_1000MS_PRIORITY,
        &s_Core2TaskB1000msHandle
    );

    return status;
}
