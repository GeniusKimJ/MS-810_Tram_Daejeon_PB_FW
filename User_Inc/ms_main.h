
/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(main)                                           			  *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                               		   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_main.h
\author             KKD
\date               2025-11-24 
\brief              main
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_MAIN_H
#define MS_MAIN_H

/* Includes ---------------------------------------------------------------------------------------*/
#include <time.h>    // time()
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>	// use double_t

//#define DET_UPATE        //2025-12-04   jkpark  DETECT 관련 수정.
//#define CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
#define PWR_SLEEP_UPATE    //2025-12-08   jkpark  Sleep 모드 적용.
#define WACHDOG_DISABLE        //2026-01-16   jkpark  테스트를 위해 Watchdog 비활성화.
#define EN_CPULOAD

#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "wwdg.h"
#include "ms_config.h"
#include "ms_util.h"
#include "ms_process.h"
#include "AFE_BasicConfig.h"



/*
#include "ms_adc.h"
#include "ms_uart1.h"
#include "ms_uart2.h"
#include "ms_uart3.h"
#include "ms_can.h"
#include "ms_eep.h"
#include "ms_gpio.h"
#include "ms_flash.h"
#include "ms_timer.h"
*/
/*
#include "AFE_BasicConfig.h"
#include "AFE_adbms6815.h"
#include "AFE_adbms6815_SM.h"
#include "EEP_m24256a.h"
#include "EEP_m24m02a.h"
#include "FIRE_phsc220xcr.h"
#include "GAS_sgxbld1.h"
#include "HALL_cab1500.h"
#include "HALL_s124.h"
#include "HUMI_sht3xdis.h"
#include "IMD_gqimdu2.h"
#include "RTC_mcp7940a.h"
*/

//#include "ms_btms.h"
//#include "ms_det.h"
//#include "ms_process.h"
//#include "ms_mvb.h"
//#include "ms_relay.h"
//#include "ms_soc.h"
//#include "ms_temp.h"
//#include "ms_det.h"
//#include "ms_alarm.h"
//#include "ms_boot.h"

/* Exported define ---------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
extern Pkt_Rack RackPkt;
extern sSystemTick g_sSysTick;
extern sMn	g_sMn;
extern u8	g_f1s;
extern volatile u8 g_u8fIdle;
extern volatile u32 g_u32CntIdle;    
extern volatile u32 g_u32CntTotal;   
extern volatile u32 g_u32LastTime;   
extern volatile u32 g_u32CntCpuLoad;  
extern volatile float g_f32CpuLoad;  
extern volatile u32 g_u32dummy; 
extern volatile float g_fratio; 



/* Exported variables ------------------------------------------------------------------------------*/

/* Exported function prototypes --------------------------------------------------------------------*/
float	GetCpuLoad(void);
void	CpuLoadTest(void);
void	ms_main(void);
void 	dbg_qac(void);



#endif //__MS_MAIN_H__

