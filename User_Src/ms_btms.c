/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(BTMS RS485i)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2026-04-08                                                                *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_btms.c
\author             KKD
\date               2026-04-08 
\brief              BTMS를 위한 코드.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "ms_btms.h"


#define STX 0x02
#define ETX 0x03

/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
typedef enum {
    CMD_START           = 0,
    CMD_TARGET_TEMP,
    CMD_STATUS,
    CMD_TEMP_STATUS,
    CMD_HUMIDITY_STATUS,
    MAX_CMD
} eCmdType;

typedef enum {
	OP_STOP				= 0,
	OP_STA_AUTO,
	OP_STA_SEMI,
	OP_VENT,
	OP_COOL,
	OP_HOT,
	OP_OTHER,
	MAX_OP
} eStart_OpSts;

/* Private variables ------------------------------------------------------------------------------*/
sBTMS_DataType      g_sBTMS_Data;
sBTMS_ParsedDataType g_sBTMS_Parsed;
u8  u8TxBuf[BTMS_TX_MAX];
static u32 g_su32BtmsLastTime;

/* Private function prototypes --------------------------------------------------------------------*/
static u16  crc16_ibm3740(const u8 *data, u32 len);
static u8   BTMS_Ascii2Dec(const u8 *pAscii);
static s16  BTMS_AsciiStrToNum(const u8 *pBuf, u16 maxLen);
static void BTMS_Dec2Ascii(u8 val, u8 *pAscii);
static void BTMS_Start(void);
static void BTMS_Status(void);				
static void BTMS_Status_Temp(void);	
static void BTMS_Status_Humi(void);	


/* Private functions ------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------
 * BTMS_Init
 *------------------------------------------------------------------------------------------------*/
void BTMS_Init(void)
{
    memset(&g_sBTMS_Data, 0x00, sizeof(sBTMS_DataType));
    g_su32BtmsLastTime = 0u;
    GPIO_DOut_EN485(FALSE);   /* Receive mode */
    HAL_UART_Receive_DMA(&huart3,
                         g_sBTMS_Data.u8RxDmaBuf,
                         (u16)BTMS_RX_MAX);
}

/*-------------------------------------------------------------------------------------------------
 * BTMS_485Rx_Timeout
 *------------------------------------------------------------------------------------------------*/
void BTMS_485Rx_Timeout(sBTMS_DataType *psBtmsData)
{
    u32 now = HAL_GetTick();
    if ((now - g_su32BtmsLastTime) >= (u32)BTMS_TIMEOUT_MS) {
        psBtmsData->bCommTimeout = TRUE;
        g_su32BtmsLastTime       = now;
    }
}

/*-------------------------------------------------------------------------------------------------
 * BTMS_Make_TxData  (가변 길이 패킷 빌더)
 *  패킷 구조:  [STX(1)] [LEN(1)] [ASCII_CMD(N)] [PARAM(0or1)] [CRC_H(1)] [CRC_L(1)] [ETX(1)]
 *  LEN = (데이터 바이트 수) + CRC(2)
 *  CRC 범위: LEN 필드 포함, ETX 이전까지
 *------------------------------------------------------------------------------------------------*/
