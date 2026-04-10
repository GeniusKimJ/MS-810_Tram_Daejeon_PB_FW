/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*

** For Doxygen ******************************
\file               AFE_adbms6815.c
\author             KKD
\date               2025-11-24 
\brief              Cell/Temp sensor - spi
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include "AFE_adbms6815.h"

/* Private define ---------------c------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private Typedef --------------------------------------------------------------------------------*/
sSM_Task g_sSm;
sAdbms6815 g_ad;

#define USE_ADBMS_B_TEST    //2026-01-16   jkpark  ADBMS_B 테스트 코드.

/* Private variables ------------------------------------------------------------------------------*/

u8	g_u8CmdCount							= 0;
u8	g_u8AfePos[NSLV]						= {0};
u8	g_u8Tx[NSPI][NGRP][MAXBUF]				= {0};
u8	g_u8Rx[NSPI][NGRP][MAXBUF]				= {0};

u8	g_u8Adbms6815_CfgrA[NSPI][NSLV][NREG]	= {0};
u8	g_u8Adbms6815_CfgrB[NSPI][NSLV][NREG]	= {0};

u8	g_u8Adbms6815_RdCfgrA[NSPI][NSLV][NREG]	= {0};
u8	g_u8Adbms6815_RdCfgrB[NSPI][NSLV][NREG]	= {0};

u16	g_u16Adbms6815_Pec15Table[256]			= {0,};
u16	g_u16Adbms6815_Pec10Table[256]			= {0,};
u8	g_u8slvpos[NSLV] = {0,}; 

u16	g_u16Adbms6815_VoltTable_10k[132] = 
{	
	31522,	31463,	31400,  31335,	 31268,   31197,    31124,	31047,	30969,	30888,
	29735,	29631,  29520,  29402,	 29274,   29136,  	28986,	28824,	28648,  28454,
	28242,	28100,  27948,  27787,	 27614,   27429,  	27230,	27016, 	26785,  26534,
	26260,	26077,  25882,  25676,	 25457,   25225,  	24977,	24712, 	24429,  24125,
	23797,	23576,  23344,  23099,	 22842,   22570,  	22284,	21980, 	21659,  21318,
	20951,	20704,  20447,  20178,	 19897,   19603,  	19296,	18974, 	18636,  18281,
	17908,	17653,  17389,  17116,	 16833,   16540,  	16235,	15919, 	15590,  15249,
	14891,	14647,  14396,  14138,	 13873,   13600,  	13319,	13030, 	12732,  12424,
	12100,	11880,  11655,  11425,	 11191,   10951,  	10706,	10455, 	10198,   9936,
	 9662,	 9475,	 9284,	 9091,	  8894,	   8694,  	 8491, 	 8284, 	 8074,   7860,
	 7629,	 7476,	 7320,	 7163,	  7004,	   6843,  	 6680, 	 6515, 	 6348,   6179,
	 5987,	 5864,	 5740,	 5615,	  5488,	   5360,  	 5231, 	 5101, 	 4970,   4837,
	 4689,	 4592,	 4495,	 4397,	  4298,	   4199,  	 4099, 	 3998, 	 3897,   3795,
	 3679,	 3604
};

/* Private function protoTypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

u32 tick_s[10]	= {0};
u32 tick_e[10]	= {0};
u32 tick_d[10]	= {0};

static inline u16 Adbms6815_Scale(eAdRead_Type eAdType,u8 msb, u8 lsb)
{
    u32 raw;
    //raw = (u32)(msb << 8) | lsb;   // AUX ADC raw
	raw = ((u32)msb << 8u) | (u32)lsb;
	if(eAdType == READ_CV_Type){
		raw = (u32)(raw / 10UL);
	}else if(eAdType == READ_TH_Type){
    	raw = (u32)((raw * 11UL) / 10UL);       // ×1.1 (정수 스케일)
    }else{

    }

    return (u16)raw;
}
u16 Adbms6815_CmdPec15(u8 *data , u16 len)
{
	u16 crc_remainder = 0U;
	u16 address = 0U;
	u16 i = 0;

	crc_remainder = 16U;//PEC seed

	for (i = 0U; i < (u16)len; i++)
	{
		address = (u16)(((u16)(crc_remainder >> 7U)) ^ ((u16)data[i] & 0x00FFu));
		//address = ((crc_remainder >> 7) ^ data[i]) & 0xff;//calculate PEC table address
		crc_remainder = (crc_remainder << 8 ) ^ g_u16Adbms6815_Pec15Table[address];
	}
	return (u16)(crc_remainder * 2U);//The CRC15 has a 0 in the LSB so the final value must be multiplied by 2
}

//data sheet lib
u16 Adbms6815_DatPec10(u8 *data, u16 len, BOOL bIsRxCmd)
{
	u16 nRemainder = 16u;/* PEC_SEED */
	u16 nPolynomial = 0x8Fu;/* x10 + x7 + x3 + x2 + x + 1 <- the CRC10 polynomial 100 1000 1111 */
	u8 nByteIndex;
	u8 nBitIndex;
	u16 nTableAddr;
	
	 for (nByteIndex = 0; nByteIndex < (u8)len; ++nByteIndex)
	 {
		 /* calculate PEC table address */
		 nTableAddr = (u16)((u16)(nRemainder >> 2) ^ (u8)data[nByteIndex]) & (u8)0xff;
		 nRemainder = ((u16)(nRemainder << 8)) ^ g_u16Adbms6815_Pec10Table[nTableAddr];
	 }

	 if (bIsRxCmd == TRUE) /* If array is from received buffer add command counter to crc calculation */
	 {
		 nRemainder ^= (u16)(((u16)data[len] & (u8)0xFC) << 2u);
	 }

	/* Perform modulo-2 division, a bit at a time on rest of the bits */
	for (nBitIndex = 6u; nBitIndex > 0u; --nBitIndex)
	{
		/* Try to divide the current data bit */
		if ((nRemainder & 0x200u) > 0u)
		{
			nRemainder = (u16)((nRemainder << 1u));
			nRemainder = (u16)(nRemainder ^ nPolynomial);
		}
		else
		{
			nRemainder = (u16)((nRemainder << 1u));
		}
	}
	
	return ((u16)(nRemainder & 0x3FFu));

}



