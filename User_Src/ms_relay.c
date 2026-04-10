/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Contector Operating Sequence)                                       *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_relay.h
\author             KKD
\date               2025-11-21 
\brief              Contector Sequence
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_relay.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
sHvRelayStatus g_sRelayStatus;

/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

void Relay_Init(void)
{
	memset(&g_sRelayStatus, 0x00, sizeof(g_sRelayStatus));
}

static BOOL Rly_DelayCheck(sHvRelayStatus *pRly)
{
    if (++pRly->u8HvRly_Delay >= 5U) {
        pRly->u8HvRly_Delay = 0;
        return TRUE;
    }return FALSE;
}
#if 0
void Relay_Status(sHvRelayStatus *pRly)
{	
	static u8 dlycnt = 0;
/* Todo : BTMS 통신 회복시 BTMS 릴레이 붙는 시퀀스 */
/*
// 1. bms on
// 2. BTMS Chk
// 3. BTMS Relay ON
// 4. MVB : BAT -> TCMS status data
// 5. MVB : TCMS -> BAT relay on command 
// 6. check detection
// 7. Main Relay ON

// . main relay on * P_OUT check (PV-BV diff check)
// . main relay off (OV/UV/OC/MVB_ERR/IMD)
// . main + btms relay off (OT/UT/BTMS_COMM_ERR)
	*/

 #if 1	
	if(g_sMvbData.u8RxDataFrameBuf[0] == (u8)TRUE){
		// MVB Rx start Flag
		Relay_Stop_BTMS();
		if((RackPkt.fdet & 0xFFFFFFFFU) != 0UL){																												//KKD 2025-12-11 MAIN Relay 상태 설정
			Relay_Stop_Main();
		}else{
			if(pRly->bHvRly_BtmsOnDone == TRUE){
				if(g_sBTMS_Data.u8RxDataFrameBuf[1] == 2){

				}
				Relay_Start_Main();
			}
		}
	}else{

	}
#else
	if((RackPkt.fdet & 0xFFFFFFFFU) != 0UL){																												//KKD 2025-12-11 MAIN Relay 상태 설정
		//Relay_Stop_Main();
		//Relay_Stop_BTMS();
	}else{
		Relay_Start_BTMS();
		if(pRly->bHvRly_BtmsOnDone == TRUE){
			Relay_Start_Main();
		}
	}
#endif
 	
	/* 1) BTMS RELAY ON */
    switch (pRly->u8HvRly_B_Status){
        case 1:                                                                     																	//KKD 2025-12-11 B_N ON
			if(++dlycnt>20){
			  dlycnt = 20;
				pRly->u8HvRly_CmdStatus |= BNCK;
				if((pRly->u8HvRly_Feedback & BNCK) != 0){
					if (Rly_DelayCheck(pRly) == TRUE) {
						pRly->u8HvRly_B_Status = 2;
						dlycnt = 0;
					}
				}
			}break;
        case 2:                                                                     																	//KKD 2025-12-11 B_P ON
			pRly->u8HvRly_CmdStatus |= BPRE;
            if (Rly_DelayCheck(pRly) == TRUE) {
                pRly->u8HvRly_B_Status = 3;
	        }break;

        case 3:         // B_PRE OFF → 완료
			pRly->u8HvRly_CmdStatus |= BPCK;			
			if((pRly->u8HvRly_Feedback & BPCK) != 0){
	            if (Rly_DelayCheck(pRly) == TRUE) {
					pRly->u8HvRly_B_Status = 4;
				}
           	}break;

		case 4:
			pRly->u8HvRly_CmdStatus &= ~(BPRE);
			if (Rly_DelayCheck(pRly) == TRUE) {
				pRly->u8HvRly_B_Status = HV_STATUS_DONE;
			}break;

        case HV_STATUS_DONE:
			if(pRly->bHvRly_BtmsOnDone == FALSE){
				if (Rly_DelayCheck(pRly) == TRUE){ 
					Relay_Start_Main();
					pRly->bHvRly_BtmsOnDone = TRUE;
				}
			}break;
        
        default:
        	break;
    }																											//KKD 2025-12-11 BTMS 완료 전에는 MAIN 시퀀스 실행 안함

	/* 2) MAIN RELAY ON */
    switch (pRly->u8HvRly_M_Status){
        case 1:                                                                         																//KKD 2025-12-11 M_PRE ON
			pRly->u8HvRly_CmdStatus |= MNCK;
			if((pRly->u8HvRly_Feedback & MNCK) != 0){
	            if (Rly_DelayCheck(pRly) == TRUE){
	                pRly->u8HvRly_M_Status = 2;
	            }
            }break;

        case 2:                                                                        																	//KKD 2025-12-11 M_N ON
			pRly->u8HvRly_CmdStatus |= MPRE;
            if (Rly_DelayCheck(pRly) == TRUE){
                pRly->u8HvRly_M_Status = 3;
            }break;

        case 3:                                                                         																//KKD 2025-12-11 PV-BV 동기화 체크
#if 0
			pRly->s32Hv_PvBvDiff = (s32)RackPkt.pv[PVINP] - (s32)RackPkt.pv[PVOUT];

			if (RackPkt.pv[PVOUT] > 432000 &&pRly->s32Hv_PvBvDiff > -20000L){
				pRly->bHvRly_DiffErr = FALSE;
				pRly->u8HvRly_Delay = 0;
			}else if (Rly_DelayCheck(pRly)){
				pRly->bHvRly_DiffErr = TRUE;
			}

			if (pRly->bHvRly_DiffErr == FALSE) {
				pRly->u8HvRly_CmdStatus |= RLY_MPCK_Type;
				pRly->u8HvRly_M_Status = 4;
			} else {
				pRly->u8HvRly_CmdStatus &= ~(RLY_MPCK_Type);
			}break;
#else
			pRly->u8HvRly_CmdStatus |= MPCK;
			if((pRly->u8HvRly_Feedback & MPCK) != 0){
           		if (Rly_DelayCheck(pRly) == TRUE){
                	pRly->u8HvRly_M_Status = 4;
               	}
            }break;
#endif
        case 4:
			pRly->u8HvRly_CmdStatus &= ~(MPRE);
			if (Rly_DelayCheck(pRly) == TRUE) {
				pRly->u8HvRly_M_Status = HV_STATUS_DONE;
			}break;
		case HV_STATUS_DONE:
			break;
        default:
           break;
    }
    
    if (pRly->u8HvRly_M_Status == (u8)HV_STATUS_DONE){
        pRly->bHvRly_MainOnDone = TRUE;
	}
}


