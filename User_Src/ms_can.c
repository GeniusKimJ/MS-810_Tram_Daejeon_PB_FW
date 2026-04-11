/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(debug can data rtx)		                                           *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_can.c
\author             KKD
\date               2025-11-21 
\brief              Can 통신을 위한 코드.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ----------------------------------------------------------------------------------------*/
#include "ms_can.h"
#include "ms_util.h"
#include "HALL_cab1500.h"
#include "IMD_gqimdu2.h"
#include "GAS_sgxbld1.h"
/* Private define -----------------------------------------------------------------------------------*/
//abc
//123
/* Private macro -----------------------------------------------------------------------------------*/
/* Private typedef ----------------------------------------------------------------------------------*/
static sFifo g_sFifo[Max_McuCan_Type];
sCanData g_stLastTxCanData[Max_McuCan_Type];

#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.
sRxFifoType g_stCanRxData;
#endif //CAN_BUF_UPATE

/* Private variables --------------------------------------------------------------------------------*/
BOOL g_bCanTxDone[Max_McuCan_Type]		= {0,};
BOOL g_bSenIcRxDone[Max_SenIC_Type]		= {0,};
u8   g_u8SenIcRxData[Max_SenIC_Type][8] 	= {0,};

#ifdef EN_BOOT
//u8	fRcvCan1Boot = 0;
u8 	Total_Judge;
#endif
/* Private function prototypes -----------------------------------------------------------------------*/
/* Private functions --------------------------------------------------------------------------------*/

#if 1
//sCanTxFailLog g_stCanTxFailLog;
#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.
msStatus_t Can_Fifo_RxInit( void )
{
    return Util_Buffer_Init( &g_stCanRxData.bufInfo, (unsigned char*)g_stCanRxData.stCanRxData , sizeof( g_stCanRxData.stCanRxData ) );
}
#endif //CAN_BUF_UPATE

void Can_Fifo_In(CAN_TxHeaderTypeDef* pTxHeader, u8 u8CanData[], Enum_McuCan_Type eMcuCan)
{
#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
    sCanData canTemp;
    msStatus_t retValue = MS_ERROR;
//    int usedSize = 0;

    memcpy( &canTemp.TxHeader , pTxHeader , sizeof( CAN_TxHeaderTypeDef ) );
    memcpy( canTemp.u8Data    , u8CanData , CAN_DATA_SIZE                 );

    retValue = Util_Buffer_Write( &g_sFifo[eMcuCan].bufInfo, (unsigned char*)&canTemp, sizeof( sCanData ), NULL );

    if( retValue != MS_SUCCESS ){
        g_sFifo[eMcuCan].u32RemainZeroCount++;
    }

#else  //CAN_BUF_UPATE
	u8 i = 0;

	// Data Input
	g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].TxHeader.StdId = pTxHeader->StdId;
	g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].TxHeader.ExtId = pTxHeader->ExtId;
	g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].TxHeader.IDE = pTxHeader->IDE;
	g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].TxHeader.RTR = pTxHeader->RTR;
	g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].TxHeader.DLC = pTxHeader->DLC;
	g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].TxHeader.TransmitGlobalTime = pTxHeader->TransmitGlobalTime;

	for(i = 0U; i < 8U; i ++){
		g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32InputPos].u8Data[i] = u8CanData[i];
	}

	// Input Count ++
	g_sFifo[eMcuCan].u32InputPos ++;
	if(g_sFifo[eMcuCan].u32InputPos >= (u32)FIFO_CAN_MAX){
		g_sFifo[eMcuCan].u32InputPos = (u32)0;
		}

	// Calcul Remain
	if(g_sFifo[eMcuCan].u32InputPos < g_sFifo[eMcuCan].u32OutPos){                    																//KKD 2025-12-01 compare in out pos
		g_sFifo[eMcuCan].u32Remain = (u32)FIFO_CAN_MAX - g_sFifo[eMcuCan].u32OutPos + g_sFifo[eMcuCan].u32InputPos;
	}else{
		g_sFifo[eMcuCan].u32Remain = g_sFifo[eMcuCan].u32InputPos - g_sFifo[eMcuCan].u32OutPos;
	}
	
	if(g_sFifo[eMcuCan].u32MaxRemain < g_sFifo[eMcuCan].u32Remain){                   																//KKD 2025-12-01 max pos = curr pos 대치
		g_sFifo[eMcuCan].u32MaxRemain = g_sFifo[eMcuCan].u32Remain;
	}
#endif //CAN_BUF_UPATE

}

u32 Can_Fifo_GetRemainCount(Enum_McuCan_Type eMcuCan)
{
#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
    return (u32)( g_sFifo[eMcuCan].bufInfo.bufSize - g_sFifo[eMcuCan].bufInfo.count );
#else  //CAN_BUF_UPATE
	if(g_sFifo[eMcuCan].u32Remain == (u32)0){
		g_sFifo[eMcuCan].u32RemainZeroCount++;
	}

	return g_sFifo[eMcuCan].u32Remain;
#endif //CAN_BUF_UPATE
}

