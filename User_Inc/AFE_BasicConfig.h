/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Afe cmd define or struct data)                                      *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               AFE_BasicConfig.h
\author             KKD
\date               2025-11-24 
\brief              Afe cmd define or struct data
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef AFE_BASICCONFIG_H
#define AFE_BASICCONFIG_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_config.h"

/* Exported define ---------------------------------------------------------------------------------*/
//NORMAL
#define NTC_OPEN        		(s16)(-400)
#define NTC_SHRT        		(s16)(900)

#define SATCV         			(u16)(65530)        // Satuarion Cell Voltage

#define NCMD          			(u8)(2U)      // CMD Len(byte)
#define NGRP          			(u8)(3U)      // Group Count
#define NREG          			(u8)(6U)      // Data Reg Size (byte)
#define NPEC          			(u8)(2U)      // PEC Size(byte)

#define NCMDFRM         		(u8)(NCMD+NPEC)
#define NDATFRM        			(u8)(NREG+NPEC)


#define MAXBUF          		(u16)(NCMDFRM+(NSLV*NDATFRM))     // Max Tx/Rx buff Size
#define MAX_DTC_CHK_COUNT     	(u16)(3)
#define NUM_SAFETY_MECH       	(u8)(MAX_SM_Type)
#define SAFETY_FTTI_MS      	(u16)(1000)
#define DLY10US         		(10)

/* Measurement Mode (MD[1:0]) */
#define ADBMS6815_MD_27khz    	(u16)(0x000)    // 00 = 27kHz
#define ADBMS6815_MD_14khz      (u16)(0x080)    // 01 = 14kHz
#define ADBMS6815_MD_7khz     	(u16)(0x100)    // 10 = 7kHz (기본)
#define ADBMS6815_MD_3khz   	(u16)(0x180)    // 11 = 3.5kHz

/*Discharge */
#define ADBMS6815_DC_OFF    	(u16)(0x000)
#define ADBMS6815_DC_ON     	(u16)(0x010)

/* Channel Select Gpio (CHG) */
#define ADBMS6815_GPIO_CH_ALL   (u16)(0x000)
#define ADBMS6815_GPIO_CH_VREF  (u16)(0x008)    // 1000b VREF2 + VREF3


/* Pull-up Detection (PUP) */
#define ADBMS6815_PUP_OFF       (u16)(0x000)
#define ADBMS6815_PUP_ON        (u16)(0x040)

/* Ghost Enable (GHST) */
#define ADBMS6815_GHST_OFF      (u16)(0x000)
#define ADBMS6815_GHST_ON       (u16)(0x020)

/* Pattern Generator (PG) */
#define ADBMS6815_PG_OFF        (u16)(0x000)
#define ADBMS6815_PG_UNIQUE   	(u16)(0x020)    // unique patterns
#define ADBMS6815_PG_INVERSE    (u16)(0x040)    // inverse patterns

/* Channel Select (CH[2:0]) */
#define ADBMS6815_CH_ALL        (u16)(0x007)      // All cells
#define ADBMS6815_CH_SC         (u16)(0x001)
#define ADBMS6815_CH_ITMP       (u16)(0x002)
#define ADBMS6815_CH_VA         (u16)(0x003)
#define ADBMS6815_CH_VD         (u16)(0x004)
#define ADBMS6815_CH_SELFTEST 	(u16)(0x005)      // Internal ref/self-test


/* CFGA 0B */
#define CFG_0_CVMIN_08			(u8)(0x04)
#define CFG_0_CVMIN_12			(u8)(0x08)
#define CFG_0_CVMIN_16			(u8)(0x0C)

/* Manual ADC Select */
#define Adbms6815_CFG_PS_ADC1 	(u16)(0x10)     // read only Adc1
#define Adbms6815_CFG_PS_ADC2 	(u16)(0x20)     // read only Adc2

#define Adbms6815_CMD_WRCMCFG 	(u16)(0x48)     // Write Config reg
#define Adbms6815_CMD_WRCFGA  	(u16)(0x01)     // Write Config reg
#define Adbms6815_CMD_RDCFGA  	(u16)(0x02)     // Read Config reg
#define Adbms6815_CMD_WRCFGB  	(u16)(0x24)     // Write Config B reg
#define Adbms6815_CMD_RDCFGB  	(u16)(0x26)     // Read Config B reg