#else
void Relay_Status(sHvRelayStatus *pRly)
{	
	static u8 dlycnt = 0;
	/* Todo : BTMS 통신 회복시 BTMS 릴레이 붙는 시퀀스 */
	/*
	// 1. bms on
	// 2. BTMS Chk
	// 3. BTMS Relay ON
	// 4. MVB : BAT -> TCMS status data
	// 5. MVB : TCMS -> BAT relay on command 
	// 6. check detection
	// 7. Main Relay ON
	
	// . main relay on * P_OUT check (PV-BV diff check)
	// . main relay off (OV/UV/OC/MVB_ERR/IMD)
	// . main + btms relay off (OT/UT/BTMS_COMM_ERR)
 	*/


	Relay_Start_BTMS();
	if(pRly->bHvRly_BtmsOnDone == TRUE){
		Relay_Start_Main();
	}
 	
	/* 1) BTMS RELAY ON */
    switch (pRly->u8HvRly_B_Status){
        case 1:                                                                     																	//KKD 2025-12-11 B_N ON
			if(++dlycnt>20){
			  dlycnt = 20;
				pRly->u8HvRly_CmdStatus |= BNCK;
				if (Rly_DelayCheck(pRly) == TRUE) {
					pRly->u8HvRly_B_Status = 2;
					dlycnt = 0;
				}
			}break;
        case 2:                                                                     																	//KKD 2025-12-11 B_P ON
			pRly->u8HvRly_CmdStatus |= BPRE;
            if (Rly_DelayCheck(pRly) == TRUE) {
                pRly->u8HvRly_B_Status = 3;
	        }break;

        case 3:         // B_PRE OFF → 완료
			pRly->u8HvRly_CmdStatus |= BPCK;			
            if (Rly_DelayCheck(pRly) == TRUE) {
				pRly->u8HvRly_B_Status = 4;
           	}break;

		case 4:
			pRly->u8HvRly_CmdStatus &= ~(BPRE);
			if (Rly_DelayCheck(pRly) == TRUE) {
				pRly->u8HvRly_B_Status = HV_STATUS_DONE;
			}break;

        case HV_STATUS_DONE:
			if(pRly->bHvRly_BtmsOnDone == FALSE){
				if (Rly_DelayCheck(pRly) == TRUE){ 
					//Relay_Start_Main();
					pRly->bHvRly_BtmsOnDone = TRUE;
				}
			}break;
    }																											//KKD 2025-12-11 BTMS 완료 전에는 MAIN 시퀀스 실행 안함

	/* 2) MAIN RELAY ON */
    switch (pRly->u8HvRly_M_Status){
        case 1:                                                                         																//KKD 2025-12-11 M_PRE ON
			pRly->u8HvRly_CmdStatus |= MNCK;
            if (Rly_DelayCheck(pRly) == TRUE){
                pRly->u8HvRly_M_Status = 2;
            }break;

        case 2:                                                                        																	//KKD 2025-12-11 M_N ON
			pRly->u8HvRly_CmdStatus |= MPRE;
            if (Rly_DelayCheck(pRly) == TRUE){
                pRly->u8HvRly_M_Status = 3;
            }break;

        case 3:                                                                         																//KKD 2025-12-11 PV-BV 동기화 체크
			pRly->u8HvRly_CmdStatus |= MPCK;
       		if (Rly_DelayCheck(pRly) == TRUE){
            	pRly->u8HvRly_M_Status = 4;
            }break;
            
        case 4:
			pRly->u8HvRly_CmdStatus &= ~(MPRE);
			if (Rly_DelayCheck(pRly) == TRUE) {
				pRly->u8HvRly_M_Status = HV_STATUS_DONE;
			}break;
		case HV_STATUS_DONE:
			break;
    }
    
    if (pRly->u8HvRly_M_Status == (u8)HV_STATUS_DONE){
        pRly->bHvRly_MainOnDone = TRUE;
	}
}


