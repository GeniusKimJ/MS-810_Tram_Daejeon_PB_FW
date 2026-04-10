/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(insulation sensor - CAN)                                            *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               IMD_gqimdu2.h
\author             KKD
\date               2025-11-24 
\brief              insulation sensor - CAN
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef IMD_GQIMDU2_H
#define IMD_GQIMDU2_H

/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_can.h"
#include "ms_timer.h"

/* Exported define ---------------------------------------------------------------------------------*/
#define IMD_CAN_ID_MEAS_BASE    ((u32)0x18FF5050UL)   // 0x18FF505n, n = CAN FLAG
#define IMD_CAN_ID_MASK         ((u32)0x1FFFFFFFUL)   // 29bit ID mask
#define IMD_CAN_ID				IMD_CAN_ID_MEAS_BASE
#define IMD_TIMEOUT_MS			(500U)

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef enum
{
    IErr_W_R500_Under_Type		= 0,	// 10 : Riso < 500 Ω/V
    IErr_F_R100_Under_Type,         	// 11 : Riso < 100 Ω/V
    IErr_Low_V_Type,        			// 
    IErr_Max_V_Type,
    IErr_GFD_Type,
    IErr_No_NewEstimates_Type	= 6,
	IErr_CanRxTimeout_Type		= 8,
    Max_ImdErr_Type
} eImd_Err_Type;


#pragma pack(push,1)
typedef struct
{
    /* Raw data ***********************************************************************************/
    s8		s8Temp_C;          			// Byte1, °C (offset -40)
    u16		u16Rp_kOhm;        			// Byte2-3, kΩ
    u16		u16Rn_kOhm;        			// Byte4-5, kΩ
    u16		u16Vb_x10;         			// Byte6-7, 0.1V 단위 (x10)

    /* Converted **********************************************************************************/
    float	fTemp_C;           			// 실제 온도 값
    float	fRp_kOhm;          			// HV+ 절연저항
    float	fRn_kOhm;          			// HV- 절연저항
    float	fRisoMin_kOhm;     			// min(Rp, Rn)
    float	fVb_V;             			// HV 전압

    eImd_Err_Type eIsoState;    		// 절연 상태

    BOOL	bWarnISO_R;
    BOOL	bFaultISO_R;
    BOOL	bLowVoltage;
    BOOL	bHighVoltage;
    BOOL	bGroundFault;
    BOOL	bNoNewEstimate;
    BOOL	bCommTimeout;	    		// 타임아웃 플래그

    u32		u32RxCnt;            		// 누적 수신 카운트
    u32		u32LastRxTick;       		// 마지막 수신 Tick (HAL_GetTick 기준)

    u16		u16ErrStatus;
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
    s32		s32TimerID;                                       //jkpark 2025-12-05 Timer컨트롤을 위한 ID
#endif //CAN_BUF_UPATE
    
} sImdData;

extern sImdData g_sImdData;

/* Exported variables ------------------------------------------------------------------------------*/
/* Exported function prototypes --------------------------------------------------------------------*/

void	IMD_Init(void);
void    IMD_Proc(void);
void	IMD_SetCanFlag(u8 u8Flag);
u32		IMD_GetCanId(void);
void	IMD_CanRxParser(sImdData *puImdData, const u8 *pu8CanRxData);
BOOL 	IMD_DiagnosicCheck(sImdData *puImdData);
float	IMD_GetRisoMin_kOhm(void);
float	IMD_GetHvVoltage_V(void);
float	IMD_GetTemperature_C(void);
u16 	IMD_GetErrStatus(void);
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
msStatus_t IMD_RxParsing  ( uint32_t canID, u8 *pu8CanRxData );
msStatus_t IMD_CanRx_Timeout( void );
#else  //CAN_BUF_UPATE
void	IMD_CanRx_Timeout(sImdData *puImdData);
#endif //CAN_BUF_UPATE

#endif //__IMD_GQIMDU2_H__

