/*=================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(HALL sensor - CAN 500kbit/s)	        	                           *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025. 11. 20                                                                *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               HALL_cab1500.c
\author             KKD
\date               2025-11-21 
\brief              HALL sensor - CAN 500kbit/s
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <HALL_cab1500.h>
#include <ms_timer.h>
/* Private define -----------------------------------------------------------------------------------*/
#define CURR_RAW_A_OFFSET    (0x800000UL)
#define CURR_RAW_D_OFFSET    (0x8000UL)

/* 시스템 파라미터 (캘리브레이션 값) */
#define CURR_LSB_A         (0.001f)   // 예시: 1 mA / bit (실제 값으로 교체)
#define CURR_LSB_D         (1.000f)   // 예시: 1 mA / bit (실제 값으로 교체)
#define CURR_CLAMP_A       (1550000L)
#define CURR_CLAMP_D       (15500L)

#define CAB_CRC_OFFSET      7
#define CAB_CRC_DATA_LEN    7

/* Private macro -----------------------------------------------------------------------------------*/
/* Private typedef ----------------------------------------------------------------------------------*/
sCab1500Data g_sHallData;
static u32 g_u32HallLastTime;
/* Private variables ---------------------------------------------------------------------------------*/
/* Private function prototypes ---------------------------------------------------------------------*/
u8 CRC8_SAE_J1850(const u8* data, size_t len);

/* Private functions ------------------------------------------------------------------------------*/
void HALL_Init(void)
{
    memset(&g_sHallData, 0, sizeof(g_sHallData));
    g_sHallData.u16ErrStatus	= FALSE;
    g_sHallData.bComTimout		= TRUE;         // 초기에는 미수신 상태
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
    TimerAdd( 500, HALL_CanRx_TimeoutCheck,& g_sHallData.s32TimerID );
#endif //CAN_BUF_UPATE
    GPIO_DOut_CAB( TRUE );
}

#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
msStatus_t HALL_RxParsing  ( uint32_t canID, u8 *pu8CanRxData )
{
    msStatus_t retValue = MS_ERROR;
    switch( canID )
    {
        case HALL_CAN_ID:
            HALL_CanRxParsing  ( &g_sHallData, pu8CanRxData );
            retValue = MS_SUCCESS;
            break;
        default:
            break;
    }
    return retValue;
}
#endif //CAN_BUF_UPATE

s32 HALL_MovAvg(s32 s32Data, u8 u8Pos){
	static s64	s_lawbuff[2][HMOV_CNT] = {0};
	static u8	s_first[2]	= {0};
	static u8	s_movcnt[2] 	= {0};
	static u8	s_movpos[2] 	= {0};
	s64 		movsum[2]		= {0};
	s64 		movavg[2]		= {0};
	u8			i = 0;
	
	if(s_first[u8Pos] == 0U){
		s_first[u8Pos] = 1;
		for(i=0;i<s_movcnt[u8Pos];i++){
			s_lawbuff[u8Pos][i] = (u64)s32Data;
		}
	}else{
		s_lawbuff[u8Pos][s_movpos[u8Pos]] = (u64)s32Data;
	}

	if(++s_movpos[u8Pos] >= (u8)HMOV_CNT){
		s_movpos[u8Pos] = 0;
	}
	if(++s_movcnt[u8Pos] >= (u8)HMOV_CNT){
		s_movcnt[u8Pos] = (u8)HMOV_CNT;
	}

	for(i=0;i<s_movcnt[u8Pos];i++){
		movsum[u8Pos] += s_lawbuff[u8Pos][i];
	}

	//if(s_movcnt[u8Pos] == 0U){
	//	movavg[u8Pos] = 0;
	//}else{
		movavg[u8Pos] = (movsum[u8Pos]/(s32)s_movcnt[u8Pos]);
	//}

	return (s32)movavg[u8Pos];
}

