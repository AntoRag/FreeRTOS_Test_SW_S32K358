/*
 * core2_main.h
 *
 * Created on: 24 ott 2026
 *     Author: anton
 */

#ifndef CORE2_MAIN_H_
#define CORE2_MAIN_H_

#include <stdint.h>

/*
 * Debugger-visible proof that CM7_2 passed the boot barrier and entered
 * its application layer.
 */
extern volatile uint32_t g_Core2MainEntered;

/*
 * Enter the CM7_2 application.
 *
 * This initial version only proves control-flow entry. FreeRTOS local
 * initialization and TaskB creation will be added after the mailbox
 * consumer has passed its hardware test.
 */
void Core2_Main(void);

#endif /* CORE2_MAIN_H_ */
