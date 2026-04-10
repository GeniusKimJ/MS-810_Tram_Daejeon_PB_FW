/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P((bms process(V/C/T Cal or Calibration)))                                           			  *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                               		   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_process.h
\author             KKD
\date               2025-11-24 
\brief              (bms process(V/C/T Cal or Calibration))
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_PROCESS_H
#define MS_PROCESS_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_config.h"

#include "ms_adc.h"
#include "ms_uart.h"
#include "ms_can.h"
#include "ms_eep.h"
#include "ms_gpio.h"
#include "ms_flash.h"
#include "ms_timer.h"

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

#include "ms_btms.h"
#include "ms_det.h"
#include "ms_process.h"
#include "ms_mvb.h"
#include "ms_relay.h"
#include "ms_soc.h"
#include "ms_temp.h"
#include "ms_det.h"
#include "ms_alarm.h"
#include "ms_boot.h"





/* Exported define ---------------------------------------------------------------------------------*/
#define	DLYBALO				(3000U/100U)                                                  //KKD 2025-12-19 bal chk time
#define	DLYBALR				(3000U/100U)                                                  //KKD 2025-12-19 bal chk time

#define MOVCNT	20


/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef enum{
	MOV_CURR_HI	= 0,
	MOV_CURR_LO	= 1,
	MOV_CAB1500	= 2,
	MAX_MOV
}eMovAvg;



/* Exported variables ------------------------------------------------------------------------------*/


/* Exported function prototypes --------------------------------------------------------------------*/
void	Proc_Bms_Init(void);
void    Proc_Afe(void);

s32		Proc_MovAvg(s32 u32Data, eMovAvg ePos);
void    Proc_MenualTest(void);
void 	Proc_SensorCheck(void);

void	Proc_Soc_Soh(Pkt_Rack *pBmsData);
void	Proc_Check_Status(Pkt_Rack *pBmsData);
void	Proc_Check_CommStatus(Pkt_Rack *pBmsData);
void 	Proc_Detection(Pkt_Rack *pBmsData);

void    Proc_EepSave(void);
void    Proc_Adc(void);
void    Proc_Io(void);
void 	Proc_Calcul_CvTh(Pkt_Rack *pBmsData);

s16 	Proc_Get_ThAvg(void);	
s16 	Proc_Get_ThMax(void);	
s16 	Proc_Get_ThMin(void);	

u16 	Proc_Get_CvAvg(void);	
u16 	Proc_Get_CvMax(void);	
u16 	Proc_Get_CvMin(void);	

#endif //__MS_PROCESS_H__