void HALL_CanRxParsing(sCab1500Data *psHallData, u8 *pu8CanRxData)
{
	static u8 i1sectim = 0;
	s32	i1sec = 0;
	u32	tmp = 0;   
	u8	prevseq = g_sHallData.u8SeqCnt;                                           	//KKD 2025-12-01 befo Sequence Count
	u8  dataTemp[8];
	

	memcpy(dataTemp, pu8CanRxData, sizeof( dataTemp ) );                                //jkpark 2025-12-24 데이터 일관성 확보.

	psHallData->u8Crc = dataTemp[ CAB_CRC_OFFSET ];                                           		//KKD 2025-11-24 BYTE 7 : CRC
	
    if( CRC8_SAE_J1850( dataTemp, CAB_CRC_DATA_LEN ) == dataTemp[ CAB_CRC_OFFSET ] )
    {
    	psHallData->u8SeqCnt	= (dataTemp[0] >> 4) & 0x0FU;                        	//KKD 2025-11-24 BYTE 0 : Update Sequence Count
    	psHallData->u8StsPwr	= (dataTemp[0] >> 2) & 0x03U;                        	//KKD 2025-12-01 BYTE 0 : Diagnosic StatusPowerSupply
    	psHallData->u8StsIntErr	= (dataTemp[0] >> 1) & 0x01U;                        	//KKD 2025-12-01 BYTE 0 : Diagnosic StatusInternalError
    	psHallData->u8StsSgv	= (dataTemp[0] >> 0) & 0x01U;                        	//KKD 2025-12-01 BYTE 0 : Diagnosic SafetyGoalViolation

    	if( psHallData->u8StsPwr == 0x03u ){              //jkpark 2025-12-22 전원 전압 측정불가.
            psHallData->eHallErr = HErr_SupPwr_Type;
        }
    	else if( psHallData->u8StsIntErr == 0x01U ){          //jkpark 2025-12-22 기준전압이상이거나 DAC, 애플리케이션 리플 전류이상, 또는 버스바과전류(>1600A)
            psHallData->eHallErr = HErr_InterErr_Type;
        }
    	else if( psHallData->u8StsSgv == 0x01U ){             //jkpark 2025-12-22 Safe State오류. Analog Current 와 Digital Current차이가 큼.
            psHallData->eHallErr = HErr_GoalViol_Type;
    	}
    	tmp  = ((u32)dataTemp[1] << 16);
    	tmp |= ((u32)dataTemp[2] << 8);
    	tmp |= ((u32)dataTemp[3] << 0);

    	//	if(tmp & 0x800000)	{ tmp |= 0xFF000000; }
		psHallData->s32Analog = (s32)tmp - CURR_RAW_A_OFFSET;                                               	//KKD 2025-11-24 BYTE 1~3 : Analog Current
		psHallData->s32AnalMovAvg = HALL_MovAvg(psHallData->s32Analog,0);
		
		tmp	 = ((u32)dataTemp[4] << 8);
		tmp	|= ((u32)dataTemp[5] << 0);

    	//	if(tmp & 0x008000)	{ tmp |= 0xFFFF0000; }
		psHallData->s32Digital = (s32)tmp - CURR_RAW_D_OFFSET;                                              	//KKD 2025-11-24 BYTE 4~5 : Digital Current
		psHallData->s32DigtMovAvg = HALL_MovAvg(psHallData->s32Digital,1);
     
        if(psHallData->u8SeqCnt == prevseq){
            if(psHallData->u8SeqStuckCnt < 0xFFU){
                psHallData->u8SeqStuckCnt++;                     							//KKD 2025-12-01 같은 값이 반복됨
            }
        }else{
            psHallData->u8SeqStuckCnt = 0;                       							//KKD 2025-12-01 정상적으로 바뀌면 리셋
        }        
    }    else    {
        psHallData->eHallErr = HErr_CRC_Type;
    }

	i1sec += psHallData->s32AnalMovAvg;
	if(++i1sectim>10u){
		i1sectim = 0;
        psHallData->i1sec = (i1sec/10);
		i1sec = 0;
	}
	
    // 수신 Check
    psHallData->bComTimout = FALSE;
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
    TimerRestart( psHallData->s32TimerID );
#else //CAN_BUF_UPATE
    g_u32HallLastTime = HAL_GetTick();
#endif //CAN_BUF_UPATE
}


BOOL HALL_SM_CurrentSenseCheck(sCab1500Data *psHallData)
{
	float iA  	= HALL_GetAnalogAmp(); 	
	float iD 	= HALL_GetDigitalAmp();	
	float absI	= FABS(iA);
	float diff	= 0.0f;

	if(psHallData->u8StsIntErr == 1U){                                            		//KKD 2025-11-28 Internal Err or >1600A 
		return FALSE;
	}

	diff = FABS(iA - iD);                                                           	//KKD 2025-11-28 diff 계산

	if(absI < CURR_SG1_TH_LOW_A){                                                   	//KKD 2025-11-28 SG1 판단
		if(diff > CURR_SG1_DIFF_LOW_A){                                             	//KKD 2025-11-28 220A 미만 오차 44A
			return FALSE;
		}
	}else{
		float th = (absI * CURR_SG1_DIFF_RATIO);                                    	//KKD 2025-11-28 220A 초과 오차 20A
		if(diff > th){
			return FALSE;
		}
	} return TRUE;
}

#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
msStatus_t HALL_CanRx_TimeoutCheck( void )
{
    g_sHallData.eHallErr = HErr_CanRxTimeout_Type;
    g_sHallData.bComTimout = TRUE;                                             		//KKD 2025-12-01 Rx 체크 플래그 초기화
    return MS_ENABLE;
}
#else  //CAN_BUF_UPATE
BOOL HALL_CanRx_TimeoutCheck(sCab1500Data *psHallData)
{
    u32 now = HAL_GetTick();

    if ((now - g_u32HallLastTime) >= HALL_TIMEOUT_MS){
        psHallData->bComTimout = TRUE;                                             		//KKD 2025-12-01 Rx 체크 플래그 초기화
        g_u32HallLastTime = now;
    }
    return psHallData->bComTimout;
}
#endif //CAN_BUF_UPATE

