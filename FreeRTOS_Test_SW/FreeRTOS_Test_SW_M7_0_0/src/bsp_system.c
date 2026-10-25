/*
 * bsp_system.c
 *
 * System-global initialization owned exclusively by the primary CM7_0
 * image. Keeping this separate from bsp_core.c prevents CM7_2 from ever
 * reconfiguring shared power or clock resources.
 */

#include "bsp_system.h"

#include "Clock_Ip.h"
#include "Clock_Ip_Cfg.h"
#include "Power_Ip.h"
#include "Power_Ip_Cfg.h"

BspInitStatusType BspSystem_Init(void)
{
    Clock_Ip_StatusType clockStatus;

    /* Prepare the generated MC_RGM/PMC/MC_ME support configuration. */
    Power_Ip_Init(&Power_Ip_HwIPsConfigPB);

    clockStatus = Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);

    if (clockStatus != CLOCK_IP_SUCCESS)
    {
        return BSP_INIT_CLOCK_FAILURE;
    }

    return BSP_INIT_SUCCESS;
}

void BspSystem_ReleaseCore2(void)
{
    /*
     * Apply the generated RUN-mode configuration. It programs the CM7_2
     * boot address, enables the core clock and performs the MC_ME update.
     * This call is intentionally the release gate after global init.
     */
    Power_Ip_SetMode(&Power_Ip_aModeConfigPB[0]);
}