#endif
void Relay_Control(sHvRelayStatus *pRly)
{
	GPIO_DOut_BPRE_RLY(		pRly->u8HvRly_CmdStatus & (BPRE));
	GPIO_DOut_BTOP_RLY(		pRly->u8HvRly_CmdStatus & (BPCK));
	GPIO_DOut_BBTM_RLY(		pRly->u8HvRly_CmdStatus & (BNCK));
	
	GPIO_DOut_MPRE_RLY(		pRly->u8HvRly_CmdStatus & (MPRE));
	GPIO_DOut_MTOP_CON(		pRly->u8HvRly_CmdStatus & (MPCK));
	GPIO_DOut_MBTM_CON(		pRly->u8HvRly_CmdStatus & (MNCK));
}

void RlyCon_Feedback(sHvRelayStatus *pRly)
{	
    u8 fbak = 0;
    
    if(GPIO_DIN_BTOP_RLY()	== 1U)  {
        fbak |= BPCK;
    }
    if(GPIO_DIN_BBTM_RLY()	== 1U)  {
        fbak |= BNCK;
    }

    if(GPIO_DIN_MTOP_CON()	== 1U)  {
        fbak |= MPCK;
    }
    if(GPIO_DIN_MBTM_CON()	== 1U)  {
        fbak |= MNCK;
    }
    pRly->u8HvRly_Feedback = fbak;
}