BOOL HALL_DiagnosicCheck(sCab1500Data *psHallData)                                         																//KKD 2025-12-02 HALL Diagnosic Check
{
	BOOL bSmErr = FALSE;

    if(psHallData->u8SeqStuckCnt >= HALL_SEQ_STUCK_TH){
        psHallData->u8SeqStuckErr = TRUE;                     							//KKD 2025-12-01 5회 이상 정지 에러
    }else{
        psHallData->u8SeqStuckErr = FALSE;
    }

	if(psHallData->u8SeqStuckErr == TRUE){                                              //KKD 2025-12-02 Diagnosic - Sequence Count Check
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_SeqStuck_Type);
	}else{
		psHallData->u16ErrStatus &= ~((u16)1<<(u16)HErr_SeqStuck_Type);
	}

	if(psHallData->u8StsPwr == 0x03U){                                                   //KKD 2025-12-02 Diagnosic - Power Check
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_SupPwr_Type);
	}else{
		psHallData->u16ErrStatus &= ~((u16)1<<(u16)HErr_SupPwr_Type);
	}

	if(psHallData->u8StsIntErr == 1U){                                                   //KKD 2025-12-02 Diagnosic - Internal Err
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_InterErr_Type);
	}else{
		psHallData->u16ErrStatus &= ~((u16)1<<(u16)HErr_InterErr_Type);
	}

	if(psHallData->u8StsSgv == 1U){                                                      //KKD 2025-12-02 Diagnosic - Safety Goal Violation
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_GoalViol_Type);
	}else{
		psHallData->u16ErrStatus &= ~((u16)1<<(u16)HErr_GoalViol_Type);
	}

	if(psHallData->bComTimout == TRUE){
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_CanRxTimeout_Type);
	}else{
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_CanRxTimeout_Type);

	}

	bSmErr = HALL_SM_CurrentSenseCheck(&g_sHallData);                                   //KKD 2025-12-02 FALSE = 이상, TRUE = 정상
	if(bSmErr == FALSE){
		psHallData->u16ErrStatus |= ((u16)1<<(u16)HErr_Comp_Type);
	}else{
		psHallData->u16ErrStatus &= ~((u16)1<<(u16)HErr_Comp_Type);
	}

	if((psHallData->u16ErrStatus &
	( ((u16)1 << (u16)HErr_SeqStuck_Type)		
      	| ((u16)1 << (u16)HErr_SupPwr_Type)  		
      	| ((u16)1 << (u16)HErr_InterErr_Type) 		
      	| ((u16)1 << (u16)HErr_GoalViol_Type)  	
      	| ((u16)1 << (u16)HErr_CanRxTimeout_Type)	
      	| ((u16)1 << (u16)HErr_Comp_Type)
	)) != 0U){
        return TRUE;    
	}return FALSE;
}

void HALL_Proc(void)
{
#ifdef CAN_BUF_UPATE    //2025-12-05   jkpark  Can Buffer 관련 수정.
	HALL_DiagnosicCheck(&g_sHallData);                                                	//KKD 2025-12-02 Diagnosic
#else  //CAN_BUF_UPATE
	if(g_bSenIcRxDone[SenIC_HALL_Type] == TRUE){
		g_bSenIcRxDone[SenIC_HALL_Type] = FALSE;
		HALL_CanRxParsing(&g_sHallData, g_u8SenIcRxData[SenIC_HALL_Type] );              //KKD 2025-11-24 Get Current
	}

	HALL_CanRx_TimeoutCheck(&g_sHallData);                                              //KKD 2025-12-02 CAN Rx Alive Check
	HALL_DiagnosicCheck(&g_sHallData);                                                	//KKD 2025-12-02 Diagnosic
#endif //CAN_BUF_UPATE
}

u16 HALL_GetErrStatus(void)
{
	return g_sHallData.u16ErrStatus;
}

float HALL_GetAnalogAmp(void)
{
	return ((float)g_sHallData.s32Analog * 0.001f);
}

float HALL_GetDigitalAmp(void)
{
	return ((float)g_sHallData.s32Digital * 0.001f);
}

u8 HALL_GetSeqCount(void)
{
	return g_sHallData.u8SeqCnt;
}

s32 HALL_GetCurr1Sec(void)
{
	return g_sHallData.i1sec;
}

u8 CRC8_SAE_J1850(const u8* data, size_t len)
{
    u8 crc = 0xFFU;                 // init

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {   // MSB-first, non-reflected
            if ((crc & 0x80U) != 0U){
                crc = (u8)((crc << 1) ^ 0x1DU);
            }else{
                crc = (u8)(crc << 1);	
			}
        }
    }
    crc ^= 0xFFU;                        // xorout
    return crc;
}





