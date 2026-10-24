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

#define APP_TASK_A_10MS_PRIORITY        				(tskIDLE_PRIORITY + 3U)
#define APP_TASK_A_100MS_PRIORITY        				(tskIDLE_PRIORITY + 2U)
#define APP_TASK_A_1000MS_PRIORITY        				(tskIDLE_PRIORITY + 1U)


#define APP_TASK_A_10MS_STACK_DEPTH    					(256U)
#define APP_TASK_A_100MS_STACK_DEPTH    				(256U)
#define APP_TASK_A_1000MS_STACK_DEPTH    				(256U)

#define APP_TASK_B_10MS_PRIORITY        				(tskIDLE_PRIORITY + 3U)
#define APP_TASK_B_100MS_PRIORITY        				(tskIDLE_PRIORITY + 2U)
#define APP_TASK_B_1000MS_PRIORITY        				(tskIDLE_PRIORITY + 1U)


#define APP_TASK_B_10MS_STACK_DEPTH    					(256U)
#define APP_TASK_B_100MS_STACK_DEPTH    				(256U)
#define APP_TASK_B_1000MS_STACK_DEPTH    				(256U)


extern volatile uint32_t g_Core0IdleIterationCount;
extern volatile uint32_t g_Core0TaskA10msExecutionCount;
extern volatile uint32_t g_Core0TaskA100msExecutionCount;
extern volatile uint32_t g_Core0TaskA1000msExecutionCount;

extern volatile uint32_t g_Core2IdleIterationCount;
extern volatile uint32_t g_Core2TaskB10msExecutionCount;
extern volatile uint32_t g_Core2TaskB100msExecutionCount;
extern volatile uint32_t g_Core2TaskB1000msExecutionCount;

BaseType_t AppTasks_CreateA(void);

BaseType_t AppTasks_CreateB(void);

void AppTasks_ReleaseA10msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseA100msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseA1000msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseB10msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseB100msFromISR(BaseType_t *higherPriorityTaskWoken);

void AppTasks_ReleaseB1000msFromISR(BaseType_t *higherPriorityTaskWoken);


#endif /* APP_TASKS_CORE0_H_ */
