/*
 * bsp_init.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef BSP_INIT_H_
#define BSP_INIT_H_

typedef enum
{
    BSP_INIT_SUCCESS = 0,
    BSP_INIT_CLOCK_FAILURE,
    BSP_INIT_INTERRUPT_FAILURE
} BspInitStatusType;

/*
 * Primary-core-only initialization of shared/global hardware.
 */
BspInitStatusType Bsp_SystemInit(void);

/*
 * Initialization of resources local to the executing core.
 */
BspInitStatusType Bsp_Core0Init(void);

/*
 * Initialization of resources local to the executing core.
 */
BspInitStatusType Bsp_Core2Init(void);

/*
 * Physically release CM7_2 through the generated Power/MC_ME
 * configuration. SYSTEM_READY must be published before this call.
 */
void Bsp_ReleaseCore2(void);

#endif /* BSP_INIT_CORE0_H_ */
