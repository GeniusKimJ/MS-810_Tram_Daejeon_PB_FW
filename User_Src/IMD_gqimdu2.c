/*=================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Insultion detection seneor - CAN)	        	                   *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025. 11. 20                                                                *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               IMD_gqimdu2.c
\author             KKD
\date               2025-11-21 
\brief              절연센서를 위한 코드.
\brief              DIP SW1[1:0] 에 해당하는 FLAG(n)을 넘겨주면 측정 CAN ID를 0x18FF505n 으로 설정
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <IMD_gqimdu2.h>
#include <ms_timer.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
sImdData g_sImdData;
static u32 g_u32ImdLastTime;
/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/




static u32 s_u32ImdCanId = IMD_CAN_ID_MEAS_BASE;   // 0x18FF5050 (FLAG = 0)

void IMD_Init(void)
{
    memset(&g_sImdData, 0, sizeof(g_sImdData));

    g_sImdData.s8Temp_C    		= (-40);
    g_sImdData.fTemp_C     		= (-40.0f);
    g_sImdData.fRp_kOhm    		= (0.0f);
    g_sImdData.fRn_kOhm    		= (0.0f);
    g_sImdData.fRisoMin_kOhm	= (0.0f);
    g_sImdData.fVb_V       		= (0.0f);
    g_sImdData.bCommTimeout 	= (TRUE);         // 초기에는 미수신 상태
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
    TimerAdd( 500, IMD_CanRx_Timeout, &g_sImdData.s32TimerID );
#endif //CAN_BUF_UPATE
    
}

//DIP SW1[1:0] 에 해당하는 FLAG(n)을 넘겨주면 측정 CAN ID를 0x18FF505n 으로 설정
void IMD_SetCanFlag(u8 u8Flag)
{
	u8 ftmp = 0;

	ftmp = (u8Flag & 0x03U);
	                                                        
    s_u32ImdCanId = IMD_CAN_ID_MEAS_BASE + (u32)ftmp;
}

u32 IMD_GetCanId(void)
{
    return s_u32ImdCanId;
}
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
msStatus_t IMD_RxParsing  ( uint32_t canID, u8 *pu8CanRxData )
{
    msStatus_t retValue = MS_ERROR;
    switch( canID )
    {
        case IMD_CAN_ID:
        		IMD_CanRxParser(&g_sImdData, pu8CanRxData );						
                retValue = MS_SUCCESS;
            break;
        default:
            break;
    }
    return retValue;
}
#endif //CAN_BUF_UPATE

//CAN Rx 콜백에서 GAS 센서와 동일한 방식으로 호출                                          
void IMD_CanRxParser(sImdData *puImdData, const u8 *pu8CanRxData)
{
	// Status
	puImdData->bWarnISO_R		= ((pu8CanRxData[0] & (u8)(1U << 0)) != 0U) ? TRUE : FALSE;				// BYTE 3 : Bit[1:0]
	puImdData->bFaultISO_R		= ((pu8CanRxData[0] & (u8)(3U << 0)) != 0U) ? TRUE : FALSE;				// BYTE 3 : Bit[1:0]
    puImdData->bLowVoltage    	= ((pu8CanRxData[0] & (u8)(1U << 2)) != 0U) ? TRUE : FALSE;				// BYTE 3 : Bit2
    puImdData->bHighVoltage   	= ((pu8CanRxData[0] & (u8)(1U << 3)) != 0U) ? TRUE : FALSE;				// BYTE 3 : Bit3
    puImdData->bGroundFault   	= ((pu8CanRxData[0] & (u8)(1U << 4)) != 0U) ? TRUE : FALSE;				// BYTE 3 : Bit4
    puImdData->bNoNewEstimate 	= ((pu8CanRxData[0] & (u8)(1U << 6)) != 0U) ? TRUE : FALSE;				// BYTE 3 : Bit6

	//Temp
    puImdData->s8Temp_C  		= (int8_t)pu8CanRxData[1] - 40;                     			// scale=1, offset=-40
    puImdData->u16Rp_kOhm		= (u16)((u16)pu8CanRxData[2] |((u16)pu8CanRxData[3] << 8));     // Little Endian
    puImdData->u16Rn_kOhm		= (u16)((u16)pu8CanRxData[4] |((u16)pu8CanRxData[5] << 8));
    puImdData->u16Vb_x10 		= (u16)((u16)pu8CanRxData[6] |((u16)pu8CanRxData[7] << 8));

    //Conv
    puImdData->fTemp_C   		= (float)puImdData->s8Temp_C;
    puImdData->fRp_kOhm   		= (float)puImdData->u16Rp_kOhm;
    puImdData->fRn_kOhm   		= (float)puImdData->u16Rn_kOhm;
    puImdData->fRisoMin_kOhm 	= (puImdData->fRp_kOhm <= puImdData->fRn_kOhm) ? puImdData->fRp_kOhm : puImdData->fRn_kOhm;
    puImdData->fVb_V      		= (float)puImdData->u16Vb_x10 * 0.1f;

    // 수신 Check
    puImdData->bCommTimeout 	= FALSE;
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
    TimerRestart( puImdData->s32TimerID );
#else //CAN_BUF_UPATE
	g_u32ImdLastTime = HAL_GetTick();
#endif //CAN_BUF_UPATE
}

BOOL IMD_DiagnosicCheck(sImdData *puImdData)  
{
	if(puImdData->bWarnISO_R == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_W_R500_Under_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_W_R500_Under_Type); }

	if(puImdData->bFaultISO_R == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_F_R100_Under_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_F_R100_Under_Type); }

	if(puImdData->bLowVoltage == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_Low_V_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_Low_V_Type); }

	if(puImdData->bHighVoltage == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_Max_V_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_Max_V_Type); }
	
	if(puImdData->bGroundFault == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_GFD_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_GFD_Type); }

	if(puImdData->bNoNewEstimate == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_No_NewEstimates_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_No_NewEstimates_Type); }

	if(puImdData->bCommTimeout == TRUE)	{ puImdData->u16ErrStatus |= ((u16)1<<(u16)IErr_CanRxTimeout_Type); }
	else					{ puImdData->u16ErrStatus &= ~((u16)1<<(u16)IErr_CanRxTimeout_Type); }
	
	if((puImdData->u16ErrStatus &  
      	( ((u16)1 << (u16)IErr_W_R500_Under_Type)  	
		| ((u16)1 << (u16)IErr_F_R100_Under_Type)		
      	| ((u16)1 << (u16)IErr_Low_V_Type)  			
      	| ((u16)1 << (u16)IErr_Max_V_Type) 			
      	| ((u16)1 << (u16)IErr_GFD_Type)				
      	| ((u16)1 << (u16)IErr_No_NewEstimates_Type)	
      	| ((u16)1 << (u16)IErr_CanRxTimeout_Type) 
	)) !=0U ){
        return TRUE;    
	}return FALSE;
}

#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
msStatus_t IMD_CanRx_Timeout(void)
{
    g_sHallData.bComTimout = TRUE;                                             		//KKD 2025-12-01 Rx 泥댄???洹?珥湲고
    return MS_ENABLE;
}
#else  //CAN_BUF_UPATE
//u32TimeoutMs : IMD 프레임 타임아웃 (예: 500ms)                                   
void IMD_CanRx_Timeout(sImdData *puImdData)
{
    u32 now = HAL_GetTick();
    if ((now - g_u32ImdLastTime) >= IMD_TIMEOUT_MS){
        puImdData->bCommTimeout = TRUE;                                             	//KKD 2025-12-01 Rx 체크 플래그 초기화
        g_u32ImdLastTime = now;
    }
}
#endif //CAN_BUF_UPATE

void IMD_Proc(void)
{
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
	IMD_DiagnosicCheck(&g_sImdData);
#else  //CAN_BUF_UPATE
	if(g_bSenIcRxDone[SenIC_IMD_Type] == TRUE){
		g_bSenIcRxDone[SenIC_IMD_Type] = FALSE;
		IMD_CanRxParser(&g_sImdData, g_u8SenIcRxData[SenIC_IMD_Type]);
	}
	IMD_CanRx_Timeout(&g_sImdData);
	IMD_DiagnosicCheck(&g_sImdData);
#endif //CAN_BUF_UPATE
}


float IMD_GetRisoMin_kOhm(void)
{
    return g_sImdData.fRisoMin_kOhm;
}

float IMD_GetHvVoltage_V(void)
{
    return g_sImdData.fVb_V;
}

float IMD_GetTemperature_C(void)
{
    return g_sImdData.fTemp_C;
}

u16 IMD_GetErrStatus(void)
{
	return g_sImdData.u16ErrStatus;
}


