/*
 * core0_boot.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */


#include "core0_boot.h"
#include "boot_sync.h"

#include "bsp_init.h"

AppFaultCodeType Core0_Boot(void)
{
    BspInitStatusType bspStatus;
    /*
     * Establish a deterministic shared startup protocol before the
     * system initialization result is published.
     */

    BootSync_PrimaryInitialize();

    /*
     * CM7_0 is the sole owner of system-global initialization.
     */
    bspStatus = Bsp_SystemInit();

    if (bspStatus != BSP_INIT_SUCCESS)
    {
        BootSync_PrimaryPublishSystemFailure(BOOT_SYNC_SYSTEM_ERROR_CLOCK_INIT);
        return APP_FATAL_CLOCK_INIT;
    }

    BootSync_PrimaryPublishSystemReady();

    return APP_FATAL_NONE;
}