u16 BTMS_Make_TxData(eCmdType cmdType, s16 param, u8 *txbuffer)
{
    const char *cmdStr  = "";
    u8          hasParam = 0u;
	u8			target_t = 0u;
    u8          strLen;
    u8          payloadLen;
    u16         idx = 0u;
    u16         crc;
 	u8			asciiTbuf[2];

    switch (cmdType) {
        case CMD_START:
            cmdStr   = "START";
            hasParam = 1u;
            break;
        case CMD_TARGET_TEMP:
			BTMS_Dec2Ascii(param,asciiTbuf);
            cmdStr   = "Target_Temp";
            hasParam = 1u;
			target_t = 1u;
            break;
        case CMD_STATUS:
            cmdStr   = "Status";
            hasParam = 0u;
            break;
        case CMD_TEMP_STATUS:
            cmdStr   = "Temp_Status";
            hasParam = 0u;
            break;
        case CMD_HUMIDITY_STATUS:
            cmdStr   = "Humidity_Status";
            hasParam = 0u;
            break;
        default:
            return 0u;
    }

    strLen     = (u8)strlen(cmdStr);
    payloadLen = strLen + (hasParam ? 1u : 0u) + (target_t ? 1u : 0u);  // Real Data Len
	


    /* ── 패킷 조립 ── */
    txbuffer[idx++] = (u8)STX;
    txbuffer[idx++] = (u8)(payloadLen + (u8)BTMS_CRC_LEN); // LEN = Data Len 'n'+ CRC Len(2) 

    memcpy(&txbuffer[idx], cmdStr, (size_t)strLen);
    idx += strLen;

	if(hasParam != 0u){
	    if (target_t == 0u){
	        txbuffer[idx++] = (u8)param;
	    }else{
	    	txbuffer[idx++] = asciiTbuf[0];
			txbuffer[idx++] = asciiTbuf[1];
	    }
	}

    // CRC 범위: [LEN 필드(index=1)] ~ [데이터 끝] = 1 + payloadLen 바이트 
    crc = crc16_ibm3740(&txbuffer[1u], (u32)(1u + payloadLen));

    txbuffer[idx++] = (u8)((crc >> 8u) & 0xFFu);  		// CRC High 
    txbuffer[idx++] = (u8)(crc & 0xFFu);           		// CRC Low 
    txbuffer[idx++] = (u8)ETX;

    return idx;
}

/*-------------------------------------------------------------------------------------------------
 * BTMS_485Tx_Data
 *------------------------------------------------------------------------------------------------*/
BOOL BTMS_485Tx_Data(eBTMS_CmdType cmd, u8 value)
{
    u8  txbuf[32];
    u16 total_len = 0u;
    BOOL result   = FALSE;
	memset(txbuf, 0x00u, sizeof(txbuf));


	
    switch (cmd) {
        case BTMS_CMD_START:
            total_len = BTMS_Make_TxData(CMD_START,           (s16)value, txbuf);
            break;
        case BTMS_CMD_TARGET_TEMP:
            total_len = BTMS_Make_TxData(CMD_TARGET_TEMP,     (s16)value, txbuf);
            break;
        case BTMS_CMD_STATUS:
            total_len = BTMS_Make_TxData(CMD_STATUS,          0,          txbuf);
            break;
        case BTMS_CMD_STATUS_TEMP:
            total_len = BTMS_Make_TxData(CMD_TEMP_STATUS,     0,          txbuf);
            break;
        case BTMS_CMD_STATUS_HUMIDITY:
            total_len = BTMS_Make_TxData(CMD_HUMIDITY_STATUS, 0,          txbuf);
            break;
        default:
            break;
    }

    if (total_len > 0u){
        result = TRUE;
        GPIO_DOut_EN485(TRUE);
        HAL_UART_Transmit(&huart3, txbuf, total_len, 20u);
        GPIO_DOut_EN485(FALSE);
    }

    return result;
}

/*-------------------------------------------------------------------------------------------------
 * BTMS_Proc
 *------------------------------------------------------------------------------------------------*/
void BTMS_Start(void)
{
	s16 tmax = Proc_Get_ThMax();
	s16 tmin = Proc_Get_ThMin();
	s16 tagv = Proc_Get_ThAvg();

	//tx
	if((tmax > (s16)400) || (tagv > (s16)350)){
		BTMS_485Tx_Data(BTMS_CMD_START, (u8)OP_STA_AUTO);
	}else if((tmax < (s16)200) || (tagv < (s16)250)){
		BTMS_485Tx_Data(BTMS_CMD_START, (u8)OP_STOP);
	}else{

	}
}

void BTMS_Status(void)					{	BTMS_485Tx_Data(BTMS_CMD_STATUS, NULL);}
void BTMS_Status_Temp(void)				{	BTMS_485Tx_Data(BTMS_CMD_STATUS_TEMP, NULL);}
void BTMS_Status_Humi(void)				{	BTMS_485Tx_Data(BTMS_CMD_STATUS_HUMIDITY, NULL);}

