/*=================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(CAN monitoring)	        	                                       *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2026-01-12                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_can_mon.h
\author             KKD
\date               2026-01-12 
\brief              CAN monitoring
*********************************************
* History:
* 2026-01-12     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_can_mon.h>

/* Private define ---------------------------------------------------------------------------------*/
#define BMS_INFO		(u32)(0x10)
#define BMS_TIMEDATE	(u32)(0x11)
#define BMS_SUMMERY		(u32)(0x12)

#define BMS_SUMRY_PV	(u32)(0x13)
#define BMS_SUMRY_CV	(u32)(0x14)
#define BMS_SUMRY_TH	(u32)(0x15)
#define BMS_POS_PVTH	(u32)(0x16)

#define BMS_INT_ADC		(u32)(0x17)

#define BMS_SUMMERY		(u32)(0x12)



/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
extern u32 maxT[51];
/* Private variables ------------------------------------------------------------------------------*/
//u8 g_mok_bal			= (u8)((NSLV)/7u);	//1
//u8 g_na_bal			= (u8)((NSLV)%7u);	//2

//u8 g_mok_aderr		= (u8)((NSLV)/3u);	//3 	
//u8 g_na_aderr 		= (u8)((NSLV)%3u);	//0

/* Private function prototypes --------------------------------------------------------------------*/
void  	Can_Proc(void);
void	Can_Tx_TimeDate(void);
void	Can_Tx_Summery(void);
void	Can_Tx_Summery_PV(void);
void	Can_Tx_Summery_CV(void);
void	Can_Tx_Summery_TH(void);
void 	Can_Tx_AlarmDet(void);
void 	Can_Tx_AdCvComFail(u8 u8SlvPos,eAdSpi_Type eSpiNum);
void 	Can_Tx_BattCfg_Info(void);
void 	Can_Tx_Balancing(u8 u8SlvPos,eAdSpi_Type eSpiNum);
void 	Can_Tx_IO(void);

void	Can_Tx_Cv(u8 u8Pos);
void 	Can_Tx_Th(u8 u8Pos);
void 	Can_Tx_Sen_LHD(void);
void	Can_Tx_Sen_IMD(void);
void	Can_Tx_Sen_GAS(void);
void	Can_Tx_Hall_Cab1500(void);
void	Can_Tx_Supply_LowV(void);
void	Can_Tx_Supply_HighV(void);
void 	Can_Tx_Btms_Status(void);


/* Private functions ------------------------------------------------------------------------------*/
void Can_Tx_Info(void)
{
	u8 txdat[8]	= {0};
	u16 ver[3]		= {0};
	u32 idx     = (u32)(0x10);

	ver[0] = 002;	//260409 _ CAN에서 Cell 동시 올라오는거 수정 memcpy인데 포지션 고정으로 버그.
	ver[1] = 110;
	ver[2] = 110;
	
	memcpy(&txdat[0],(u16*)&ver[0],sizeof(ver));
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}