u32 Can_Fifo_GetMaxRemainCount(Enum_McuCan_Type eMcuCan)
{
#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
	return (u32)g_sFifo[eMcuCan].bufInfo.bufSize;
#else  //CAN_BUF_UPATE
	return g_sFifo[eMcuCan].u32MaxRemain;                                              																//KKD 2025-12-01 최대 fifo
#endif //CAN_BUF_UPATE
}

u32 Can_Fifo_GetRemainZeroCount(Enum_McuCan_Type eMcuCan)
{
	return g_sFifo[eMcuCan].u32RemainZeroCount;
}

BOOL Can_Fifo_Out(CAN_TxHeaderTypeDef* pTxHeader, u8 u8CanData[], Enum_McuCan_Type eMcuCan)
{
#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
    sCanData canTemp;
    
	if( Util_Buffer_Read( &g_sFifo[eMcuCan].bufInfo, (unsigned char*)&canTemp, sizeof( sCanData ), NULL ) != MS_SUCCESS ){
		return FALSE;
	}

	// Data Input
    memcpy( pTxHeader , &canTemp.TxHeader , sizeof( CAN_TxHeaderTypeDef ) );
    memcpy( u8CanData , canTemp.u8Data    , CAN_DATA_SIZE                 );

	return TRUE;
#else  //CAN_BUF_UPATE
	u8 i = 0;
	
	if(g_sFifo[eMcuCan].u32Remain == (u32)0){
		return FALSE;
	}
	
	// Data Input
	pTxHeader->StdId	= g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].TxHeader.StdId;
	pTxHeader->ExtId	= g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].TxHeader.ExtId;
	pTxHeader->IDE		= g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].TxHeader.IDE;
	pTxHeader->RTR		= g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].TxHeader.RTR;
	pTxHeader->DLC		= g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].TxHeader.DLC;
	pTxHeader->TransmitGlobalTime = g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].TxHeader.TransmitGlobalTime;

	for(i = 0U; i < 8U; i ++){
		u8CanData[i] = g_sFifo[eMcuCan].stCanData[g_sFifo[eMcuCan].u32OutPos].u8Data[i];
	}
	
	// Input Count ++
	g_sFifo[eMcuCan].u32OutPos++;
	if(g_sFifo[eMcuCan].u32OutPos >= (u32)FIFO_CAN_MAX){
		g_sFifo[eMcuCan].u32OutPos = (u32)0;
	}

	// Calcul Remain
	if(g_sFifo[eMcuCan].u32InputPos < g_sFifo[eMcuCan].u32OutPos){
		g_sFifo[eMcuCan].u32Remain = (u32)FIFO_CAN_MAX - g_sFifo[eMcuCan].u32OutPos + g_sFifo[eMcuCan].u32InputPos;
	}else{
		g_sFifo[eMcuCan].u32Remain = g_sFifo[eMcuCan].u32InputPos - g_sFifo[eMcuCan].u32OutPos;
	}
	
	if(g_sFifo[eMcuCan].u32MaxRemain < g_sFifo[eMcuCan].u32Remain){
		g_sFifo[eMcuCan].u32MaxRemain = g_sFifo[eMcuCan].u32Remain;
	}

	return TRUE;
#endif //CAN_BUF_UPATE
}

void Can_Fifo_TxCheck_In(CAN_TxHeaderTypeDef* pTxHeader, u8 u8CanData[], Enum_McuCan_Type eMcuCan)
{
	u8 i = 0;
	
	g_stLastTxCanData[eMcuCan].TxHeader.StdId = pTxHeader->StdId;
	g_stLastTxCanData[eMcuCan].TxHeader.ExtId= pTxHeader->ExtId;
	g_stLastTxCanData[eMcuCan].TxHeader.IDE = pTxHeader->IDE;
	g_stLastTxCanData[eMcuCan].TxHeader.RTR= pTxHeader->RTR;
	g_stLastTxCanData[eMcuCan].TxHeader.DLC = pTxHeader->DLC;
	g_stLastTxCanData[eMcuCan].TxHeader.TransmitGlobalTime = pTxHeader->TransmitGlobalTime;

	for(i = 0U; i < 8U; i ++){
		g_stLastTxCanData[eMcuCan].u8Data[i] = u8CanData[i];
	}
	g_sFifo[eMcuCan].bTxDone = FALSE;
}

void Can_Fifo_TxCheck_SetTxDone(Enum_McuCan_Type eMcuCan)
{
	g_sFifo[eMcuCan].bTxDone = TRUE;
	g_sFifo[eMcuCan].u8CanTxOkFalseCount = 0;

#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
    Util_Buffer_Init( &g_sFifo[eMcuCan].bufInfo, (unsigned char*)g_sFifo[eMcuCan].stCanData , sizeof( g_sFifo[eMcuCan].stCanData ));
#endif //CAN_BUF_UPATE
}

