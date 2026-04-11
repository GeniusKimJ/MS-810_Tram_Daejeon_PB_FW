/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P((bms process(V/C/T Cal or Calibration)))                            *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_process.h
\author             KKD
\date               2025-11-24 
\brief              (bms process(V/C/T Cal or Calibration))
*********************************************

* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_process.h"
#include "ms_rs232_mon.h"

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------*/

/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

void Proc_Calcul_CvTh(Pkt_Rack *pBmsData)
{
	s16	max = 0, min = 0;
	u16	cnt 	= 0;
	u16	min_num	= 0;
	u16	max_num	= 0;
	s32	avgth	= 0;
	u32	avgsum	= 0;
	u32	u32_buf	= 0;

	//if(g_sMn.mn_type & (u8)(1U<< (u8)MN_CELL_TYPE)){
	if((g_sMn.mn_type & ((u16)1U << (u16)MN_CELL_TYPE)) != 0U){		// static analizye
	    memcpy(&pBmsData->TrayPkt[0].cell[0],&g_sMn.mn_cv[0],sizeof(g_sMn.mn_cv));
	}else{
	    memcpy(&pBmsData->TrayPkt[0].cell[0],&g_ad.u16Cv[0],sizeof(g_ad.u16Cv));
	}
	//if(g_sMn.mn_type & (1<<MN_TEMP_TYPE)){
	if((g_sMn.mn_type & ((u16)1U << (u16)MN_TEMP_TYPE)) != 0U){		// static analizye
	    memcpy(&pBmsData->TrayPkt[0].temp[0],&g_sMn.mn_th[0],sizeof(g_sMn.mn_th));
	}else{
	    memcpy(&pBmsData->TrayPkt[0].temp[0],&g_ad.s16Th[0],sizeof(g_ad.s16Th));
	}

	if((g_sMn.mn_type & ((u16)1U << (u16)MN_DET_TYPE)) != 0U){
		for(u16 pos=0; pos<NCV; pos++){
			pBmsData->TrayPkt[0].cell[pos] = (u16)((s32)pBmsData->TrayPkt[0].cell[pos] + g_sMn.offset_cv);
		}
	}
	avgth = 0;
	max = (s16)-32768;
	min = (s16)32767;
	for(cnt = 0; cnt < (NTH); cnt++) {
		if(pBmsData->TrayPkt[0].temp[cnt] > max) {
			max = pBmsData->TrayPkt[0].temp[cnt];
			max_num = cnt;
		}
		if(pBmsData->TrayPkt[0].temp[cnt] < min) {
			min = pBmsData->TrayPkt[0].temp[cnt];
			min_num = cnt;
		}
		avgth += pBmsData->TrayPkt[0].temp[cnt];
	}
	pBmsData->maxth		= max;
	pBmsData->maxthn	= (u8)((u8)max_num+1U);
	pBmsData->minth		= min;
	pBmsData->minthn	= (u8)((u8)min_num+1U);
	//pBmsData->avgth  	= (s16)avgth/(s16)(MAXNTH-2U);
	pBmsData->avgth 	= (s16)((s32)avgth/(s32)(NTH));
	pBmsData->diffth	= max-min;
	
	avgsum	= 0;
	max		= 0;
	min		= 32767;
	for(cnt = 0; cnt < NCV; cnt++) {
		if(pBmsData->TrayPkt[0].cell[cnt] > (u32)max) {
			max = (s16)pBmsData->TrayPkt[0].cell[cnt];
			max_num 	= cnt;
		}
		if(pBmsData->TrayPkt[0].cell[cnt] < (u32)min) {
			min = (s16)pBmsData->TrayPkt[0].cell[cnt];
			min_num 	= cnt;
		}
		avgsum += pBmsData->TrayPkt[0].cell[cnt];
	}
	pBmsData->maxcv		= (u16)max;
	pBmsData->maxcvn	= max_num+(u16)1;
	pBmsData->mincv		= (u16)min;
	pBmsData->mincvn	= min_num+(u16)1;
	pBmsData->sumcv		= avgsum;
	//pBmsData->pv[PVINP]	= pBmsData->sumcv;
	
	pBmsData->pv[PVINP]	= (u32)ADC_Get_PVIN();
	pBmsData->pv[PVOUT]	= (u32)ADC_Get_PVOUT();
	pBmsData->pv[PVBTM]	= (u32)ADC_Get_PV_BT();
	
	u32_buf				= avgsum/(u32)(NCV);
	pBmsData->avgcv		= (u16)(u32_buf);
	pBmsData->diffcv	= max-min;
}

