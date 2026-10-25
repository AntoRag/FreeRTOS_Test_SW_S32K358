/*
 * bsp_core.c
 *
 * Shared core-local initialization. Configuration remains image-local.
 */

#include <stddef.h>

#include "app_core.h"
#include "bsp_core.h"

#include "IntCtrl_Ip.h"

BspInitStatusType BspCore_Init(void)
{
    const BspCoreConfigType *config;
    IntCtrl_Ip_StatusType interruptStatus;

    config = BspCore_GetConfig();

    if ((config == NULL) || (config->interruptConfig == NULL))
    {
        return BSP_INIT_INVALID_CONFIG;
    }

    /* Detect an ELF loaded or started on the wrong physical core. */
    if (AppCore_GetCurrentPhysicalId() !=
        config->expectedPhysicalCoreId)
    {
        return BSP_INIT_CORE_MISMATCH;
    }

    interruptStatus = IntCtrl_Ip_Init(config->interruptConfig);

    if (interruptStatus != INTCTRL_IP_STATUS_SUCCESS)
    {
        return BSP_INIT_INTERRUPT_FAILURE;
    }

    return BSP_INIT_SUCCESS;
}