BOOL Can_Fifo_TxCheck_TxOkFalseCount(Enum_McuCan_Type eMcuCan)
{
	if(g_sFifo[eMcuCan].u8CanTxOkFalseCount < (u32)30){
		g_sFifo[eMcuCan].u8CanTxOkFalseCount ++;
		return FALSE;
	}else{
		return TRUE;
	}
}

BOOL Can_Fifo_TxCheck_GetTxDone(Enum_McuCan_Type eMcuCan)
{
	return g_sFifo[eMcuCan].bTxDone;                                                   																//KKD 2025-11-26 TX 확인
}

void Can_Fifo_TxCheck_SetTxReady(Enum_McuCan_Type eMcuCan)
{
	g_sFifo[eMcuCan].bTxDone = FALSE;                                                  																//KKD 2025-11-26 TX 상태 설정
}


BOOL Can_Fifo_TxCheck_GetLastTx(sCanData* pstLastData, Enum_McuCan_Type eMcuCan)
{
	u8 i = 0;

	if(g_stLastTxCanData[eMcuCan].TxHeader.DLC == (u32)0){
		return FALSE;
	}
	pstLastData->TxHeader.StdId = g_stLastTxCanData[eMcuCan].TxHeader.StdId;            //KKD 2025-11-26 마지막 송신값 Check
	pstLastData->TxHeader.ExtId = g_stLastTxCanData[eMcuCan].TxHeader.ExtId;
	pstLastData->TxHeader.IDE = g_stLastTxCanData[eMcuCan].TxHeader.IDE;
	pstLastData->TxHeader.RTR = g_stLastTxCanData[eMcuCan].TxHeader.RTR;
	pstLastData->TxHeader.DLC = g_stLastTxCanData[eMcuCan].TxHeader.DLC;
	pstLastData->TxHeader.TransmitGlobalTime = g_stLastTxCanData[eMcuCan].TxHeader.TransmitGlobalTime;

	for(i = 0U; i< 8U; i ++){
		pstLastData->u8Data[i] = g_stLastTxCanData[eMcuCan].u8Data[i];
	}
	return TRUE;
}

void Can_Fifo_WriteLog_TxFail(CAN_TxHeaderTypeDef* BeforeTxHeader, sCanTxFailLog* pstCanTxFailLog)
{
	pstCanTxFailLog->u32Count ++;
/*	
	pstCanTxFailLog->TxFaultLog_Header[pstCanTxFailLog->u32StartPos].

	pBeforeTxHeader->StdId = g_LastTxHeader.StdId;
	pBeforeTxHeader->ExtId = g_LastTxHeader.ExtId;
	pBeforeTxHeader->IDE = g_LastTxHeader.IDE;
	pBeforeTxHeader->RTR = g_LastTxHeader.RTR;
	pBeforeTxHeader->DLC = g_LastTxHeader.DLC;
	pBeforeTxHeader->TransmitGlobalTime = g_LastTxHeader.TransmitGlobalTime;
*/		
}

u32 Can_Fifo_Get_MaxFifoCount(Enum_McuCan_Type eMcuCan)
{
#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
	return (u32)g_sFifo[eMcuCan].bufInfo.bufSize;
#else  //CAN_BUF_UPATE
	return g_sFifo[eMcuCan].u32MaxRemain;
#endif //CAN_BUF_UPATE
}

void Can_Fifo_TxCheck_CheckAbortMsg(u8 u8TxMailBoxPos, sCanData* pstLastTxCanData, Enum_McuCan_Type eMcuCan)
{
	g_sFifo[eMcuCan].u32AbortCount[u8TxMailBoxPos]++;                                 																	//KKD 2025-11-26 비정상 check
}

void Can_Fifo_TxCheck_CheckErrorMsg(sCanData* pstLastTxCanData, Enum_McuCan_Type eMcuCan)
{
	g_sFifo[eMcuCan].u32TxErrorCount++;                                                																//KKD 2025-12-26 에러 check
}

u32 Can_Fifo_TxCheck_GetErrorCount(Enum_McuCan_Type eMcuCan)
{
	return g_sFifo[eMcuCan].u32TxErrorCount;                                           																//KKD 2025-12-26 Get 에러 Count 
}
#endif


void Can_MakeConfigFilter(void)
{
	CAN_FilterTypeDef sFilterConfig;

	sFilterConfig.FilterMaskIdHigh = 0x0000;
	sFilterConfig.FilterMaskIdLow = 0x0000;
	sFilterConfig.FilterIdHigh = 0x0000;
	sFilterConfig.FilterIdLow = 0x0000;
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	sFilterConfig.FilterBank = 0;
	
	sFilterConfig.FilterActivation = (u32)ENABLE; //???? en

	if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK){
		Error_Handler();
	}
}