//FuSa SM8 : Command CRC
void Adbms6815_Init_Pec15_Table(void)
{
	u16 u16Adbms6815_Crc15_Poly				= 0x4599U;
	u16 crc_remainder = 0u;
	u16 i = 0u;
	u16 bit = 0u;
	
	for (i = 0u; i < 256u; i++)
	{
		crc_remainder = i << 7u;
		for (bit = 8u; bit > 0u; --bit)
		{
			if ((crc_remainder & 0x4000u) != 0u)
			{
				crc_remainder = ((crc_remainder << 1));
				crc_remainder = (crc_remainder ^ u16Adbms6815_Crc15_Poly);
			}
			else
			{
				crc_remainder = ((crc_remainder << 1));
			}
		}
		g_u16Adbms6815_Pec15Table[i] = crc_remainder&0xFFFFu;
	}
}


void Adbms6815_Init_Pec10_Table(void)
{
	u16 u16Adbms6815_Crc10_Poly				= 0x8FU;			// 48F
	u16 crc_remainder = 0u;
	u16 i = 0u;
	u16 bit = 0u;

	BOOL s_bFirst = TRUE;
	if(s_bFirst == TRUE)
	{
		for (i = 0u; i < 256u; i++)
		{
			crc_remainder = i << 2u;
			for (bit = 8u; bit > 0u; --bit)
			{
				if ((crc_remainder & 0x200u) != 0u)
				{
					crc_remainder = ((crc_remainder << 1));
					crc_remainder = (crc_remainder ^ u16Adbms6815_Crc10_Poly);
				}
				else
				{
					crc_remainder = ((crc_remainder << 1));
				}
			}
			g_u16Adbms6815_Pec10Table[i] = crc_remainder & 0x3FFu;
		}

		s_bFirst = FALSE;
	}

}