#define Adbms6815_CMD_RDCVAL  	(u16)(0x38)     // 1~12
#define Adbms6815_CMD_RDCVA   	(u16)(0x04)     // 1~3
#define Adbms6815_CMD_RDCVB   	(u16)(0x06)     // 4~6
#define Adbms6815_CMD_RDCVC   	(u16)(0x08)     // 7~11

#define Adbms6815_CMD_RDAUXA	(u16)(0x0C)     // 6815 aux A group
#define Adbms6815_CMD_RDAUXB  	(u16)(0x0E)     // 6815 aux B group
#define Adbms6815_CMD_RDAUXC	(u16)(0x0D)     // 6815 aux C group

#define Adbms6815_CMD_RDSTSA  	(u16)(0x10)     // 6815 Status Reg Group A
#define Adbms6815_CMD_RDSTSB  	(u16)(0x12)     // 6815 Status Reg Group B
#define Adbms6815_CMD_RDSTSC  	(u16)(0x13)     // 6815 Status Reg Group C

#define Adbms6815_CMD_RDCDA   	(u16)(0x30)     // 6815 Cell Diagnostic Register Group A
#define Adbms6815_CMD_RDCDB   	(u16)(0x31)     // 6815 Cell Diagnostic Register Group B
#define Adbms6815_CMD_RDCDC   	(u16)(0x32)     // 6815 Cell Diagnostic Register Group C


#define Adbms6815_CMD_ADCVAX  	(u16)(0x56F)    // 6815 ADCVAX
#define Adbms6815_CMD_ADAX    	(u16)(0x500)    // 6815 ADAX
#define Adbms6815_CMD_ADCV    	(u16)(0x360)    // 6815 C-pin V Start
#define Adbms6815_CMD_ADSC    	(u16)(0x168)    // 6815 S-pin V Start
#define Adbms6815_CMD_CVOW    	(u16)(0x308)    // 6815 CVOW C-pin openwire check
#define Adbms6815_CMD_ADOL    	(u16)(0x301)    // 6815 ADOL Overlap check 7Cell
#define Adbms6815_CMD_AXOW    	(u16)(0x410)    // 6815 GPIO Openwire ADC Conversion and Poll Status
#define Adbms6815_CMD_AXPG    	(u16)(0x407)    // 6815 Start Auxiliary patern Generation and Poll Status
#define Adbms6815_CMD_ADSTAT  	(u16)(0x468)    // 6815 diagnostic command that measures the following internal device parameters
#define Adbms6815_CMD_SRST    	(u16)(0x01F)    // 6815 Soft Reset


#define Adbms6815_CMD_RDSID   	(u16)(0x02C)    // 6815 Read Serial ID Register Groups
#define Adbms6815_CMD_CLRCELL 	(u16)(0x711)    // 6815 Clear Cell Voltage Register Groupsc
#define Adbms6815_CMD_CLRAUX  	(u16)(0x712)    // 6815 Clear Auxiliary Register Groups
#define Adbms6815_CMD_CLRSTAT 	(u16)(0x713)    // 6815 Clear Status Register Groups
#define Adbms6815_CMD_PLADC   	(u16)(0x714)    // 6815 Poll ADC Status
#define Adbms6815_CMD_DIAGN   	(u16)(0x715)    // 6815 Diagnose Mux and Poll Status
#define Adbms6815_CMD_CLRCD   	(u16)(0x716)    // 6815 Clear Cell Diagnostic Register Groups
#define Adbms6815_CMD_CLRFLAG 	(u16)(0x717)    // 6815 Clear Register Group C
#define Adbms6815_CMD_CVPG    	(u16)(0x519)    // 6815 Start Cell Voltage Pattern Generation and Poll Status
#define Adbms6815_CMD_CDPG    	(u16)(0x21F)    // 6815 Start Cell Diagnostic Pattern Generation and Poll Status
#define Adbms6815_CMD_STATPG  	(u16)(0x40F)    // 6815 Start Status Pattern Generation and Poll Status