void Can_SetupInterrupt(void)
{
	u32 activeInt = 0; 
		/*--------------------------------------------------------------
		 * CAN 인터럽트 활성화 비트 설명
		 *--------------------------------------------------------------
		 * CAN_IT_TX_MAILBOX_EMPTY
		 *	 - 송신 메일박스(MB0~MB2)가 비게되면 발생
		 *	 - 즉, 메시지 전송 완료 시 인터럽트
		 *
		 * CAN_IT_RX_FIFO0_MSG_PENDING
		 *	 - FIFO0에 새로운 수신 메시지가 도착할 때 발생
		 *	 - 일반적인 CAN 수신 처리용 인터럽트
		 *
		 * CAN_IT_ERROR
		 *	 - CAN Error Warning / Error Passive / Arbitration / Stuff 등
		 *	   다양한 오류 이벤트가 발생할 때 인터럽트
		 *	 - 통신 상태 이상 감시용 (Error 통합 인터럽트)
		 *
		 * CAN_IT_BUSOFF
		 *	 - Bus-Off 상태로 진입할 때 발생
		 *	 - TEC >= 256 → 송신 불가 상태
		 *	 - 반드시 소프트웨어에서 CAN 재시작 필요
		 *
		 * CAN_IT_LAST_ERROR_CODE
		 *	 - 최근 오류 코드(LEC)가 변경될 때 발생
		 *	 - Stuff, Form, ACK, Bit Error 등 상세 에러 추적 가능
		 --------------------------------------------------------------*/
	
		activeInt = (
			  CAN_IT_TX_MAILBOX_EMPTY                                                   //KKD 2025-12-01 전송완료(메일박스 비었음)
			 | CAN_IT_RX_FIFO0_MSG_PENDING												//KKD 2025-11-26 수신 FIFO0 메시지 도착
//			 | CAN_IT_RX_FIFO0_MSG_PENDING												//KKD 2025-11-26 수신 FIFO0 메시지 도착
//			 | CAN_IT_ERROR																//KKD 2025-11-26 CAN 통합 에러 발생
//			 | CAN_IT_BUSOFF 															//KKD 2025-11-26 Bus-Off 진입
//			 | CAN_IT_LAST_ERROR_CODE													//KKD 2025-11-26 최근 Error Code 변경
		  );
	if(HAL_CAN_ActivateNotification(&hcan1, activeInt) != HAL_OK){
		Error_Handler();
	}	
}

void Can_Init(void)
{
	memset(g_sFifo,0x00,sizeof(g_sFifo));                               																				//KKD 2026-01-14 init buff
	memset(g_stLastTxCanData,0x00,sizeof(g_stLastTxCanData));                               															//KKD 2026-01-14 init buff
	memset(g_u8SenIcRxData,0x00,sizeof(g_u8SenIcRxData));                               																//KKD 2026-01-14 init buff
	
	Can_MakeConfigFilter();                                                             //KKD 2025-11-26 init 필터
	Can_SetupInterrupt();                                                               //KKD 2025-11-26 init 인터럽트
	Can_Fifo_TxCheck_SetTxDone(McuCan_Ext_Type);                                        //KKD 2025-11-26 init Ext tx_Flag
	//Can_Fifo_TxCheck_SetTxDone(McuCan_Int_Type);                                      //KKD 2025-11-26 init Std tx_Flag
#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.
    Can_Fifo_RxInit();
#endif //CAN_BUF_UPATE
	HAL_CAN_Start(&hcan1);                                                              //KKD 2025-11-26 Can start
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.

    sCanRxData rxData; 

//    int remainBufSize = 0;
//    msStatus_t  retValue = MS_ERROR;
	
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxData.RxHeader, rxData.u8Data) != HAL_OK){															//KKD 2025-12-01 Rx Callback
        return;
    }
//    retValue = Util_Buffer_Write( &g_stCanRxData.bufInfo, (unsigned char*)&rxData, sizeof( sCanRxData ), &remainBufSize );
    Util_Buffer_Write( &g_stCanRxData.bufInfo, (unsigned char*)&rxData, sizeof( sCanRxData ), NULL );

