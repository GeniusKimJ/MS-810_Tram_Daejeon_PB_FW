/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Event EEP - I2C)                                           			  *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                               		   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               EEP_m24256a.c
\author             KKD
\date               2025-11-24 
\brief              Event EEP - I2C
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <EEP_m24256a.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
Pkt_EEP		EepPkt[2];
/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

void EEP_Proc(void){
	EEP_SaveNormalData();		//savedata1
	EEP_SaveFaultData();		//savedata2
}


/*************************************** 
** Discrip	: EEP_SaveNormalData
** Date 	: 2024
** Author	: KHR
****************************************/
void EEP_SaveNormalData(void) {
	static u8 	fEepNorWr = 1, fNorSave = 0;
	static u8 	Cnt1S_N = 0;
	//static u16 	fBmsstsSav = 0U;
	static u16 	EepAdr;
	u8 			k, crc, feeperr;
	u8 			*str, *strw;
/*
	if((RackPkt.fbmssts & FPWDOWN) != 0U) {
		if(fBmsstsSav != 0U) {
			fEepNorWr = 1U;
		}
	}
	fBmsstsSav = RackPkt.fbmssts & FPWDOWN;
*/
	switch(Cnt1S_N++) {
	case 0:
		if(fEepNorWr != 0U) {
			fEepNorWr = 0U;
			fNorSave = 1U;
			EEP_SaveData();
			if(RackPkt.norcnt == 0xFFFFU) {
			 	RackPkt.norcnt -= ECNTNOR;
			}
			EepAdr = ((RackPkt.norcnt % ECNTNOR) * PAGEWRT);
			EepPkt[0].errcnt = RackPkt.errcnt;
			str = (void *)&EepPkt[0];
			crc = 0x41;
			for(k = 0; k < (sizeof(Pkt_EEP)-1U); k++) {
			 	crc = TwiCRC[crc ^ str[k]];
			}
			EepPkt[0].crc = crc;
		}
		break;

	case 1:
		if(fNorSave != 0U) {
		 	HAL_I2C_Mem_Write_IT(&hi2c1, SLA24EEP, EepAdr, I2C_MEMADD_SIZE_16BIT, (void *)&EepPkt[0], sizeof(Pkt_EEP));
		}break;

	case 2:
		break;

	case 3:
		if(fNorSave != 0U) {
			HAL_I2C_Mem_Read_IT(&hi2c1, SLA24EEP, EepAdr, I2C_MEMADD_SIZE_16BIT, (void *)&EepPkt[1], sizeof(Pkt_EEP));
		}break;

	default:
		if(fNorSave != 0U) {
			strw = (void *)&EepPkt[0];
			str  = (void *)&EepPkt[1];
			feeperr = 0;
			for(k = 0; k < sizeof(Pkt_EEP); k++) {
				if((str[k]^strw[k]) != 0U) {
					feeperr = 1U;
				}
			}
			++RackPkt.norcnt;
			if(feeperr != 0U) {
				if((RackPkt.fcomsts & FCOMEEP) == 0U) {
					*(u8 *)&RackPkt.fcomsts |= FCOMEEP;
					fEepNorWr = 1U;
				}
			} else {
				*(u8 *)&RackPkt.fcomsts &= ~FCOMEEP;
				fNorSave = 0U;
			}
		}
		Cnt1S_N = 0U;
		break;
	}
}