void BTMS_Proc(void)
{
#if 1
	static eBTMS_CmdType ebcmd = BTMS_CMD_START;

	//tx
	if(ebcmd == BTMS_CMD_START){
		BTMS_Start();		
		ebcmd = BTMS_CMD_STATUS;
	}else if(ebcmd == BTMS_CMD_STATUS){
		BTMS_Status();		
		ebcmd = BTMS_CMD_STATUS_TEMP;
	}else if(ebcmd == BTMS_CMD_STATUS_TEMP){
		BTMS_Status_Temp(); 
		ebcmd = BTMS_CMD_STATUS_HUMIDITY;
	}else if(ebcmd == BTMS_CMD_STATUS_HUMIDITY){
		BTMS_Status_Temp(); 
		ebcmd = BTMS_CMD_START;
	}
#endif
		
	//rx
    BTMS_Receive_DMA_Poll();

    if (g_sBTMS_Data.bRxOk == TRUE){
		BTMS_ParseRxData();
    }
}


void BTMS_Receive_DMA_Poll(void)
{
    u16 currPos;

    // DMA 휴지 상태 확인.
   	// DMA Ready 상태 진입 및 init
    if (huart3.hdmarx->State != HAL_DMA_STATE_BUSY)
    {
        g_sBTMS_Data.u16PrevPos = 0u;
        g_sBTMS_Data.bRxOk      = FALSE;
        memset(g_sBTMS_Data.u8RxDmaBuf, 0x00u, (size_t)BTMS_RX_MAX);
        HAL_UART_Receive_DMA(&huart3,
                             g_sBTMS_Data.u8RxDmaBuf,
                             (u16)BTMS_RX_MAX);
        return; 
    }

    // 2026-04-08 KKD 현재 DMA 쓰기 위치 = 버퍼 크기 - 남은 카운터
    currPos = (u16)BTMS_RX_MAX -
              (u16)__HAL_DMA_GET_COUNTER(huart3.hdmarx);

    while (g_sBTMS_Data.u16PrevPos != currPos)
    {
        BTMS_RxFrameParser(g_sBTMS_Data.u8RxDmaBuf[g_sBTMS_Data.u16PrevPos]);
        g_su32BtmsLastTime = HAL_GetTick();
        g_sBTMS_Data.u16PrevPos++;
        if (g_sBTMS_Data.u16PrevPos >= (u16)BTMS_RX_MAX)
        {
            g_sBTMS_Data.u16PrevPos = 0u;  // circular wrap
        }
    }
}

/*-------------------------------------------------------------------------------------------------
 * BTMS_RxData_ReParser parser init
 *------------------------------------------------------------------------------------------------*/
void BTMS_RxData_ReParser(void)
{
    g_sBTMS_Data.ePacket    = BTMS_PACKET_STX;
    g_sBTMS_Data.u16DataPos = 0u;
    g_sBTMS_Data.u16StateLen = 0u;
}

