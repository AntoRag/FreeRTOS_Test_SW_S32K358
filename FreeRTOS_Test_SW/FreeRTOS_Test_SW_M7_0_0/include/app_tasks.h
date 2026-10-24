/*
 * app_tasks.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef APP_TASKS_H_
#define APP_TASKS_H_

/* FreeRTOS kernel interfaces. */
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>


extern volatile uint32_t g_Core0IdleIterationCount;
extern volatile uint32_t g_Core0TaskA10msExecutionCount;
extern volatile uint32_t g_Core0TaskA100msExecutionCount;
extern volatile uint32_t g_Core0TaskA1000msExecutionCount;


BaseType_t AppTasks_Create(void);

void AppTasks_ReleaseA10msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseA100msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseA1000msFromISR(BaseType_t *higherPriorityTaskWoken);


#endif /* APP_TASKS_H_ */
