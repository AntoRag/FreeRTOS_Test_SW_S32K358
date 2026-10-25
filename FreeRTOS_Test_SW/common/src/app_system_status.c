/*
 * app_system_status.c
 *
 * Shared, single-writer-per-core AMP status publication.
 */

#include <stddef.h>

#include "app_system_status.h"

/*
 * The linker scripts in both images map this legacy-named section to the
 * same 256-byte SRAM range at 0x204BFF00. The range is shareable,
 * non-cacheable and initialized for ECC by the CM7_0 startup code.
 */
#define APP_SYSTEM_STATUS_SECTION                              \
    __attribute__((section(".boot_mailbox"), aligned(32), used))

volatile AppSystemStatusType g_AppSystemStatus
    APP_SYSTEM_STATUS_SECTION;

/* Fail the build if C layout and linker reservation ever diverge. */
typedef char AppSystemStatusSizeCheck[
    (sizeof(AppSystemStatusType) == APP_SYSTEM_STATUS_SIZE_BYTES)
        ? 1
        : -1
];

static void AppSystemStatus_DataMemoryBarrier(void)
{
    /*
     * Complete payload writes before publishing a state, and complete a
     * state observation before consuming its payload. Cache maintenance is
     * unnecessary while the MPU keeps this region non-cacheable.
     */
    __asm volatile ("dmb sy" ::: "memory");
}

static bool AppSystemStatus_IsProtocolValid(void)
{
    uint32_t observedMagic;

    /* Magic is the producer's final publication field for the object. */
    observedMagic = g_AppSystemStatus.magic;
    AppSystemStatus_DataMemoryBarrier();

    return (observedMagic == APP_SYSTEM_STATUS_MAGIC) &&
        (g_AppSystemStatus.protocolVersion ==
            APP_SYSTEM_STATUS_PROTOCOL_VERSION);
}

bool AppSystemStatus_PrimaryInitialize(void)
{
    uint32_t index;

    if (AppCore_GetCurrentIndex() != APP_CORE_PRIMARY)
    {
        return false;
    }

    /* Invalidate the protocol identity while every word is rewritten. */
    g_AppSystemStatus.magic = 0U;
    g_AppSystemStatus.protocolVersion = 0U;

    for (index = 0U; index < (uint32_t)APP_CORE_COUNT; index++)
    {
        g_AppSystemStatus.core[index].state =
            (uint32_t)APP_CORE_STATE_RESET;
        g_AppSystemStatus.core[index].fatalError = 0U;
    }

    for (index = 0U;
         index < APP_SYSTEM_STATUS_RESERVED_WORD_COUNT;
         index++)
    {
        g_AppSystemStatus.reserved[index] = 0U;
    }

    /* Magic is the final publication point for a valid protocol object. */
    g_AppSystemStatus.protocolVersion =
        APP_SYSTEM_STATUS_PROTOCOL_VERSION;
    AppSystemStatus_DataMemoryBarrier();
    g_AppSystemStatus.magic = APP_SYSTEM_STATUS_MAGIC;

    return AppSystemStatus_PublishCurrentCore(
        APP_CORE_STATE_STARTING,
        0U
    );
}

bool AppSystemStatus_PublishCurrentCore(
    AppCoreStateType state,
    uint32_t fatalError
)
{
    AppCoreIndexType coreIndex;

    coreIndex = AppCore_GetCurrentIndex();

    if ((coreIndex >= APP_CORE_COUNT) ||
        (state > APP_CORE_STATE_FAILED) ||
        !AppSystemStatus_IsProtocolValid())
    {
        return false;
    }

    /*
     * Each core writes only its own slot. Write the diagnostic payload
     * first, order it globally, and publish the state last.
     */
    g_AppSystemStatus.core[coreIndex].fatalError = fatalError;
    AppSystemStatus_DataMemoryBarrier();
    g_AppSystemStatus.core[coreIndex].state = (uint32_t)state;

    return true;
}

bool AppSystemStatus_GetCoreStatus(
    AppCoreIndexType coreIndex,
    AppCoreStatusType *status
)
{
    uint32_t observedState;

    if ((coreIndex >= APP_CORE_COUNT) ||
        (status == NULL) ||
        !AppSystemStatus_IsProtocolValid())
    {
        return false;
    }

    /* State is the producer's publication field. */
    observedState = g_AppSystemStatus.core[coreIndex].state;
    AppSystemStatus_DataMemoryBarrier();

    status->state = observedState;
    status->fatalError =
        g_AppSystemStatus.core[coreIndex].fatalError;

    return true;
}