void BTMS_RxFrameParser(u8 readByte)
{
    switch (g_sBTMS_Data.ePacket)
    {
        /* ── STX ── */
        case BTMS_PACKET_STX:
            if (readByte == (u8)STX){
                g_sBTMS_Data.u16DataPos  = 0u;
                g_sBTMS_Data.u16StateLen = 0u;
                g_sBTMS_Data.u8RxDataFrameBuf[g_sBTMS_Data.u16DataPos++] = readByte;
                g_sBTMS_Data.ePacket = BTMS_PACKET_LEN;
            }break;

        /* ── LEN ── */
        case BTMS_PACKET_LEN:
            if (readByte <= (u8)BTMS_RX_DATA_MAX){
                g_sBTMS_Data.u8RxDataFrameBuf[g_sBTMS_Data.u16DataPos++] = readByte;
                g_sBTMS_Data.u16DataLen  = (u16)readByte;
                g_sBTMS_Data.u16StateLen = 0u;

                if (g_sBTMS_Data.u16DataLen <= (u16)BTMS_CRC_LEN) {
                    g_sBTMS_Data.ePacket = BTMS_PACKET_CRC; 	// 데이터 없이 CRC만 있는 최소 패킷
                }else{
                    g_sBTMS_Data.ePacket = BTMS_PACKET_DATA;
                }
            }else{
                BTMS_RxData_ReParser();
            }break;

        /* ── DATA ── */
        case BTMS_PACKET_DATA:
        {
            /* 데이터 바이트 수 = DataLen - CRC_LEN */
            u16 dataOnlyLen = g_sBTMS_Data.u16DataLen - (u16)BTMS_CRC_LEN;

            g_sBTMS_Data.u8RxDataFrameBuf[g_sBTMS_Data.u16DataPos++] = readByte;
            g_sBTMS_Data.u16StateLen++;

            if (g_sBTMS_Data.u16StateLen >= dataOnlyLen){
                g_sBTMS_Data.u16StateLen = 0u;
                g_sBTMS_Data.ePacket     = BTMS_PACKET_CRC;
            }
        }break;

        /* ── CRC ── */
        case BTMS_PACKET_CRC:
        {
            g_sBTMS_Data.u8RxDataFrameBuf[g_sBTMS_Data.u16DataPos++] = readByte;
            g_sBTMS_Data.u16StateLen++;

            if (g_sBTMS_Data.u16StateLen == (u16)BTMS_CRC_LEN){
                u16 crcPosH = g_sBTMS_Data.u16DataPos - 2u;
                u16 crcPosL = g_sBTMS_Data.u16DataPos - 1u;
                u16 crc_rx  = ((u16)g_sBTMS_Data.u8RxDataFrameBuf[crcPosH] << 8u)
                            |  (u16)g_sBTMS_Data.u8RxDataFrameBuf[crcPosL];

                
                 
				// CRC 범위 = LEN(1) + 데이터(DataLen - CRC_LEN) = DataLen - CRC_LEN + LEN_LEN
                u32 crcLen = (u32)g_sBTMS_Data.u16DataLen
                           + (u32)BTMS_LEN_LEN
                           - (u32)BTMS_CRC_LEN;  

                u16 crc_calc = crc16_ibm3740(
                    &g_sBTMS_Data.u8RxDataFrameBuf[BTMS_LEN_IDX],
                    crcLen);

                if (crc_rx == crc_calc){
                    g_sBTMS_Data.u16StateLen = 0u;
                    g_sBTMS_Data.ePacket     = BTMS_PACKET_ETX;
                }else{
                    BTMS_RxData_ReParser();
                }
            }
        }break;

        /* ── ETX ── */
        case BTMS_PACKET_ETX:
            if (readByte == (u8)ETX)
            {
                g_sBTMS_Data.u8RxDataFrameBuf[g_sBTMS_Data.u16DataPos++] = readByte;
                g_sBTMS_Data.ePacket = BTMS_PACKET_STX;
                g_sBTMS_Data.bRxOk   = TRUE;
            }else{
                BTMS_RxData_ReParser();
            }break;

        default:
            BTMS_RxData_ReParser();
            break;
    }
}

/*-------------------------------------------------------------------------------------------------
 * BTMS_ParseRxData
 *------------------------------------------------------------------------------------------------*/