void Adbms6815_ChipSel(eAdSpi_Type eSpiNum, eAdComDir_Type bComDir, BOOL bSelect)
{
    GPIO_TypeDef *gpio = NULL;
    GPIO_PinState pinState = GPIO_PIN_RESET;
    u16 u16SetPin   = 0U;
    u16 u16InversPin = 0U;

    if (eSpiNum == SPI1_Type){
        gpio = GPIOE;
        if( bComDir == ComDir_A_Type ){
            u16SetPin  = GPIO_PIN_0;
            u16InversPin = GPIO_PIN_1;
        }else{
            u16SetPin  = GPIO_PIN_1;
            u16InversPin = GPIO_PIN_0;
        }
    }else{
        gpio = GPIOC;
        if( bComDir == ComDir_A_Type ){
            u16SetPin  = GPIO_PIN_14;
            u16InversPin = GPIO_PIN_15;
        }else{
            u16SetPin  = GPIO_PIN_15;
            u16InversPin = GPIO_PIN_14;
        }
    }
    if( bSelect == TRUE ){
    	//gpio->BSRR = u16InversPin;
    	//gpio->BSRR = (u32)(u16SetPin << 16U);
        HAL_GPIO_WritePin(gpio, u16InversPin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(gpio, u16SetPin , GPIO_PIN_RESET);
    }else{
		HAL_GPIO_WritePin(gpio, u16InversPin, GPIO_PIN_SET);  // 링 토폴로지: 반대 채널 LOW 방지 (6820_B 의도치 않은 활성화 차단)
        HAL_GPIO_WritePin(gpio, u16SetPin, GPIO_PIN_SET);
    }

	Delay_us(DLY10US);
}

void Adbms6815_Wakeup(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir)
{
	u32	dlywk 	= 5UL+NSLV;																											// 1us
	u32	i 		= 0UL;

	for(i=0UL;i < (u32)NSLV; i ++){
		Adbms6815_ChipSel(eSpiNum,eComDir, (BOOL)TRUE);
		Delay_us(dlywk);
		Adbms6815_ChipSel(eSpiNum,eComDir, (BOOL)FALSE); 
		Delay_us(dlywk);
	}
}

void Adbms6815_Cmd_Write(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd)
{
	SPI_HandleTypeDef *hspi = (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
	u8	tx[4]	= {0,}; 
	u8	rx[4]	= {0,}; 
	u16	pec		= 0;

	tx[0] = (u8)(u16Cmd >> 8) & 0xFFU;
	tx[1] = (u8)(u16Cmd & 0xFFU);

	pec = Adbms6815_CmdPec15(&tx[0], 2);
	tx[2] = (u8)((pec >> 8) & 0xFFU);
	tx[3] = (u8)(pec & 0xFFU);


	//Adbms6815_Wakeup(eSpiNum,g_ad.eComDir[eSpiNum]);
	Adbms6815_ChipSel(eSpiNum,eComDir, TRUE);
	Delay_us(DLY10US);
	//HAL_SPI_TransmitReceive(hspi,tx,rx,sizeof(tx),20);
	HAL_SPI_Transmit(hspi,tx,sizeof(tx),20);
	Adbms6815_ChipSel(eSpiNum,eComDir, FALSE);
	Delay_us(DLY10US);
}


void Adbms6815_ClearCmd_Write(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd, u8 *data)
{
	SPI_HandleTypeDef *hspi = (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
    u16 cmd = CLRFLAG; 
    u16 pec;
    u8 tx_buf[NCMDFRM + (NDATFRM * NSLV)] = {0,};  // BUG FIX: 4+(4*N)→4+(8*N), 버퍼 오버플로우 수정

    // 1. 커맨드 및 커맨드 PEC 생성
    tx_buf[0] = (u8)(cmd >> 8);
    tx_buf[1] = (u8)(cmd & 0xFF);
    pec = Adbms6815_CmdPec15(&tx_buf[0], 2);
    tx_buf[2] = (u8)(pec >> 8);
    tx_buf[3] = (u8)(pec & 0xFF);

    // 2. 슬레이브 개수(NSLV)만큼 6바이트 데이터 + 데이터 PEC 추가
    for(int k = 0; k < NSLV; k++) {
        // 실제 유효한 CFD 2바이트(0xFF, 0xFF) + 나머지 4바이트는 더미(0x00)로 채워 6바이트 규격 맞춤
        for(int i = 0; i < 2; i++) {
            tx_buf[NCMDFRM + (k * NDATFRM) + i] = data[i];  // BUG FIX: k*4→k*NDATFRM(8) 인덱스 수정
        }

        // 완성된 6바이트에 대한 Data PEC 계산
        pec = Adbms6815_DatPec10(data, 2, FALSE);
        tx_buf[NCMDFRM + (k * NDATFRM) + 6] = (u8)(pec >> 8);   // BUG FIX: +2→+6 (6byte data 이후)
        tx_buf[NCMDFRM + (k * NDATFRM) + 7] = (u8)(pec & 0xFF); // BUG FIX: +3→+7
    }
    
    // 3. SPI 데이터 전송
    Adbms6815_Wakeup(eSpiNum, eComDir);
    Adbms6815_ChipSel(eSpiNum, eComDir, TRUE);
    Delay_us(DLY10US);
    
    HAL_SPI_Transmit(hspi, tx_buf, sizeof(tx_buf), 10);
    
    Adbms6815_ChipSel(eSpiNum, eComDir, FALSE);
    Delay_us(DLY10US);
}

void Adbms6815_Daisy_Read_DmaStart(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,eRegGroup_Type eRegGroup,u16 u16Cmd)
{
	SPI_HandleTypeDef *hspi = (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
	
	u8	i 		= 0;
	u8	k 		= 0;
	u16	pec 	= 0;
	u16	len		= (u16)(NCMDFRM+(NSLV*NDATFRM));

	g_u8Tx[eSpiNum][eRegGroup][0] = (u8)((u16Cmd >> 8) & 0xFFU);
	g_u8Tx[eSpiNum][eRegGroup][1] = (u8)(u16Cmd & 0xFFU);

	pec = Adbms6815_CmdPec15(&g_u8Tx[eSpiNum][eRegGroup][0], 2);
	g_u8Tx[eSpiNum][eRegGroup][2] = (u8)((pec >> 8) & 0xFFU);
	g_u8Tx[eSpiNum][eRegGroup][3] = (u8)(pec & 0xFFU);

	for(i = 0; i < (u8)NSLV; i++){
		for(k = 0; k < (u8)8 ; k ++){
			g_u8Tx[eSpiNum][eRegGroup][4U+k+(i*8U)] = 0xFFU;
		}
	}
	//Adbms6815_Wakeup(eSpiNum,g_ad.eComDir[eSpiNum]);
	Adbms6815_ChipSel(eSpiNum,eComDir, TRUE);
	Delay_us(DLY10US);
#if EN_DMA
	//GPIO_DOut_TP1(TRUE);
	HAL_SPI_TransmitReceive_DMA(hspi, g_u8Tx[eSpiNum][eRegGroup], g_u8Rx[eSpiNum][eRegGroup],len);
	//GPIO_DOut_TP1(FALSE);
	Delay_us(DLY10US);
#else
	HAL_SPI_TransmitReceive(hspi, g_u8Tx[eSpiNum][eRegGroup], g_u8Rx[eSpiNum][eRegGroup],len,5);
	Adbms6815_ChipSel(eSpiNum,eComDir, FALSE);
#endif
}

//void Adbms6815_Daisy_Read_DmaEnd(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir, eRegGroup_Type eRegGroup,sAdbms6815 *pAd, eAdRead_Type eRdType, eAd_Adc_Type eRdFusaType)
void Adbms6815_Daisy_Read_DmaEnd(sAdbms6815 *pAd, eAdRead_Type eRdType, eAd_Adc_Type eRdAdcType)
{
	u8	eRegGroup 	= (u8)pAd->u8GrpNum;
	u8	eSpiNum		= (u8)pAd->eSpiNum;
	u8  eComDir   	= (u8)pAd->eComDir;

	u8	nreg 		= 0;
	u8	nslv		= 0;
	u8	stapos		= ((u8)eRegGroup * (u8)NGRP);
	u8	reg[NREG]	= {0,};
	u8	ffpos		= 0;
	u16	pec_rx		= 0;
	u16	pec_mk 		= 0;
	u16	cmdcnt 		= 0;
	u16	raw[3]		= {0};
	float 	fcv[3]		= {0};

#if EN_DMA
	Adbms6815_ChipSel(eSpiNum,eComDir, FALSE);
#endif

	for(nslv=0; nslv<(u8)NSLV; nslv++){
		g_u8slvpos[nslv] = Adbms6815_Get_AfePos(nslv);																				// get afe pos
	}

	for(nslv=0; nslv<(u8)NSLV; nslv++){
		pec_rx = (u16)(((g_u8Rx[eSpiNum][eRegGroup][10U+(nslv*NDATFRM)])* 0x100U)+g_u8Rx[eSpiNum][eRegGroup][11U+(nslv*NDATFRM)]);							// read pec
		cmdcnt = Adbms6815_CmdCnt(&pec_rx);																						// cmdcount
		pec_mk = Adbms6815_DatPec10(&g_u8Rx[eSpiNum][eRegGroup][NCMDFRM + (nslv * NDATFRM)], NREG, TRUE);												// make pec
		
		if(pec_rx == pec_mk){																									// compare pec
			for(nreg=0; nreg<NREG; nreg++){
				reg[nreg] = g_u8Rx[eSpiNum][eRegGroup][NCMDFRM+(nslv * NDATFRM)+nreg];
			}

			switch (eRdType) {
				case READ_CV_Type:                      
					raw[0] = (u16)(((u16)reg[1] << 8U) | (u16)reg[0]); 
					fcv[0] = ((float)raw[0]) * 0.1f;
					raw[1] = (u16)(((u16)reg[3] << 8U) | (u16)reg[2]); 
					fcv[1] = ((float)raw[1]) * 0.1f;
					
					if(stapos < 6U){
						raw[2] = (u16)(((u16)reg[5] << 8U) | (u16)reg[4]); 
						fcv[2] = ((float)raw[2]) * 0.1f;
					}

					// cell 3ea / 123 / 456 / 78				
					pAd->u16Cv[eSpiNum][g_u8slvpos[nslv]][stapos + 0U] = (u16)fcv[0];
					pAd->u16Cv[eSpiNum][g_u8slvpos[nslv]][stapos + 1U] = (u16)fcv[1];
					if (stapos < 6U) {
						pAd->u16Cv[eSpiNum][g_u8slvpos[nslv]][stapos + 2U] = (u16)fcv[2];
					}break;					
				case READ_TH_Type:
				    if (eRegGroup == GA_TYPE) {
				    	raw[0] = (u16)(((u16)reg[1] << 8U) | (u16)reg[0]); 
						fcv[0] = ((float)raw[0]) * 0.1f;
				      	pAd->u16Vref2[eSpiNum][g_u8slvpos[nslv]] = (u16)fcv[0];												// reg [0][1]										// Vref
						
						pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][0] = Adbms6815_Scale(READ_TH_Type,reg[3],reg[2]);			// reg [2][3]
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][0] = (s16)TempAmbient(pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][0]);

						pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][1] = Adbms6815_Scale(READ_TH_Type,reg[5],reg[4]);			// reg [4][5]
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][1] = (s16)TempAmbient(pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][1]);
				    }else if (eRegGroup == GB_TYPE) {
						pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][2] = Adbms6815_Scale(READ_TH_Type,reg[1],reg[0]);
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][2] = (s16)TempAmbient(pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][2]);
					}else if (eRegGroup == GC_TYPE) {
						pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][3] = Adbms6815_Scale(READ_TH_Type,reg[1],reg[0]);
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][3] = (s16)TempAmbient(pAd->u16ThAdc[eSpiNum][g_u8slvpos[nslv]][3]);

				    	raw[0] = (u16)(((u16)reg[5] << 8U) | (u16)reg[4]); 
						fcv[0] = ((float)raw[0]) * 0.1f;
						pAd->u16Vref3[eSpiNum][g_u8slvpos[nslv]] = (u16)fcv[0];    											// Vref3
				    }break;

				case READ_ST_Type:
				    // TODO: status
				    break;

				default:
				    break;
			}
		}else{
			//TODO : All 0xff status (open line, ic err, noise)
			#if 1
			switch (eRdType) {
				case READ_CV_Type:
					if(eSpiNum == SPI2_Type) {ffpos = NSLV;}
					if(++pAd->u8ReadFailCheck[eSpiNum][READ_CV_Type][g_u8slvpos[nslv]+ffpos]>=0xFFU) {
						pAd->u8ReadFailCheck[eSpiNum][READ_CV_Type][g_u8slvpos[nslv]+ffpos]=0xFFU;  // BUG FIX: ffpos 누락으로 잘못된 인덱스에 0xFF 저장하던 오류
					}
					pAd->u16Cv[eSpiNum][g_u8slvpos[nslv]][stapos + 0U] = 0U;
					pAd->u16Cv[eSpiNum][g_u8slvpos[nslv]][stapos + 1U] = 0U;
				    if (stapos < 6U) {
				    	pAd->u16Cv[eSpiNum][g_u8slvpos[nslv]][stapos + 2U] = 0U;
				    }break;
				case READ_TH_Type:
					if(eSpiNum == SPI2_Type) {
						ffpos = NSLV;
					}
					if(++pAd->u8ReadFailCheck[eSpiNum][READ_TH_Type][g_u8slvpos[nslv]+ffpos]>=0xFFU) {
						pAd->u8ReadFailCheck[eSpiNum][READ_TH_Type][g_u8slvpos[nslv]+ffpos]=0xFFU;
					}
					
					if (eRegGroup == GA_TYPE) {
						pAd->u16Vref2[eSpiNum][g_u8slvpos[nslv]] = 0;																	// Vref2
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][0U] = 0;															// T1
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][1U] = 0; 														// T2
					} else if(eRegGroup == GB_TYPE){
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][2U] = 0; 														// T3
					}else if(eRegGroup == GC_TYPE){
						pAd->s16Th[eSpiNum][g_u8slvpos[nslv]][3U] = 0; 														// T4
						pAd->u16Vref3[eSpiNum][g_u8slvpos[nslv]] = 0;																	// Vref3
					}break;
				default:
					break;
			}
			#endif
		}
	}
}

