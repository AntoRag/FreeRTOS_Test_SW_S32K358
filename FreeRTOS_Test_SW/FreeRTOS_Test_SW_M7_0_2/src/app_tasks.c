/*
 * app_tasks.c
 *
 * CM7_2-owned TaskB definitions. The task API and diagnostics layout are
 * common, while the actual workloads remain local to this AMP image.
 */

#include <stddef.h>

#include "app_tasks.h"

volatile AppTaskDiagnosticsType
    g_AppTaskDiagnostics[APP_TASK_RATE_COUNT] = { { 0U, 0U } };

static TaskHandle_t s_TaskHandles[APP_TASK_RATE_COUNT] = { NULL };

static void App_TaskB10ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /* Consume one release produced by the local STM 10 ms channel. */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        g_AppTaskDiagnostics[APP_TASK_RATE_10MS].executionCount++;

        /* Future CM7_2 10 ms workload goes here. */
    }
}

static void App_TaskB100ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /* Consume one release produced by the local STM 100 ms channel. */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        g_AppTaskDiagnostics[APP_TASK_RATE_100MS].executionCount++;

        /* Future CM7_2 100 ms workload goes here. */
    }
}

static void App_TaskB1000ms(void *parameters)
{
    (void)parameters;

    for (;;)
    {
        /* Consume one release produced by the local STM 1000 ms channel. */
        (void)ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        g_AppTaskDiagnostics[APP_TASK_RATE_1000MS].executionCount++;

        /* Future CM7_2 1000 ms workload goes here. */
    }
}

void AppTasks_ReleaseFromISR(
    AppTaskRateType rate,
    BaseType_t *higherPriorityTaskWoken
)
{
    /* The enum is also the index shared by tasks, STM channels and counters. */
    if ((rate < APP_TASK_RATE_COUNT) &&
        (s_TaskHandles[rate] != NULL) &&
        (higherPriorityTaskWoken != NULL))
    {
        vTaskNotifyGiveFromISR(
            s_TaskHandles[rate],
            higherPriorityTaskWoken
        );
    }
}

BaseType_t AppTasks_Create(void)
{
    BaseType_t status;

    status = xTaskCreate(
        App_TaskB10ms,
        "TaskB_10ms",
        APP_TASK_10MS_STACK_DEPTH,
        NULL,
        APP_TASK_10MS_PRIORITY,
        &s_TaskHandles[APP_TASK_RATE_10MS]
    );

    if (status != pdPASS)
    {
        return pdFAIL;
    }

    status = xTaskCreate(
        App_TaskB100ms,
        "TaskB_100ms",
        APP_TASK_100MS_STACK_DEPTH,
        NULL,
        APP_TASK_100MS_PRIORITY,
        &s_TaskHandles[APP_TASK_RATE_100MS]
    );

    if (status != pdPASS)
    {
        return pdFAIL;
    }

    return xTaskCreate(
        App_TaskB1000ms,
        "TaskB_1000ms",
        APP_TASK_1000MS_STACK_DEPTH,
        NULL,
        APP_TASK_1000MS_PRIORITY,
        &s_TaskHandles[APP_TASK_RATE_1000MS]
    );
}
