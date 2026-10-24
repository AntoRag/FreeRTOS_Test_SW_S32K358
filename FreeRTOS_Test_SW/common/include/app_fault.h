/*
 * app_fault.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef APP_FAULT_H_
#define APP_FAULT_H_


#include <stdint.h>

typedef enum
{
    APP_FATAL_NONE = 0U,
    APP_FATAL_TASK_CREATE,
    APP_FATAL_SCHEDULER_RETURNED,
    APP_FATAL_MALLOC_FAILED,
    APP_FATAL_STACK_OVERFLOW,
    APP_FATAL_CLOCK_INIT,
    APP_FATAL_INTERRUPT_INIT
} AppFaultCodeType;

extern volatile uint32_t g_Core0FatalError;

extern volatile uint32_t g_Core2FatalError;

void AppFault_A_Set(AppFaultCodeType faultCode);

AppFaultCodeType AppFault_A_Get(void);

void AppFault_B_Set(AppFaultCodeType faultCode);

AppFaultCodeType AppFault_B_Get(void);


#endif /* APP_FAULT_CORE0_H_ */
