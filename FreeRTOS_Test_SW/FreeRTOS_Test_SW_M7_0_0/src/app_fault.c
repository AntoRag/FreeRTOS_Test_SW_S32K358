/*
 * app_fault.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */
#include <app_fault.h>

volatile uint32_t g_Core0FatalError              	= 	APP_FATAL_NONE;
/* vApplicationIdleHook() is our background activity. It must never block. */


void AppFault_A_Set(AppFaultCodeType faultCode)
{
    g_Core0FatalError = (uint32_t)faultCode;
}

AppFaultCodeType AppFault_A_Get(void)
{
    return g_Core0FatalError;
}



//////////////////////////////////////
