/*
 * boot_sync_secondary.c
 *
 * Created on: 24 ott 2026
 *     Author: anton
 *
 * CM7_2 consumer side of the boot synchronization protocol.
 *
 * CM7_2 does not initialize the mailbox and does not write the
 * systemState or systemError fields. It waits for CM7_0 to publish a
 * valid system state through the shared SRAM mailbox.
 */

#include <stddef.h>

#include "boot_sync.h"

/*
 * Both core images define this symbol because they are independent ELF
 * files. The linker scripts place both definitions at the same physical
 * SRAM address, 0x204BFF00.
 */
#define BOOT_SYNC_SECTION                                      \
    __attribute__((section(".boot_mailbox"), aligned(32), used))

volatile BootSyncMailboxType g_BootSyncMailbox BOOT_SYNC_SECTION;

/*
 * Fail compilation if the C structure no longer matches the 256-byte
 * linker reservation.
 */
typedef char BootSyncMailboxSizeCheck[
    (sizeof(BootSyncMailboxType) == BOOT_SYNC_MAILBOX_SIZE_BYTES)
        ? 1
        : -1
];

/*
 * Order accesses to the shared, non-cacheable mailbox.
 *
 * After observing a state published by CM7_0, the barrier ensures that
 * the associated payload fields, such as systemError, are observed
 * consistently.
 */
static void BootSync_DataMemoryBarrier(void)
{
    __asm volatile ("dmb sy" ::: "memory");
}

BootSyncWaitStatusType BootSync_SecondaryWaitForSystemReady(
    uint32_t maxPollCount,
    uint32_t *pollCount
)
{
    uint32_t pollIndex;
    uint32_t observedState;

    if (pollCount != NULL)
    {
        *pollCount = 0U;
    }

    for (pollIndex = 0U;
         pollIndex < maxPollCount;
         pollIndex++)
    {
        /*
         * systemState is the publication field written last by CM7_0.
         */
        observedState = g_BootSyncMailbox.systemState;

        if (pollCount != NULL)
        {
            *pollCount = pollIndex + 1U;
        }

        /*
         * INVALID means that CM7_0 has not yet established a state that
         * CM7_2 is allowed to consume.
         */
        if (observedState ==
            (uint32_t)BOOT_SYNC_SYSTEM_INVALID)
        {
            continue;
        }

        /*
         * Pair this consumer barrier with the producer barrier executed
         * before CM7_0 publishes systemState.
         */
        BootSync_DataMemoryBarrier();

        /*
         * A non-matching magic value may mean that CM7_0 is still
         * establishing the mailbox. Keep polling until the bounded
         * timeout expires.
         */
        if (g_BootSyncMailbox.magic != BOOT_SYNC_MAGIC)
        {
            continue;
        }

        /*
         * Once the magic is valid, a different protocol version is a
         * genuine incompatibility. Waiting longer cannot correct it.
         */
        if (g_BootSyncMailbox.protocolVersion !=
            BOOT_SYNC_PROTOCOL_VERSION)
        {
            return BOOT_SYNC_WAIT_PROTOCOL_MISMATCH;
        }

        switch (observedState)
        {
            case (uint32_t)BOOT_SYNC_SYSTEM_INITIALIZING:
                /*
                 * CM7_0 is still executing global initialization.
                 */
                break;

            case (uint32_t)BOOT_SYNC_SYSTEM_READY:
                /*
                 * READY must not carry a system initialization error.
                 */
                if (g_BootSyncMailbox.systemError ==
                    (uint32_t)BOOT_SYNC_SYSTEM_ERROR_NONE)
                {
                    return BOOT_SYNC_WAIT_READY;
                }

                return BOOT_SYNC_WAIT_INVALID_STATE;

            case (uint32_t)BOOT_SYNC_SYSTEM_FAILED:
                /*
                 * systemError can be inspected by the caller or
                 * debugger to determine the CM7_0 failure reason.
                 */
                return BOOT_SYNC_WAIT_SYSTEM_FAILED;

            default:
                /*
                 * The mailbox identity is valid, but its state is not
                 * part of protocol version 1.
                 */
                return BOOT_SYNC_WAIT_INVALID_STATE;
        }
    }

    return BOOT_SYNC_WAIT_TIMEOUT;
}
