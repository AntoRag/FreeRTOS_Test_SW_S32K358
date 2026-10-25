/*
 * bsp_core.h
 *
 * Core-local BSP initialization shared by both AMP images.
 */

#ifndef BSP_CORE_H_
#define BSP_CORE_H_

#include <stdint.h>

#include "IntCtrl_Ip_TypesDef.h"

typedef enum
{
    BSP_INIT_SUCCESS = 0U,
    BSP_INIT_CLOCK_FAILURE,
    BSP_INIT_INTERRUPT_FAILURE,
    BSP_INIT_CORE_MISMATCH,
    BSP_INIT_INVALID_CONFIG
} BspInitStatusType;

typedef struct
{
    uint32_t expectedPhysicalCoreId;
    const IntCtrl_Ip_CtrlConfigType *interruptConfig;
} BspCoreConfigType;

/* Implemented once per ELF by the project-local RTD binding file. */
const BspCoreConfigType *BspCore_GetConfig(void);

/* Validate the executing core and initialize its local interrupt context. */
BspInitStatusType BspCore_Init(void);

#endif /* BSP_CORE_H_ */