void Can_Tx_TimeDate(void)
{
	u8 txdat[8]	= {0};
	u32 idx     = (u32)(0x11);

	memcpy(&txdat[0],&RackPkt.rtc[0],sizeof(RackPkt.rtc));
	txdat[7] = (u8)g_f32CpuLoad;
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_Summery(void)
{
	u8 txdat[8]	= {0};
	u16 tmp		= 0;
	u32 idx     = (u32)(0x12u);

	tmp  = RackPkt.soc;
	txdat[0] = (u8)(tmp & 0xffu);
	txdat[1] = (u8)((tmp >> 8) & 0xffu);

	tmp  = RackPkt.soh;
	txdat[2] = (u8)(tmp & 0xffu);
	txdat[3] = (u8)((tmp >> 8) & 0xffu);

	if(RackPkt.fdet != 0uL) { txdat[4] = (u8)(1uL<<0); }
	if(RackPkt.falm != 0uL) { txdat[4] |= (u8)(1uL<<1); }

	txdat[5] = g_sMn.mn_start;
	txdat[6] = (u8)g_sRelayStatus.u8HvRly_CmdStatus;
	txdat[7] = (u8)g_sRelayStatus.u8HvRly_Feedback;
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}
void Can_Tx_Summery_PV(void)
{
	u8 	txdat[8]	= {0};
	u16	tmp		= 0;
	u32	idx     	= (u32)(0x13u);
	//Todo...
	tmp = (u16)(float)((float)RackPkt.pi[IREAL] * 0.01f);
	txdat[0] = (u8)tmp & 0xffu;
	txdat[1] = (u8)(u8)(tmp >> 8) & 0xffu;

	tmp = (u16)(float)((float)RackPkt.pv[PVINP] * 0.01f);
	txdat[2] = (u8)tmp & 0xffu;
	txdat[3] = (u8)(tmp >> 8) & 0xffu;

	tmp = (u16)(float)((float)RackPkt.pv[PVOUT] * 0.01f);
	txdat[4] = (u8)tmp & 0xffu;
	txdat[5] = (u8)(tmp >> 8) & 0xffu;

	tmp = (u16)(float)((float)RackPkt.pv[PVBTM] * 0.01f);
	txdat[6] = (u8)tmp & 0xffu;
	txdat[7] = (u8)(tmp >> 8) & 0xffu;

	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_Summery_CV(void)
{
	u8 txdat[8]	= {0};
	u16 tmp		= 0;
	u32 idx     = (u32)(0x14);

	tmp	= RackPkt.maxcv;
	txdat[0] = (u8)(tmp & 0xffu);
	txdat[1] = (u8)((tmp >> 8) & 0xffu);
	
	tmp	= RackPkt.mincv;
	txdat[2] = (u8)(tmp & 0xffu);
	txdat[3] = (u8)((tmp >> 8) & 0xffu);

	tmp	= RackPkt.avgcv;
	txdat[4] = (u8)(tmp & 0xffu);
	txdat[5] = (u8)((tmp >> 8) & 0xffu);
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}
void Can_Tx_Summery_TH(void)
{
	u8 txdat[8]	= {0};
	u16 tmp		= 0;
	u32 idx     = (u32)(0x15u);

	tmp	= (u16)RackPkt.maxth;
	txdat[0] = (u8)(tmp & 0xffu);
	txdat[1] = (u8)((tmp >> 8) & 0xffu);
	
	tmp	= (u16)RackPkt.minth;
	txdat[2] = (u8)(tmp & 0xffu);
	txdat[3] = (u8)((tmp >> 8) & 0xffu);

	tmp	= (u16)RackPkt.avgth;
	txdat[4] = (u8)(tmp & 0xffu);
	txdat[5] = (u8)(tmp >> 8) & 0xffu;
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_Pos(void)
{
	u8 txdat[8]   = {0};
	u32 idx 	  = (u32)(0x16u);

	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_Supply_LowV(void)
{
	u8	txdat[8]	= {0};
	u32	idx 	  	= (u32)(0x17u);
	u16	tmp			= 0;

	tmp = g_sAdcData.supp3v3;
	txdat[0] = (u8)(tmp & 0xff);
	txdat[1] = (u8)(tmp >> 8) & 0xff;

	tmp = g_sAdcData.supp5v0;
	txdat[2] = (u8)(tmp & 0xff);
	txdat[3] = (u8)(tmp >> 8) & 0xff;

	tmp = g_sAdcData.supp13v;
	txdat[4] = (u8)(tmp & 0xff);
	txdat[5] = (u8)(tmp >> 8) & 0xff;

	tmp = g_sAdcData.supp24v;
	txdat[6] = (u8)(tmp & 0xff);
	txdat[7]= (u8)(tmp >> 8) & 0xff;
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_AlarmDet(void)
{
	u8 txdat[8]   = {0};
	u32 idx 	  = (u32)(0x18u);
	
	memcpy(&txdat[0],(&RackPkt.falm),sizeof(RackPkt.falm));
	memcpy(&txdat[4],(&RackPkt.fdet),sizeof(RackPkt.fdet));
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_BattCfg_Info(void)
{
	u8	txdat[8]   	= {0};
	u16	tmp			= 0;
	u32	idx 	  	= (u32)(0x19u);

	txdat[0] = NSPI;
	txdat[1] = NSLV;
	txdat[2] = NSCV;
	txdat[3] = NSTH;

	tmp = NSPI*NSLV*NSCV;
	txdat[4] = (u8)tmp & 0xffu;
	txdat[5] = (u8)(tmp >> 8) & 0xffu;
	
	tmp = NSPI*NSLV*NSTH;
	txdat[6] = (u8)tmp & 0xffu;
	txdat[7] = (u8)(tmp >> 8) & 0xffu;
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_IO(void)
{
	u8 txdat[8]	= {0};
	u32 idx     = (u32)(0x20u);

	if(RackPkt.hvbat_on == (u8)TRUE){
		txdat[0] |= (1<<0);
	}
	
	if(RackPkt.charge_on == (u8)TRUE){
		txdat[0] |= (1<<1);
	}	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}

void Can_Tx_Hall_Cab1500(void)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x21U); 																						//KKD 2025-11-26 송신 ID 설정
	u32 tmp			= 0L;
	
	if(g_sHallData.u16ErrStatus & ((u16)1<<(u16)HErr_GoalViol_Type)) { txdat[0] = 1U; }
	if(g_sHallData.u16ErrStatus & ((u16)1<<(u16)HErr_InterErr_Type)) { txdat[0] |= (1U<<1); }
	if(g_sHallData.u16ErrStatus & ((u16)1<<(u16)HErr_SupPwr_Type)) { txdat[0] |= (3U<<2); }

	tmp = (u32)g_sHallData.s32AnalMovAvg;
	txdat[1] = (u8)(tmp >> 16U) & 0xffU;
	txdat[2] = (u8)(tmp >> 8U) & 0xffU;
	txdat[3] = (u8)tmp & 0xffU;

	tmp = (s32)g_sHallData.s32DigtMovAvg;
	txdat[4] = (u8)(tmp >> 8U) & 0xffU; 
	txdat[5] = (u8)tmp & 0xffU;

	txdat[7] = g_sHallData.u8Crc;

	//Can_TxFifo(eCanType, idx, (u8*)&txdat, eMcuCan);																											//KKD 2025-11-26 Fifo 에 적재
	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);
}

void Can_Tx_Summery_PVAdc(void)
{
	u8 	txdat[8]	= {0};
	u16	tmp		= 0;
	u32	idx     	= (u32)(0x22u);
	tmp = ADC_GetAdc_PVIN();
	txdat[0] = (u8)tmp & 0xffu;
	txdat[1] = (u8)(tmp >> 8) & 0xffu;

	tmp = ADC_GetAdc_PVOUT();
	txdat[2] = (u8)tmp & 0xffu;
	txdat[3] = (u8)(tmp >> 8) & 0xffu;

	tmp = ADC_GetAdc_PV_BT();
	txdat[4] = (u8)tmp & 0xffu;
	txdat[5] = (u8)(tmp >> 8) & 0xffu;

	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}
void Can_Tx_Btms_Status(void)
{
	u8	txdat[8]	= {0};
	u32	idx 	  	= (u32)(0x23u);
	
	const sBTMS_ParsedDataType *pBtmsPa = BTMS_GetParsedData();

	u32 sts			= pBtmsPa->u32ErrFlags;
	s8 st 			= (s8)(pBtmsPa->s16SupplyTemp & 0xff);
	s8 rt 			= (s8)(pBtmsPa->s16ReturnTemp & 0xff);


	txdat[0] = (u8)pBtmsPa->eLastCmd;
	txdat[1] = (u8)pBtmsPa->u8StartMode;
	txdat[2] = (u8)st;
	txdat[3] = (u8)rt;
	memcpy(&txdat[4],&sts,sizeof(sts));
	

	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}


void Can_Tx_Balancing(u8 u8SlvPos,eAdSpi_Type eSpiNum)
{
	u8	txdat[8]	= {0};
	u32	idx 	  	= (u32)(0x30u);

	if(eSpiNum == 0){
		txdat[0] = u8SlvPos;
	}else{
		txdat[0] = u8SlvPos+3U;
	}
	
	if(u8SlvPos == 2){
		memcpy(&txdat[1],&g_ad.u8CvBalFlag[eSpiNum][7*u8SlvPos],4);
	}else{
		memcpy(&txdat[1],&g_ad.u8CvBalFlag[eSpiNum][7*u8SlvPos],7);
	}
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}


void Can_Tx_AdCvComFail(u8 u8SlvPos, eAdSpi_Type eSpiNum)
{
	u8	txdat[8]	= {0};
	u32	idx 	  	= (u32)(0x31u);

	if(eSpiNum == 0){
		txdat[0] = u8SlvPos;
	}else{
		txdat[0] = u8SlvPos+6;
	}
	
	txdat[1] = g_ad.u8ReadFailCheck[eSpiNum][READ_CV_Type][(u8SlvPos*3)+0]; //0, 3, 6
	txdat[2] = g_ad.u8ReadFailCheck[eSpiNum][READ_TH_Type][(u8SlvPos*3)+0];

	txdat[3] = g_ad.u8ReadFailCheck[eSpiNum][READ_CV_Type][(u8SlvPos*3)+1]; //1, 4, 7
	txdat[4] = g_ad.u8ReadFailCheck[eSpiNum][READ_TH_Type][(u8SlvPos*3)+1];

	txdat[5] = g_ad.u8ReadFailCheck[eSpiNum][READ_CV_Type][(u8SlvPos*3)+2]; //2, 5, 8
	txdat[6] = g_ad.u8ReadFailCheck[eSpiNum][READ_TH_Type][(u8SlvPos*3)+2];
	
	Can_TxFifo(Can_Extended_Type, idx, txdat, McuCan_Ext_Type);
}






/*
void Can_Tx_08_AdThComFail(u8 u8SlvPos)
{
	u8 txdat[8]   = {0};
	u32 idx 	  = (u32)(0x17);

	txdat[0] = 0+u8SlvPos;

	if(u8SlvPos < 3){
		memcpy(&txdat[1],&g_ad.u8ReadFailCheck[0][(u8SlvPos*7)],sizeof(txdat));
	}else{
		memcpy(&txdat[1],&g_ad.u8ReadFailCheck[0][(u8SlvPos*7)],4);
	}
	Can_TxFifo(eCanType, idx, txdat, eMcuCan);/
}
*/


/*Cell 288s*/
void Can_Tx_Cv(u8 u8Pos)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x100uL+(u32)u8Pos); //0x1000~0x1113     288s                                              										//KKD 2025-11-26 송신 ID 설정

	//memcpy(&txdat[0],(u16*)&RackPkt.TrayPkt[0].cell[0u+(u8Pos*4u)], 8u);

	txdat[0] = (u8)RackPkt.TrayPkt[0].cell[0u+(u8Pos*4u)] & 0xffu;
	txdat[1] = (u8)(RackPkt.TrayPkt[0].cell[0u+(u8Pos*4u)] >> 8) & 0xffu;

	txdat[2] = (u8)RackPkt.TrayPkt[0].cell[1u+(u8Pos*4u)] & 0xffu;
	txdat[3] = (u8)(RackPkt.TrayPkt[0].cell[1u+(u8Pos*4u)] >> 8) & 0xffu;

	txdat[4] = (u8)RackPkt.TrayPkt[0].cell[2u+(u8Pos*4u)] & 0xffu;
	txdat[5] = (u8)(RackPkt.TrayPkt[0].cell[2u+(u8Pos*4u)] >> 8) & 0xffu;

	txdat[6] = (u8)RackPkt.TrayPkt[0].cell[3u+(u8Pos*4u)] & 0xffu;
	txdat[7] = (u8)(RackPkt.TrayPkt[0].cell[3u+(u8Pos*4u)] >> 8) & 0xffu;
	
	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);																									//KKD 2025-11-26 Fifo 에 적재
}

void Can_Tx_Th(u8 u8Pos)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x200uL+(u32)u8Pos); //0x1000~0x1113																								//KKD 2025-11-26 송신 ID 설정

	//memcpy(&txdat[0],(s16*)&RackPkt.TrayPkt[0].temp[0u+(u8Pos*4u)], 8u);
	//Can_TxFifo(eCanType, idx, (u8*)&txdat, eMcuCan);																											//KKD 2025-11-26 Fifo 에 적재
	txdat[0] = (u8)RackPkt.TrayPkt[0].temp[0u+(u8Pos*4u)] & 0xffu;
	txdat[1] = (u8)(RackPkt.TrayPkt[0].temp[0u+(u8Pos*4u)] >> 8) & 0xffu;

	txdat[2] = (u8)RackPkt.TrayPkt[0].temp[1u+(u8Pos*4u)] & 0xffu;
	txdat[3] = (u8)(RackPkt.TrayPkt[0].temp[1u+(u8Pos*4u)] >> 8) & 0xffu;

	txdat[4] = (u8)RackPkt.TrayPkt[0].temp[2u+(u8Pos*4u)] & 0xffu;
	txdat[5] = (u8)(RackPkt.TrayPkt[0].temp[2u+(u8Pos*4u)] >> 8) & 0xffu;

	txdat[6] = (u8)RackPkt.TrayPkt[0].temp[3u+(u8Pos*4u)] & 0xffu;
	txdat[7] = (u8)(RackPkt.TrayPkt[0].temp[3u+(u8Pos*4u)] >> 8) & 0xffu;

	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);
}