/*************************************** 
** Discrip	: EEP_SaveFaultData
** Date 	: 2024
** Author	: KHR
****************************************/
void EEP_SaveFaultData(void) {
	static u8 	fErr = 0, fErrSave = 0;
	static u8 	Cnt1S_F = 0;
	static uint32_t 	fDetSav = 0U;
	static u16 	EepAdr;
	uint32_t 			fdet;
	u8 			k, crc, feeperr;
	u8 			*str, *strw;

	fdet = RackPkt.fdet;

	if((fDetSav^fdet) != 0U) {
		fDetSav = fdet;
		if(fErrSave == 0U) {
		 	EEP_SaveData();
		}
		fErr = 1U;
	}

	switch(Cnt1S_F++) {
	case 0:
		if(fErr != 0U) {
			fErr = 0;
			fErrSave = 1;
		}

		if(fErrSave != 0U) {
			if(RackPkt.errcnt == 0xFFFFU) {
			 	RackPkt.errcnt -= ECNTERR;
			}
			EepAdr = ((RackPkt.errcnt % ECNTERR) * PAGEWRT) + EERRSTR;
		 	EepPkt[0].errcnt = RackPkt.errcnt + 1U;
			str = (void *)&EepPkt[0];
			crc = 0x41;
			for(k = 0; k < (sizeof(Pkt_EEP)-1U); k++) {
			 	crc = TwiCRC[crc ^ str[k]];
			}
			EepPkt[0].crc = crc;
		}break;

	case 1:
		if(fErrSave != 0U) {
			HAL_I2C_Mem_Write_IT(&hi2c1, SLA24EEP, EepAdr, I2C_MEMADD_SIZE_16BIT, (void *)&EepPkt[0], sizeof(Pkt_EEP));
		}break;

	case 2:
		break;

	case 3:
		if(fErrSave != 0U) {
			HAL_I2C_Mem_Read_IT(&hi2c1, SLA24EEP, EepAdr, I2C_MEMADD_SIZE_16BIT, (void *)&EepPkt[1], sizeof(Pkt_EEP));
		}break;

	case 4:
		if(fErrSave != 0U) {
			strw = (void *)&EepPkt[0];
			str  = (void *)&EepPkt[1];
			feeperr = 0;
			for(k = 0; k < sizeof(Pkt_EEP); k++) {
				if((str[k]^strw[k]) != 0U) {
					feeperr = 1U;
				}
			}
			++RackPkt.errcnt;
			if(feeperr == 1U) {
				if((RackPkt.fcomsts & FCOMEEP) == 0U) {
					RackPkt.fcomsts |= FCOMEEP;
					fErr = 1U;
				}
			} else {
				RackPkt.fcomsts &= ~FCOMEEP;
				fErrSave = 0U;
			}
		}
		Cnt1S_F = 0U;
		break;
	}
}


void EEP_SaveData(void) {
	memcpy(EepPkt[0].rtc, (void *)RackPkt.rtc, 7);

	//EepPkt[0].flag[0] 	= *((u8 *)&RackPkt.fdet + 0);
	//EepPkt[0].flag[1] 	= *((u8 *)&RackPkt.fdet + 1);
	//EepPkt[0].flag[2]	 	= *((u8 *)&RackPkt.fdet + 2);
	memcpy(&EepPkt[0].flag[0], &RackPkt.fdet, 2U);

	EepPkt[0].flag[3] 		= RackPkt.fcomsts;
	EepPkt[0].flag[4] 		= RackPkt.frlyctr;
	EepPkt[0].flag[5] 		= RackPkt.frlysts;

	//EepPkt[0].flag[6] 	= *((u8 *)&RackPkt.fbmssts + 0);
	//EepPkt[0].flag[7] 	= *((u8 *)&RackPkt.fbmssts + 1);
	memcpy(&EepPkt[0].flag[6], &RackPkt.fbmssts, 2U);

	//EepPkt[0].pv 			= (u16)(RackPkt.pv[PVOUT]/10U);																								// 10[mV]
	EepPkt[0].pv 			= (u16)(RackPkt.pv[PVINP]/10U);																								// 10[mV]
	EepPkt[0].pi 			= (int16_t)(RackPkt.pi[IREAL]/100L);																						// 100[mA]
	EepPkt[0].aux			= RackPkt.aux;
	EepPkt[0].avgcv			= RackPkt.avgcv;
	EepPkt[0].maxcv			= RackPkt.maxcv;

	//EepPkt[0].maxcvn		= (u8)(((NSLV) * RackPkt.maxcvtrayn) + RackPkt.maxcvn);
	//EepPkt[0].maxcvn		= (u8)(RackPkt.maxcvtrayn + RackPkt.maxcvn);
	EepPkt[0].mincv			= RackPkt.mincv;
	//EepPkt[0].mincvn		= (u8)(RackPkt.mincvtrayn + RackPkt.mincvn);
	EepPkt[0].avgth			= RackPkt.avgth;
	EepPkt[0].maxth			= RackPkt.maxth;
	EepPkt[0].maxthn		= (u8)(((NTH) * RackPkt.maxthtrayn) + RackPkt.maxthn);
	EepPkt[0].minth			= RackPkt.minth;
	EepPkt[0].minthn		= (u8)(((NTH) * RackPkt.minthtrayn) + RackPkt.minthn);
	EepPkt[0].soc			= RackPkt.soc;
	EepPkt[0].cap			= RackPkt.datacap/3600U;					// [mAh] Unit
	EepPkt[0].fcap			= RackPkt.ratefullcap/3600U;				// [mAh] Unit
	EepPkt[0].ccnt			= RackPkt.cyccnt;
	EepPkt[0].bmsontime		= RackPkt.bmsontime;
}