s32 Proc_MovAvg(s32 u32Data, eMovAvg ePos){
	static s32	s_lawbuff[MAX_MOV][MOVCNT] = {0};
	static u8	s_first[MAX_MOV]	= {0};
	static u8	s_movcnt[MAX_MOV] 	= {0};
	static u8	s_movpos[MAX_MOV] 	= {0};
	s32 		movsum[MAX_MOV]		= {0};
	s32 		movavg[MAX_MOV]		= {0};
	u8			i = 0;
	
	if(s_first[ePos] == 0U){
		s_first[ePos] = 1;
		for(i=0;i<s_movcnt[ePos];i++){
			s_lawbuff[ePos][i] = u32Data;
		}
	}else{
		s_lawbuff[ePos][s_movpos[ePos]] = u32Data;
	}

	if(++s_movpos[ePos] >= (u8)MOVCNT){
		s_movpos[ePos] = 0;
	}
	if(++s_movcnt[ePos] >= (u8)MOVCNT){
		s_movcnt[ePos] = (u8)MOVCNT;
	}

	for(i=0;i<s_movcnt[ePos];i++){
		movsum[ePos] += s_lawbuff[ePos][i];
	}

	if(s_movcnt[ePos] == 0U){
		movavg[ePos] = 0;
	}else{
		movavg[ePos] = (movsum[ePos]/(s32)s_movcnt[ePos]);
	}

	return movavg[ePos];
}

void Proc_SensorCheck(void)
{
	IMD_Proc();
	HALL_Proc();
	Fire_Proc();
	GAS_Proc();
}

void Proc_Soc_Soh(Pkt_Rack *pBmsData)
{
	pBmsData->pi[I1SEC] = HALL_GetCurr1Sec();
	pBmsData->pi[IREAL] = HALL_GetAnalogAmp();
	
	SOC_CycleCount(pBmsData->pi[IREAL],pBmsData->ratefullcap);
	SOC_StateOfCharge(pBmsData->ratefullcap, pBmsData->avgcv,pBmsData->pi[I1SEC],pBmsData->fdet );
	SOC_StateOfHealth(pBmsData->cyccnt);	
}



u8 Proc_Charing(void)
{
	static u16 chkcnt = 0;
	u8 result = 0;
	if(RackPkt.pi[IREAL] > (s32)(3000)){
		if(++chkcnt>=30){	//3sec
			chkcnt = 30;
			result = 1;
		}
	}else{
		chkcnt = 0;
		result = 0;
	}return result;
}
u8 Proc_Discharing(void)
{
	static u16 chkcnt = 0;
	u8 result = 0;
	if(RackPkt.pi[IREAL] < (s32)(-3000)){
		if(++chkcnt>=30){	//3sec
			chkcnt = 30;
			result = 1;
		}
	}else{
		chkcnt = 0;
		result = 0;
	}return result;
}
void Proc_Check_CommStatus(Pkt_Rack *pBmsData)
{
	u16 tmp = 0;
	//u16 chkerr = 0;
	//Rs422_Mvb
	tmp = (u16)MVB_GetTimeoutStatus();
	if(tmp != 0){
		//pBmsData->fdet |= (FMVCFP);
	}
	
	//Rs485_Btms
	//tmp = (u16)BTMS_GetTimeoutStatus();
	if(tmp != 0){
		//pBmsData->fdet |= (FBTCFP);
	}
	//Can_IMD
	tmp =IMD_GetErrStatus();
	if(tmp == (u16)IErr_CanRxTimeout_Type){
		//pBmsData->fdet |= (FIMDCFP);
	}

	//Can_Gas
	tmp =GAS_GetErrStatus();
	if(tmp == (u16)GErr_CanRxTimeout_Type){
		//pBmsData->fdet |= (FGASCFP);
	}

	//Can_Hall
	tmp = HALL_GetErrStatus();
	if(tmp == (u16)HErr_CanRxTimeout_Type){
		//pBmsData->fdet |= (FHALCFP);
	}
}		

void Proc_EepSave(void)
{
	EEP_SaveNormalData();		//savedata1
	EEP_SaveFaultData();		//savedata2
}