void Adbms6815_WrCfgA(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir, u8 EnPs,u8 FlagD,u8 Soak, u8 Owrn)
{	
	for(u8 nslv = 0;nslv<NSLV;nslv++) {
		g_u8Adbms6815_CfgrA[eSpiNum][nslv][0] = (u8)(EnPs|REFON|CFG_0_CVMIN_08);																			// cfga 1000 0000b
		g_u8Adbms6815_CfgrA[eSpiNum][nslv][1] = FlagD;
		if(Soak == 1U) {
			g_u8Adbms6815_CfgrA[eSpiNum][nslv][2] = (u8)(Soak|Owrn|OWC1|OWC2); 																//OWC 6 ( sm4 1 1 0 )
		}else{
			g_u8Adbms6815_CfgrA[eSpiNum][nslv][2] = 0;
		}
		g_u8Adbms6815_CfgrA[eSpiNum][nslv][3] = 0x27U;
		g_u8Adbms6815_CfgrA[eSpiNum][nslv][4] = 0x00U;
		g_u8Adbms6815_CfgrA[eSpiNum][nslv][5] = 0x00U;
		//g_u8Adbms6815_CfgrA[eSpiNum][nslv][3] = (u8)NSTH;
	}
	Adbms6815_Daisy_CfgWrite(eSpiNum,eComDir,WRCFGA,g_u8Adbms6815_CfgrA);
}