#define WRCMCFG         		(Adbms6815_CMD_WRCMCFG)
#define WRCFGA         			(Adbms6815_CMD_WRCFGA)
#define WRCFGB          		(Adbms6815_CMD_WRCFGB)
#define RDCFGA          		(Adbms6815_CMD_RDCFGA)
#define RDCFGB          		(Adbms6815_CMD_RDCFGB)
#define ADCV          			(Adbms6815_CMD_ADCV)

#define ADSC          			(Adbms6815_CMD_ADSC)
#define ADAX          			(Adbms6815_CMD_ADAX)
#define CVOW          			(Adbms6815_CMD_CVOW)
#define ADOL          			(Adbms6815_CMD_ADOL)
#define AXOW          			(Adbms6815_CMD_AXOW)
#define AXPG          			(Adbms6815_CMD_AXPG)
#define SRST          			(Adbms6815_CMD_SRST)

#define RDCVA         			(Adbms6815_CMD_RDCVA)
#define RDCVB         			(Adbms6815_CMD_RDCVB)
#define RDCVC         			(Adbms6815_CMD_RDCVC)

#define RDAXA         			(Adbms6815_CMD_RDAUXA)
#define RDAXB         			(Adbms6815_CMD_RDAUXB)
#define RDAXC         			(Adbms6815_CMD_RDAUXC)

#define RDCDA         			(Adbms6815_CMD_RDCDA)
#define RDCDB         			(Adbms6815_CMD_RDCDB)
#define RDCDC         			(Adbms6815_CMD_RDCDC)

#define RDSTSA          		(Adbms6815_CMD_RDSTSA)
#define RDSTSB          		(Adbms6815_CMD_RDSTSB)
#define RDSTSC          		(Adbms6815_CMD_RDSTSC)
#define CLRCELL         		(Adbms6815_CMD_CLRCELL)
#define CLRAUX          		(Adbms6815_CMD_CLRAUX)
#define DIAGN         			(Adbms6815_CMD_DIAGN)
#define CVPG          			(Adbms6815_CMD_CVPG)
#define CDPG          			(Adbms6815_CMD_CDPG)
#define STATPG          		(Adbms6815_CMD_STATPG)
#define CLRSTAT         		(Adbms6815_CMD_CLRSTAT)
#define CLRCD         			(Adbms6815_CMD_CLRCD)
#define CLRSTAT					(Adbms6815_CMD_CLRSTAT)
#define CLRFLAG        			(Adbms6815_CMD_CLRFLAG)
#define PLADC         			(Adbms6815_CMD_PLADC)
#define ADSTAT          		(Adbms6815_CMD_ADSTAT)

#define MAX_TEMP_MOV_AVE_CNT  	(20)
#define MAX_ISOSPI_ERR_CNT    	(30)
#define MAX_OUT_CELL_CNT    	(1)

//6815 CFG
#define COMMBK          		(u8)(1U<<0)     // [0]:0bit Communication Break.
#define MCAL          			(u8)(1U<<1)     // [0]:1bit Minimum Cell Voltage for CVOW Operation.
#define CVMINL          		(u8)(1U<<2)     // [0]:2bit Digital Redundancy Path Selection.
#define CVMINH          		(u8)(1U<<3)     // [0]:3bit ADC Mode Option Bit.
#define PS0           			(u8)(1U<<4)     // [0]:4bit Digital Redundancy Path Selection. only ADC 1CH [01b]
#define PS1           			(u8)(1U<<5)     // [0]:5bit Digital Redundancy Path Selection. only ADC 2Ch [10b]
#define ADCOPT          		(u8)(1U<<6)     // [0]:6bit ADC Mode (Conversiond time)
#define REFON         			(u8)(1U<<7)     // [0]:7bit 3.3V ON Reference Powered Up.

#define FLAGD0          		(u8)(1U<<0)     // [0]:0bit
#define FLAGD1          		(u8)(1U<<1)     // [0]:1bit
#define FLAGD2          		(u8)(1U<<2)     // [0]:2bit
#define FLAGD3          		(u8)(1U<<3)     // [0]:3bit
#define FLAGD4          		(u8)(1U<<4)     // [0]:4bit
#define FLAGD5          		(u8)(1U<<5)     // [0]:5bit
#define FLAGD6          		(u8)(1U<<6)     // [0]:6bit
#define FLAGD7          		(u8)(1U<<7)     // [0]:7bit

