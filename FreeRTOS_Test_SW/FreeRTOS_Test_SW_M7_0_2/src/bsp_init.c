/*
 * bsp_init.c
 *
 *  Created on: 24 ott 2026
 *      Author: anton
 */


#include <bsp_init.h>
#include "Clock_Ip.h"
#include "Clock_Ip_Cfg.h"

/* Interrupt interfaces. */
#include "IntCtrl_Ip.h"
#include "IntCtrl_Ip_Cfg.h"



BspInitStatusType Bsp_Core2Init(void)
{
    IntCtrl_Ip_StatusType interruptStatus;

    interruptStatus = IntCtrl_Ip_Init(&IntCtrlConfig_2);

    if (interruptStatus != INTCTRL_IP_STATUS_SUCCESS)
    {
        return BSP_INIT_INTERRUPT_FAILURE;
    }

    return BSP_INIT_SUCCESS;
}
