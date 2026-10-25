/*
 * core2_main.h
 *
 * Created on: 24 ott 2026
 *     Author: anton
 */

#ifndef CORE2_MAIN_H_
#define CORE2_MAIN_H_

#include <stdint.h>
#include <app_fault.h>

/*
 * Debugger-visible proof that CM7_2 was released by CM7_0 and entered its
 * application layer.
 */
extern volatile uint32_t g_Core2MainEntered;

/*
 * Enter the CM7_2 application.
 *
 * On success this function does not return because the independent CM7_2
 * FreeRTOS scheduler runs. A return therefore identifies a fatal error.
 */
AppFaultCodeType Core2_Main(void);

#endif /* CORE2_MAIN_H_ */