#else  //CAN_BUF_UPATE
  static BOOL btog = TRUE;
  CAN_RxHeaderTypeDef RxHeader;
	Enum_MenaulTest eMnPos	= MN_STANDBY_TYPE;
	u32 idx 				= 0;
	u8 rxdat[8] 			= {0};
	u8 dlc					= 0;
  	u16 pos;

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, rxdat) != HAL_OK){																			//KKD 2025-12-01 Rx Callback
        return;
    }
	
	dlc = (u8)RxHeader.DLC;
	if(dlc <8U) { return; }

	if(RxHeader.IDE == CAN_ID_STD)
	{
		idx = (RxHeader.StdId & 0x7FFU);

		if(idx == GAS_CAN_ID){
			memcpy(&g_u8SenIcRxData[SenIC_GAS_Type],&rxdat,dlc);
			g_bSenIcRxDone[SenIC_GAS_Type] = TRUE;
		}else if(idx == HALL_CAN_ID){
			memcpy(&g_u8SenIcRxData[SenIC_HALL_Type],&rxdat,dlc);
			g_bSenIcRxDone[SenIC_HALL_Type] = TRUE;
		}
	}
	else if(RxHeader.IDE == CAN_ID_EXT)
	{                                                    																								//KKD 2025-12-01 Ext
		idx = (RxHeader.ExtId & 0x1FFFFFFFU);
		if(idx ==IMD_CAN_ID){
			memcpy(&g_u8SenIcRxData[SenIC_IMD_Type],&rxdat,dlc);
			g_bSenIcRxDone[SenIC_IMD_Type] = TRUE;
			
		}
		if(idx == 0x700U){										// manual test
			eMnPos = (Enum_MenaulTest)rxdat[0];
			if(eMnPos == MN_STANDBY_TYPE)
			{
				g_sMn.mn_start = rxdat[1];
				if(g_sMn.mn_start == FALSE){
					memset(&g_sMn.mn_start,0x00,sizeof(g_sMn));
					btog = TRUE;
				}else{
					if(btog == TRUE){
						btog = FALSE;
						//(KKD) Init set v/t
						for(pos=0;pos<NCV;pos++)	{g_sMn.mn_cv[pos] = (u16)3800;}
						for(pos=0;pos<NTH;pos++)	{g_sMn.mn_th[pos] = (s16)250;}
					}
					g_sMn.mn_type = 0;
				}
			}
			if(g_sMn.mn_start == TRUE)
			{
				if(eMnPos == MN_CELL_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_CELL_TYPE);
					g_sMn.mn_set_cv = (u16)(((u16)rxdat[4] << 8u) | (u16)rxdat[3]);
					g_sMn.mn_set_cvpos = (u16)(((u16)rxdat[2] << 8u) | (u16)rxdat[1]);

					
					if((g_sMn.mn_set_cvpos > (u16)NCV)) 						{ return; }
					if((g_sMn.mn_set_cv < (u16)1500) || (g_sMn.mn_set_cv > (u16)5000)) { return; }


					if(g_sMn.mn_set_cvpos < NCV){
						g_sMn.mn_cv[g_sMn.mn_set_cvpos] = g_sMn.mn_set_cv;
					}else if(g_sMn.mn_set_cvpos == NCV){
						for(pos=0;pos < NCV;pos++){
							g_sMn.mn_cv[pos] = g_sMn.mn_set_cv;
						}
					}else{

					}
				}
				else if(eMnPos == MN_TEMP_TYPE)
				{
					g_sMn.mn_type |= ((u16)1u << (u16)MN_TEMP_TYPE);
					g_sMn.mn_set_thpos = rxdat[1];
					g_sMn.mn_set_th    = (s16)(((u16)rxdat[3] << 8u) | (u16)rxdat[2]);

					if(g_sMn.mn_set_thpos > (u8)NTH)                                   { return; }
					if((g_sMn.mn_set_th < (s16)-400) || (g_sMn.mn_set_th > (s16)900)) { return; }

					if(g_sMn.mn_set_thpos < (u8)NTH){
					    g_sMn.mn_th[g_sMn.mn_set_thpos] = g_sMn.mn_set_th;
					}else if(g_sMn.mn_set_thpos == (u8)NTH)	{
					    for(pos = 0u; pos < NTH; pos++){
					        g_sMn.mn_th[pos] = g_sMn.mn_set_th;
					    }
					}
				}
				else if(eMnPos == MN_CURR_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_CURR_TYPE);
					memcpy(&g_sMn.mn_set_curr,&rxdat[1], sizeof(g_sMn.mn_set_curr));
					if((g_sMn.mn_set_curr < (-800000L)) && (g_sMn.mn_set_curr > 800000))	{return;}
				}
				else if(eMnPos == MN_CAL_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_CAL_TYPE);
				}
				else if(eMnPos == MN_RLY_TYPE)
				{
				  	g_sMn.mn_type |= ((u16)1<<(u16)MN_RLY_TYPE);
					g_sMn.mn_set_rlysts = rxdat[1];
					if(g_sMn.mn_set_rlysts > 0x7FU) 										{return;}
					g_sRelayStatus.u8HvRly_CmdStatus = g_sMn.mn_set_rlysts;
				}
				else if(eMnPos == MN_DET_TYPE)
				{
					u16 refcv = 0u;
					g_sMn.mn_type |= ((u16)1<<(u16)MN_DET_TYPE);
					g_sMn.mn_set_det = (u16)(((u16)rxdat[2] << 8u) | (u16)rxdat[1]);
					g_sMn.offset_cv = 0;
					g_sMn.offset_th = 0;
					g_sMn.offset_curr = 0;
										
					if((g_sMn.mn_set_det & FCOVP) != 0){
						refcv = (u16)CELLOVPD - 20u;
						
						if(refcv > RackPkt.maxcv ){
							g_sMn.offset_cv = (s32)(refcv-RackPkt.maxcv);
						}	
					}else if((g_sMn.mn_set_det & FCUVP) != 0){
						refcv = (u16)CELLUVPD + 20u;
						
						if(refcv < RackPkt.mincv){
							g_sMn.offset_cv = (s32)(refcv -RackPkt.mincv);
						}	
					}else if((g_sMn.mn_set_det & FCOTP) != 0){
			
					}else if((g_sMn.mn_set_det & FCUTP) != 0){
			
					}else if((g_sMn.mn_set_det & FDOTP) != 0){
			
					}else if((g_sMn.mn_set_det & FDUTP) != 0){
					
					}else if((g_sMn.mn_set_det & FCOCP) != 0){
					
					}else if((g_sMn.mn_set_det & FDOCP) != 0){
					
					}	
				}
				else if(eMnPos == MN_BAL_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_BAL_TYPE);
					g_sMn.mn_set_bal_spi_pos = 0;
					g_sMn.mn_set_bal_slv_pos = 0;
					g_sMn.mn_set_bal_spi_pos = 0;
					g_sMn.mn_set_bal_slv_pos = 0;
					g_sMn.mn_set_bal_start = 0;


					if((rxdat[1] > 0) && (rxdat[1] <= NSPI)){
					  g_sMn.mn_set_bal_spi_pos = rxdat[1]-1;
					}

					if((rxdat[2] > 0) && (rxdat[2] <= NSLV)){
					  g_sMn.mn_set_bal_slv_pos = rxdat[2]-1;
					}
					
					g_sMn.mn_set_bal_flag[g_sMn.mn_set_bal_spi_pos][g_sMn.mn_set_bal_slv_pos] = rxdat[3];

					if(rxdat[4] == 1){
						g_sMn.mn_set_bal_start = rxdat[4];
					}else{
						g_sMn.mn_set_bal_spi_pos = 0;
						g_sMn.mn_set_bal_slv_pos = 0;
						g_sMn.mn_set_bal_spi_pos = 0;
						g_sMn.mn_set_bal_slv_pos = 0;
						g_sMn.mn_set_bal_start = 0;
					}
				}
				else if(eMnPos == MN_SPI_DIR_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_SPI_DIR_TYPE);
					g_ad.eSpiNum = (eAdComDir_Type)rxdat[1];
					g_ad.eComDir = (eAdSpi_Type)rxdat[2];
				}
				else if(eMnPos == MN_RTC_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_RTC_TYPE);
					g_sMn.mn_set_rtc[6] = rxdat[1];	//year
					g_sMn.mn_set_rtc[5] = rxdat[2];	//month
					g_sMn.mn_set_rtc[4] = rxdat[3];	//day
					g_sMn.mn_set_rtc[3] = 1U;		//week
					g_sMn.mn_set_rtc[2] = rxdat[4];	//hour
					g_sMn.mn_set_rtc[1] = rxdat[5];	//min
					g_sMn.mn_set_rtc[0] = rxdat[6];	//sec
				}
				else if(eMnPos == MN_RS485_BT_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_RS485_BT_TYPE);
					if(rxdat[1] == (u8)BTMS_CMD_START){
						if(rxdat[2] <= 5){
							BTMS_485Tx_Data((eBTMS_CmdType)rxdat[1], rxdat[2]);
						}rxdat[3] = 0;
					}else if(rxdat[1] == (u8)BTMS_CMD_TARGET_TEMP){
						if((rxdat[3] <= 50) && (rxdat[3] >= 0)){
							BTMS_485Tx_Data((eBTMS_CmdType)rxdat[1], rxdat[3]);
						}rxdat[2] = 0;
					}else{
						BTMS_485Tx_Data((eBTMS_CmdType)rxdat[1], 0);
					}
					
				}
				else if(eMnPos == MN_PACKV_TYPE)
				{
					g_sMn.mn_type |= ((u16)1<<(u16)MN_PACKV_TYPE);
					g_sMn.mn_set_pv_in	= (u16)(((u16)rxdat[2] << 8u) | (u16)rxdat[1]); 
					g_sMn.mn_set_pv_ou	= (u16)(((u16)rxdat[4] << 8u) | (u16)rxdat[3]);
					g_sMn.mn_set_pv_bt	= (u16)(((u16)rxdat[6] << 8u) | (u16)rxdat[5]);					
				}
				else if(eMnPos == MN_CALIBRATION_TYPE)
				{
					g_sMn.bset_cal = TRUE;
					g_sMn.mn_type |= ((u16)1<<(u16)MN_CALIBRATION_TYPE);
					g_sMn.mn_calpos = rxdat[1];
					g_sMn.mn_ref_data = (u16)((u16)(rxdat[3] << 8) | (u16)rxdat[2] );
				}
			}
		}
	}
