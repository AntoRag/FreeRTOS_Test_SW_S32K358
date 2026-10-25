/*
 * app_system_status.h
 *
 * Shared AMP startup and fatal-state publication.
 */

#ifndef APP_SYSTEM_STATUS_H_
#define APP_SYSTEM_STATUS_H_

#include <stdbool.h>
#include <stdint.h>

#include "app_core.h"

#define APP_SYSTEM_STATUS_MAGIC                 (0x41535453U) /* "ASTS" */
#define APP_SYSTEM_STATUS_PROTOCOL_VERSION      (3U)
#define APP_SYSTEM_STATUS_SIZE_BYTES            (256U)
#define APP_SYSTEM_STATUS_RESERVED_WORD_COUNT   (58U)

typedef enum
{
    APP_CORE_STATE_RESET = 0U,
    APP_CORE_STATE_STARTING,
    APP_CORE_STATE_READY,
    APP_CORE_STATE_FAILED
} AppCoreStateType;

typedef struct
{
    uint32_t state;
    uint32_t fatalError;
} AppCoreStatusType;

typedef struct
{
    uint32_t magic;
    uint32_t protocolVersion;
    AppCoreStatusType core[APP_CORE_COUNT];
    uint32_t reserved[APP_SYSTEM_STATUS_RESERVED_WORD_COUNT];
} AppSystemStatusType;

/*
 * Both ELFs place this symbol at the same physical non-cacheable SRAM
 * address. CM7_0 initializes the complete object before releasing CM7_2.
 */
extern volatile AppSystemStatusType g_AppSystemStatus;

/* Initialize the complete shared object. Valid only on the primary core. */
bool AppSystemStatus_PrimaryInitialize(void);

/*
 * Publish the executing core's state. Each core is the sole writer of its
 * own slot after primary initialization.
 */
bool AppSystemStatus_PublishCurrentCore(
    AppCoreStateType state,
    uint32_t fatalError
);

/* Read a coherent snapshot of one core's published state and error. */
bool AppSystemStatus_GetCoreStatus(
    AppCoreIndexType coreIndex,
    AppCoreStatusType *status
);

#endif /* APP_SYSTEM_STATUS_H_ */
