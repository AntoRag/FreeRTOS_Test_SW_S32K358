/*
 * core0_main.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */
#include "core0_main.h"

#include "FreeRTOS.h"
#include "task.h"

#include "app_tasks.h"
#include "bsp_init.h"
#include "bsp_timebase.h"

AppFaultCodeType Core0_Main(void)
{
	/*
	 * Initialization local to CM7_0.
	 */
	BspInitStatusType bspStatus;

	bspStatus = Bsp_CoreInit();
	if (bspStatus != BSP_INIT_SUCCESS)
	{
		return APP_FATAL_INTERRUPT_INIT;
	}


	/*
	 * CM7_0-owned application resources.
	 */
	/* Initialise timebase + create task(s) */
	BspTimebase_Init();

	if (AppTasks_Create() != pdPASS)
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
