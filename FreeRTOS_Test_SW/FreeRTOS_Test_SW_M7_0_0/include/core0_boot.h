/*
 * core0_boot.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef CORE0_BOOT_H_
#define CORE0_BOOT_H_

#include <app_fault.h>

/*
 * Execute primary-core boot coordination and system-global initialization.
 *
 * Returns APP_FATAL_NONE on success or the corresponding fatal error.
 */
AppFaultCodeType Core0_Boot(void);

#endif /* CORE0_BOOT_H_ */