void Adbms6815_WrCmCfg(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir)
{	
    u8	g_uAdbms6815_CmCfgr[NSPI][NSLV][NREG]	= {0};

	for(u8 nslv = 0;nslv<NSLV;nslv++) {
		g_uAdbms6815_CmCfgr[eSpiNum][nslv][4] = 0x20;                                      //jkpark 2026-01-19 역방향 Enable.
	}
	Adbms6815_Daisy_CfgWrite(eSpiNum,eComDir,WRCMCFG,g_uAdbms6815_CmCfgr);
}


void Adbms6815_WrCfgB(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir)
{	


	for(u8 nslv = 0;nslv<NSLV;nslv++) {																							// cfgb VOV, VUV default 0xF0
		//g_u8Adbms6815_CfgrA[eSpiNum][nslv][4] = 0x00;
		//g_u8Adbms6815_CfgrA[eSpiNum][nslv][5] = 0x00;

		g_u8Adbms6815_CfgrB[eSpiNum][nslv][0] = 0x00;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][1] = 0xF0;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][2] = 0xFF;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][3] = 0x00;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][4] = 0x00;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][5] = 0x00;  // BUG FIX: [4]중복→[5] (index[5] 누락)
	}


	Adbms6815_Daisy_CfgWrite(eSpiNum,eComDir,WRCFGB,g_u8Adbms6815_CfgrB);
}

/**
 * @brief  RDCFGA / RDCFGB 명령으로 Config 레지스터를 읽어 u8Cfgr 버퍼에 저장한다.
 * @param  eSpiNum   : SPI1_Type / SPI2_Type
 * @param  eComDir   : 통신 방향
 * @param  u16Cmd    : RDCFGA(0x02) 또는 RDCFGB(0x26)
 * @param  u8Cfgr    : 읽은 6바이트 데이터를 저장할 버퍼 [NSPI][NSLV][6]
 * @retval TRUE  : 전체 슬레이브 PEC 검증 OK
 *         FALSE : 하나 이상 슬레이브에서 PEC 오류
 */