void Can_Tx_Sen_LHD(void)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x3000U); 																						//KKD 2025-11-26 송신 ID 설정
	
	txdat[0] |= ((u8)GPIO_DIN_LHD_Open()<<0);
	txdat[0] |= ((u8)GPIO_DIN_LHD_Short()<<1);
	
	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);
}

void Can_Tx_Sen_IMD(void)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x3001U); 		

	memcpy(txdat,&g_u8SenIcRxData[SenIC_IMD_Type],8);
	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);
}

void Can_Tx_Sen_GAS(void)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x3002U); 		

	memcpy(txdat,&g_u8SenIcRxData[SenIC_GAS_Type],8);
	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);
}



void Can_Tx_SM_Timer(u8 u8Pos)
{
	u8 txdat[8] 	= {0};
	u32 idx 		= (u32)(0x3003U+u8Pos); 		
	u16 tmp			= 0;
	if(u8Pos == 0){
		tmp =(u16)(maxT[SM1_Type]*0.01f);		memcpy(&txdat[0],&tmp,2);
		tmp =(u16)(maxT[SM2_Type]*0.01f);		memcpy(&txdat[2],&tmp,2);
		tmp =(u16)(maxT[SM4_Type]*0.01f);		memcpy(&txdat[4],&tmp,2);
		tmp =(u16)(maxT[SM7_Type]*0.01f);		memcpy(&txdat[6],&tmp,2);
	}else if(u8Pos == 1){
		tmp =(u16)(maxT[SM9_Type]*0.01f);		memcpy(&txdat[0],&tmp,2);
		tmp =(u16)(maxT[SM13_Type]*0.01f);		memcpy(&txdat[2],&tmp,2);
		tmp =(u16)(maxT[SM15_Type]*0.01f);		memcpy(&txdat[4],&tmp,2);
		tmp =(u16)(maxT[SM17_Type]*0.01f);		memcpy(&txdat[6],&tmp,2);
	}else if(u8Pos == 2){
		tmp =(u16)(maxT[SM22_Type]*0.01f);		memcpy(&txdat[0],&tmp,2);
		tmp =(u16)(maxT[SM23_Type]*0.01f);		memcpy(&txdat[2],&tmp,2);
		tmp =(u16)(maxT[SM29_Type]*0.01f);		memcpy(&txdat[4],&tmp,2);
		tmp =(u16)(maxT[SM31_Type]*0.01f);		memcpy(&txdat[6],&tmp,2);
	}else if(u8Pos == 3){
		tmp =(u16)(maxT[SM41_Type]*0.01f);		memcpy(&txdat[0],&tmp,2);
		tmp =(u16)(maxT[SM44_Type]*0.01f);		memcpy(&txdat[2],&tmp,2);
		tmp =(u16)(maxT[SM45_Type]*0.01f);		memcpy(&txdat[4],&tmp,2);
	}

	Can_TxFifo(Can_Extended_Type, idx, (u8*)&txdat, McuCan_Ext_Type);
}