#define MN_OSCCHK_FAST_FAIL 	(u8)(1U<<0)     // [1]:0bit FLAG_D[0]: forces oscillator counter fast (sets OSCCHK on ADC operation).
#define MN_OSCCHK_SLOW_FAIL 	(u8)(1U<<1)     // [1]:1bit FLAG_D[1]: forces oscillator counter slow (sets OSCCHK on ADC operation).

                          						//		 FLAG_D[3:2]: no overvoltage, undervoltage, or CP detection diagnostic is forced
#define MN_CP_UVCHK_FAIL    	(u8)(1U<<2)     // [1]:2bit FLAG_D[3:2]: forces undervoltage and CP detection (sets VA_UVLO and VD_UVLO on ADC operation, sets CPCHK on ADAX).
#define MN_CP_OVCHK_FAIL    	(u8)(3U<<2)     // [1]:3bit FLAG_D[3:2]: forces overvoltage detection (sets VA_OVHI and VD_OVHI on ADC operation).

#define MN_MUXFAIL        		(u8)(1U<<4)     // [1]:4bit FLAG_D[4]: forces mux diagnostic to fail (sets MUXFAIL on DIAGN, sets COMPCHK on ADC operation and DIAGN).
#define MN_RED_FAIL       		(u8)(1U<<5)     // [1]:5bit FLAG_D[5]: forces digital redundancy check to fail (ADC result values receive digital redundancy error codes) and sets the REDFAIL bit on ADC operation.
#define MN_FUSE_FAIL      		(u8)(1U<<6)     // [1]:6bit FLAG_D[6]: forces fuse ED/MED (sets A_OTP_ED and A_OTP_MED and OTP_ED and  OTP_MED on ADC operation).
#define MN_TMODE_FAIL     		(u8)(1U<<7)     // [1]:7bit FLAG_D[7]: forces TMODE diagnostic flag (sets TMODECHK) and sets the THSD flag bit.

#define OWC0          			(u8)(1U<<0)     // [2]:0bit CV Openwire set Soaking time
#define OWC1          			(u8)(1U<<1)     // [2]:1bit CV Openwire set Soaking time
#define OWC2          			(u8)(1U<<2)     // [2]:2bit CV Openwire set Soaking time
#define OWA0          			(u8)(1U<<3)     // [2]:3bit Aux CV Openwire set Soak time
#define OWA1          			(u8)(1U<<4)     // [2]:4bit Aux CV Openwire set Soak time
#define OWA2          			(u8)(1U<<5)     // [2]:5bit Aux CV Openwire set Soak time
#define OWRNG         			(u8)(1U<<6)     // [2]:6bit Openwire Soaking time range
#define SOAKON          		(u8)(1U<<7)     // [2]:7bit Charge time enable

 //6815 Register Status Group C
#define OTP_MED         		(u8)(1U<<0)     // [0]:0bit Other OTP Trim Multiple Error Detection
#define OPT_ED          		(u8)(1U<<1)     // [0]:1bit Other OTP Trim Error Detection
#define A_OTP_MED       		(u8)(1U<<2)     // [0]:2bit ADC OTP Trim Multiple Error Detection.
#define A_OTP_ED        		(u8)(1U<<3)     // [0]:3bit ADC OTP Trim Error Detection
#define VD_UVLO         		(u8)(1U<<4)     // [0]:4bit Digital Rail Undervoltage
#define VD_OVHI         		(u8)(1U<<5)     // [0]:5bit Digital Rail Overvoltage
#define VA_UVLO         		(u8)(1U<<6)     // [0]:6bit Analog Rail Undervoltage
#define VA_OVHI         		(u8)(1U<<7)     // [0]:7bit Analog Rail Overvoltage