BOOL Adbms6815_Daisy_CfgRead(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir, u16 u16Cmd, u8 u8Cfgr[][NSLV][6])
{
	SPI_HandleTypeDef *hspi = (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
	u8  tx[NCMDFRM + (NDATFRM * NSLV)] = {0,};
	u8  rx[NCMDFRM + (NDATFRM * NSLV)] = {0,};

	u8   nslv     = 0;
	u8   i     = 0;
	u16  pec_mk   = 0;
	u16  pec_rx = 0;
	BOOL bResult = TRUE;
	u8	nreg 		= 0;
	
	/* 1. 커맨드 프레임 구성: [CMD_H][CMD_L][PEC_H][PEC_L] */
	tx[0] = (u8)((u16Cmd >> 8) & 0xFFU);
	tx[1] = (u8)(u16Cmd & 0xFFU);
	pec_mk   = Adbms6815_CmdPec15(&tx[0], 2);
	tx[2] = (u8)((pec_mk >> 8) & 0xFFU);
	tx[3] = (u8)(pec_mk & 0xFFU);

	/* 2. 데이터 영역은 0xFF 더미로 채움 (슬레이브가 응답) */
	for(nslv = 0; nslv < (u8)NSLV; nslv++) {
		for(i = 0; i < (u8)NDATFRM; i++) {
			tx[NCMDFRM + (nslv * NDATFRM) + i] = 0xFFU;
		}
	}

	/* 3. SPI 트랜시버 */
	Adbms6815_ChipSel(eSpiNum, eComDir, TRUE);
	Delay_us(DLY10US);
	HAL_SPI_TransmitReceive(hspi, tx, rx, (u16)sizeof(tx), 20);
	Adbms6815_ChipSel(eSpiNum, eComDir, FALSE);
	Delay_us(DLY10US);

	/* 4. 각 슬레이브 데이터 파싱 및 PEC10 검증 */
	for(nslv = 0; nslv < (u8)NSLV; nslv++) {
		/* RX 버퍼에서 데이터 PEC 추출 */
		pec_rx = (u16)(((u16)rx[NCMDFRM + (nslv * NDATFRM) + NREG] << 8)  | (u16)rx[NCMDFRM + (nslv * NDATFRM) + NREG + 1U]);

		/* 수신 데이터로 PEC 재계산 (bIsRxCmd=TRUE: 커맨드 카운터 포함) */
		pec_mk = Adbms6815_DatPec10(&rx[NCMDFRM + (nslv * NDATFRM)], NREG, TRUE);

		if(pec_mk == pec_rx) {
			/* PEC OK → 결과 버퍼에 저장 */
			for(i = 0; i < (u8)NREG; i++) {
				u8Cfgr[eSpiNum][nslv][i] = rx[NCMDFRM + (nslv * NDATFRM) + i];
			}
		} else {
			/* PEC 오류 → 해당 슬레이브 데이터 무효, 에러 플래그 세트 */
			bResult = FALSE;
		}
	}

	return bResult;
}

/** @brief RDCFGA: Config A 레지스터 읽기. 결과는 g_u8Adbms6815_CfgrA 에 저장 */
BOOL Adbms6815_RdCfgA(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir)
{
	return Adbms6815_Daisy_CfgRead(eSpiNum, eComDir, RDCFGA, g_u8Adbms6815_RdCfgrA);
}

/** @brief RDCFGB: Config B 레지스터 읽기. 결과는 g_u8Adbms6815_CfgrB 에 저장 */
BOOL Adbms6815_RdCfgB(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir)
{
	return Adbms6815_Daisy_CfgRead(eSpiNum, eComDir, RDCFGB, g_u8Adbms6815_RdCfgrB);
}

void Adbms6815_Daisy_CfgWrite(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd,u8 u8Cfgr[][NSLV][6])
{
	SPI_HandleTypeDef *hspi = (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
	u8	tx[(NCMD+NPEC)+((NREG+NPEC)*NSLV)]			= {0,};
	u8	rx[(NCMD+NPEC)+((NREG+NPEC)*NSLV)]			= {0,};

	u8	i 				= 0;
	u8	k 				= 0;
	u16	pec 			= 0;
	tx[0] = (u8)((u16Cmd>>8) & 0xFFU);
	tx[1] = (u8)(u16Cmd & 0xFFU);

	pec = Adbms6815_CmdPec15(&tx[0], 2);
	tx[2] = (u8)((pec >> 8) & 0xFFU);
	tx[3] = (u8)(pec & 0xFFU);

	for(k=0;k<NSLV;k++)
	{
		pec = Adbms6815_DatPec10(u8Cfgr[eSpiNum][k],6,FALSE);  // BUG FIX: 루프 밖에서 k=0 데이터로만 PEC 계산하던 것을 각 슬레이브마다 계산하도록 이동
		for(i=0;i<6U;i++){
			tx[NCMDFRM+i+(k*NDATFRM)] = u8Cfgr[eSpiNum][k][i];
		}
		
		tx[10U+(k*NDATFRM)] = (u8)((pec >> 8U) & 0xFFU);
		tx[11U+(k*NDATFRM)] = (u8)(pec & 0xFFU);
	}
	//Adbms6815_Wakeup(eSpiNum, eComDir);
	Adbms6815_ChipSel(eSpiNum,eComDir, TRUE);
	Delay_us(DLY10US);
	//HAL_SPI_TransmitReceive(hspi,tx,rx,sizeof(tx),20);
	HAL_SPI_Transmit(hspi,tx,sizeof(tx),20);
	Adbms6815_ChipSel(eSpiNum,eComDir, FALSE);
	Delay_us(DLY10US);
}


void Adbms6815_Daisy_BalWrite(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd,u8 u8Cfgr[][NSLV][6])
{
	SPI_HandleTypeDef *hspi = (eSpiNum == SPI1_Type) ? &hspi1 : &hspi2;
	u8	tx[(NCMD+NPEC)+((NREG+NPEC)*NSLV)]			= {0,};
	u8	rx[(NCMD+NPEC)+((NREG+NPEC)*NSLV)]			= {0,};

	u8	i 				= 0;
	u8	k 				= 0;
	u16	pec 			= 0;
	tx[0] = (u8)((u16Cmd>>8) & 0xFFU);
	tx[1] = (u8)(u16Cmd & 0xFFU);

	pec = Adbms6815_CmdPec15(&tx[0], 2);
	tx[2] = (u8)((pec >> 8U) & 0xFFU);
	tx[3] = (u8)(pec & 0xFFU);

	for(k=0;k<NSLV;k++){
		pec = Adbms6815_DatPec10(u8Cfgr[eSpiNum][k],6,FALSE);
		for(i=0;i<6U;i++){
			tx[NCMDFRM+i+(k*NDATFRM)] = u8Cfgr[eSpiNum][k][i];
		}
		tx[10U+(k*NDATFRM)] = (u8)((pec >> 8) & 0xFFU);
		tx[11U+(k*NDATFRM)] = (u8)(pec & 0xFFU);
	}
	Adbms6815_Wakeup(eSpiNum, eComDir);
	Adbms6815_ChipSel(eSpiNum,eComDir, TRUE);
	Delay_us(DLY10US);
	HAL_SPI_TransmitReceive(hspi,tx,rx,sizeof(tx),20);
	Adbms6815_ChipSel(eSpiNum,eComDir, FALSE);
	Delay_us(DLY10US);
}



/* CONFIG BAL BEGIN**************************************************************************************************************************/
void Adbms6815_WrCfgBal(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir)
{
	static BOOL baltog	= FALSE;
	u8	nslv			= 0;
	u8	bufidx			= 0;
	u8	fill 			= 0;
	u8 	finbal			= 0;
	u8	fbal			= 0;

	if(baltog == TRUE){
		baltog = FALSE;
		fill = 0xAA;
	}else{
		baltog = TRUE;
		fill = 0x55;
	}

	/*
	 * [Daisy Chain Write 버퍼 인덱스 결정]
	 *
	 * ADBMS6815 Write 동작: buffer[0] 이 먼저 전송되어 FAR slave(IC17)에 도달하고,
	 *                        buffer[NSLV-1] 이 마지막으로 NEAR slave(IC0)에 남는다.
	 *   → buffer[k] 는 물리 slave (NSLV-1-k) 에 전달됨
	 *
	 * Cal_AfePos(ComDir_A, WR_WR) 는 g_u8AfePos[i]=i (Direct)를 반환하므로,
	 * Get_AfePos()를 그대로 쓰면 slave 0의 데이터가 buffer[0] → IC17로 잘못 전달된다.
	 *
	 * 올바른 매핑:
	 *   ComDir_A : bufidx = (NSLV-1) - nslv  (Reversed)
	 *   ComDir_B : bufidx = nslv  (Direct, B측 master 기준 체인 방향 반전)
	 */
	for(nslv = 0; nslv < (u8)NSLV; nslv++) {
		if(fill == 0x55 && g_ad.u8CvBalFlag[eSpiNum][0] == 0xAA){
			// 여기서 걸리면 원인 확인됨
			__NOP(); // breakpoint
		}
		
		finbal	= g_ad.u8CvBalFlag[eSpiNum][nslv];
		fbal	= (u8)(finbal & fill);

		bufidx = (eComDir == ComDir_A_Type) ? ((u8)(NSLV-1U) - nslv) : nslv;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][0] = 0x00;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][1] = 0xF0;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][2] = 0xFF;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][3] = 0x00;
		g_u8Adbms6815_CfgrB[eSpiNum][bufidx][4] = fbal;
		g_u8Adbms6815_CfgrB[eSpiNum][nslv][5] = 0x00;
	}

	Adbms6815_Daisy_BalWrite(eSpiNum, eComDir, WRCFGB, g_u8Adbms6815_CfgrB);
}

