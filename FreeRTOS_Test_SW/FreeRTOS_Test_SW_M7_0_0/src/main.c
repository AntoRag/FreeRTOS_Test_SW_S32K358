/*==================================================================================================
* Project : RTD AUTOSAR 4.9
* Platform : CORTEXM
* Peripheral : S32K3XX
* Dependencies : none
*
* Autosar Version : 4.9.0
* Autosar Revision : ASR_REL_4_9_REV_0000
* Autosar Conf.Variant :
* SW Version : 7.0.0
* Build Version : S32K3_RTD_7_0_0_QLP03_D2512_ASR_REL_4_9_REV_0000_20251210
*
* Copyright 2020 - 2025 NXP
*
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be
*   used strictly in accordance with the applicable license terms. By expressly
*   accepting such terms or by downloading, installing, activating and/or otherwise
*   using the software, you are agreeing that you have read, and that you agree to
*   comply with and are bound by, such license terms. If you do not agree to be
*   bound by the applicable license terms, then you may not retain, install,
*   activate or otherwise use the software.
==================================================================================================*/

/**
*   @file main.c
*
*   @addtogroup main_module main module documentation
*   @{
*/

#ifdef __cplusplus
extern "C"{
#endif


/* Including necessary configuration files. */
#include "Mcal.h"

/* include our headers */
#include "app_fault.h"
#include "app_tasks.h"
#include "bsp_init.h"
#include "bsp_timebase.h"


/* Scheduler-smoke configuration. */


static void App_Halt(void)
{
    for (;;)
    {
        /* Some error occurred, inspect g_Core0FatalError */
    }
}


int main(void)
{

	/* Initialize BPS (Clock + Interrupts) */
	BspInitStatusType bspStatus;

	bspStatus = Bsp_Init();

	switch (bspStatus)
	{
	case BSP_INIT_CLOCK_FAILURE:
		AppFault_Set(APP_FATAL_CLOCK_INIT);
		App_Halt();
		break;
	case BSP_INIT_INTERRUPT_FAILURE:
		AppFault_Set(APP_FATAL_INTERRUPT_INIT);
		App_Halt();
		break;
	case BSP_INIT_SUCCESS:
	default:
		AppFault_Set(APP_FATAL_NONE);
		break;
	}

    /* Initialise timebase + create task(s) */
	BspTimebase_Init();

    if (AppTasks_Create() != pdPASS)
    {
        AppFault_Set(APP_FATAL_TASK_CREATE);
        App_Halt();
    }

    /*
     * Task handles exist before the STM channels are started.
     */
    BspTimebase_Start();

    vTaskStartScheduler();

    AppFault_Set(APP_FATAL_SCHEDULER_RETURNED);

    App_Halt();

}

#ifdef __cplusplus
}
#endif
