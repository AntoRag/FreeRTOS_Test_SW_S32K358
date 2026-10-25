/*
 * core0_main.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */
#include "app_system_status.h"
#include "app_tasks.h"
#include "bsp_core.h"
#include "bsp_timebase.h"
#include "core0_main.h"
#include "stm_test.h"

#include "FreeRTOS.h"
#include "task.h"


AppFaultCodeType Core0_Main(void)
{
	/*
	 * Initialization local to CM7_0.
	 */
	BspInitStatusType bspStatus;

	bspStatus = BspCore_Init();
	if (bspStatus != BSP_INIT_SUCCESS)
	{
		if (bspStatus == BSP_INIT_CORE_MISMATCH)
		{
			return APP_FATAL_CORE_ID;
		}

		return (bspStatus == BSP_INIT_INVALID_CONFIG)
		    ? APP_FATAL_BSP_CONFIG
		    : APP_FATAL_INTERRUPT_INIT;
	}


	/*
	 * CM7_0-owned application resources.
	 */
	/* Initialise timebase + create task(s) */
	if (!BspTimebase_Init())
	{
		return APP_FATAL_TIMEBASE_INIT;
	}

	if (!StmTest_Init())
	{
		return APP_FATAL_STM_TEST_INIT;
	}

	if (AppTasks_Create() != pdPASS)
	{
		return APP_FATAL_TASK_CREATE;
	}

	/*
	 * Task handles exist before the STM channels are started.
	 */
	/*
	 * READY means that all core-local resources and task handles exist.
	 * It does not claim that the scheduler has already started executing.
	 */
	(void)AppSystemStatus_PublishCurrentCore(
	    APP_CORE_STATE_READY,
	    (uint32_t)APP_FATAL_NONE
	);

	if (!BspTimebase_Start())
	{
		return APP_FATAL_TIMEBASE_INIT;
	}

	vTaskStartScheduler();


	return APP_FATAL_SCHEDULER_RETURNED;
}