/* CONFIG BAL END****************************************************************************************************************************/

//chip select direction
void Adbms6815_Cal_AfePos(eAdComDir_Type eComDir,eAdRw_Type eWriteRead)
{
	u8 i = 0;
	if(eWriteRead == WR_WR_Type){ 																								// Write
		if(eComDir == ComDir_A_Type){					
			for(i = 0;i<NSLV;i++){
				g_u8AfePos[i] = i;																								// AFE1 -> Write(Near(0) -> Far(35))
			}	
		}else if(eComDir == ComDir_B_Type){			
			for(i = 0;i<NSLV;i++){
				g_u8AfePos[i] = ((NSLV-1U)-i); 																					// AFE1 -> Write(Far(35) -> Near(0))
			}
		}
	}else{																														// Read
		if(eComDir == ComDir_A_Type){					
			for(i = 0;i<NSLV;i++){
				g_u8AfePos[i] = ((NSLV-1U)-i); 																					// AFE2 -> Read(Near(0) -> Far(35))
			}
		}else if(eComDir == ComDir_B_Type){
			for(i = 0;i<NSLV;i++){
				g_u8AfePos[i] = i;																								// AFE2 -> Read(Far(35) -> Near(0))
			}
		}
	}	
}

u8 Adbms6815_Get_AfePos(u8 u8AfePos)
{
	if(u8AfePos >= NSLV) { u8AfePos = 0; }
	return g_u8AfePos[u8AfePos];
}


u16 Adbms6815_CmdCnt(u16* pu16ReadPec)
{
	u16 u16CmdCounter = 0;
	u16 u16Temp = 0;

	u16CmdCounter = (*pu16ReadPec >>10) & 0x3FU;
	u16Temp = (*pu16ReadPec) & 0x3FFU;

	(*pu16ReadPec) = u16Temp;
	return u16CmdCounter;
}

void Adbms6815_Init(void)
{		
	//HAL_DMA_RegisterCallback(&hspi1, HAL_DMA_XFER_CPLT_CB_ID, Adbms6815_Callback_DMAReComplete());							//HAL_DMA_XFER_CPLT_CB_ID  DMA Send Check Index
	memset(g_u8Tx,0x00,sizeof(g_u8Tx));
	memset(g_u8Rx,0x00,sizeof(g_u8Rx));
	memset(&g_ad.eRegGrp ,0x00,sizeof(g_ad));
	memset(g_u8Adbms6815_CfgrA,0x00,sizeof(g_u8Adbms6815_CfgrA));
	memset(g_u8Adbms6815_CfgrB,0x00,sizeof(g_u8Adbms6815_CfgrB));
	
	g_ad.eSpiNum = SPI1_Type;
	g_ad.eComDir = ComDir_A_Type;
	g_ad.eRw = WR_WR_Type;
	Adbms6815_Cal_AfePos(ComDir_A_Type,g_ad.eRw);
	Adbms6815_Init_Pec15_Table();
	Adbms6815_Init_Pec10_Table();
	g_ad.bAdbmsInitDone = TRUE; 
}