void EEP_DataRead(u8 mode, u16 rxcnt, u8 *eepdata) {
	static u16	EepAdrRef;
	u8 	k, crc;
	u16	cnt, adr;
	u8 	*str;

	switch(mode) {
	case 0x01:					// Normal Data
		if(rxcnt == 0U) {		// return Normal Data Num
			if(RackPkt.norcnt >= ECNTNOR) {
				cnt = ECNTNOR;
			} else {
				cnt = RackPkt.norcnt;
			}
			//eepdata[1] = *((u8 *)&cnt + 1);
			//eepdata[0] = *((u8 *)&cnt + 0);
			memcpy(eepdata, &cnt, 2U);

			EepAdrRef = ((RackPkt.norcnt-1U)%ECNTNOR);
		} else {
			if(EepAdrRef >= (rxcnt-1U))	{
//				adr = ((EepAdrRef-rxcnt+1U)*PAGEWRT) + ENORSTR;
				adr = ((EepAdrRef-rxcnt+1U)*PAGEWRT);
			} else {
				adr = EERRSTR - (((rxcnt-1U)-EepAdrRef)*PAGEWRT);
			}
			while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, adr, I2C_MEMADD_SIZE_16BIT, (void *)eepdata, sizeof(Pkt_EEP), 50) != HAL_OK) {}
			//HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, adr, I2C_MEMADD_SIZE_16BIT, (void *)eepdata, sizeof(Pkt_EEP), 50);
			//str = (void *)eepdata;
			crc = 0x41;
			for(k = 0; k < (sizeof(Pkt_EEP)-1U); k++) {
			 	crc = TwiCRC[crc^eepdata[k]];
			}
		}
		break;

	case 0x02:					// Error Data
		if(rxcnt == 0U) {		// return Error Data Num
			if(RackPkt.errcnt >= ECNTERR) {
			 	cnt = ECNTERR;
			} else {
				cnt = RackPkt.errcnt;
			}
			//eepdata[1] = *((u8 *)&cnt + 1);
			//eepdata[0] = *((u8 *)&cnt + 0);
			memcpy(eepdata, &cnt, 2U);
			EepAdrRef = ((RackPkt.errcnt-1U)%ECNTERR);
		} else {
			if(EepAdrRef >= (rxcnt-1U)) {
				adr = ((EepAdrRef-rxcnt+1U)*PAGEWRT) + EERRSTR;
			} else {
				adr = EERREND - (((rxcnt-1U)-EepAdrRef)*PAGEWRT);
			}
			while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, adr, I2C_MEMADD_SIZE_16BIT, (void *)eepdata, sizeof(Pkt_EEP), 50) != HAL_OK) {		}
			//HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, adr, I2C_MEMADD_SIZE_16BIT, (void *)eepdata, sizeof(Pkt_EEP), 50);
			str = (void *)eepdata;
			crc = 0x41;
			for(k = 0; k < (sizeof(Pkt_EEP)-1U); k++) {
				crc = TwiCRC[crc^str[k]];
			}
		}
		break;
	case 0x03:		// BMS On Time
		//memcpy(eepdata, &RackPkt.bmsontime, 4U);
		break;
	}
}


