/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_can.h
\author             KKD
\date               2025-11-21 
\brief              CAN을 위한 헤더.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_CAN_H
#define MS_CAN_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"
#include "ms_Util.h"

/* Exported define ---------------------------------------------------------------------------------*/
#define FIFO_CAN_MAX				(500)
#define FIFO_CAN_TX_FAIL_LOG_MAX	(500)
#define MANUAL_TEST					(0x700)
#define CAN_DATA_SIZE               8
#define EN_BOOT
#define EXT				0x40U
#define STD				0x00U

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef enum{
	Can_Standard_Type				= 0,
	Can_Extended_Type,
	Max_Can_type
}Enum_Can_Type;

typedef enum{
	McuCan_Ext_Type					= 0,
	McuCan_Int_Type,
	Max_McuCan_Type
}Enum_McuCan_Type;	

typedef enum{
	SenIC_HALL_Type					= 0,
	SenIC_GAS_Type,
	SenIC_IMD_Type,
	Max_SenIC_Type
}Enum_Sencing_IC_Type;
	
typedef enum{
	MN_STANDBY_TYPE					= 0,
	MN_CELL_TYPE,					
	MN_TEMP_TYPE,
	MN_CURR_TYPE,
	MN_CAL_TYPE,
	MN_RLY_TYPE,
	MN_DET_TYPE,
	MN_BAL_TYPE,
	MN_SPI_DIR_TYPE,
	MN_RTC_TYPE,
	MN_RS485_BT_TYPE				= 10,
	MN_PACKV_TYPE,
	MN_CALIBRATION_TYPE,
	MAX_MN_TYPE
}Enum_MenaulTest;

typedef enum{
	MN_TOG_SPI_Type					= 0,
	MN_TOG_CS_Type,
	MAX_TOG_Type
}Enum_MnDIr_Type;

typedef enum{
	CAL_CURR_CAB_TYPE					= 0,
	CAL_CURR_S124_TYPE,
	CAL_PV_IN_TYPE,
	CAL_PV_OU_TYPE,
	CAL_PV_BT_TYPE,
	CAL_LowV_24_TYPE,
	CAL_LowV_13_TYPE,
	CAL_LowV_5_TYPE,
	CAL_LowV_3_TYPE,
	MAX_CAL_TYPE
}Eunm_MnCal_Type;
#pragma pack(1)
typedef struct{
    CAN_TxHeaderTypeDef TxHeader;
	u8 u8Data[CAN_DATA_SIZE];
}sCanData;

typedef struct{
	sCanData stCanData[FIFO_CAN_MAX];
#ifdef CAN_BUF_UPATE    //2025-11-27   jkpark  Can Buffer 관련 수정.
	BufferInfoType bufInfo;
#else
	u32 u32InputPos;
	u32 u32OutPos;
	u32 u32Remain;

	u32 u32MaxRemain;
#endif //CAN_BUF_UPATE
    
	u32 u32RemainZeroCount;

	BOOL bTxDone;
	u8 u8CanTxOkFalseCount;

	u32 u32AbortCount[3];
	u32 u32TxErrorCount;
}sFifo;

#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.
typedef struct{
    CAN_RxHeaderTypeDef RxHeader;
	u8 u8Data[CAN_DATA_SIZE];
}sCanRxData;

typedef struct{
	sCanRxData stCanRxData[FIFO_CAN_MAX];
    BufferInfoType bufInfo;
}sRxFifoType;
#endif //CAN_BUF_UPATE


typedef struct{
    CAN_TxHeaderTypeDef TxFaultLog_Header[FIFO_CAN_TX_FAIL_LOG_MAX];
	u32 TxFaultLog_TimeMs[FIFO_CAN_TX_FAIL_LOG_MAX];
	u32 u32StartPos;
	u32 u32EndPos;
	u32 u32Count;
}sCanTxFailLog;

#ifdef EN_BOOT
typedef struct __attribute__((__packed__)) {
   	uint32_t	canid;
	uint16_t	ide;
	uint8_t	dlc;
	uint8_t	rtr;
    uint8_t	cantxd[8];
} Pkt_CanTx_CAN;


