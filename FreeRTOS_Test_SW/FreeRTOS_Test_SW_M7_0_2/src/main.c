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

#include "boot_sync.h"
#include "core2_boot.h"
#include "core2_main.h"

static void Core2_Halt(void)
{
    for (;;)
    {
        /*
         * Inspect:
         *
         * g_Core2BootWaitStatus
         * g_Core2BootPollCount
         * g_BootSyncMailbox
         */
    }
}

int main(void)
{
    BootSyncWaitStatusType bootStatus;

    AppFaultCodeType faultCode = APP_FATAL_NONE;

    bootStatus = Core2_Boot();

    if (bootStatus == BOOT_SYNC_WAIT_READY)
    {
        faultCode = Core2_Main();
        AppFault_B_Set(faultCode);
    }

    Core2_Halt();

    return 0;
}
#ifdef __cplusplus
}
#endif

/** @} */