void EEP_DataClear(u8 mode) {
	switch(mode) {
	case 0x01:					// Normal Data
		RackPkt.norcnt = 0;
		break;
	case 0x02:					// Error Data
		RackPkt.errcnt = 0;
		break;
	case 0x03:					// All DataClear
		RackPkt.bmsontime = 0;
		RackPkt.norcnt = 0;
		RackPkt.errcnt = 0;
		//EEP_WriteBalontime_init();
		break;
	}
}

void EEP_ReadCalData(void) {
	u8	k;
	s32		cmpr, cmpg;
	double 	d64_buf[2];
	s32		i32_buf[2];

	for(k = 0U; k < 4U; k++) {
		while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, (u16)(MRATPI + (sizeof(float)*k)), I2C_MEMADD_SIZE_16BIT, (void *)&RatPI[k], sizeof(float), 20) == HAL_BUSY) {
		}
		while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, (u16)(MGABPI + (sizeof(float)*k)), I2C_MEMADD_SIZE_16BIT, (void *)&GabPI[k], sizeof(float), 20) == HAL_BUSY) {
		}
		///cmpr = (RatPI[k] * 1000. - 1000.);
		d64_buf[0] = (double)RatPI[k] * (double)1000.;
		d64_buf[1] = d64_buf[0] - (double)1000.;
		cmpr = (s32)d64_buf[1];

		cmpg = (s32)GabPI[k];

		///if(((labs(cmpr) > 250) || (labs(cmpg) > 10000)) || (!cmpr && !cmpg)) {			// rate : 0.75 ~ 1.25, pi_gab : 10A
		i32_buf[0] = labs(cmpr);
		i32_buf[1] = labs(cmpg);
		if(((i32_buf[0] > (s32)350) || (i32_buf[1] > (s32)10000U)) || ((cmpr == (s32)0U) && (cmpg == (s32)0U)))  {				// rate : 0.75 ~ 1.25, aux_gab : 2[V]
			RatPI[k] = (float)1.;
			GabPI[k] = (float)0.;
		}
		while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, (u16)(MRATPV + (sizeof(float)*k)), I2C_MEMADD_SIZE_16BIT, (void *)&RatPV[k], sizeof(float), 20) == HAL_BUSY) {
		}
		while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, (u16)(MGABPV + (sizeof(float)*k)), I2C_MEMADD_SIZE_16BIT, (void *)&GabPV[k], sizeof(float), 20) == HAL_BUSY) {
		}
		///cmpr = (RatPV[k] * 1000. - 1000.);
		d64_buf[0] = (double)RatPV[k] * (double)1000.;
		d64_buf[1] = d64_buf[0] - (double)1000.;
		cmpr = (s32)d64_buf[1];

		cmpg = (s32)GabPV[k];

		///if(((labs(cmpr) > 250)||(labs(cmpg) > 10000)) || (!cmpr && !cmpg)) {			// rate : 0.75 ~ 1.25, pv_gab : 10V
		i32_buf[0] = labs(cmpr);
		i32_buf[1] = labs(cmpg);
		if(((i32_buf[0] > (s32)250) || (i32_buf[1] > (s32)10000)) || ((cmpr == (s32)0U) && (cmpg == (s32)0U)))  {				// rate : 0.75 ~ 1.25, aux_gab : 2[V]
			RatPV[k] = (float)1.;
			GabPV[k] = (float)0.;
		}
	}

	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MRATAX, I2C_MEMADD_SIZE_16BIT, (void *)&RatAX, sizeof(float), 20) == HAL_BUSY) {
	}
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MGABAX, I2C_MEMADD_SIZE_16BIT, (void *)&GabAX, sizeof(float), 20) == HAL_BUSY) {
	}
	///cmpr = (RatAX * 1000. - 1000.);
	d64_buf[0] = (double)RatAX * (double)1000.;
	d64_buf[1] = d64_buf[0] - (double)1000.;
	cmpr = (s32)d64_buf[1];

	cmpg = (s32)GabAX;

	///if(((labs(cmpr) > 250)||(labs(cmpg) > 2000)) || (!cmpr && !cmpg))  {				// rate : 0.75 ~ 1.25, aux_gab : 2[V]
	i32_buf[0] = labs(cmpr);
	i32_buf[1] = labs(cmpg);
	if(((i32_buf[0] > (s32)250) || (i32_buf[1] > (s32)2000)) || ((cmpr == (s32)0U) && (cmpg == (s32)0U)))  {				// rate : 0.75 ~ 1.25, aux_gab : 2[V]
		RatAX = (float)1.;
		GabAX = (float)0.;
	}
}