#define OSCCHK          		(u8)(1U<<0)     // 0x01 [1]:0bit Oscillator Check
#define CPCHK         			(u8)(1U<<1)     // 0x02 [1]:1bit Charge Pump Check
#define THSD          			(u8)(1U<<2)     // 0x04 [1]:2bit Thermal Shutdown Status
#define MUXFAIL         		(u8)(1U<<3)     // 0x08 [1]:3bit Multiplexer Self Test Result
#define TMODCHK         		(u8)(1U<<4)     // 0x10 [1]:4bit Test Mode Detection
#define SLEEP         			(u8)(1U<<5)     // 0x20 [1]:5bit Sleep State Detection
#define COMPCHK         		(u8)(1U<<6)     // 0x40 [1]:6bit ADC Current Compensation Logic Error Detection
#define REDFAIL         		(u8)(1U<<7)     // 0x80 [1]:7bit Digital Redundancy Failure Detection


//Clear Flag Bit
#define CFD0_MED          		(u8)(1U<<0)     // 0x01 
#define CFD0_ED       			(u8)(1U<<1)     // 0x02 
#define CFD0_AMED     			(u8)(1U<<2)     // 0x04 
#define CFD0_AED     			(u8)(1U<<3)     // 0x08 
#define CFD0_VDUV  				(u8)(1U<<4)     // 0x10 
#define CFD0_VDOV     			(u8)(1U<<5)     // 0x20 
#define CFD0_VAUV    			(u8)(1U<<6)     // 0x40 
#define CFD0_UAOV     			(u8)(1U<<7)     // 0x80 

#define CFD1_OSCCHK          	(u8)(1U<<0)     // 0x01 
#define CFD1_CPCHK         		(u8)(1U<<1)     // 0x02 
#define CFD1_THSD          		(u8)(1U<<2)     // 0x04 
#define CFD1_UNDIFN         	(u8)(1U<<3)     // 0x08 
#define CFD1_TMODE         		(u8)(1U<<4)     // 0x10 
#define CFD1_SLEEP         		(u8)(1U<<5)     // 0x20 
#define CFD1_COMP         		(u8)(1U<<6)     // 0x40 
#define CFD1_REDFAIL         	(u8)(1U<<7)     // 0x80 




/* Exported macro ----------------------------------------------------------------------------------*/

/* Exported typedef --------------------------------------------------------------------------------*/




/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef enum{
  NTC_NORMAL_Type       = 0,
  NTC_OPEN_Type,
  NTC_SHORT_Type,
  MAX_NTC_OPENSHORT_Type
}eAdNtcOpenShort_Type;

typedef enum{
  ComDir_A_Type       = 0,      // Communication Direction A
  ComDir_B_Type,                // Communication Direction B
  MAX_ComDir_Type
}eAdComDir_Type;

typedef enum{
  SPI1_Type         = 0U,
  SPI2_Type,
  MAX_SPI_Type
}eAdSpi_Type;

typedef enum{
  WR_WR_Type          = 0,
  WR_RD_Type,
  MAX_RW_Type
}eAdRw_Type;

typedef enum{
  READ_CV_Type        = 0,
  READ_TH_Type,
  READ_ST_Type,
  MAX_READ_Type
}eAdRead_Type;

typedef enum{
  ADC_CV_Type        = 0,
  ADC_SV_Type,
  ADC_OW_Type,               // cv open check cell data read
  ADC_EV_Type,               // cv even read
  ADC_OD_Type,               // cv odd read
  ADC_CV_AD1_Type,             // adc1 read All ch
  ADC_CV_AD2_Type,              // adc2 read All ch
  MAX_ADC_RD_Type
}eAd_Adc_Type;

typedef enum{
  CMD_INIT_Type       = 0,
  CMD_CV_START_Type,
  CMD_CV_READ_Type,

  CMD_SV_START_Type,
  CMD_SV_READ_Type,

  CMD_CVOW_START_Type,
  CMD_CVOW_READ_Type,

  CMD_TH_START_Type,
  CMD_TH_READ_Type,

  CMD_BAL_START_Type,
  CMD_BAL_STOP_Type,

  CMD_OFF_Type,
  CMD_SM_START_Type
}eAdCmd_Type;