void Proc_Adc(void)
{
	ADC1_Ch0_GetAd24V();
	ADC1_Ch1_GetAd13V();
	ADC1_Ch2_GetAd5V0();
	ADC1_Ch3_GetAd3V3();
	ADC1_Ch10_GetPIL();			//cab1500 current

	
	if((g_sMn.mn_type & ((u16)1U << (u16)MN_PACKV_TYPE)) != 0U){		
		ADC_SetAdc_PVIN(g_sMn.mn_set_pv_in);
		ADC_SetAdc_PVOUT(g_sMn.mn_set_pv_ou);
		ADC_SetAdc_PV_BT(g_sMn.mn_set_pv_bt);
	}else{
		ADC2_Ch0_GetCPV();
		ADC2_Ch1_GetAVPV();
		ADC2_Ch2_GetBtmsVPV();
	}
}

void Proc_Detection(Pkt_Rack *pBmsData)
{
	pBmsData->falm = Alarm_Occur();
	pBmsData->falm = Alarm_Release();
	pBmsData->fdet = Det_Occur();
	pBmsData->fdet = Det_Release();
}
void Proc_Bms_Init(void)
{
	s32 readdata;

	/* Manual Test - Sturct init*/	
	memset(&g_sMn.mn_start,0x00,sizeof(g_sMn));

	RTC_Init();

	/*EEP*/
	EEP_ReadSysInfo();
	EEP_ReadCalData();
	EEP_Read_Gain_PvIN();
	EEP_Read_Gain_PvOU();
	EEP_Read_Gain_PvBT();
	EEP_Read_Gain_ax24v();
	EEP_Read_Gain_ax13v();
	EEP_Read_Gain_ax5v();
	EEP_Read_Gain_ax3v();
	//EEP_Read_CalData(CalPos_Pv_In_Type, &readdata);
	

	/*GPIO*/
	Gpio_Init();

	/*COMM*/
	Can_Init();
	Uart1_Init();
  	//MVB_Init(MVB_MODE_POLLING);                                          //KKD 2025-11-26 DMA 폴링모드 시작
  	BTMS_Init();

	/*SENSOR*/
	HUMI_Init();
	HALL_Init();
	IMD_Init();
	GAS_Init();
	Fire_Init();

	/*ADC*/
  	ADC_InitAll();
	SOC_Init();
	
	/*CV TH*/
	Adbms6815_Init();
	Adbms6815_SM_Init();
	
	/*CONTECTOR*/	
	Relay_Init();
}

void Proc_Check_Status(Pkt_Rack *pBmsData)
{
	u16	fsts = 0U;
	
	if(Proc_Charing() == 1U){
		fsts |= FCHGING;
	}else if(Proc_Discharing() == 1U){
		fsts |= FDCHING;
	}else{
		//fsts |= 
	}

	pBmsData->fbmssts = fsts;
}

