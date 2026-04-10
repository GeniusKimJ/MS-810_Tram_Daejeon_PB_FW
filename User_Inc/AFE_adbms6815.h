/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               AFE_adbms6815.h
\author             KKD
\date               2025-11-24 
\brief              Cell/Temp sensor - spi
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef AFE_ADBMS6815_H
#define AFE_ADBMS6815_H
/* Includes ---------------------------------------------------------------------------------------*/
//#include <ms_main.h>
#include "ms_config.h"
//#include "ms_can.h"
#include "ms_spi.h"
#include "spi.h"

/* Exported define ---------------------------------------------------------------------------------*/
//#define TOTAL_CELL_COUNT		(14)			
//#define TOTAL_MON_IC_COUNT	(12)			
//#define PACK_V_DIFF			(500)			
//#define TEMP_AVE_USE_COUNT	(10)				// 14 - Up 2, Down 2			
//#define AFE_TOP_START_INDEX	(7)					// Do not USE	

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
extern sAdbms6815 	g_ad;
extern sSM_Task 	g_sSm;

/* Exported variables ------------------------------------------------------------------------------*/
extern u8 	g_u8CmdCount;
extern u8 	g_u8AfePos[NSLV];

extern u8 	g_u8Tx[NSPI][NGRP][MAXBUF];
extern u8 	g_u8Rx[NSPI][NGRP][MAXBUF];

extern u8	g_u8Adbms6815_CfgrA[NSPI][NSLV][NREG];
extern u8	g_u8Adbms6815_CfgrB[NSPI][NSLV][NREG];

extern u16 	g_u16Adbms6815_Pec15Table[256];	
extern u16 	g_u16Adbms6815_Crc15_Poly;		
extern u16 	g_u16Adbms6815_Pec10Table[256];	
extern u16 	g_u16Adbms6815_Crc10_Poly;		
extern u16 	g_u16Adbms6815_VoltTable_10k[132]; 

extern u32 tick_s[10]	;
extern u32 tick_e[10]	;
extern u32 tick_d[10]	;



/* Exported function prototypes --------------------------------------------------------------------*/
u16		Adbms6815_CmdPec15(u8 *data, u16 len);
u16		Adbms6815_DatPec10(u8 *data, u16 len, BOOL bIsRxCmd);
void	Adbms6815_Init_Pec15_Table(void);
void 	Adbms6815_Init_Pec10_Table(void);

void	Adbms6815_ChipSel(eAdSpi_Type eSpiNum, eAdComDir_Type bComDir, BOOL bSelect);


void 	Adbms6815_Wakeup(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir);

void 	Adbms6815_Cmd_Write(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir, u16 u16Cmd);
void 	Adbms6815_ClearCmd_Write(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd, u8 *data);
BOOL	Adbms6815_Daisy_ReadEnd(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir);
void 	Adbms6815_Daisy_Read_DmaStart(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir, eRegGroup_Type eRegGroup, u16 u16Cmd);
//void 	Adbms6815_Daisy_Read_DmaEnd(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir, eRegGroup_Type eRegGroup, sAdbms6815 *pAd, eAdRead_Type eRdType, eAd_Adc_Type eRdFusaType);
void 	Adbms6815_Daisy_Read_DmaEnd(sAdbms6815 *pAd, eAdRead_Type eRdType, eAd_Adc_Type eRdFusaType);
BOOL	Adbms6815_Daisy_CfgRead(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir, u16 u16Cmd, u8 u8Cfgr[][NSLV][6]);
BOOL	Adbms6815_RdCfgA(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir);
BOOL	Adbms6815_RdCfgB(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir);
void	Adbms6815_Daisy_CfgWrite(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd,u8 u8Cfgr[][NSLV][6]);
void 	Adbms6815_Daisy_BalWrite(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir,u16 u16Cmd,u8 u8Cfgr[][NSLV][6]);

void 	Adbms6815_WrCfgA(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir, u8 EnPs, u8 FlagD, u8 Soak, u8 Owrn);
void 	Adbms6815_WrCfgB(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir);
void 	Adbms6815_WrCmCfg(eAdSpi_Type eSpiNum,eAdComDir_Type eComDir);

void 	Adbms6815_WrCfgBal(eAdSpi_Type eSpiNum, eAdComDir_Type eComDir);
void 	Adbms6815_Cal_AfePos(eAdComDir_Type eComDir, eAdRw_Type eWriteRead);
u8 		Adbms6815_Get_AfePos(u8 u8AfePos);

u16 	Adbms6815_CmdCnt(u16 *pu16ReadPec);
void 	Adbms6815_Init(void);
void	Adbms6815_Make_BalFlag(Pkt_Rack *pBmsData,sAdbms6815 *pAd);
void 	Adbms6815_Balancing_Proc(void);

void 	Adbms6815_CV_Proc(void);
void    Adbms6815_Th_Proc(void);

void 	Adbms6815_Start_Balancing(void);
void 	Adbms6815_Stop_Balancing(void);

void 	Adbms6815_01_CV_Config(void);
void 	Adbms6815_01_CV_StartConv(void);
void 	Adbms6815_02_CV_Read_S(eRegGroup_Type grppos);
void 	Adbms6815_03_CV_Read_E(eRegGroup_Type grppos);
void 	Adbms6815_04_TH_Config(void);
void 	Adbms6815_05_TH_Read_S(eRegGroup_Type grppos);
void 	Adbms6815_06_TH_Read_E(eRegGroup_Type grppos);

#endif //__AFE_ADBMS6815_H__

