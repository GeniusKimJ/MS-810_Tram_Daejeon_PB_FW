/*=================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Hall sensor - CAN 500kbit/s)	        	                           *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025. 11. 20                                                                *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*

\file               HALL_cab1500.h
\author             KKD
\date               2025-11-21 
\brief              HALL sensor - CAN 500kbit/s
*/
/************************ (C) COPYRIGHT 2025 MisumSystech ***************************
* History:
* 2025-11-21     v0.01    KKD    Create
********************************************************************************/
#ifndef HALL_CAB1500_H
#define HALL_CAB1500_H

/* Includes ------------------------------------------------------------------*/
//#include <ms_main.h>
#include "ms_can.h"
/* Exported define ------------------------------------------------------------*/
#define CURR_SG1_TH_LOW_A        	(float)(220.0f)
#define CURR_SG1_DIFF_LOW_A      	(float)(44.0f)       	// <220A
#define CURR_SG1_DIFF_RATIO      	(float)(0.20f)       	// ±20%


#define HALL_CAN_ID              	(0x3C2U)
#define HALL_TIMEOUT_MS          	(2000U)					// safety timing
#define HALL_SEQ_STUCK_TH			(5U)
#define HMOV_CNT					(10U)


/* Exported macro -------------------------------------------------------------*/
/* Exported typedef -----------------------------------------------------------*/

typedef enum{
	HErr_SeqStuck_Type	= 0,
	HErr_SupPwr_Type,
	HErr_InterErr_Type,
	HErr_GoalViol_Type,
	HErr_CanTimeout_Type,
	HErr_Comp_Type,
	HErr_CanRxTimeout_Type,
	HErr_CRC_Type,
	Max_HallErr_Type
}eHall_Err_Type;

typedef struct{
	eHall_Err_Type eHallErr;                                                         	//KKD 2025-12-02 Error pos
	u8	u8SeqCnt;									                                 	//KKD 2025-12-02  Sequence Counter (0~15)
	u8	u8StsPwr;									                                 	//KKD 2025-12-02 PowerSupply Status (bit1~0)
	u8	u8StsIntErr;								                                  	//KKD 2025-12-02 Internal Error Flag (bit2)
	u8	u8StsSgv;							                                         	//KKD 2025-12-02 SafetyGoalViolation (bit3)
	u8	u8SeqStuckErr;									                            	//KKD 2025-12-02 Sequence Counter Stuck Err
    u8	bComTimout;                                                                		//KKD 2025-12-02 0.5s 미응답
    
	s32	s32Analog;										                           		//KKD 2025-12-02 Analog Current (A 단위 Scaled 0.001A 단위)
	s32	s32AnalMovAvg;
	s32	s32Digital;																		//KKD 2025-12-02 Digital Current (SF 버전만, 0.001A 단위)
	s32	s32DigtMovAvg;

	u8	u8SeqStuckCnt;
	u8	u8Crc;                                                                        	//KKD 2025-12-02  CRC_IP Raw
	u16	u16ErrStatus;																	//KKD 2025-12-02 Total fault

	s32	i1sec;
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
	s32	s32TimerID;                                                                      //jkpark 2025-12-05 타이머를 구분하기 위햔 ID.
#else  //CAN_BUF_UPATE
#endif //CAN_BUF_UPATE
	
}sCab1500Data;

extern sCab1500Data g_sHallData;
/* Exported variables ---------------------------------------------------------*/

/* Exported function prototypes -----------------------------------------------*/
void 	HALL_Init(void);
s32 	HALL_MovAvg(s32 s32Data, u8 u8Pos);

void	HALL_CanRxParsing( sCab1500Data *psHallData, u8 *pu8CanRxData );
BOOL	HALL_DiagnosicCheck(sCab1500Data *psHallData);
BOOL	HALL_SM_CurrentSenseCheck(sCab1500Data *psHallData);
#ifdef CAN_BUF_UPATE    //2025-12-08   jkpark  Can Buffer 관련 수정.
msStatus_t HALL_RxParsing  ( uint32_t canID, u8 *pu8CanRxData );
msStatus_t HALL_CanRx_TimeoutCheck( void );
#else  //CAN_BUF_UPATE
BOOL	HALL_CanRx_TimeoutCheck(sCab1500Data *psHallData);
#endif //CAN_BUF_UPATE
void	HALL_Proc(void);

u16		HALL_GetErrStatus(void);
float	HALL_GetAnalogAmp(void);
float	HALL_GetDigitalAmp(void);
u8		HALL_GetSeqCount(void);
s32		HALL_GetCurr1Sec(void);



#endif
