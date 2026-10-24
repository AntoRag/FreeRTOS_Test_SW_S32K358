/*
 * bsp_init.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */


#include "bsp_init.h"

/* Clock interfaces. */
#include "Clock_Ip.h"
#include "Clock_Ip_Cfg.h"

/* Interrupt interfaces. */
#include "IntCtrl_Ip.h"
#include "IntCtrl_Ip_Cfg.h"

BspInitStatusType Bsp_Init(void)
{
    Clock_Ip_StatusType clockStatus;
    IntCtrl_Ip_StatusType interruptStatus;

    clockStatus = Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);

    if (clockStatus != CLOCK_IP_SUCCESS)
    {
        return BSP_INIT_CLOCK_FAILURE;
    }

    interruptStatus = IntCtrl_Ip_Init(&IntCtrlConfig_0);

    if (interruptStatus != INTCTRL_IP_STATUS_SUCCESS)
    {
        return BSP_INIT_INTERRUPT_FAILURE;
    }

    return BSP_INIT_SUCCESS;
}

