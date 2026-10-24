/*
 * bsp_timebase.h
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */

#ifndef BSP_TIMEBASE_H_
#define BSP_TIMEBASE_H_


#include "Mcal.h"
#include <stdint.h>

/* Stm specific defines */
#define APP_A_STM_INSTANCE            				(STM_0_IP_INSTANCE_NUMBER)

#define APP_A_STM_10MS_CHANNEL        				(0U)
#define APP_A_STM_100MS_CHANNEL       				(1U)
#define APP_A_STM_1000MS_CHANNEL      				(2U)

#define APP_A_STM_10MS_TICKS          				(10000U)
#define APP_A_STM_100MS_TICKS         				(100000U)
#define APP_A_STM_1000MS_TICKS        				(1000000U)

#define APP_B_STM_INSTANCE            				(STM_2_IP_INSTANCE_NUMBER)

#define APP_B_STM_10MS_CHANNEL        				(0U)
#define APP_B_STM_100MS_CHANNEL       				(1U)
#define APP_B_STM_1000MS_CHANNEL      				(2U)

#define APP_B_STM_10MS_TICKS          				(10000U)
#define APP_B_STM_100MS_TICKS         				(100000U)
#define APP_B_STM_1000MS_TICKS        				(1000000U)


void BspTimebase_Init(void);
void BspTimebase_Start(void);

void App_Core0Stm10msCallback(uint8 channel);
void App_Core0Stm100msCallback(uint8 channel);
void App_Core0Stm1000msCallback(uint8 channel);

void App_Core2Stm10msCallback(uint8 channel);
void App_Core2Stm100msCallback(uint8 channel);
void App_Core2Stm1000msCallback(uint8 channel);

extern volatile uint32_t g_Core0Stm10msInterruptCount;
extern volatile uint32_t g_Core0Stm100msInterruptCount;
extern volatile uint32_t g_Core0Stm1000msInterruptCount;

extern volatile uint32_t g_Core2Stm10msInterruptCount;
extern volatile uint32_t g_Core2Stm100msInterruptCount;
extern volatile uint32_t g_Core2Stm1000msInterruptCount;


#endif /* BSP_TIMEBASE_CORE0_H_ */
