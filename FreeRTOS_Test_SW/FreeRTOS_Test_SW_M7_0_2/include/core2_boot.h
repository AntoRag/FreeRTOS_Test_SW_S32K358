/*
 * core2_boot.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef CORE2_BOOT_H_
#define CORE2_BOOT_H_

/*
 * Publish STARTING in CM7_2's own shared status slot. CM7_0's MC_ME
 * release remains the global-readiness gate; no polling is performed.
 */
void Core2_Boot(void);

#endif /* CORE2_BOOT_H_ */