#endif
}


#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
    Can_TxProc(McuCan_Int_Type);
}
#endif //CAN_BUF_UPATE


void Can_TxFifo(Enum_Can_Type eCanType, u32 u32CanId, u8 u8CanData[], Enum_McuCan_Type eMcuCan)
{
	CAN_TxHeaderTypeDef TxHeader;
	TxHeader.RTR = CAN_RTR_DATA;
/*
	if(eCanType == Can_Standard_Type){
		TxHeader.IDE = CAN_ID_STD;
		TxHeader.StdId = u32CanId;
	}else{
		TxHeader.IDE = CAN_ID_EXT;
		TxHeader.ExtId = u32CanId;
	}*/
	TxHeader.IDE = CAN_ID_EXT;
	TxHeader.ExtId = u32CanId;
	TxHeader.DLC = 8;
	TxHeader.TransmitGlobalTime = DISABLE;

	Can_Fifo_In(&TxHeader, u8CanData, eMcuCan);                                         																//KKD 2025-11-26 분류된 데이터 Fifo 저장
}

void Can_TxProc(Enum_McuCan_Type eMcuCan)
{
#ifdef CAN_BUF_UPATE    																//2025-11-27   jkpark  Can Buffer 관련 수정.
    CAN_TxHeaderTypeDef TxHeader;
    u32 txMailBox = 0;
    u8 u8CanData[8] = {0};
    BOOL bBeforeTxOk = FALSE;

    CAN_HandleTypeDef* phCan = NULL;
    phCan = &hcan1;
   													
    txMailBox = HAL_CAN_GetTxMailboxesFreeLevel(phCan);									//jkpark 2025-11-27 송신 가능한지 확인.
    if(txMailBox == 0) { return; }

    bBeforeTxOk = Can_Fifo_Out(&TxHeader, u8CanData, eMcuCan);							//jkpark 2025-11-27 송신데이터가 있는지 확인.
    if(bBeforeTxOk == FALSE) { return; }

    HAL_CAN_AddTxMessage(phCan, &TxHeader, u8CanData, (uint32_t *)&txMailBox);                      //KKD 2025-12-01 TX

#else  //CAN_BUF_UPATE
    CAN_TxHeaderTypeDef TxHeader;
//    CAN_TxHeaderTypeDef BeforeTxHeader;
    u32 txMailBox = 0;
    u32 u32Remain = 0;
    u8 u8CanData[8] = {0};
    BOOL bBeforeTxOk = FALSE;

    CAN_HandleTypeDef* phCan = NULL;
    phCan = &hcan1;
    
    txMailBox = HAL_CAN_GetTxMailboxesFreeLevel(phCan);
    if(txMailBox == (u32)0) { return; }

    bBeforeTxOk = Can_Fifo_TxCheck_GetTxDone(eMcuCan);                                  //jkpark 2025-11-27 ???
	if(bBeforeTxOk == FALSE) { return; }
	// KKD 2025-11-26 Last Message is Sended
	// KKD 2025-11-26 So
	// KKD 2025-11-26 이전 송신 내용 확인.


	u32Remain = Can_Fifo_GetRemainCount(eMcuCan);
	if(u32Remain == (u32)0) { return; }

	// KKD 2025-11-26 Message to Send is Exist
	// KKD 2025-11-26 So	
	Can_Fifo_Out(&TxHeader, u8CanData, eMcuCan);

  // KKD 2025-11-26 송신 확인 버퍼에 넣기.
  Can_Fifo_TxCheck_In(&TxHeader, u8CanData, eMcuCan);                                 	//jkpark 2025-11-27 ???
    
  // KKD 2025-11-26 송신
  Can_Fifo_TxCheck_SetTxReady(eMcuCan);                                               	//jkpark 2025-11-27 ???
  HAL_CAN_AddTxMessage(phCan, &TxHeader, u8CanData, (uint32_t *)&txMailBox);
#endif //CAN_BUF_UPATE
}

