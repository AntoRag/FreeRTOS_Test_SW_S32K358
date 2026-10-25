/*
 * core2_main.c
 *
 * Created on: 24 ott 2026
 *     Author: anton
 *
 * CM7_2 application entrypoint.
 */

#include "app_system_status.h"
#include "app_tasks.h"
#include "bsp_core.h"
#include "bsp_timebase.h"
#include "core2_main.h"

#include "FreeRTOS.h"
#include "task.h"

volatile uint32_t g_Core2MainEntered = 0U;

AppFaultCodeType Core2_Main(void)
{
    /*
     * Reaching this assignment proves that:
     *
     * 1. CM7_0 completed global initialization;
     * 2. CM7_0 released CM7_2 through MC_ME;
     * 3. CM7_2 executed Core2_Boot().
     */
    g_Core2MainEntered = 1U;

	/*
	 * Initialization local to CM7_2.
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
	 * CM7_2-owned application resources.
	 */
	/* Initialise timebase + create task(s) */
	if (!BspTimebase_Init())
	{
		return APP_FATAL_TIMEBASE_INIT;
	}

	if (AppTasks_Create() != pdPASS)
	{
		return APP_FATAL_TASK_CREATE;
	}

    /*
     * All CM7_2-local initialization and task creation succeeded.
     * Publish the acknowledgement before enabling task releases and
     * transferring control to the scheduler.
     */
    (void)AppSystemStatus_PublishCurrentCore(
        APP_CORE_STATE_READY,
        (uint32_t)APP_FATAL_NONE
    );

	/*
	 * Task handles exist before the STM channels are started.
	 */
	if (!BspTimebase_Start())
	{
		return APP_FATAL_TIMEBASE_INIT;
	}

	vTaskStartScheduler();


	return APP_FATAL_SCHEDULER_RETURNED;
}