typedef enum{
  SM1_Type          = 1,		// WRCFGA  FLAG_D = 1로 설정후 READ C해서 MUXFAIL 플래그 1인지 확인
  SM2_Type			= 2,
  SM4_Type          = 4,
  SM5_Type          = 5,
  SM6_Type          = 6,
  SM7_Type          = 7,
  SM8_Type          = 8,
  SM9_Type          = 9,
  SM10_Type          = 10,
  SM11_Type          = 11,
  SM12_Type          = 12,
  SM13_Type          = 13,
  SM15_Type          = 15,
  SM16_Type          = 16,
  SM17_Type          = 17,
  SM19_Type          = 19,
  SM22_Type          = 22,
  SM23_Type          = 23,
  SM24_Type          = 24,
  SM26_Type          = 26,
  SM28_Type          = 28,
  SM29_Type          = 29,
  SM30_Type          = 30,
  SM31_Type          = 31,
  SM33_Type          = 33,
  SM34_Type          = 34,
  SM37_Type          = 37,
  SM38_Type          = 38,
  SM41_Type          = 41,
  SM42_Type          = 42,
  SM43_Type          = 43,
  SM44_Type          = 44,
  SM45_Type          = 45,
  SM46_Type          = 46,
  SM50_Type          = 50,
  MAX_SM_Type
}eSM_Num;

typedef enum{
  GA_TYPE             = 0,
  GB_TYPE,
  GC_TYPE,
  MAX_GRP_TYPE
}eRegGroup_Type;

#pragma pack(push,1)

typedef enum{                                                                           //KKD 2025-12-21 -
  AdReadProc_NoOp_Type      = 0,
  AdReadProc_Start_Type,
  AdReadProc_StartDma_Type,
  AdReadProc_Read_Type,
  Max_AdReadProc_Type
}Enum_AdReadProc_Type;

typedef struct
{
  eRegGroup_Type      	eRegGrp[NSPI];
  eAdComDir_Type      	eComDir;      // if else로만 가 능
  eAdSpi_Type       	eSpiNum;
  eAdNtcOpenShort_Type  eThSts[NSLV][NSTH];
  eAdRead_Type     		eRead;
  eAdRw_Type        	eRw;
  eSM_Num				eSM;
  //u6815CfgAr0       uCfgAr0;
  //u6815CfgAr1       uCfgAr1;
  //u6815CfgAr2       uCfgAr2;
/*
  BOOL bStartCv;
  BOOL bReadCvDone;
  BOOL bCastCvDone;

  BOOL bStartTh[NSPI];
  BOOL bReadThDone[NSPI];
  BOOL bCastThDone[NSPI];
*/
  BOOL bAdbmsInitDone;
  BOOL bDmaReadDone[NSPI];

  //BOOL bLs_AdVStartFlag;                                                            //KKD 2025-12-21 -
  Enum_AdReadProc_Type eAdCvRead[NSPI];                                                 //KKD 2025-12-21 -

  //BOOL bLs_AdTStartFlag;                                                            //KKD 2025-12-21 -
  Enum_AdReadProc_Type eAdThRead[NSPI];                                                 //KKD 2025-12-21 -

  //BOOL bLs_AdAuxStartFlag;                                                          //KKD 2025-12-21 -
  Enum_AdReadProc_Type eAdAuxRead[NSPI];                                               //KKD 2025-12-21 -

  u8  u8CvRegGroupCnt;
  u8  u8ThRegGroupCnt;
  u8  u8CvBalFlag[NSPI][NSLV];


  u16 u16Cv[NSPI][NSLV][NSCV];        // cv288

  /* SM */
  u16 u16Sv[NSPI][NSLV][NSCV];        // sv288
  u16 u16Ow[NSPI][NSLV][NSCV];        // owcv 288
  u16 u16OwPos[NSPI][NSLV];

  u16 u16EvCv[NSPI][NSLV][NSCV];        // even cv
  u16 u16OdCv[NSPI][NSLV][NSCV];        // odd cv

  u16 u16Ad1Cv[NSPI][NSLV][NSCV];       // adc only ch1 read cv
  u16 u16Ad2Cv[NSPI][NSLV][NSCV];       // adc only ch2 read cv


  u16 u16DCcv[NSPI][NSLV][NSCV];
  u16 u16DCv[NSPI][NSLV][NSCV];
  u16 u16DSv[NSPI][NSLV][NSCV];

  u16 u16CvChkDone[NSPI][NSLV][NSCV];

  s16 s16Th[NSPI][NSLV][NSTH];
  u16 u16ThAdc[NSPI][NSLV][NSTH];

  u16 u16ThOwPu[NSPI][NSLV][NSTH];
  u16 u16ThOwPd[NSPI][NSLV][NSTH];

  
  u16 u16fBal[NSPI][NSLV];


  u16 u16Adol1[NSPI][NSLV];
  u16 u16Adol2[NSPI][NSLV];
  u16 u16AdolDiff[NSPI][NSLV];



  u16 u16Vad[NSPI][NSLV];
  u16 u16Vdd[NSPI][NSLV];
  u16 u16VadDiff[NSPI][NSLV];

  u16 u16Vref2[NSPI][NSLV];
  u16 u16Vref3[NSPI][NSLV];
  u16 u16CmdCnt;

  u8 u8ReadFailCheck[NSPI][MAX_READ_Type][NSLV*NSPI];
  BOOL bSetBalancingStart;

  u8 u8GrpNum;
}sAdbms6815;
#if 0
/* Status A register Data structure*/
typedef struct{
  u16  vref2;
  u16  itmp;
  u16  vref3;
} sta_;

