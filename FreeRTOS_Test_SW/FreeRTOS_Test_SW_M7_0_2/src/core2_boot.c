/*
 * core2_boot.c
 *
 * Created on: 24 ott 2026
 *     Author: anton
 *
 * CM7_2 boot coordination.
 */

#include "core2_boot.h"

/*
 * This is a bounded iteration count, not a timeout expressed in
 * milliseconds. Its execution time depends on CPU and memory clocks.
 *
 * It prevents CM7_2 from remaining silently blocked forever if CM7_0
 * never publishes a valid mailbox state.
 */
#define CORE2_BOOT_POLL_ITERATION_LIMIT    (10000000U)

volatile BootSyncWaitStatusType g_Core2BootWaitStatus =
    BOOT_SYNC_WAIT_NOT_STARTED;

volatile uint32_t g_Core2BootPollCount = 0U;

BootSyncWaitStatusType Core2_Boot(void)
{
    BootSyncWaitStatusType waitStatus;
    uint32_t pollCount;

    g_Core2BootWaitStatus = BOOT_SYNC_WAIT_NOT_STARTED;
    g_Core2BootPollCount = 0U;

    /*
     * CM7_2 must not perform core-local or peripheral initialization
     * until CM7_0 publishes SYSTEM_READY.
     */
    waitStatus = BootSync_SecondaryWaitForSystemReady(
        CORE2_BOOT_POLL_ITERATION_LIMIT,
        &pollCount
    );

    /*
     * Publish diagnostics only after the wait operation completes.
     * They remain available if main() enters its failure halt loop.
     */
    g_Core2BootPollCount = pollCount;
    g_Core2BootWaitStatus = waitStatus;

    return waitStatus;
}
