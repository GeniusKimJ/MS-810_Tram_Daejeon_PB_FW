/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(battery thermal management)                                         *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_btms.h
\author             KKD
\date               2025-11-24 
\brief              battery thermal management
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_BTMS_H
#define MS_BTMS_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"

/* Exported define ---------------------------------------------------------------------------------*/
#define BTMS_RX_MAX         		(200)
#define BTMS_TX_MAX         		(200)

#define BTMS_LEN_IDX        		(1)
#define BTMS_DATA_IDX       		(2)

#define BTMS_STX_LEN        		(1)
#define BTMS_LEN_LEN        		(1)
#define BTMS_CRC_LEN        		(2)
#define BTMS_ETX_LEN        		(1)
//#define BTMS_MIN_LEN        		( BTMS_STX_LEN + MVB_LEN_LEN + BTMS_CRC_LEN + BTMS_ETX_LEN )
/* BUG-5 수정: MVB_LEN_LEN → BTMS_LEN_LEN */
#define BTMS_MIN_LEN    ( BTMS_STX_LEN + BTMS_LEN_LEN + BTMS_CRC_LEN + BTMS_ETX_LEN )

#define BTMS_RX_DATA_MAX    		(250)

#define BTMS_STARTn_IDX         	(5)
#define BTMS_TARGET_TEMPn_IDX   	(11)
#define BTMS_STATU_IDX   			(11)

#define BTMS_TIMEOUT_MS				(2000)

// CRC-16/IBM-3740 parameters
#define CRC16_IBM3740_POLY   		(0x1021U)
#define CRC16_IBM3740_INIT   		(0xFFFFU)
#define CRC16_IBM3740_XOROUT 		(0x0000U)



#define BTMS_ERR_BLOW_OPEN          (u8)(01)
#define BTMS_ERR_BLOW_SHORT         (u8)(02)
#define BTMS_ERR_COND_OPEN          (u8)(03)
#define BTMS_ERR_COND_SHORT         (u8)(04)
#define BTMS_ERR_RT_OPEN            (u8)(05)
#define BTMS_ERR_RT_SHORT           (u8)(06)
#define BTMS_ERR_ST_OPEN            (u8)(07)
#define BTMS_ERR_SP_SHORT           (u8)(08)
#define BTMS_ERR_PTC_HIGH_TEMP      (u8)(09)
#define BTMS_ERR_PTC_IGBT           (u8)(10)
#define BTMS_ERR_PTC_RELAY          (u8)(11)
#define BTMS_ERR_PTC_CURRENT        (u8)(12)
#define BTMS_ERR_HIGH_PRESS         (u8)(13)
#define BTMS_ERR_LOW_PRESS          (u8)(14)
#define BTMS_ERR_DEFROST            (u8)(15)
#define BTMS_ERR_BTMS_LOW_VOLTAGE   (u8)(16)
#define BTMS_ERR_BTMS_HIGH_VOLTAGE  (u8)(17)
#define BTMS_ERR_COMP_ERROR         (u8)(18)

#define BTMS_ERR_CODE_MAX           (u8)(18)

#define BTMS_START_MODE_STOP        (0x00)
#define BTMS_START_MODE_AUTO        (0x01)
#define BTMS_START_MODE_SEMI        (0x02)
#define BTMS_START_MODE_VENT        (0x03)
#define BTMS_START_MODE_COOL        (0x04)
#define BTMS_START_MODE_HOT         (0x05)
#define BTMS_START_MODE_ERR         (0x06)

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef enum{
    BTMS_PACKET_STX = 0,
    BTMS_PACKET_LEN,
    BTMS_PACKET_DATA,
    BTMS_PACKET_CRC,
    BTMS_PACKET_ETX,    
    BTMS_PACKET_MAX
}eBTMS_PacketType;

typedef enum{
    BTMS_CMD_START =0,
    BTMS_CMD_TARGET_TEMP,
    BTMS_CMD_STATUS,
    BTMS_CMD_STATUS_TEMP,
    BTMS_CMD_STATUS_HUMIDITY,
    BTMS_CMD_MAX
}eBTMS_CmdType;

#pragma pack(push,1)
typedef struct 
{
    u8 u8RxDmaBuf[BTMS_RX_MAX];
    u8 u8RxDataFrameBuf[BTMS_RX_MAX];
    volatile BOOL bRxOk;
    u16 u16PrevPos;

    eBTMS_PacketType ePacket;
    eBTMS_CmdType    eState;
    u16 u16DataPos;
    u16 u16DataLen;
    u16 u16StateLen;

    BOOL bCommTimeout;

} sBTMS_DataType;

typedef struct
{
    eBTMS_CmdType eLastCmd;
    u8 	u8StartMode;
    u8  u8TargetTemp;
    s16	s16ReturnTemp;
    s16	s16SupplyTemp;
    u8  u8Humidity;
    u32	u32ErrFlags;            // bit0~bit17 → err01~err18
    u8  u8ErrCount;
    BOOL bStatusNormal;
    BOOL bDataValid;
} sBTMS_ParsedDataType;
#pragma pack(pop)

/* Exported variables ------------------------------------------------------------------------------*/
extern sBTMS_DataType g_sBTMS_Data;
extern sBTMS_ParsedDataType g_sBTMS_Parsed;
extern u8 u8TxBuf[BTMS_TX_MAX];

/* Exported function prototypes --------------------------------------------------------------------*/
void	BTMS_Init(void);
void	BTMS_485Rx_Timeout(sBTMS_DataType *psBtmsData);
void	BTMS_Proc(void);
void	BTMS_Receive_DMA_Poll(void);
void	BTMS_RxData_ReParser(void);

void	BTMS_RxFrameParser(u8 readByte);
//u16		crc16_ibm3740(const u8 *data, u32 len);
void	BTMS_CmdProc(const u8 *data, u32 len);
u8 		BTMS_GetTimeoutStatus(void);

void	BTMS_ParseRxData(void);
u32		BTMS_GetErrFlags(void);
u8		BTMS_GetErrCount(void);
BOOL	BTMS_IsStatusNormal(void);
BOOL	BTMS_IsErrActive(u8 errCode);
s16		BTMS_GetReturnTemp(void);
s16		BTMS_GetSupplyTemp(void);
u8		BTMS_GetHumidity(void);
u8		BTMS_GetStartMode(void);
u8		BTMS_GetTargetTemp(void);
const 	sBTMS_ParsedDataType* BTMS_GetParsedData(void);
#endif //__MS_BTMS_H__
