/*
 * core2_main.c
 *
 * Created on: 24 ott 2026
 *     Author: anton
 *
 * CM7_2 application entrypoint.
 */

#include <app_tasks.h>
#include <app_fault.h>
#include <bsp_init.h>
#include <bsp_timebase.h>
#include "core2_main.h"

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t g_Core2MainEntered = 0U;

AppFaultCodeType Core2_Main(void)
{
    /*
     * Reaching this assignment proves that:
     *
     * 1. CM7_2 executed Core2_Boot();
     * 2. the shared mailbox was recognized;
     * 3. CM7_0 published SYSTEM_READY;
     * 4. the protocol version matched.
     */
    g_Core2MainEntered = 1U;

	/*
	 * Initialization local to CM7_2.
	 */
	BspInitStatusType bspStatus;

	bspStatus = Bsp_Core2Init();
	if (bspStatus != BSP_INIT_SUCCESS)
	{
		return APP_FATAL_INTERRUPT_INIT;
	}


	/*
	 * CM7_2-owned application resources.
	 */
	/* Initialise timebase + create task(s) */
	BspTimebase_Init();

	if (AppTasks_CreateB() != pdPASS)
	{
		return APP_FATAL_TASK_CREATE;
	}

	/*
	 * Task handles exist before the STM channels are started.
	 */
	BspTimebase_Start();

	vTaskStartScheduler();


	return APP_FATAL_SCHEDULER_RETURNED;
}
