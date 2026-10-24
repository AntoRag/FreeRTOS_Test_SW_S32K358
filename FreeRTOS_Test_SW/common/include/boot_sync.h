/*
 * boot_sync.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef BOOT_SYNC_H_
#define BOOT_SYNC_H_

#include <stdint.h>

#define BOOT_SYNC_MAGIC                  (0x4253594EU) /* To identify the memory contents as a valid instance of our mailbox protocol */
#define BOOT_SYNC_PROTOCOL_VERSION       (1U)

#define BOOT_SYNC_MAILBOX_SIZE_BYTES     (256U)
#define BOOT_SYNC_RESERVED_WORD_COUNT    (58U)

typedef enum
{
    BOOT_SYNC_WAIT_NOT_STARTED = 0U,
    BOOT_SYNC_WAIT_READY,
    BOOT_SYNC_WAIT_SYSTEM_FAILED,
    BOOT_SYNC_WAIT_PROTOCOL_MISMATCH,
    BOOT_SYNC_WAIT_INVALID_STATE,
    BOOT_SYNC_WAIT_TIMEOUT
} BootSyncWaitStatusType;

typedef enum
{
    BOOT_SYNC_SYSTEM_INVALID = 0U,
    BOOT_SYNC_SYSTEM_INITIALIZING,
    BOOT_SYNC_SYSTEM_READY,
    BOOT_SYNC_SYSTEM_FAILED
} BootSyncSystemStateType;

typedef enum
{
    BOOT_SYNC_CORE2_RESET = 0U,
    BOOT_SYNC_CORE2_WAITING,
    BOOT_SYNC_CORE2_READY,
    BOOT_SYNC_CORE2_FAILED
} BootSyncCore2StateType;

typedef enum
{
    BOOT_SYNC_SYSTEM_ERROR_NONE = 0U,
    BOOT_SYNC_SYSTEM_ERROR_CLOCK_INIT
} BootSyncSystemErrorType;

typedef struct
{
    uint32_t magic;
    uint32_t protocolVersion;

    uint32_t systemState;
    uint32_t systemError;

    uint32_t core2State;
    uint32_t core2Error;

    uint32_t reserved[BOOT_SYNC_RESERVED_WORD_COUNT];
} BootSyncMailboxType;

/*
 * Shared protocol object. Ownership of individual fields is defined by D-020.
 */
extern volatile BootSyncMailboxType g_BootSyncMailbox;

/* CM7_0 producer interfaces. */
void BootSync_PrimaryInitialize(void);

void BootSync_PrimaryPublishSystemReady(void);

void BootSync_PrimaryPublishSystemFailure(BootSyncSystemErrorType error);


/* CM7_2 consumer interface. */
/*
 * Poll the shared mailbox until CM7_0 publishes a terminal boot state
 * or the polling limit is reached.
 *
 * maxPollCount is a bounded iteration count, not a duration in
 * milliseconds.
 *
 * pollCount may be NULL. When it is not NULL, it receives the number
 * of mailbox observations performed.
 */
BootSyncWaitStatusType BootSync_SecondaryWaitForSystemReady(
    uint32_t maxPollCount,
    uint32_t *pollCount
);


#endif /* BOOT_SYNC_H_ */