#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.
/*********************************************************************************/
//3   Function Name  : Can_RxProc
/*----------------------------------------------------------------------------*//**
\fn        Can_RxProc
\brief     Main Process 에서 호출. Parsing이 성공하거나 데이터가 소진될때까지 읽어서 처리.
\parm      deviceID : Parsing이 완료된 deviceID.
\return    Parsing 성공 여부.
\warning   인터럽트 루틴 아님.
*//*******************************************************************************/
msStatus_t Can_RxParser( u32 *deviceID )
{
    sCanRxData  readData;
    int         readSize = 0;
    u32    readId = 0;
    msStatus_t  retValue = MS_FALSE;
    
    //jkpark 2025-12-02 버퍼에서 데이터를 읽어옴.
    while( Util_Buffer_Read( &g_stCanRxData.bufInfo, (unsigned char *)&readData, sizeof( sCanRxData ), &readSize) == MS_SUCCESS )
    {
        //jkpark 2025-12-02 IDE값에 따라 유효한 ID가 다름.
        if( readData.RxHeader.IDE == CAN_ID_STD )
        {
            //jkpark 2025-12-02 IDE가 CAN_ID_STD이면 StdID가 유효.
            readId = readData.RxHeader.StdId & 0x000007FF;
        }

        if( readData.RxHeader.IDE == CAN_ID_EXT )
        {
            //jkpark 2025-12-02 IDE가 CAN_ID_EXT이면 ExtId가 유효.
            readId = readData.RxHeader.ExtId & 0x1FFFFFFF;
        }else{
            //jkpark 2025-12-02 IDE가 유효하지 않으면 데이터 무시.
            continue;
        }

        switch( readId )
        {
            case HALL_CAN_ID:
                retValue = HALL_RxParsing( readId, readData.u8Data );
                break;

            case GAS_CAN_ID:
                retValue = GAS_RxParsing( readId, readData.u8Data );
                break;
            
            case IMD_CAN_ID:
                retValue = IMD_RxParsing( readId, readData.u8Data );
                break;

/*            
            case DEVICE_ID1:
                //jkpark 2025-12-02 읽은 데이터를 Device Parser에 넘겨주고 Parsing결과를 받는다. 해당데이터는 Device루틴에서 저장해야함.
                retValue = device1_Parser( &readData );
                break;
            case DEVICE_ID2:
                retValue = device2_Parser( &readData );
                break;
*/
            default:
                break;
        }
        
        if( retValue == MS_SUCCESS )                                                    //jkpark 2025-12-02 Parsing이 성공하면 파싱 중단.
        {
            *deviceID = readId;
            break;
        }
    }
    return retValue;
}
#endif //CAN_BUF_UPATE