void Adbms6815_Make_BalFlag(Pkt_Rack *pBmsData,sAdbms6815 *pAd) 
{
	BOOL bstart	= TRUE;
	u16	diffcv	= 0;
	u8	nspi = 0,nslv = 0, nscv = 0;

	diffcv = (u16)(pBmsData->maxcv - pBmsData->mincv);

	//balancing~~
	if(pAd->bSetBalancingStart == TRUE){
		//stop condition
		if(diffcv < UBOPR){
			bstart = FALSE;
		}
	}else{
		if(diffcv < UBSTP){
			bstart = FALSE;
		}
	}

	if(pBmsData->pi[IREAL] >= (-2000)){	                                             			//KKD 2026-01-20 -2A 이상 bal stop
		bstart = FALSE;
	}
	
	if(pBmsData->pi[IREAL] >= (10000) ){	                                         			//KKD 2026-01-20  10A 이상 bal stop
		bstart = FALSE;
	}

	if( (u16)2200 < (pBmsData->avgcv) < (u16)2700){	                                         				//KKD 2026-01-20  평균 Cell 전압 3.5V 미만 bal stop
		bstart = FALSE;
	}

	if(pBmsData->fdet !=0U){
		bstart = FALSE;
	}

	pAd->bSetBalancingStart = bstart;

	if((bstart == TRUE) || (g_sMn.mn_set_bal_start == TRUE)){
		for(nspi = 0; nspi < NSPI; nspi++){
			for(nslv = 0; nslv < NSLV; nslv++){
				for(nscv = 0; nscv < NSCV; nscv++){
					if(pAd->u16Cv[nspi][nslv][nscv] > pBmsData->avgcv){
						pAd->u8CvBalFlag[nspi][nslv] |= ((u8)1 << nscv);
					}else{
						pAd->u8CvBalFlag[nspi][nslv] &= ~((u8)1 << nscv);
					}
				}
			}
		}
	}else{
		for(nspi = 0; nspi < NSPI; nspi++){
			for(nslv = 0; nslv < NSLV; nslv++){
				pAd->u8CvBalFlag[nspi][nslv] = 0;
			}
		}
	}
}

void Adbms6815_Balancing_Proc(void)
{
	sAdbms6815 		*pad		= &g_ad;

	//if(g_sMn.mn_type & (1<< MN_BAL_TYPE)){
	if((g_sMn.mn_type & ((u16)1U << (u16)MN_BAL_TYPE)) != 0U){		// static analizye
		memcpy( &g_ad.u8CvBalFlag,&g_sMn.mn_set_bal_flag,sizeof(g_sMn.mn_set_bal_flag));
		g_ad.bSetBalancingStart = TRUE;
		//g_ad.u8CvBalFlag[g_sMn.mn_set_bal_spi_pos][g_sMn.mn_set_bal_slv_pos] = g_sMn.mn_set_bal_flag;
	}else{
		Adbms6815_Make_BalFlag(&RackPkt, &g_ad);
	}

	if(g_ad.bSetBalancingStart == TRUE){
		Adbms6815_Wakeup(pad->eSpiNum,pad->eComDir);
		Adbms6815_WrCfgBal(pad->eSpiNum,pad->eComDir);
	}else{

	}
}

void Adbms6815_01_CV_Config(void)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDCVA,RDCVB,RDCVC}; 
	//tick_s = HAL_GetTick();					// debug start tick																																																																																																																																																																																																																																																	
	Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
	//Adbms6815_Cmd_Write(pad->eSpiNum,pad->eComDir,CLRCELL);																	   		// 1. clear reg
	Adbms6815_WrCfgA(pad->eSpiNum,pad->eComDir,0,0,0,0);																		   		// cfg a
   	Adbms6815_Cmd_Write(pad->eSpiNum,pad->eComDir,ADCV|ADBMS6815_MD_7khz);
}

void Adbms6815_01_CV_StartConv(void)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDCVA,RDCVB,RDCVC}; 

   	Adbms6815_Cmd_Write(pad->eSpiNum,pad->eComDir,ADCV|ADBMS6815_MD_7khz);
}

void Adbms6815_02_CV_Read_S(eRegGroup_Type grppos)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDCVA,RDCVB,RDCVC}; 

	Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
	Adbms6815_Daisy_Read_DmaStart(pad->eSpiNum,pad->eComDir,grppos,cmd[grppos]);
}

void Adbms6815_03_CV_Read_E(eRegGroup_Type grppos)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDCVA,RDCVB,RDCVC}; 
	pad->u8GrpNum = (u8)grppos;
	
	Adbms6815_Daisy_Read_DmaEnd(pad, READ_CV_Type,ADC_CV_Type);
}
void Adbms6815_04_TH_Config(void)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDAXA,RDAXB,RDAXC}; 
	
	Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);																					 // cfg b																		// 1. clear reg
	Adbms6815_Cmd_Write(pad->eSpiNum,pad->eComDir,ADAX|ADBMS6815_MD_7khz);
}
void Adbms6815_05_TH_Read_S(eRegGroup_Type grppos)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDAXA,RDAXB,RDAXC}; 

	Adbms6815_Wakeup(pad->eSpiNum, pad->eComDir);
	Adbms6815_Daisy_Read_DmaStart(pad->eSpiNum,pad->eComDir,grppos,cmd[grppos]);
}

void Adbms6815_06_TH_Read_E(eRegGroup_Type grppos)
{
	sAdbms6815 		*pad		= &g_ad;
	u16 cmd[3]					= {RDAXA,RDAXB,RDAXC}; 
	pad->u8GrpNum = (u8)grppos;
	
	Adbms6815_Daisy_Read_DmaEnd(pad, READ_TH_Type,ADC_CV_Type);
}

void Adbms6815_Start_Balancing(void)
{
	g_ad.bSetBalancingStart = TRUE;
}
void Adbms6815_Stop_Balancing(void)
{
	g_ad.bSetBalancingStart = FALSE;
}