void EEP_ReadSysInfo(void) {

	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MFDATA, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.Mf_year, 3U*sizeof(u8), 20U) != HAL_OK) {
	}
	///HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MFDATA, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.Mf_year, 3U*sizeof(u8), 20);
	//if(RackPkt.Mf_year  > 25U)	RackPkt.Mf_year		= 0;   		//25 ?�덇????�?��,
	if(RackPkt.Mf_month > 12U) {
		RackPkt.Mf_month 	= 0U;   		//month
	}
	if(RackPkt.Mf_day > 31U) {
		RackPkt.Mf_day		= 0U;   		//day
	}
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MSRNUM, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.sr_num, sizeof(u16), 20) != HAL_OK) {
	}
	///HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MSRNUM, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.sr_num, sizeof(u16), 20);
}

void EEP_WriteBalontime_init(void){
	//memset(&RackPkt.balontime, 0x00,50);
	//EEP_Writebalontime();
	//EEP_Readbalontime();
}

void EEP_Writebalontime(void){
	HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, MSRNUM, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.balontime,sizeof(RackPkt.balontime),20U);
	HAL_Delay(10);
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MSRNUM, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.balontime,sizeof(RackPkt.balontime),20U) != HAL_OK){
	}
}


void EEP_Readbalontime(void){
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MSRNUM, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.balontime,sizeof(RackPkt.balontime),20U) != HAL_OK){
	}
}

void EEP_WriteHostIpAddr(void){
	HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, MSBONT, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.hostip,sizeof(RackPkt.hostip),20U);
	HAL_Delay(10);
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MSBONT, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.hostip,sizeof(RackPkt.hostip),20U) != HAL_OK){
	}
}
u8 EEP_ReadHostIpAddr(void){
	u8	result = 0;
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, MSBONT, I2C_MEMADD_SIZE_16BIT, (void *)&RackPkt.hostip,sizeof(RackPkt.hostip),20U) != HAL_OK){
	}
	for(u8 i=0U;i<4U;i++){
		if(RackPkt.hostip[i] == 0U){result++;	}
	}
	return result;
}

BOOL EEP_Write_CurrGain(u16 value)
{
	HAL_StatusTypeDef sts_wr;
	HAL_StatusTypeDef sts_rd;
	u8 wr_dat[8]= {0};
	u8 rd_dat[8]= {0};
	int icmp = 0;

	wr_dat[0] = (u8)value & 0xFFu;
	wr_dat[1] = (u8)(value >> 8) & 0xFFu;

// Write
	sts_wr = HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, EEP_PI_GAIN, I2C_MEMADD_SIZE_16BIT, (void *)&wr_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Read
	sts_wr = HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PI_GAIN, I2C_MEMADD_SIZE_16BIT, (void *)&rd_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Compare
	icmp = memcmp(wr_dat, rd_dat, 2);
	if(icmp != 0){
		return FALSE;
	}else{
		return TRUE;
	}
}