void Proc_Calibration(Enum_CalPos_Type calpos)
{


	if(g_sMn.mn_ref_data <= 0) {return;}
	
	if(calpos == CalPos_Curr_Gain_Type){
		//todo . . .
		//s124 Gain
	}else if(calpos == CalPos_Curr_Offset_Type){
		//todo . . .
		//s124 offset
	}else if(calpos == CalPos_Pv_In_Type){
		/* ── PV IN (CPV) 캘리브레이션 (복리 없는 방식) ──────────────────────────
		 * 공식: gain_u16 = ref × 10000 / ADC15_TO_HV_mV(adc_raw)
		 *  - adc_raw은 HW 고정값 → 반복 캘 시 항상 동일한 gain 계산 (복리 원천 차단)
		 *  - 이전 방식(RackPkt.pv[PVINP] 사용): gain 보정된 값 → 매번 누적 문제
		 *  검증: pv = ADC15_TO_HV_mV(adc)×gain×0.0001 = ADC15_TO_HV_mV×(ref×10000/HV_mV)×0.0001 = ref ✓ */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN4_CPV);
			if(s32_adc_raw > 0) {
				float f32_hv_mv = (float)ADC15_TO_HV_mV(s32_adc_raw);
				if(f32_hv_mv > 0.0f) {
					g_sAdcGain.gain_pv_in = (u16)((float)g_sMn.mn_ref_data * 10000.0f / f32_hv_mv);
					EEP_Write_Gain_PvIN(g_sAdcGain.gain_pv_in);
					HAL_Delay(10);
				}
			}
		}
	}else if(calpos == CalPos_Pv_Ou_Type){
		/* ── PV OUT (VPV) 캘리브레이션 (복리 없는 방식) */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN5_VPV);
			if(s32_adc_raw > 0) {
				float f32_hv_mv = (float)ADC15_TO_HV_mV(s32_adc_raw);
				if(f32_hv_mv > 0.0f) {
					g_sAdcGain.gain_pv_ou = (u16)((float)g_sMn.mn_ref_data * 10000.0f / f32_hv_mv);
					EEP_Write_Gain_PvOU(g_sAdcGain.gain_pv_ou);
					HAL_Delay(10);
				}
			}
		}
	}else if(calpos == CalPos_Pv_Bt_Type){
		/* ── PV BT (TVPV) 캘리브레이션 (복리 없는 방식) */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN6_TVPV);
			if(s32_adc_raw > 0) {
				float f32_hv_mv = (float)ADC15_TO_HV_mV(s32_adc_raw);
				if(f32_hv_mv > 0.0f) {
					g_sAdcGain.gain_pv_bt = (u16)((float)g_sMn.mn_ref_data * 10000.0f / f32_hv_mv);
					EEP_Write_Gain_PvBT(g_sAdcGain.gain_pv_bt);
					HAL_Delay(10);
				}
			}
		}
	}else if(calpos == CalPos_Ax_24V_Type){
		/* ── 24V 캘리브레이션 (복리 없는 방식) ──────────────────────────────
		 * 공식: gain_u16 = ref_mV × 10000 / adc_raw
		 *  검증: ax_24v = adc_raw × gain × 0.0001 = adc_raw × (ref×10000/adc_raw) × 0.0001 = ref ✓ */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN0_24V);
			if(s32_adc_raw > 0) {
				g_sAdcGain.gain_ax_24v = (u16)((float)g_sMn.mn_ref_data * 10000.0f / (float)s32_adc_raw);
				EEP_Write_Gain_Ax24v(g_sAdcGain.gain_ax_24v);
				HAL_Delay(10);
			}
		}
	}else if(calpos == CalPos_Ax_13V_Type){
		/* ── 13V 캘리브레이션 (복리 없는 방식) */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN1_13V);
			if(s32_adc_raw > 0) {
				g_sAdcGain.gain_ax_13v = (u16)((float)g_sMn.mn_ref_data * 10000.0f / (float)s32_adc_raw);
				EEP_Write_Gain_Ax13v(g_sAdcGain.gain_ax_13v);
				HAL_Delay(10);
			}
		}
	}else if(calpos == CalPos_Ax_5V_Type){
		/* ── 5V 캘리브레이션 (복리 없는 방식) */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN2_5V);
			if(s32_adc_raw > 0) {
				g_sAdcGain.gain_ax_5v = (u16)((float)g_sMn.mn_ref_data * 10000.0f / (float)s32_adc_raw);
				EEP_Write_Gain_Ax5v(g_sAdcGain.gain_ax_5v);
				HAL_Delay(10);
			}
		}
	}else if(calpos == CalPos_Ax_3V_Type){
		/* ── 3.3V 캘리브레이션 (복리 없는 방식) */
		{
			s32 s32_adc_raw = ADC_GetStmAdc15bit(ID_ADIN3_3V3);
			if(s32_adc_raw > 0) {
				g_sAdcGain.gain_ax_3v = (u16)((float)g_sMn.mn_ref_data * 10000.0f / (float)s32_adc_raw);
				EEP_Write_Gain_Ax3v(g_sAdcGain.gain_ax_3v);
				HAL_Delay(10);
			}
		}
	}

	/* 캘리브레이션 완료 후 현재 게인값을 CAN 0x701 로 1회 송신 */
	CAN_Tx_GainValues();

}


/* Getter */
s16 Proc_Get_ThAvg(void)	{	return RackPkt.avgth;	}
s16 Proc_Get_ThMax(void)	{	return RackPkt.maxth;	}
s16 Proc_Get_ThMin(void)	{	return RackPkt.minth;	}

u16 Proc_Get_CvAvg(void)	{	return RackPkt.avgcv;	}
u16 Proc_Get_CvMax(void)	{	return RackPkt.maxcv;	}
u16 Proc_Get_CvMin(void)	{	return RackPkt.mincv;	}
u16 Proc_Get_CvDiff(void)	{ return (u16)(RackPkt.maxcv - RackPkt.mincv);	}