typedef struct __attribute__((__packed__)) {
   	uint32_t	canid;
	uint16_t	ide;
	uint8_t	dlc;
	uint8_t	rtr;
    uint8_t	canrxd[8];
} Pkt_CanRx_CAN;
#endif


#pragma pack(push,1)
typedef struct{
	

}sExtCalibration;


#pragma pack(pop)



/* Exported variables ------------------------------------------------------------------------------*/


extern	u8		g_u8SenIcRxData[Max_SenIC_Type][8];
extern	sCanData g_stLastTxCanData[Max_McuCan_Type];
extern	BOOL	g_bSenIcRxDone[Max_SenIC_Type];
extern	BOOL 	g_bCanTxDone[Max_McuCan_Type];

#ifdef EN_BOOT
//extern u8	fRcvCan1Boot;
extern u8 	Total_Judge;

#endif



/* Exported function prototypes --------------------------------------------------------------------*/


//fifo
void	Can_Fifo_In(CAN_TxHeaderTypeDef* pTxHeader, u8 u8CanData[], Enum_McuCan_Type eMcuCan);
BOOL	Can_Fifo_Out(CAN_TxHeaderTypeDef* pTxHeader, u8 u8CanData[], Enum_McuCan_Type eMcuCan);

u32 	Can_Fifo_GetRemainCount(Enum_McuCan_Type eMcuCan);
u32 	Can_Fifo_GetMaxRemainCount(Enum_McuCan_Type eMcuCan);
u32 	Can_Fifo_GetRemainZeroCount(Enum_McuCan_Type eMcuCan);

void 	Can_Fifo_TxCheck_In(CAN_TxHeaderTypeDef* pTxHeader, u8 u8CanData[], Enum_McuCan_Type eMcuCan);

BOOL	 Can_Fifo_TxCheck_TxOkFalseCount(Enum_McuCan_Type eMcuCan);

void 	Can_Fifo_TxCheck_SetTxDone(Enum_McuCan_Type eMcuCan);
BOOL 	Can_Fifo_TxCheck_GetTxDone(Enum_McuCan_Type eMcuCan);
void 	Can_Fifo_TxCheck_SetTxReady(Enum_McuCan_Type eMcuCan);

BOOL 	Can_Fifo_TxCheck_GetLastTx(sCanData* pstLastData, Enum_McuCan_Type eMcuCan);

void 	Can_Fifo_WriteLog_TxFail(CAN_TxHeaderTypeDef* BeforeTxHeader, sCanTxFailLog* pstCanTxFailLog);
u32 	Can_Fifo_Get_MaxFifoCount(Enum_McuCan_Type eMcuCan);
void 	Can_Fifo_TxCheck_CheckAbortMsg(u8 u8TxMailBoxPos, sCanData* pstLastTxCanData, Enum_McuCan_Type eMcuCan);
void 	Can_Fifo_TxCheck_CheckErrorMsg(sCanData* pstLastTxCanData, Enum_McuCan_Type eMcuCan);
u32 	Can_Fifo_TxCheck_GetErrorCount(Enum_McuCan_Type eMcuCan);
//. . .  fifo


//can
void 	Can_Init(void);

void 	Can_MakeConfigFilter(void);
void 	Can_SetupInterrupt(void);

void 	Can_TxFifo(Enum_Can_Type eCanType, u32 u32CanId, u8 u8CanData[], Enum_McuCan_Type eMcuCan);
void 	Can_TxProc(Enum_McuCan_Type eMcuCan);

/* CAN 100ms 주기 자동 호출 (SM Delay 내부에서도 사용) ----------------------*/
void 	Can_Independent(void);
void 	Can_Independent_Init(void);

/* 캘리브레이션 완료 후 게인값 송신 (Standard ID 0x701, 3프레임) */
void	CAN_Tx_GainValues(void);

//. . . can
#endif // __MS_CAN_H__
