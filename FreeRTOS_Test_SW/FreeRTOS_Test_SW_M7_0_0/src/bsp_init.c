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

/* Power IP interfaces */
#include "Power_Ip.h"
#include "Power_Ip_Cfg.h"


BspInitStatusType Bsp_SystemInit(void)
{

	Clock_Ip_StatusType clockStatus;

    /*
     * Initialize the Power IP driver on the boot coordinator.
     * This prepares the generated MC_RGM/PMC support configuration;
     * it does not yet release CM7_2.
     */
    Power_Ip_Init(&Power_Ip_HwIPsConfigPB);

    clockStatus = Clock_Ip_Init(&Clock_Ip_aClockConfig[0]);

    if (clockStatus != CLOCK_IP_SUCCESS)
    {
        return BSP_INIT_CLOCK_FAILURE;
    }

    return BSP_INIT_SUCCESS;
}

BspInitStatusType Bsp_Core0Init(void)
{
    IntCtrl_Ip_StatusType interruptStatus;

    interruptStatus = IntCtrl_Ip_Init(&IntCtrlConfig_0);

    if (interruptStatus != INTCTRL_IP_STATUS_SUCCESS)
    {
        return BSP_INIT_INTERRUPT_FAILURE;
    }

    return BSP_INIT_SUCCESS;
}

void Bsp_ReleaseCore2(void)
{
    /*
     * Apply the generated RUN-mode configuration.
     *
     * The configuration writes 0x00700000 to the CORE4 boot-address
     * register, enables the CM7_2 clock and triggers the MC_ME update.
     */
    Power_Ip_SetMode(&Power_Ip_aModeConfigPB[0]);
}
