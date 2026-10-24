/*
 * core0_main.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef CORE0_MAIN_H_
#define CORE0_MAIN_H_

#include "app_fault.h"

/*
 * Initialize and start the CM7_0 application.
 *
 * On success this function does not return because the FreeRTOS scheduler runs.
 * A return value therefore always identifies a fatal startup failure.
 */
AppFaultCodeType Core0_Main(void);

#endif /* CORE0_MAIN_H_ */