/* ─────────────────────────────────────────────────────────────────────────────
 * CAN_Tx_GainValues
 *   캘리브레이션 완료 후 현재 게인값 7개를 Standard ID 0x701 로 3프레임 송신.
 *
 *   Frame 1  MUX=0x01 : gain_pv_in(2B) | gain_pv_ou(2B) | gain_pv_bt(2B) | 0x00
 *   Frame 2  MUX=0x02 : gain_ax_24v(2B)| gain_ax_13v(2B)| gain_ax_5v(2B) | 0x00
 *   Frame 3  MUX=0x03 : gain_ax_3v(2B) | 0x00 0x00 0x00 0x00 0x00
 *
 *   Can_TxFifo() 는 내부에서 강제로 Extended ID 를 설정하므로,
 *   Standard ID 를 사용하기 위해 Can_Fifo_In() 을 직접 호출한다.
 * ───────────────────────────────────────────────────────────────────────────*/
void CAN_Tx_GainValues(void)
{
	CAN_TxHeaderTypeDef TxHeader;
	u8 txdat[CAN_DATA_SIZE];

	/* Standard ID 0x701 헤더 공통 설정 */
	TxHeader.IDE                = CAN_ID_STD;
	TxHeader.RTR                = CAN_RTR_DATA;
	TxHeader.StdId              = 0x701u;
	TxHeader.ExtId              = 0u;
	TxHeader.DLC                = 8u;
	TxHeader.TransmitGlobalTime = DISABLE;

	/* ── Frame 1: MUX=0x01 — pv_in, pv_ou, pv_bt ── */
	txdat[0] = 0x01u;
	txdat[1] = (u8)( g_sAdcGain.gain_pv_in        & 0xFFu);
	txdat[2] = (u8)((g_sAdcGain.gain_pv_in  >> 8u) & 0xFFu);
	txdat[3] = (u8)( g_sAdcGain.gain_pv_ou        & 0xFFu);
	txdat[4] = (u8)((g_sAdcGain.gain_pv_ou  >> 8u) & 0xFFu);
	txdat[5] = (u8)( g_sAdcGain.gain_pv_bt        & 0xFFu);
	txdat[6] = (u8)((g_sAdcGain.gain_pv_bt  >> 8u) & 0xFFu);
	txdat[7] = 0x00u;
	Can_Fifo_In(&TxHeader, txdat, McuCan_Ext_Type);

	/* ── Frame 2: MUX=0x02 — ax_24v, ax_13v, ax_5v ── */
	txdat[0] = 0x02u;
	txdat[1] = (u8)( g_sAdcGain.gain_ax_24v        & 0xFFu);
	txdat[2] = (u8)((g_sAdcGain.gain_ax_24v >> 8u) & 0xFFu);
	txdat[3] = (u8)( g_sAdcGain.gain_ax_13v        & 0xFFu);
	txdat[4] = (u8)((g_sAdcGain.gain_ax_13v >> 8u) & 0xFFu);
	txdat[5] = (u8)( g_sAdcGain.gain_ax_5v         & 0xFFu);
	txdat[6] = (u8)((g_sAdcGain.gain_ax_5v  >> 8u) & 0xFFu);
	txdat[7] = 0x00u;
	Can_Fifo_In(&TxHeader, txdat, McuCan_Ext_Type);

	/* ── Frame 3: MUX=0x03 — ax_3v ── */
	txdat[0] = 0x03u;
	txdat[1] = (u8)( g_sAdcGain.gain_ax_3v         & 0xFFu);
	txdat[2] = (u8)((g_sAdcGain.gain_ax_3v  >> 8u) & 0xFFu);
	txdat[3] = 0x00u;
	txdat[4] = 0x00u;
	txdat[5] = 0x00u;
	txdat[6] = 0x00u;
	txdat[7] = 0x00u;
	Can_Fifo_In(&TxHeader, txdat, McuCan_Ext_Type);
}
