/*
 * bsp_system.h
 *
 * CM7_0-only system-global initialization and secondary-core release.
 */

#ifndef BSP_SYSTEM_H_
#define BSP_SYSTEM_H_

#include "bsp_core.h"

/* Initialize Power support and the global clock tree on CM7_0 only. */
BspInitStatusType BspSystem_Init(void);

/* Release CM7_2 using the generated Power/MC_ME mode configuration. */
void BspSystem_ReleaseCore2(void);

#endif /* BSP_SYSTEM_H_ */
