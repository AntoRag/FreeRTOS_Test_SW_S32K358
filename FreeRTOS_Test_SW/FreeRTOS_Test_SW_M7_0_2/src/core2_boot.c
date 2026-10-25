/*
 * core2_boot.c
 *
 * Created on: 24 ott 2026
 *     Author: anton
 *
 * CM7_2 boot coordination.
 */

#include "core2_boot.h"
#include "app_system_status.h"

void Core2_Boot(void)
{
    /*
     * CM7_0 releases CM7_2 through MC_ME only after successful global
     * initialization. Reaching this function is therefore the readiness
     * guarantee; no shared-memory polling loop is required.
     */
    (void)AppSystemStatus_PublishCurrentCore(
        APP_CORE_STATE_STARTING,
        0U
    );
}
