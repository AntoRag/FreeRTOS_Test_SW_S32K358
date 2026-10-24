/*
 * core2_boot.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef CORE2_BOOT_H_
#define CORE2_BOOT_H_

#include <stdint.h>

#include "boot_sync.h"

/*
 * Result of the most recent CM7_2 boot synchronization attempt.
 *
 * These variables are intentionally externally visible for debugger
 * observation and later test-harness reporting.
 */
extern volatile BootSyncWaitStatusType g_Core2BootWaitStatus;
extern volatile uint32_t g_Core2BootPollCount;

/*
 * Wait for CM7_0 to complete system-global initialization.
 *
 * This function does not initialize shared peripherals. It only
 * consumes the boot mailbox and returns the observed synchronization
 * result.
 */
BootSyncWaitStatusType Core2_Boot(void);

#endif /* CORE2_BOOT_H_ */