BOOL EEP_Write_CurrOffset(u16 value)
{
	HAL_StatusTypeDef sts_wr;
	HAL_StatusTypeDef sts_rd;
	u8 wr_dat[8]= {0};
	u8 rd_dat[8]= {0};
	int icmp = 0;
	u8 i = 0;

	wr_dat[0] = (u8)value & 0xFFu;
	wr_dat[1] = (u8)(value >> 8) & 0xFFu;

// Write
	sts_wr = HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, EEP_PI_OFFS, I2C_MEMADD_SIZE_16BIT, (void *)&wr_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Read
	sts_wr = HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PI_OFFS, I2C_MEMADD_SIZE_16BIT, (void *)&rd_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Compare
	icmp = memcmp(wr_dat, rd_dat, 2);
	if(icmp != 0){
		return FALSE;
	}else{
		return TRUE;
	}
}



BOOL EEP_Write_Gain_PvIN(u16 value)
{
	HAL_StatusTypeDef sts_wr;
	HAL_StatusTypeDef sts_rd;
	u8 wr_dat[8]= {0};
	u8 rd_dat[8]= {0};
	int icmp = 0;

	wr_dat[0] = (u8)value & 0xFFu;
	wr_dat[1] = (u8)(value >> 8) & 0xFFu;

// Write
	sts_wr = HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, EEP_PV_IN, I2C_MEMADD_SIZE_16BIT, (void *)&wr_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Read
	sts_wr = HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PV_IN, I2C_MEMADD_SIZE_16BIT, (void *)&rd_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Compare
	icmp = memcmp(wr_dat, rd_dat, 2);
	if(icmp != 0){
		return FALSE;
	}else{
		return TRUE;
	}
}

BOOL EEP_Write_Gain_PvOU(u16 value)
{
	HAL_StatusTypeDef sts_wr;
	HAL_StatusTypeDef sts_rd;
	u8 wr_dat[8]= {0};
	u8 rd_dat[8]= {0};
	int icmp = 0;

	wr_dat[0] = (u8)value & 0xFFu;
	wr_dat[1] = (u8)(value >> 8) & 0xFFu;

// Write
	sts_wr = HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, EEP_PV_OU, I2C_MEMADD_SIZE_16BIT, (void *)&wr_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Read
	sts_wr = HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PV_OU, I2C_MEMADD_SIZE_16BIT, (void *)&rd_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Compare
	icmp = memcmp(wr_dat, rd_dat, 2);
	if(icmp != 0){
		return FALSE;
	}else{
		return TRUE;
	}
}

BOOL EEP_Write_Gain_PvBT(u16 value)
{
	HAL_StatusTypeDef sts_wr;
	HAL_StatusTypeDef sts_rd;
	u8 wr_dat[8]= {0};
	u8 rd_dat[8]= {0};
	int icmp = 0;

	wr_dat[0] = (u8)value & 0xFFu;
	wr_dat[1] = (u8)(value >> 8) & 0xFFu;

// Write
	sts_wr = HAL_I2C_Mem_Write(&hi2c1, SLA24EEP, EEP_PV_BT, I2C_MEMADD_SIZE_16BIT, (void *)&wr_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Read
	sts_wr = HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PV_BT, I2C_MEMADD_SIZE_16BIT, (void *)&rd_dat[0],2,20U);
	if(sts_wr != HAL_OK){
		return FALSE;
	}

// Compare
	icmp = memcmp(wr_dat, rd_dat, 2);
	if(icmp != 0){
		return FALSE;
	}else{
		return TRUE;
	}
}