void Can_Proc(void)
{
#ifdef CAN_BUF_UPATE    //2025-12-02   jkpark  Can Buffer 관련 수정.
    u32	readId	= 0;

    while( Can_RxParser( &readId ) == MS_SUCCESS );

    //jkpark 2025-12-05 Rx로 명령을 받지 않아도 주기적으로 실행될 필요가 있는 경우 처리.
    HALL_Proc(void)();
    GAS_Proc();
    IMD_Proc();
    
#else  //CAN_BUF_UPATE

  	u8 cnt = 0;
	Can_Tx_Info();
	Can_Tx_TimeDate();																								// KKD 2025-11-26	timddate  
	Can_Tx_Summery();																								// KKD 2026-01-12	soc/soh/alm/det
	Can_Tx_Summery_PV();																							// KKD 2026-01-12	pv in/out/batt _ adding (Todo. . .)
	Can_Tx_Summery_CV();																							// KKD 2026-01-12	cv avg/max/min
	Can_Tx_Summery_TH();																							// KKD 2026-01-12	th avg/max/min
	Can_Tx_AlarmDet();	
	Can_Tx_BattCfg_Info();
	Can_Tx_IO();
	Can_Tx_Summery_PVAdc();
	
	Can_Tx_Btms_Status();


	
	for(cnt=0;cnt<(u8)(NSLV*2u*NSPI);cnt++){
		Can_Tx_Cv(cnt);
	}
	//72
	for(cnt=0;cnt<(u8)(NSLV*NSPI);cnt++){
		Can_Tx_Th(cnt);
	}
	//36
	for(cnt=0; cnt < 3;cnt++) { 
		Can_Tx_Balancing(cnt,SPI1_Type);
		Can_Tx_Balancing(cnt,SPI2_Type);
	}
	//6
	for(cnt=0; cnt < 6;cnt++) { 
		Can_Tx_AdCvComFail(cnt,SPI1_Type); 
		Can_Tx_AdCvComFail(cnt,SPI2_Type); 
	}
	//12
	for(cnt=0;cnt<4;cnt++){
		//Can_Tx_SM_Timer(cnt);
	}
	//15
	
	Can_Tx_Hall_Cab1500();								
	Can_Tx_Sen_LHD();
	Can_Tx_Supply_LowV();
	Can_Tx_Sen_IMD();
	Can_Tx_Sen_GAS();
#endif //CAN_BUF_UPATE
}

/* ================================================================
 * Can_Independent / Can_Independent_Init
 * ------------------------------------------------------------------
 * HAL_GetTick() 기반 정확한 100ms 주기로 Can_Proc()을 호출한다.
 * SM_Delay_us_YieldCan() 루프 내에서도 호출되므로,
 * SM이 수십 ms 블로킹하더라도 CAN 100ms 주기가 유지된다.
 * += 100U 방식으로 드리프트 누적을 방지한다.
 * ================================================================ */
static u32 s_u32CanProcLast = 0U;

void Can_Independent_Init(void)
{
    s_u32CanProcLast = HAL_GetTick();
}

void Can_Independent(void)
{
    if((HAL_GetTick() - s_u32CanProcLast) >= 100U)
    {
        s_u32CanProcLast += 100U;
        Can_Proc();
    }
}

