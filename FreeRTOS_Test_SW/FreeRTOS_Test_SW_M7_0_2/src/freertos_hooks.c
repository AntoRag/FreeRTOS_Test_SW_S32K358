/*
 * freertos_hooks.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */
#include <app_fault.h>
#include "FreeRTOS.h"
#include "task.h"


volatile uint32_t g_Core2IdleIterationCount			=	0;

void vApplicationIdleHook(void)
{
    g_Core2IdleIterationCount++;
}


/* vApplicationMallocFailedHook() captures failures from the temporary dynamic-allocation configuration. */
void vApplicationMallocFailedHook(void)
{
    AppFault_B_Set(APP_FATAL_MALLOC_FAILED);

    taskDISABLE_INTERRUPTS();

    for (;;)
    {
        /* Fatal error: FreeRTOS heap allocation failed. */
    }
}

/*vApplicationStackOverflowHook() captures stack corruption detected during context switching*/
void vApplicationStackOverflowHook(
    TaskHandle_t taskHandle,
    char *taskName
)
{
    (void)taskHandle;
    (void)taskName;

    AppFault_B_Set(APP_FATAL_STACK_OVERFLOW);

    taskDISABLE_INTERRUPTS();

    for (;;)
    {
        /* Fatal error: task stack corruption detected. */
    }
}
