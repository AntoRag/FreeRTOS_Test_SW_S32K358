/*
 * boot_sync_primary.c
 *
 * Created on: 24 ott 2026
 *      Author: anton
 *
 * CM7_0 producer side of the boot synchronization protocol.
 *
 * The mailbox is stored in a fixed 256-byte shared-SRAM region:
 *
 *     0x204BFF00 - 0x204BFFFF
 *
 * Both core images reserve this exact range through their linker scripts.
 * The memory is configured by the startup MPU as shareable and
 * non-cacheable.
 *
 * The mailbox is marked NOLOAD in the linker scripts. Therefore:
 *
 * - its content is not loaded from flash;
 * - it is not automatically initialized as normal .data or .bss;
 * - stale data may remain after some reset scenarios;
 * - CM7_0 must explicitly initialize every mailbox word before either
 *   core treats its content as valid.
 *
 * State ownership:
 *
 * - CM7_0 writes systemState and systemError;
 * - CM7_2 writes core2State and core2Error;
 * - CM7_0 may initialize the complete mailbox at the start of a new
 *   boot sequence, before publishing SYSTEM_INITIALIZING.
 *
 * A state word is the publication point for its associated payload.
 * The producer writes the payload, executes a data-memory barrier, and
 * only then writes the new state. The consumer observes the state,
 * executes a matching barrier, and then reads the payload.
 */

#include "boot_sync.h"

/*
 * Place the mailbox object in the dedicated linker section.
 *
 * section:
 *     Routes the object to the .boot_mailbox output section at
 *     0x204BFF00.
 *
 * aligned(32):
 *     Keeps the shared object naturally isolated and aligned to the
 *     selected protocol boundary.
 *
 * used:
 *     Forces the compiler to emit the object even if it cannot see all
 *     external accesses. The linker scripts additionally use KEEP() to
 *     prevent --gc-sections from discarding the section.
 *
 * No C initializer is used. Initialization is performed explicitly by
 * BootSync_PrimaryInitialize().
 */
#define BOOT_SYNC_SECTION                                      \
    __attribute__((section(".boot_mailbox"), aligned(32), used))

volatile BootSyncMailboxType g_BootSyncMailbox BOOT_SYNC_SECTION;

/*
 * C99-compatible compile-time size check.
 *
 * A negative array dimension causes compilation to fail if a future
 * structure change makes the mailbox different from the 256-byte
 * linker reservation.
 */
typedef char BootSyncMailboxSizeCheck[
    (sizeof(BootSyncMailboxType) == BOOT_SYNC_MAILBOX_SIZE_BYTES)
        ? 1
        : -1
];

/*
 * Enforce ordering of shared-memory accesses.
 *
 * volatile guarantees that the compiler emits the mailbox accesses,
 * but it does not by itself guarantee the order in which another core
 * observes those accesses.
 *
 * "dmb sy" ensures that all explicit memory accesses before the barrier
 * are globally ordered before accesses following the barrier.
 *
 * This barrier does not perform cache maintenance. That is acceptable
 * because the startup MPU configures the mailbox region as
 * non-cacheable. If its MPU attributes change, the coherency protocol
 * must be reviewed.
 */
static void BootSync_DataMemoryBarrier(void)
{
    __asm volatile ("dmb sy" ::: "memory");
}

void BootSync_PrimaryInitialize(void)
{
    uint32_t index;

    /*
     * First invalidate any state left by a previous execution.
     *
     * In particular, a stale SYSTEM_READY value must not remain valid
     * while CM7_0 is rewriting the mailbox.
     */
    g_BootSyncMailbox.systemState =
        (uint32_t)BOOT_SYNC_SYSTEM_INVALID;

    /*
     * Ensure that invalidation is ordered before the remaining
     * initialization writes.
     */
    BootSync_DataMemoryBarrier();

    /*
     * Explicitly write all fixed protocol fields.
     *
     * The mailbox lies outside the startup shared-SRAM initialization
     * loop, so these stores also establish deterministic contents for
     * the SRAM/ECC storage that the protocol will access.
     *
     * Magic and version are initially cleared so that partially
     * initialized contents cannot be accepted as a valid mailbox.
     */
    g_BootSyncMailbox.magic = 0U;
    g_BootSyncMailbox.protocolVersion = 0U;

    g_BootSyncMailbox.systemError =
        (uint32_t)BOOT_SYNC_SYSTEM_ERROR_NONE;

    /*
     * At the start of a new boot sequence CM7_2 has not yet completed
     * its local initialization.
     *
     * CM7_0 is allowed to reset the CM7_2-owned fields only during this
     * initial mailbox-establishment step. After INITIALIZING has been
     * published, only CM7_2 may update these fields.
     */
    g_BootSyncMailbox.core2State =
        (uint32_t)BOOT_SYNC_CORE2_RESET;
    g_BootSyncMailbox.core2Error = 0U;

    /*
     * Initialize the complete reserved portion, not just the currently
     * defined protocol fields.
     *
     * This produces deterministic debugger/QEMU observations and makes
     * the complete 256-byte mailbox safe to access.
     */
    for (index = 0U;
         index < BOOT_SYNC_RESERVED_WORD_COUNT;
         index++)
    {
        g_BootSyncMailbox.reserved[index] = 0U;
    }

    /*
     * Install the protocol identity only after the mailbox storage has
     * been completely initialized.
     */
    g_BootSyncMailbox.magic = BOOT_SYNC_MAGIC;
    g_BootSyncMailbox.protocolVersion =
        BOOT_SYNC_PROTOCOL_VERSION;

    /*
     * Publish SYSTEM_INITIALIZING.
     *
     * The barrier ensures that another core cannot validly observe the
     * INITIALIZING state without also being able to observe the magic,
     * version, reset error fields, and initialized reserved area that
     * belong to that state.
     */
    BootSync_DataMemoryBarrier();

    g_BootSyncMailbox.systemState =
        (uint32_t)BOOT_SYNC_SYSTEM_INITIALIZING;
}

void BootSync_PrimaryPublishSystemReady(void)
{
    /*
     * SYSTEM_READY means that CM7_0 has completed system-global
     * initialization.
     *
     * It does not mean that the CM7_0 FreeRTOS scheduler or TaskA set is
     * already running. It only authorizes CM7_2 to perform its own
     * core-local and exclusively owned peripheral initialization.
     */
    g_BootSyncMailbox.systemError =
        (uint32_t)BOOT_SYNC_SYSTEM_ERROR_NONE;

    /*
     * systemError is the payload associated with systemState.
     * Complete the payload write before publishing READY.
     */
    BootSync_DataMemoryBarrier();

    g_BootSyncMailbox.systemState =
        (uint32_t)BOOT_SYNC_SYSTEM_READY;
}

void BootSync_PrimaryPublishSystemFailure(
    BootSyncSystemErrorType error
)
{
    /*
     * Record the diagnostic payload before publishing FAILED.
     *
     * When CM7_2 later observes SYSTEM_FAILED, it will execute its
     * consumer barrier and can then safely read systemError.
     */
    g_BootSyncMailbox.systemError = (uint32_t)error;

    BootSync_DataMemoryBarrier();

    g_BootSyncMailbox.systemState =
        (uint32_t)BOOT_SYNC_SYSTEM_FAILED;
}
