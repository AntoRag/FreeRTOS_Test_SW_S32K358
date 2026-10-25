/*
 * app_fault.c
 *
 * Common fatal-error publication for both AMP images.
 */

#include "app_fault.h"
#include "app_system_status.h"

void AppFault_Set(AppFaultCodeType faultCode)
{
    if (faultCode != APP_FATAL_NONE)
    {
        (void)AppSystemStatus_PublishCurrentCore(
            APP_CORE_STATE_FAILED,
            (uint32_t)faultCode
        );
    }
}

AppFaultCodeType AppFault_Get(AppCoreIndexType coreIndex)
{
    AppCoreStatusType status;

    if (!AppSystemStatus_GetCoreStatus(coreIndex, &status))
    {
        return APP_FATAL_CORE_ID;
    }

    return (AppFaultCodeType)status.fatalError;
}
