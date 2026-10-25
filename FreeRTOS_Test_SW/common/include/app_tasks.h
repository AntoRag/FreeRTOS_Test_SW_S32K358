/*
 * app_tasks.h
 *
 * Common interface implemented independently by the TaskA and TaskB images.
 */

#ifndef APP_TASKS_H_
#define APP_TASKS_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#define APP_TASK_10MS_PRIORITY       (tskIDLE_PRIORITY + 3U)
#define APP_TASK_100MS_PRIORITY      (tskIDLE_PRIORITY + 2U)
#define APP_TASK_1000MS_PRIORITY     (tskIDLE_PRIORITY + 1U)

#define APP_TASK_10MS_STACK_DEPTH    (256U)
#define APP_TASK_100MS_STACK_DEPTH   (256U)
#define APP_TASK_1000MS_STACK_DEPTH  (256U)

typedef enum
{
    APP_TASK_RATE_10MS = 0U,
    APP_TASK_RATE_100MS,
    APP_TASK_RATE_1000MS,
    APP_TASK_RATE_COUNT
} AppTaskRateType;

typedef struct
{
    volatile uint32_t executionCount;
    volatile uint32_t lastError;
} AppTaskDiagnosticsType;

/*
 * These symbols exist independently in each ELF. The ELF/debug session
 * identifies whether the array describes TaskA or TaskB.
 */
extern volatile AppTaskDiagnosticsType
    g_AppTaskDiagnostics[APP_TASK_RATE_COUNT];

extern volatile uint32_t g_AppIdleIterationCount;

BaseType_t AppTasks_Create(void);

void AppTasks_ReleaseFromISR(
    AppTaskRateType rate,
    BaseType_t *higherPriorityTaskWoken
);

#endif /* APP_TASKS_H_ */