/* Status B register Data structure*/
typedef struct{
  u16 vd;
  u16 va;
  u16 vr4k;
} stb_;
/* Status C register Data structure*/
typedef struct{
  u16      cs_flt;
  uint8_t       va_ov   :1;
  uint8_t       va_uv   :1;
  uint8_t       vd_ov   :1;
  uint8_t       vd_uv   :1;
  uint8_t       otp1_ed :1;
  uint8_t       otp1_med:1;
  uint8_t       otp2_ed :1;
  uint8_t       otp2_med:1;
  uint8_t       vde     :1;
  uint8_t       vdel    :1;
  uint8_t       comp    :1;
  uint8_t       spiflt  :1;
  uint8_t       sleep   :1;
  uint8_t       thsd    :1;
  uint8_t       tmodchk :1;
  uint8_t       oscchk  :1;
} stc_;


typedef struct{
  u8  comm_bk   : 1;
  u8  mcal    : 1;
  u8  cvmin   : 2;
  u8  ps      : 2;
  u8  adcopt    : 1;
  u8  refon   : 1;

  u8  flag_d0   : 1;
  u8  flag_d1   : 1;
  u8  flag_d2   : 1;
  u8  flag_d3   : 1;
  u8  flag_d4   : 1;
  u8  flag_d5   : 1;
  u8  flag_d6   : 1;
  u8  flag_d7   : 1;

  u8  owc     : 3;
  u8  owa     : 3;
  u8  owrng   : 1;
  u8  soakon    : 1;

  u8  gpo     : 7;
  u8  rsvd    : 1;

  u8  gpi     : 7;
  u8  rsvd    : 1;
  u8  dType   : 4;
  u8  rev     : 4;
}cfgar;

typedef struct{
  u16 vuv     : 12;
  u16 mov     : 12;

  u8  dtco    : 6;
  u8  dtrng   : 1;
  u8  dtmen   : 1;

  u16 dcc     : 12;
  u8  rsvd    : 3;
  u8  mute_st   : 1;
}cfgbr;
#endif

typedef struct{
    u8  id;                           // SM 번호 (예: SM1, SM2, ...)
    //eSM_Type  eType;                    // SM 유형 (HW, SW, SYS)
    //eSM_State estate;                   // 현재 상태 (INIT, WAIT, READ, ...)
    //eSM_Result  eresult;                // Pass/Fail 결과
  eSM_Num   eSMNum;
    u16 u16retry_count;                 // 재시도 횟수
    u32 u32tick_elapsed;                // 경과 시간
  u16 u16DtcCount[NSPI][NSLV][NUM_SAFETY_MECH]; // 진단 에러 카운트

  u8  u8CurrSmNum;
  BOOL bSM_busy;
  BOOL bSM_result[NUM_SAFETY_MECH];
  u8  u8CmdCount;
   
}sSM_Task;
#pragma pack(pop)
/* Exported variables ------------------------------------------------------------------------------*/
/* Exported function prototypes --------------------------------------------------------------------*/

#endif //__AFE_BASICCONFIG_H__

