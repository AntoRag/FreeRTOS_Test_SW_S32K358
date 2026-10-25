/*
 * app_core.c
 *
 * Hardware-to-application core identity mapping.
 */

#include "app_core.h"

#include "Platform_Types.h"

/* Provided by the S32K3 startup support in system.c. */
extern uint8 Sys_GetCoreID(void);

uint32_t AppCore_GetCurrentPhysicalId(void)
{
    return (uint32_t)Sys_GetCoreID();
}

AppCoreIndexType AppCore_GetCurrentIndex(void)
{
    /*
     * This switch maps identities only. It deliberately does not select
     * RTD configuration objects belonging to another AMP image.
     */
    switch (AppCore_GetCurrentPhysicalId())
    {
        case APP_CORE_PRIMARY_PHYSICAL_ID:
            return APP_CORE_PRIMARY;

        case APP_CORE_SECONDARY_PHYSICAL_ID:
            return APP_CORE_SECONDARY;

        default:
            return APP_CORE_INVALID;
    }
}