BOOL EEP_Read_CurrGain(u16* value)
{
	u8 dat[8]= {0};
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PI_GAIN, I2C_MEMADD_SIZE_16BIT, (void *)&dat[0],2,20U) != HAL_OK){}
	*value = (u16)((u16)(dat[1] << 8) | (u16)dat[0]);
	return TRUE;
}

BOOL EEP_Read_CurrOffset(u16* value)
{
	u8 dat[8]= {0};
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PI_OFFS, I2C_MEMADD_SIZE_16BIT, (void *)&dat[0],2,20U) != HAL_OK){}
	*value = (u16)((u16)(dat[1] << 8) | (u16)dat[0]);
	return TRUE;
}

BOOL EEP_Read_Gain_PvIN(u16* value)
{
	u8 dat[8]= {0};
	while( HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PV_IN, I2C_MEMADD_SIZE_16BIT, (void *)&dat[0],2,20U) != HAL_OK){}
	*value = (u16)((u16)(dat[1] << 8) | (u16)dat[0]);
	return TRUE;
}

BOOL EEP_Read_Gain_PvOU(u16* value)
{
	u8 dat[8]= {0};
	while( HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PV_OU, I2C_MEMADD_SIZE_16BIT, (void *)&dat[0],2,20U) != HAL_OK){}
	*value = (u16)((u16)(dat[1] << 8) | (u16)dat[0]);
	return TRUE;
}

BOOL EEP_Read_Gain_PvBT(u16* value)
{
	u8 dat[8]= {0};
	while(HAL_I2C_Mem_Read(&hi2c1, SLA24EEP, EEP_PV_BT, I2C_MEMADD_SIZE_16BIT, (void *)&dat[0],2,20U) != 0){}
	*value = (u16)((u16)(dat[1] << 8) | (u16)dat[0]);
	return TRUE;
}

BOOL EEP_Write_CalData(Enum_CalPos_Type calpos, s32 caldata)
{
	BOOL bok = FALSE;
	
	if(calpos == CalPos_Curr_Gain_Type){
		bok = EEP_Write_CurrGain((u16)caldata);
	}else if(calpos == CalPos_Curr_Offset_Type){
		bok = EEP_Write_CurrOffset((u16)caldata);
	}else if(calpos == CalPos_Pv_In_Type){
		bok = EEP_Write_Gain_PvIN((u16)caldata);
	}else if(calpos == CalPos_Pv_Ou_Type){
		bok = EEP_Write_Gain_PvOU((u16)caldata);
	}else if(calpos == CalPos_Pv_Bt_Type){
		bok = EEP_Write_Gain_PvBT((u16)caldata);
	}else if(calpos == CalPos_Ax_24V_Type){

	}else if(calpos == CalPos_Ax_13V_Type){

	}else if(calpos == CalPos_Ax_5V_Type){

	}else if(calpos == CalPos_Ax_3V_Type){

	}
	return bok;
}

BOOL EEP_Read_CalData(Enum_CalPos_Type calpos, s32* pcaldata)
{
	BOOL bok = FALSE;
	
	if(calpos == CalPos_Curr_Gain_Type){
		bok = EEP_Read_CurrGain((u16)(*pcaldata));
	}else if(calpos == CalPos_Curr_Offset_Type){
		bok = EEP_Read_CurrOffset((u16)(*pcaldata));
	}else if(calpos == CalPos_Pv_In_Type){
		bok = EEP_Read_Gain_PvIN((u16)(*pcaldata));
	}else if(calpos == CalPos_Pv_Ou_Type){
		bok = EEP_Read_Gain_PvOU((u16)(*pcaldata));
	}else if(calpos == CalPos_Pv_Bt_Type){
		bok = EEP_Read_Gain_PvBT((u16)(*pcaldata));
	}else if(calpos == CalPos_Ax_24V_Type){

	}else if(calpos == CalPos_Ax_13V_Type){

	}else if(calpos == CalPos_Ax_5V_Type){

	}else if(calpos == CalPos_Ax_3V_Type){

	}
	return bok;
}

/**************************  End Of Source  *******************************************************/









