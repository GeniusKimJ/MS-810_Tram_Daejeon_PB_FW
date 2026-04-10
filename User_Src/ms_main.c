/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_main.c
\author             KKD
\date               2025-11-21 
\brief              Main ������ ���� �ڵ�.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"
#include "ms_can_mon.h"
#include "ms_rs232_mon.h"

/* Private define ---------------------------------------------------------------------------------*/
#define CPU_LOAD_PERCENT   70u
#define CPU_LOAD_PERIOD_US 1000u   // 1 ms
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
Pkt_Rack 	RackPkt;
sSystemTick	g_sSysTick;
sMn 		g_sMn;

u8 			g_f1s;


s16 		DataTh[NSTH] = {0,};
s32 		DataPIH		= 0;												// Current High Data
s32 			DataPIL		= 0;											// Curr\ent Low Data
u32			DataPvDet[2] = {0,};											// Test Set PV Value
float		RatPI[4];														// charge low, charge high, discharge low, discharge high
float		GabPI[4];														// pack in, pack out discharge, pack out charge, pack volt reserved
float		RatPV[4];														// charge low, charge high, discharge low, discharge high
float		GabPV[4];														// pack in, pack out discharge, pack out charge, pack volt reserved
float		RatAX;
float		GabAX;


volatile u8 g_u8fIdle   = 0U;
volatile u32 g_u32CntIdle   = 0U;
volatile u32 g_u32CntTotal   = 0UL;
volatile u32 g_u32LastTime   = 0UL;
volatile u32 g_u32CntCpuLoad = 28000U; //  168Mhz (28000) 70%
volatile float g_f32CpuLoad    = 0.0f;
volatile u32 g_u32dummy = 0;
volatile float g_fratio = 0.0f;

u8	g_dbg[20];			//timer dbg

u32 st,en,diff,maxT[51];
u8	g_hvbaton_cnt;
u8	g_chargeon_cnt;

/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

float GetCpuLoad(void)
{
#if 0
	float tmp1 = (float)g_u32CntIdle/3.0f;
	float tmp2 = tmp1/(float)g_u32CntTotal;

	if(g_fratio > 1.0f) {g_fratio = 1.0f;}

	g_u32CntIdle = 0U;
	g_u32CntTotal = 0U;

	return 100.0f * (1.0f - g_fratio);  
#else
	g_fratio = (float)(g_u32CntIdle/4) / (float)g_u32CntTotal;
	if(g_fratio > 1.0f) g_fratio = 1.0f;

	g_u32CntIdle = 0U;
	g_u32CntTotal = 0U;

	return 100.0f * (1.0f - g_fratio);  

#endif
}

void CpuLoadTest(void)
{
    static u32 u32k = 0;
    if(++u32k < g_u32CntCpuLoad){
    	g_u32dummy += u32k ^ 0x3UL;
    }else{
      u32k = 0;
    }
}