void Relay_Diagnosic(sHvRelayStatus *pRly)
{
	static u8 s_chkcnt	= 0;
	BOOL berrbtms 		= FALSE;
	u8	 cmd			= (u8)pRly->u8HvRly_CmdStatus;                                         																//KKD 2025-12-11 내가 출력한 명령
   	u8	 feed			= (u8)pRly->u8HvRly_Feedback;   	  																								//KKD 2025-12-11 실제 릴레이 피드백 입력값
	u8	 result			= 0;
   	u8	 diff			= cmd ^ feed;                                                   																//KKD 2025-12-11 서로 다른 비트가 1이 됨
   	
                                                         																								//KKD 2025-12-11 진단 결과 기본
   /* 1) 명령과 피드백 불일치 */
   if (diff != 0U) {
		if ((cmd & MPCK) != (feed & MPCK))	{ result |= (u8)(MPCK);  }
		if ((cmd & MNCK) != (feed & MNCK))	{ result |= (u8)(MNCK); }
		if ((cmd & BPCK) != (feed & BPCK))	{ result |= (u8)(BPCK);   }
		if ((cmd & BNCK) != (feed & BNCK))	{ result |= (u8)(BNCK);  }
		berrbtms = TRUE;
	}
	
/* Todo : 통신 fault시 적용....
	if()                                                                                																//KKD 2025-12-11 BTMS 통신 에러시
		pRly->eHvRly_ErrStatus = RERR_COMFAIL
*/	
	if(berrbtms != 0U){
		if(++s_chkcnt>30U){
			s_chkcnt = 20;
			pRly->u8HvRly_ErrStatus |= RERR_RLYNG;
		}
	}else{
		s_chkcnt = 0;
	}
}


void Relay_Proc(void)
{
	//if(g_sMn.mn_type & ((u8)1 << (u8)MN_RLY_TYPE)){			//ori
	if((g_sMn.mn_type & ((u16)1U << (u16)MN_RLY_TYPE)) != 0U){		// static analizye
	}else{
		Relay_Status(&g_sRelayStatus);
	}
	Relay_Control(&g_sRelayStatus);
	RlyCon_Feedback(&g_sRelayStatus);
	Relay_Diagnosic(&g_sRelayStatus);
}

void Relay_Start_BTMS(void)
{
	if(g_sRelayStatus.u8HvRly_B_Status 		== 0U) {
		g_sRelayStatus.u8HvRly_B_Status		= 1;
		
		g_sRelayStatus.u8HvRly_CmdStatus	= 0;
		g_sRelayStatus.bHvRly_MainOnDone	= FALSE;
	}
}

void Relay_Stop_BTMS(void)
{
	g_sRelayStatus.bHvRly_BtmsOnDone		= FALSE;
	g_sRelayStatus.bHvRly_MainOnDone 		= FALSE;
	g_sRelayStatus.u8HvRly_B_Status 		= 0;
	g_sRelayStatus.u8HvRly_M_Status			= 0;
	g_sRelayStatus.u8HvRly_CmdStatus 		&= ~(u8)(BPRE);
	g_sRelayStatus.u8HvRly_CmdStatus 		&= ~(u8)(BPCK);
	g_sRelayStatus.u8HvRly_CmdStatus 		&= ~(u8)(BNCK);
}

void Relay_Start_Main(void)
{
	if(g_sRelayStatus.u8HvRly_M_Status 		== 0U) {
		g_sRelayStatus.u8HvRly_M_Status		= 1;
	}
}

void Relay_Stop_Main(void)
{
	g_sRelayStatus.u8HvRly_M_Status			= 0;
	g_sRelayStatus.u8HvRly_CmdStatus 		&= ~(u8)(MPRE);
	g_sRelayStatus.u8HvRly_CmdStatus 		&= ~(u8)(MPCK);
	g_sRelayStatus.u8HvRly_CmdStatus 		&= ~(u8)(MNCK);
}

u8 Relay_Check_ErrStatus(void)
{
	return g_sRelayStatus.u8HvRly_ErrStatus;
}




