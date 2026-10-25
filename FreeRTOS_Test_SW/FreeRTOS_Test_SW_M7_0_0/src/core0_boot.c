/*
 * core0_boot.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */


#include "app_system_status.h"
#include "bsp_system.h"
#include "core0_boot.h"


AppFaultCodeType Core0_Boot(void)
{
    BspInitStatusType bspStatus;
    /*
     * Establish a deterministic shared startup protocol before the
     * system initialization result is published.
     */

    if (!AppSystemStatus_PrimaryInitialize())
    {
        return APP_FATAL_CORE_ID;
    }

    /*
     * CM7_0 is the sole owner of system-global initialization.
     */
    bspStatus = BspSystem_Init();

    if (bspStatus != BSP_INIT_SUCCESS)
    {
        /* CM7_2 remains held because the MC_ME release is not executed. */
        return APP_FATAL_CLOCK_INIT;
    }

    /*
     * MC_ME release is the readiness gate. Reaching this call proves that
     * CM7_0 completed the system-global initialization successfully.
     */
    BspSystem_ReleaseCore2();

    return APP_FATAL_NONE;
}