void ms_main(void)
{
	static BOOL s_bsmstart	= FALSE;
	static u8 cnt = 0;
	u8 i = 0;

	Proc_Bms_Init();
	Can_Independent_Init();						// CAN 100ms 주기 타이머 초기화
	while(1)
	{
		//HAL_IWDG_Refresh(&hiwdg);
		Can_TxProc(McuCan_Ext_Type);
		//Can_Independent();					// SM 외부에서도 100ms 주기 체크
		if(g_f1ms == TRUE){
      		g_f1ms = FALSE;
			if(g_dbg[13] == 0){
				if(g_sSysTick.b10ms_Task == TRUE){	// 10ms task
					g_sSysTick.b10ms_Task = FALSE;
					Adbms6815_Cal_AfePos(ComDir_A_Type,g_ad.eRw);
					GPIO_DOut_Rly_toggle(); 
					Gpio_Led();
					Gpio_Fault_Out();
					if(Gpio_Hv_In() == TRUE){
						if(++g_hvbaton_cnt>200u){
							RackPkt.hvbat_on = TRUE;
						}
					}else{
						g_hvbaton_cnt = (u8)0;
						RackPkt.hvbat_on = (u8)FALSE;
					}
					
					if(Gpio_Ch_In() == TRUE){
						if(++g_chargeon_cnt>200u){
							RackPkt.charge_on = TRUE;
						}
					}else{
						g_chargeon_cnt = (u8)0;
						RackPkt.charge_on = (u8)FALSE;
					}
					

					if((g_sMn.mn_type & ((u16)1U << (u16)MN_RTC_TYPE)) != 0U){
						g_sMn.mn_type &= ~((u16)1U << (u16)MN_RTC_TYPE);
						RTC_SetLocalTime(g_sMn.mn_set_rtc);
					}
					RTC_GetLocalTime(RackPkt.rtc);
				}
			}

			switch(g_sSysTick.u16SysTick){
			case 1:
				if(g_dbg[0] == 1) break;
				Adbms6815_01_CV_Config(); //3.3ms
				break;
			case 4:
				if(g_dbg[1] == 1) break;
				Adbms6815_02_CV_Read_S(GA_TYPE);					
				break;
			case 8:
				if(g_dbg[2] == 1) break;
				Adbms6815_03_CV_Read_E(GA_TYPE);
				Adbms6815_02_CV_Read_S(GB_TYPE);
				break;
			case 12:
				if(g_dbg[3] == 1) break;
				Adbms6815_03_CV_Read_E(GB_TYPE);
				Adbms6815_02_CV_Read_S(GC_TYPE);
				break; 
			case 16:
				if(g_dbg[4] == 1) break;
				Adbms6815_03_CV_Read_E(GC_TYPE);
				Adbms6815_04_TH_Config();
				break;
			case 20:
				if(g_dbg[5] == 1) break;
				Adbms6815_05_TH_Read_S(GA_TYPE);
				break;
			case 24:
				if(g_dbg[6] == 1) break;
				Adbms6815_06_TH_Read_E(GA_TYPE);
				Adbms6815_05_TH_Read_S(GB_TYPE);
				break; 
			case 29:
				if(g_dbg[7] == 1) break;
				Adbms6815_06_TH_Read_E(GB_TYPE);
				Adbms6815_05_TH_Read_S(GC_TYPE);
				break;
			case 33:
				Adbms6815_06_TH_Read_E(GC_TYPE);
				break;
			case 36:
				if(g_dbg[8] == 1) break;
				if(g_ad.bSetBalancingStart ==FALSE && g_sMn.mn_set_bal_start == FALSE){
					Adbms6815_SM_Start();	/* SM timer Reset, SM Enable */
				}break;
			case 76:
				if(g_dbg[9] == 1) break;
				Adbms6815_Balancing_Proc();
				break;
			case 80:
				if(g_dbg[10] == 1) break;
				Proc_Check_Status(&RackPkt);
				Proc_Calcul_CvTh(&RackPkt);
				Proc_Soc_Soh(&RackPkt);
				Proc_SensorCheck();
				break;
			case 85:
				if(g_dbg[11] == 1) break;
				Proc_Detection(&RackPkt);
				//Relay_Proc();
				Proc_Adc();
				Uart1_Proc();
				if(++cnt>=10){
					cnt = 0;
					BTMS_Proc();
				}break;
			case 92:
				if(g_dbg[12] == 1) break;
				Proc_Check_CommStatus(&RackPkt);
				Proc_EepSave();
				if(++g_f1s>=10u){
					RTC_SaveSocToMcp7940(RackPkt.rtc);
					g_f1s = 0;
				}
			  if((g_sMn.mn_type & ((u16)1U << (u16)MN_RS485_BT_TYPE)) != 0U){
					g_sMn.mn_type &= ~(1u << (u16)MN_RS485_BT_TYPE);
					//BTMS_485Tx_Data(g_sMn.mn_btms_cmd,g_sMn.mn_btms_subcmd);
				}
				

				break; 
			case 96:
				if(g_ad.eSpiNum == SPI1_Type){
					g_ad.eSpiNum = SPI2_Type;
				}else{
					g_ad.eSpiNum = SPI1_Type;
				}
				
				//MVB_Proc();
				Can_Proc();  //외부 독립 타이머 사용
				break;
			default:
			  break;
				/* tick 39~79: SM 1ms step 실행 */
				
			}
			
			if(g_sSysTick.u16SysTick >=36U && g_sSysTick.u16SysTick < 76U){
			
				Can_TxProc(McuCan_Ext_Type);
				Adbms6815_SM_Proc();				
			}

#ifdef EN_CPULOAD
#if 0
			CpuLoadTest();
			g_u8fIdle = 1U;
			if ( (u32)(HAL_GetTick() - g_u32LastTime) >= 1000UL){
				g_u32LastTime = HAL_GetTick();
				g_f32CpuLoad = GetCpuLoad();
			}
#endif
#endif
		}
#ifdef PWR_SLEEP_UPATE                                    //2025-12-08   jkpark  Sleep 모드 적용.
    __WFI();
#endif //PWR_STANBAY_UPATE
	}
}

void dbg_qac(void)
{
	if(g_f1s == 0U){}
	if(fCheckRtc == 0U){}

	if(g_u8fIdle == 0U){}
	if(g_u32CntIdle == 0U){}
	if(g_u32CntTotal == 0U){}
	if(g_u32CntCpuLoad == 0U){} //  168Mhz (28000) 70%
	if(g_f32CpuLoad == 0.0f){}
	if(g_u32dummy == 0U){}
	if(g_fratio == 0.0f){}
	if(g_u32LastTime == 0U){}

	if(g_u32Adc1_Buf[ ADC1_CH_NUM ][ ADC_BUF_SIZE ] == 0U){}
	if( g_u32Adc2_Buf[ ADC2_CH_NUM ][ ADC_BUF_SIZE ] == 0U){}
	if( g_u32Adc3_Buf[ ADC3_CH_NUM ][ ADC_BUF_SIZE ] == 0U){}
	if( g_u32Adc1_index==0U){}
	if( g_u32Adc2_index==0U){}
	if( g_u32Adc3_index==0U){}
}