void BTMS_ParseRxData(void)
{
    u16         dataLen;
    const u8   *pData;
    u8          i;
    eBTMS_CmdType cmdID = BTMS_CMD_MAX;

    static const char * const aCmdStr[] =
    {
        "START",            /* BTMS_CMD_START          */
        "Target_Temp",      /* BTMS_CMD_TARGET_TEMP    */
        "Status",           /* BTMS_CMD_STATUS         */
        "Temp_Status",      /* BTMS_CMD_STATUS_TEMP    */
        "Humidity_Status",  /* BTMS_CMD_STATUS_HUMIDITY */
    };
    static const u8 aCmdStrLen[] = { 5u, 11u, 6u, 11u, 15u };

    /* 유효성: DataLen 이 CRC(2) 이하이면 데이터 없음 */
    if (g_sBTMS_Data.u16DataLen <= (u16)BTMS_CRC_LEN){
        g_sBTMS_Data.bRxOk = FALSE;
        return;
    }

    dataLen = g_sBTMS_Data.u16DataLen - (u16)BTMS_CRC_LEN;
    pData   = &g_sBTMS_Data.u8RxDataFrameBuf[BTMS_DATA_IDX];

    /* 26.04.08 KKD : 명령어 문자열 매칭 */
    for (i = 0u; i < (u8)BTMS_CMD_MAX; i++)
    {
        if ((dataLen >= (u16)aCmdStrLen[i]) &&
            (memcmp(pData, aCmdStr[i], (size_t)aCmdStrLen[i]) == 0))
        {
            cmdID = (eBTMS_CmdType)i;
            break;
        }
    }

    g_sBTMS_Parsed.bDataValid = FALSE;

    switch (cmdID) {
        case BTMS_CMD_START:
            if (dataLen >= (u16)(BTMS_STARTn_IDX + 1u)){
                g_sBTMS_Parsed.u8StartMode = pData[BTMS_STARTn_IDX];
                g_sBTMS_Parsed.eLastCmd    = BTMS_CMD_START;
                g_sBTMS_Parsed.bDataValid  = TRUE;
            }break;

        case BTMS_CMD_TARGET_TEMP:
            if (dataLen > (u16)BTMS_TARGET_TEMPn_IDX){
                u16 numLen = dataLen - (u16)BTMS_TARGET_TEMPn_IDX;
                g_sBTMS_Parsed.u8TargetTemp = (u8)BTMS_AsciiStrToNum(
                                                  &pData[BTMS_TARGET_TEMPn_IDX], numLen);
                g_sBTMS_Parsed.eLastCmd    = BTMS_CMD_TARGET_TEMP;
                g_sBTMS_Parsed.bDataValid  = TRUE;
            }break;

        case BTMS_CMD_STATUS:
        {
            u16 payloadOff = 6u;   /* strlen("Status") */
            if (dataLen > payloadOff){
                if (memcmp(&pData[payloadOff], "Normal", 6u) == 0){
                    g_sBTMS_Parsed.bStatusNormal = TRUE;
                    g_sBTMS_Parsed.u32ErrFlags   = 0u;
                    g_sBTMS_Parsed.u8ErrCount    = 0u;
                }else{
                    u16 errDataLen = dataLen - payloadOff;
                    u16 j;
                    u8  errCode;

                    g_sBTMS_Parsed.bStatusNormal = FALSE;
                    g_sBTMS_Parsed.u32ErrFlags   = 0u;
                    g_sBTMS_Parsed.u8ErrCount    = 0u;

                    for (j = 0u; (j + 1u) < errDataLen; j += 2u){
                        errCode = BTMS_Ascii2Dec(&pData[payloadOff + j]);
                        if ((errCode >= 1u) && (errCode <= (u8)BTMS_ERR_CODE_MAX)){
                            g_sBTMS_Parsed.u32ErrFlags |= ((u32)1u << (errCode - 1u));
                            g_sBTMS_Parsed.u8ErrCount++;
                        }
                    }
                }
                g_sBTMS_Parsed.eLastCmd   = BTMS_CMD_STATUS;
                g_sBTMS_Parsed.bDataValid = TRUE;
            }
        }break;

        case BTMS_CMD_STATUS_TEMP:
        {
            u16 payloadOff = 11u;  /* strlen("Temp_Status") */
            if (dataLen > (payloadOff + 3u)){
                if (memcmp(&pData[payloadOff], "RT:", 3u) == 0){
                    u16 rtStart = payloadOff + 3u;
                    u16 k;
                    u16 stPos   = 0u;

                    for (k = rtStart; (k + 2u) < dataLen; k++){
                        if ((pData[k]      == (u8)'S') &&
                            (pData[k + 1u] == (u8)'T') &&
                            (pData[k + 2u] == (u8)':'))
                        {
                            stPos = k;
                            break;
                        }
                    }

                    if (stPos > rtStart){
                        g_sBTMS_Parsed.s16ReturnTemp = BTMS_AsciiStrToNum(
                            &pData[rtStart], stPos - rtStart);
                        g_sBTMS_Parsed.s16SupplyTemp = BTMS_AsciiStrToNum(
                            &pData[stPos + 3u], dataLen - (stPos + 3u));
                        g_sBTMS_Parsed.eLastCmd   = BTMS_CMD_STATUS_TEMP;
                        g_sBTMS_Parsed.bDataValid = TRUE;
                    }
                }
            }
        }break;

        case BTMS_CMD_STATUS_HUMIDITY:
        {
            u16 payloadOff = 15u;  /* strlen("Humidity_Status") */
            if (dataLen > payloadOff)
            {
                u16 numLen = dataLen - payloadOff;
                if (pData[dataLen - 1u] == (u8)'%')
                {
                    numLen--;
                }
                g_sBTMS_Parsed.u8Humidity = (u8)BTMS_AsciiStrToNum(
                                               &pData[payloadOff], numLen);
                g_sBTMS_Parsed.eLastCmd   = BTMS_CMD_STATUS_HUMIDITY;
                g_sBTMS_Parsed.bDataValid = TRUE;
            }
        }break;
        default:
            break;
    }

    g_sBTMS_Data.bRxOk        = FALSE;
    g_sBTMS_Data.bCommTimeout = FALSE;
}

