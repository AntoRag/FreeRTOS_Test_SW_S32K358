/*
 * app_fault.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef APP_FAULT_H_
#define APP_FAULT_H_


#include <stdint.h>

#include "app_core.h"

typedef enum
{
    APP_FATAL_NONE = 0U,
    APP_FATAL_CORE_ID,
    APP_FATAL_BSP_CONFIG,
    APP_FATAL_TASK_CREATE,
    APP_FATAL_SCHEDULER_RETURNED,
    APP_FATAL_MALLOC_FAILED,
    APP_FATAL_STACK_OVERFLOW,
    APP_FATAL_CLOCK_INIT,
    APP_FATAL_INTERRUPT_INIT,
    APP_FATAL_TIMEBASE_INIT
} AppFaultCodeType;

/* Publish a fatal error in the executing core's shared status slot. */
void AppFault_Set(AppFaultCodeType faultCode);

/* Read the last fatal error published by the requested application core. */
AppFaultCodeType AppFault_Get(AppCoreIndexType coreIndex);


#endif /* APP_FAULT_H_ */