/*-------------------------------------------------------------------------------------------------
 * Getter 함수들
 *------------------------------------------------------------------------------------------------*/
u32  BTMS_GetErrFlags(void)    					{ return g_sBTMS_Parsed.u32ErrFlags;    }
u8   BTMS_GetErrCount(void)    					{ return g_sBTMS_Parsed.u8ErrCount;     }
BOOL BTMS_IsStatusNormal(void) 					{ return g_sBTMS_Parsed.bStatusNormal;	}
s16  BTMS_GetReturnTemp(void)  					{ return g_sBTMS_Parsed.s16ReturnTemp;  }
s16  BTMS_GetSupplyTemp(void)					{ return g_sBTMS_Parsed.s16SupplyTemp;  }
u8   BTMS_GetHumidity(void)    					{ return g_sBTMS_Parsed.u8Humidity;     }
u8   BTMS_GetStartMode(void)					{ return g_sBTMS_Parsed.u8StartMode;    }
u8   BTMS_GetTargetTemp(void)					{ return g_sBTMS_Parsed.u8TargetTemp;   }
u8   BTMS_GetTimeoutStatus(void)				{ return (u8)g_sBTMS_Data.bCommTimeout;	}
const	sBTMS_ParsedDataType* BTMS_GetParsedData(void) { return &g_sBTMS_Parsed;}


BOOL BTMS_IsErrActive(u8 errCode)
{
    if ((errCode >= 1u) && (errCode <= (u8)BTMS_ERR_CODE_MAX)){
        return (BOOL)((g_sBTMS_Parsed.u32ErrFlags & ((u32)1u << (errCode - 1u))) != 0u);
    }return FALSE;
}

/*-------------------------------------------------------------------------------------------------
 * internal helper
 *------------------------------------------------------------------------------------------------*/
static u8 BTMS_Ascii2Dec(const u8 *pAscii)
{
    return (u8)(((pAscii[0] - (u8)'0') * 10u) + (pAscii[1] - (u8)'0'));
}

static void BTMS_Dec2Ascii(u8 val, u8 *pAscii)
{
    pAscii[0] = (u8)((val / 10u) + (u8)'0');
    pAscii[1] = (u8)((val % 10u) + (u8)'0');
}


static s16 BTMS_AsciiStrToNum(const u8 *pBuf, u16 maxLen)
{
    s16 val   = 0;
    u16 i     = 0u;
    s16 sign  = 1;

    if ((maxLen > 0u) && (pBuf[0] == (u8)'-')){
        sign = -1;
        i    = 1u;
    }
    for (; i < maxLen; i++) {
        if ((pBuf[i] >= (u8)'0') && (pBuf[i] <= (u8)'9')){
            val = (s16)((val * 10) + (s16)(pBuf[i] - (u8)'0'));
        }else{
            break;
        }
    }return (s16)(val * sign);
}

/*-------------------------------------------------------------------------------------------------
 * crc16_ibm3740 
 *------------------------------------------------------------------------------------------------*/
static u16 crc16_ibm3740(const u8 *data, u32 len)
{
    u16 crc = (u16)CRC16_IBM3740_INIT;
    u32 j;
    int i;

    for (j = 0u; j < len; j++){
        crc ^= ((u16)data[j]) << 8u;
        for (i = 0; i < 8; i++){
            if ((crc & 0x8000u) != 0u){
                crc = (u16)((crc << 1u) ^ (u16)CRC16_IBM3740_POLY);
            }else{
                crc <<= 1u;
            }
        }
    }return (u16)(crc ^ (u16)CRC16_IBM3740_XOROUT);
}




