/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Battery Protection)                                           	   *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_eep.c
\author             KKD
\date               2025-11-21 
\brief              보호동작
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_det.h>
#include <ms_util.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

//Warning, fault
#if EN_ORIDET //	ORI DET	/////////////////////////////////////////////////////////////////////////////////////


u32 fDet = 0U;

// 발생 순서를 기록하는 배열과 현재 순서 인덱스
///u32 detFlag[MAX_FLAGS];

u32 Det_Occur(void) {																																	// per 100ms
	u32 chg		= ((RackPkt.fbmssts&FCHGING) !=0U) ? (u32)1:(u32)0;
	u32 dchg	= ((RackPkt.fbmssts&FDCHING) !=0U) ? (u32)1:(u32)0;
	
	if(CellOVPD(RackPkt.maxcv) == 1U)		{fDet |= FCOVP;}			// Cell over voltage Protection detect
	if(CellUVPD(RackPkt.mincv) == 1U) 		{fDet |= FCUVP;}			// Cell under voltage Protection detect
	if(PackOVPD(RackPkt.pv[PVINP]) == 1U) 	{fDet |= FPOVP;}			// Pack over voltage Protection detect
	if(PackUVPD(RackPkt.pv[PVINP]) == 1U) 	{fDet |= FPUVP;}			// Pack under voltage Protection detect
	if(ChagOCPD(RackPkt.pi[IREAL]) == 1U) 	{fDet |= FCOCP;}			// Charge over current Protection detect
	if(DchgOCPD(RackPkt.pi[IREAL]) == 1U) 	{fDet |= FDOCP;}			// Discharge over current Protection detect
	if(ChagOTPD(RackPkt.maxth, chg) == 1U) 	{fDet |= FCOTP;}			// Charge over TMEP Protection detect
	if(ChagUTPD(RackPkt.minth, chg) == 1U) 	{fDet |= FCUTP;}			// Charge under TMEP Protection detect
	if(DchgOTPD(RackPkt.maxth, dchg) == 1U)	{fDet |= (FDOTP|FCOTP);}	// Discharge over TMEP Protection detect
	if(DchgUTPD(RackPkt.minth, dchg) == 1U)	{fDet |= (FDUTP|FCUTP);}	// Discharge under TMEP Protection detect
	
/*	if(fcomfail == 1U){
		fDet |= FCOMF;
	}*/
	if(CellVImbalPD(RackPkt.diffcv) == 1U) 	{fDet |= FIMBVP;}			// Cell imbalance Protection Detection
	if(CellTImbalPD(RackPkt.diffcv) == 1U) 	{fDet |= FIMBVP;}			// Cell imbalance Protection Detection
	return fDet;
}

u32 Det_Release(void) {																																		// per 100ms
	u32 chg		= ((RackPkt.fbmssts&FCHGING) !=0U) ? (u32)1:(u32)0;
	u32 dchg	= ((RackPkt.fbmssts&FDCHING) !=0U) ? (u32)1:(u32)0;

	if(CellOVPR(RackPkt.maxcv) == 1U) 		{fDet &= ~FCOVP;}			// Cell over voltage Protection release
	if(CellUVPR(RackPkt.mincv) == 1U) 		{fDet &= ~FCUVP;}			// Cell under voltage Protection release
	if(PackOVPR(RackPkt.pv[PVINP]) == 1U)	{fDet &= ~FPOVP;}			// Pack over voltage Protection release
	if(PackUVPR(RackPkt.pv[PVINP]) == 1U)	{fDet &= ~FPUVP;}			// Pack under voltage Protection release
	
	if(ChagOCPR(RackPkt.pi[IREAL]) == 1U)	{fDet &= ~FCOCP;}			// Charge over current Protection release
	if(DchgOCPR(RackPkt.pi[IREAL]) == 1U) 	{fDet &= ~FDOCP;}			// Discharge over current Protection release

	if(ChagOTPR(RackPkt.maxth, chg) == 1U)	{fDet &= ~FCOTP;}			// Charge over TMEP Protection release
	if(ChagUTPR(RackPkt.minth, chg) == 1U) 	{fDet &= ~FCUTP;}			// Charge under TMEP Protection release
	if(DchgOTPR(RackPkt.maxth, dchg) == 1U)	{fDet &= ~FDOTP;}			// Discharge over TMEP Protection release
	if(DchgUTPR(RackPkt.minth, dchg) == 1U)	{fDet &= ~FDUTP;}			// Discharge under TMEP Protection release
/*	if(fcomfail == 0U){
		fDet &= ~FCOMF;
	}
	if(fcomfail == 0U){
		fDet &= ~FCOMF;
	}*/

	if(CellVImbalPR(RackPkt.diffcv) == 1U) 	{fDet &= ~FIMBVP;}			// Cell imbalance Protection Detection release
	if(CellTImbalPR(RackPkt.diffth) == 1U) 	{fDet &= ~FIMBVP;}			// Cell imbalance Protection Detection release
	return fDet;
}

u8 CellOVPD(u16 volt) 
{											/* Cell over voltage Protection detect, 100ms */
	static u8 Dly_Cell_OvpD = 0U;
	u8 status = 0U;
	
	if(volt > CELLOVPD) {
		if(Dly_Cell_OvpD > DLYCOVPD)	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Cell_OvpD; }	// 0, 1, 2, 3 30
	} else {
		if(Dly_Cell_OvpD > 0U) 			{ --Dly_Cell_OvpD; }
	}return status;
}

u8 CellUVPD(u16 volt) 
{											/* Cell under voltage Protection detect */
	static u8 Dly_Cell_UvpD = 0U;
	u8 status = 0U;

	if(volt < CELLUVPD) {
		if(Dly_Cell_UvpD > DLYCUVPD) 	{ status = 1U; }
		if(status == 0U) 				{++Dly_Cell_UvpD; }// 1, 2, 3
	} else {
		if(Dly_Cell_UvpD > 0U) 			{ --Dly_Cell_UvpD; }
	}return status;
}


u8 PackOVPD(u32 volt) 
{											// Pack over voltage Protection detect
	static u8 Dly_Pack_OvpD = 0U;
	u8 status = 0U;

	if(volt > PACKOVPD) {
		if(Dly_Pack_OvpD > DLYPOVPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Pack_OvpD; }
	} else {
		if(Dly_Pack_OvpD > 0U) 			{ --Dly_Pack_OvpD; }
	}return status;
}

u8 PackUVPD(u32 volt) 
{											// Pack under voltage Protection detect
	static u8 Dly_Pack_UvpD = 0U;
	u8 status = 0U;

	if(volt < PACKUVPD) {
		if(Dly_Pack_UvpD > DLYPUVPD) 	{ status = 1U; }
		if(status == 0U) 				{++Dly_Pack_UvpD; }
	} else {
		if(Dly_Pack_UvpD > 0U) 			{ --Dly_Pack_UvpD;}
	}return status;
}

u8 ChagOCPD(s32 curr) 
{											// Charge over current Protection detect
	static u8 Dly_Chag_OcpD = 0;
	u8 status = 0U;

	if(curr > CHAGOCPD) {
		if(Dly_Chag_OcpD > DLYCOCPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Chag_OcpD; }
	} else {
		if(Dly_Chag_OcpD > 0U) 			{ --Dly_Chag_OcpD; }
	}return status;
}

u8 DchgOCPD(s32 curr) 
{																// Discharge over current Protection detect
	static u8 Dly_Dchg_OcpD = 0;
	u8 status = 0U;

	if(curr < DCHGOCPD) {
		if(Dly_Dchg_OcpD > DLYDOCPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Dchg_OcpD; }
	} else {
		if(Dly_Dchg_OcpD > 0U) 			{ --Dly_Dchg_OcpD; }
	}return status;
}

u8 ChagOTPD(s16 temp, u32 fchag) 
{																// Charge over TMEP Protection detect
	static u8 Dly_Chag_OtpD = 0;
	u8 status = 0U;

	if((fchag != 0U) && (temp > (s16)CHAGOTPD)) {
		if(Dly_Chag_OtpD > DLYCOTPD) 	{ status = 1U;}
		if(status == 0U) 				{ ++Dly_Chag_OtpD; }
	} else {
		if(Dly_Chag_OtpD > 0U) 			{ --Dly_Chag_OtpD; }
	}return status;
 }

u8 ChagUTPD(s16 temp, u32 fchag) 
{																// Charge under TMEP Protection detect
	static u8 Dly_Chag_UtpD = 0;
	u8 status = 0U;

	if((fchag != 0U)&& (temp < CHAGUTPD)) {
		if(Dly_Chag_UtpD > DLYCUTPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Chag_UtpD;}
	} else {
		if(Dly_Chag_UtpD > 0U) 			{ --Dly_Chag_UtpD;}
	}return status;
}

u8 DchgOTPD(s16 temp, u32 fdchg) 
{							// Discharge over TMEP Protection detect
	static u8 Dly_Dchg_OtpD = 0;
	u8 status = 0U;

	if(temp > DCHGOTPD) {
		if(Dly_Dchg_OtpD > DLYDOTPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Dchg_OtpD; }
	} else {
		if(Dly_Dchg_OtpD > 0U) 			{ --Dly_Dchg_OtpD; }
	}return status;
}

u8 DchgUTPD(s16 temp, u32 fdchg) 
{																			// Discharge over TMEP Protection detect
	static u8 Dly_Dchg_UtpD = 0;
	u8 status = 0U;

	if(temp < DCHGUTPD) {
		if(Dly_Dchg_UtpD > DLYDUTPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Dchg_UtpD;}
	} else {
		if(Dly_Dchg_UtpD > 0U) 			{ --Dly_Dchg_UtpD; }
	}return status;
}

//230410 SJH Ref.Temp
u8 ADTempFD(s16 temp) 
{																			// Reference TMEP Protection detect
	static u8 Dly_Ad_TempfD = 0;
	u8 status = 0U;

	if((temp < REFTPD1) || (temp > REFTPD2)) {
		if(Dly_Ad_TempfD > DLYREFTPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Ad_TempfD; }
	} else {
		if(Dly_Ad_TempfD > 0U)			{ --Dly_Ad_TempfD; }
	}return status;
}

u8 ADTempFDR(s16 temp) 
{																			// Reference TMEP Protection release
	static u8 Dly_ADTemp_FDR = 0;
	u8 status = 0U;

	if((temp > REFTPD1) && (temp < REFTPD2)) {								// Ref 10K(25.0C : 21.0C~29.0C)
		if(Dly_ADTemp_FDR > DLYREFTDR) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_ADTemp_FDR; }
	} else {
		if(Dly_ADTemp_FDR > 0U) 		{ --Dly_ADTemp_FDR; }
	}return status;
}


u8 AuxiOVPD(u16 volt) 
{																			// Auxiliary over voltage Protection detect
	static u8 Dly_Auxi_OvpD = 0;
	u8 status = 0U;

	if(volt >= AUXIOVPD) {
		if(Dly_Auxi_OvpD > DLYAOVPD) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Auxi_OvpD; }
	} else {
		if(Dly_Auxi_OvpD > 0U) 			{ --Dly_Auxi_OvpD; }
	}return status;
}

u8 AuxiUVPD(u16 volt) 
{																			// Auxiliary under voltage Protection detect
	static u8 Dly_Auxi_UvpD = 0;
	u8 status = 0U;

	if(volt <= AUXIUVPD) {
		if(Dly_Auxi_UvpD > DLYAUVPD)	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Auxi_UvpD; }
	} else {
		if(Dly_Auxi_UvpD > 0U) 			{ --Dly_Auxi_UvpD; }
	}return status;
}

u8 CellVImbalPD(s16 volt)
{																			// Cell imbalance Fault Detection
	static u8 Dly_Cell_VImbalpD = 0;
	u8 status = 0U;

	if(volt < 1500){
		if(volt > (s16)CVIMBLPD) {
			if(Dly_Cell_VImbalpD >= DLYIMBPD) { status = 1U; }
			if(status == 0U) 			{ ++Dly_Cell_VImbalpD; }
		} else {
			if(Dly_Cell_VImbalpD > 0U) 	{ --Dly_Cell_VImbalpD; }
		}
	}return status;
}
u8 CellTImbalPD(s16 tmpe)
{																			// Cell imbalance Fault Detection
	static u8 Dly_Cell_TImbalpD = 0;
	u8 status = 0U;

	if(tmpe < 1500){
		if(tmpe > (s16)THDIFFPD) {
			if(Dly_Cell_TImbalpD >= DLYTDFPD) { status = 1U; }
			if(status == 0U) 			{ ++Dly_Cell_TImbalpD; }
		} else {
			if(Dly_Cell_TImbalpD > 0U) 	{ --Dly_Cell_TImbalpD; }
		}
	}return status;
}

u8 VoltMeasFD(u32 pvin, u32 cvsum) 
{																			// Volt difference Protection Detection
	static u8 Dly_Volt_MeasfD = 0;
	u8 status = 0U;
	s32	volt;

	if(pvin > cvsum) {
		volt = (s32)pvin - (s32)cvsum;
	} else	{
		volt = (s32)cvsum - (s32)pvin;
	}

	if(volt >= CVMEASFD) {
		if(Dly_Volt_MeasfD > DLYCVMFD)	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Volt_MeasfD; }
	} else {
		if(Dly_Volt_MeasfD > 0U) 		{ --Dly_Volt_MeasfD; }
	}return status;
}

u8 CurrMeasFD(u8 *p) 
{																			// CURR Measurement Fault Detection
	static u8 	Dly_Curr_MeasfD = 0U;
	u8 status = 0U;
	s32 diffpi, *pi = (void *)p;
	s32	datapifd;

	pi[0] = 5000L;
	datapifd = 2000L;
	if(labs(pi[0]) > CURRL) {
		status = 0U;
	} else {	
		diffpi = labs(pi[0] - datapifd);
		if(diffpi >= PIMEASFD) {
			if(Dly_Curr_MeasfD > DLYPIMFD) {
				status = 1U;
			}
			if(status == 0U) {
				++Dly_Curr_MeasfD;
			}
		} else {
			if(Dly_Curr_MeasfD > 0U) {
				--Dly_Curr_MeasfD;
			}
		}
	}
	return status;
}
// Detection Release
u8 CellOVPR(u16 volt) 
{																			// Cell Over Voltage release
	static u8 Dly_Cell_OvpR = 0;
	u8 status = 0U;

	if(volt < CELLOVPR){
		if(Dly_Cell_OvpR > DLYCOVPR) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Cell_OvpR; }
	} else {
		if(Dly_Cell_OvpR > 0U) 			{ --Dly_Cell_OvpR; }
	}return status;
}

u8 CellUVPR(u16 volt) 
{																			// Cell under Voltage release
	static u8 Dly_Cell_UvpR = 0;
	u8 status = 0U;

	if(volt > CELLUVPR) {
		if(Dly_Cell_UvpR > DLYCUVPR) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Cell_UvpR; }
	} else {
		if(Dly_Cell_UvpR > 0U) 			{ --Dly_Cell_UvpR; }
	}return status;
}

u8 CellPackDchgOVPR(s32 curr) 
{																			// over under Voltage release @ Discharge -2.0A
	static u8 Dly_CellPack_DchgOvpR = 0;
	u8 status = 0U;

	if(curr <= CHAGOCPR) {
		if(Dly_CellPack_DchgOvpR > DLYVDHGR) { status = 1U; }
		if(status == 0U) 				{ ++Dly_CellPack_DchgOvpR; }
	} else {
		if(Dly_CellPack_DchgOvpR > 0U) 	{ --Dly_CellPack_DchgOvpR; }
	}return status;
}

u8 CellPackChagUVPR(s32 curr) 
{																			// Cell under Voltage release@Chg2.0A
	static u8 Dly_CellPack_ChagUVPR = 0;
	u8 status = 0U;

	if(curr >= DCHGOCPR) {
		if(Dly_CellPack_ChagUVPR > DLYVCHGR) { status = 1U; }
		if(status == 0U) 				{ ++Dly_CellPack_ChagUVPR; }
	} else {
		if(Dly_CellPack_ChagUVPR > 0U) 	{ --Dly_CellPack_ChagUVPR; }
	}return status;
}

u8 PackOVPR(u32 volt) {											// Pack Over Voltage release
	static u8 Dly_Pack_OvpR = 0;
	u8 status = 0U;

	if(volt < PACKOVPR)	{
		if(Dly_Pack_OvpR > DLYPOVPR) {
			status = 1U;
		}
		if(status == 0U) {
			++Dly_Pack_OvpR;
		}
	} else {
		if(Dly_Pack_OvpR > 0U) {
			--Dly_Pack_OvpR;
		}
	}
	return status;
}

u8 PackUVPR(u32 volt) {											// Pack under Voltage release
	static u8 Dly_Pack_UvpR = 0;
	u8 status = 0U;

	if(volt > PACKUVPR) {
		if(Dly_Pack_UvpR > DLYPUVPR) {
			status = 1U;
		}
		if(status == 0U) {
			++Dly_Pack_UvpR;
		}
	} else {
		if(Dly_Pack_UvpR > 0U) {
			--Dly_Pack_UvpR;
		}
	}
	return status;
}

u8 ChagOCPR(s32 curr) {											// Charge over current Protection Release
	static u8 Dly_Chag_OcpR = 0;
	u8 status = 0U;

	if(curr < CHAGOCPR) {
		if(Dly_Chag_OcpR > DLYCOCPR){
			status = 1U;
		}

		if(status == 0U){
			++Dly_Chag_OcpR;
		}
	} else {
		if(Dly_Chag_OcpR > 0U) {
			--Dly_Chag_OcpR;
		}
	}
	return status;
}

u8 DchgOCPR(s32 curr) {											// Discharge over current Protection Release
	static u8 Dly_Dchg_OcpR = 0;
	u8 status = 0U;

	if(curr > DCHGOCPR) {
		if(Dly_Dchg_OcpR > DLYDOCPR) {
			status = 1U;
		}
		if(status == 0U) {
			++Dly_Dchg_OcpR;
		}
	} else {
		if(Dly_Dchg_OcpR > 0U) {
			--Dly_Dchg_OcpR;
		}
	}
	return status;
}
u8 ChagOTPR(s16 temp, u32 fdchg) {							// Charge over TMEP Protection Release
	static u8 Dly_Chag_OtpR = 0;
	u8 status = 0U;

	if((fdchg != 0U) || (temp < (s16)CHAGOTPR)) {
		if(Dly_Chag_OtpR > DLYCOTPR) {
			status = 1U;
		}
		if(status == 0U) {
			++Dly_Chag_OtpR;
		}
	} else {
		if(Dly_Chag_OtpR > 0U) {
			--Dly_Chag_OtpR;
		}
	}
	return status;
}

u8 ChagUTPR(s16 temp, u32 fdchg) {							// Charge under TMEP Protection Release
	static u8 Dly_Chag_UtpR = 0;
	u8 status = 0U;

	if(fdchg == 0U) {
		status = 0U;
	} else {
		if(temp > CHAGUTPR) {
			if(Dly_Chag_UtpR > DLYCUTPR) {
				status = 1U;
			}
			if(status == 0U)  {
				++Dly_Chag_UtpR;
			}
		} else {
			if(Dly_Chag_UtpR > 0U) {
				--Dly_Chag_UtpR;
			}
		}
	}
	return status;
}

u8 DchgOTPR(s16 temp, u32 fchag) {							// Discharge over TMEP Protection Release
    static u8 Dly_Dchg_OtpR = 0;
	u8 status = 0U;

	if(fchag == 0U) {
		status = 0U;
	} else {
		if(temp < DCHGOTPR) {
			if(Dly_Dchg_OtpR > DLYDOTPR) {
				status = 1U;
			}
			if(status == 0U) {
				++Dly_Dchg_OtpR;
			}
		} else {
			if(Dly_Dchg_OtpR > 0U) {
				--Dly_Dchg_OtpR;
			}
		}
	}
	return status;
}


u8 DchgUTPR(s16 temp, u32 fchag) {							// Discharge under TMEP Protection Release
	static u8 Dly_Dchg_UtpR = 0;
	u8 status = 0U;

	if(fchag == 0U) {
		status = 0U;
	} else {
		if(temp > DCHGUTPR) {
			if(Dly_Dchg_UtpR > DLYDUTPR) {
				status = 1U;
			}
			if(status == 0U) {
				++Dly_Dchg_UtpR;
			}
		} else {
			if(Dly_Dchg_UtpR > 0U) {
				--Dly_Dchg_UtpR;
			}
		}
	}
	return status;
}

u8 AuxiOVPR(u16 volt) {											// Auxiliary over voltage Protection Release
	static u8 Dly_Auxi_OvpR = 0;
	u8 status = 0U;

	if(volt <= AUXIOVPR) {
		if(Dly_Auxi_OvpR > DLYAOVPR) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Auxi_OvpR; }
	} else {
		if(Dly_Auxi_OvpR > 0U) 			{ --Dly_Auxi_OvpR; }
	}return status;
}

u8 AuxiUVPR(u16 volt) {											// Auxiliary over voltage Protection Release
	static u8 Dly_Auxi_UvpR = 0;
	u8 status = 0U;

	if(volt >= AUXIUVPR) {
		if(Dly_Auxi_UvpR > DLYAUVPR) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Auxi_UvpR; }
	} else {
		if(Dly_Auxi_UvpR > 0U) 			{ --Dly_Auxi_UvpR; }
	}return status;
}

u8 CellVImbalPR(s16 volt) {										// Cell imbalance Fault Release
	static u8 Dly_Cell_VImbalpR = 0;
	u8 status = 0U;

	if(volt < (s16)CVIMBLPR) {
		if(Dly_Cell_VImbalpR > DLYIMBPR) 	{ status = 1U; }
		if(status == 0U) 					{ ++Dly_Cell_VImbalpR; }
	} else {
		if(Dly_Cell_VImbalpR > 0U) 			{--Dly_Cell_VImbalpR; }
	}return status;
}

u8 CellTImbalPR(s16 temp) {										// TEMP difference Protection Release
	static u8 Dly_Cell_TImbalpR = 0;
	u8 status = 0U;

	if(temp < (s16)THDIFFPR) {
		if(Dly_Cell_TImbalpR > DLYTDFPR) 	{ status = 1U; }
		if(status == 0U) 					{ ++Dly_Cell_TImbalpR; }
	} else {
		if(Dly_Cell_TImbalpR > 0U) 			{--Dly_Cell_TImbalpR; }
	}return status;
}

u8 VoltMeasFR(u32 pvin, u32 cvsum) {							// Cell voltage measurement fault Release
	static u8 	Dly_Volt_MeasfR = 0;
	s32			volt;
	u8 status = 0U;

	if(pvin > cvsum) {
		volt = (s32)pvin - (s32)cvsum;
	} else {
		volt = (s32)cvsum - (s32)pvin;
	}

	if(volt <= CVMEASFR) {
		if(Dly_Volt_MeasfR > DLYCVMFR) 	{ status = 1U; }
		if(status == 0U) 				{ ++Dly_Volt_MeasfR; }
	} else {
		if(Dly_Volt_MeasfR > 0U) 		{ --Dly_Volt_MeasfR; }
	}return status;
}

u8 ScpFD(u32 pvin, u32 pvout){
	static u8	Dly_Volt_MeasfD = 0;
	u32		diffpv	= 0;
	u8 		sts 	= 0;

	if(pvin > pvout){
		diffpv = pvin - pvout;
	}else{
		diffpv = pvout - pvin;
	}

	if(diffpv > 13000UL){		//105(4.2v) - 72.5(2.9v)   32.5 * 40% = 13V
		if(Dly_Volt_MeasfD > 50U){
			sts = 1U;
		}
	}else{
		if(Dly_Volt_MeasfD > 0U){
			--Dly_Volt_MeasfD;
		}
	}
	return sts;

}

u8 ScpPR(u32 pvin, u32 pvout){
	static u8	Dly_Volt_MeaspR = 0;
	u32		diffpv	= 0;
	u8 		sts 	= 0;

	if(pvin > pvout){
		diffpv = pvin - pvout;
	}else{
		diffpv = pvout - pvin;
	}

	if(diffpv <= 13000UL){		//105(4.2v) - 72.5(2.9v)   32.5 * 40% = 13V
		if(Dly_Volt_MeaspR > 50U){
			sts = 1;
		}
	}else{
		if(Dly_Volt_MeaspR > 0U){
			--Dly_Volt_MeaspR;
		}
	}
	return sts;
}



#else	//	NEW DET	/////////////////////////////////////////////////////////////////////////////////////

sWF_Spec g_sWF_Spec[MAX_WF_Type];

u16 g_u16OccurCheckCount[MAX_WF_Type][MAX_WFPos_Type];
u16 g_u16ReleaseCheckCount[MAX_WF_Type][MAX_WFPos_Type];

u32 g_u32WarnStatus;
u32 g_u32FaultStatus;

#ifdef DET_UPATE    //2025-12-04   jkpark  DETECT 관련 수정.

const DetRefType g_DetRef[ LA_INDEX_MAX ] = {
{ FCOVA ,     2650 , 3000/TIME_UNIT,      2600 , 3000/TIME_UNIT , COMP_G },  // 	LA_COVA_INDEX	= 0,   //jkpark 2025-12-04 Cell Over Voltage Alarm detect
{ FCOVP ,     2700 , 3000/TIME_UNIT,      2600 , 3000/TIME_UNIT , COMP_G },  // 	LA_COVP_INDEX	,      //jkpark 2025-12-04 Cell Over Voltage Protection detect
{ FPOVA ,   763200 , 3000/TIME_UNIT,    748800 , 3000/TIME_UNIT , COMP_G },  // 	LA_POVA_INDEX	,      //jkpark 2025-12-04 Pack Over Voltage Alarm detect
{ FPOVP ,   792000 , 3000/TIME_UNIT,    763200 , 3000/TIME_UNIT , COMP_G },  // 	LA_POVP_INDEX	,      //jkpark 2025-12-04 Pack Over Voltage Protection detect
{ FCUVA ,     1750 , 3000/TIME_UNIT,      1800 , 3000/TIME_UNIT , COMP_L },  //	LA_CUVA_INDEX	,      //jkpark 2025-12-04 Cell Under Voltage Alarm detect
{ FCUVP ,     1500 , 3000/TIME_UNIT,      1800 , 3000/TIME_UNIT , COMP_L },  //	LA_CUVP_INDEX	,      //jkpark 2025-12-04 Cell Under Voltage Protection detect
{ FPUVA ,    50400 , 3000/TIME_UNIT,     51800 , 3000/TIME_UNIT , COMP_L },  //	LA_PUVA_INDEX	,      //jkpark 2025-12-04 Pack Under Voltage Alarm detect
{ FPUVP ,    41760 , 3000/TIME_UNIT,     51800 , 3000/TIME_UNIT , COMP_L },  //	LA_PUVP_INDEX	,      //jkpark 2025-12-04 Pack Under Voltage Protection detect
{ FDOCA ,  -330000 , 3000/TIME_UNIT,   -325000 , 3000/TIME_UNIT , COMP_G },  //	LA_DOCA_INDEX	,      //jkpark 2025-12-04 Discharge Over Current Alarm detect
{ FDOCP ,  -357000 , 3000/TIME_UNIT, INT32_MIN , 3000/TIME_UNIT , COMP_G },  //	LA_DOCP_INDEX	,      //jkpark 2025-12-04 Discharge Over Current Protection detect
{ FCOCA ,   660000 , 3000/TIME_UNIT,    650000 , 3000/TIME_UNIT , COMP_L },  //	LA_COCA_INDEX	,      //jkpark 2025-12-04 Charge Over Current Alarm detect
{ FCOCP ,   723000 , 3000/TIME_UNIT, INT32_MAX , 3000/TIME_UNIT , COMP_L },  //	LA_COCP_INDEX	,      //jkpark 2025-12-04 Charge Over Current Protection detect
{ FCOTA ,      550 , 3000/TIME_UNIT,       500 , 3000/TIME_UNIT , COMP_G },  //	LA_COTA_INDEX	,      //jkpark 2025-12-04 Charge Over TMEP Alarm detect
{ FCOTP ,      600 , 3000/TIME_UNIT,       500 , 3000/TIME_UNIT , COMP_G },  //	LA_COTP_INDEX	,      //jkpark 2025-12-04 Charge Over TMEP Protection detect
{ FCUTA ,     -250 , 3000/TIME_UNIT,      -200 , 3000/TIME_UNIT , COMP_L },  //	LA_CUTA_INDEX	,      //jkpark 2025-12-04 Charge Under TMEP Alarm detect
{ FCUTP ,     -300 , 3000/TIME_UNIT,      -200 , 3000/TIME_UNIT , COMP_L },  //	LA_CUTP_INDEX	,      //jkpark 2025-12-04 Charge Under TMEP Protection detect
{ FTHDFA,      150 , 3000/TIME_UNIT,       120 , 3000/TIME_UNIT , COMP_G },  //	LA_THDFA_INDEX	,      //jkpark 2025-12-04 Difference TMEP Alarm detect
{ FTHDFP,      200 , 3000/TIME_UNIT,       120 , 3000/TIME_UNIT , COMP_G },  //	LA_THDFP_INDEX	,      //jkpark 2025-12-04 Difference TMEP Protection detect
{ FIMBA ,      250 , 3000/TIME_UNIT,       200 , 3000/TIME_UNIT , COMP_G },  //	LA_IMBA_INDEX	,      //jkpark 2025-12-04 cell IMBalance Alarm detection	
{ FIMBVP ,      300 , 3000/TIME_UNIT,       250 , 3000/TIME_UNIT , COMP_G },  //	LA_IMBP_INDEX	,      //jkpark 2025-12-04 cell IMBalance Protection detection	
{ FSDVA ,    20000 , 3000/TIME_UNIT,     15000 , 3000/TIME_UNIT , COMP_G },  //	LA_SDVA_INDEX	,      //jkpark 2025-12-04 Pack Voltage Sensing Difference Alarm detection	
{ FSDVP ,    25000 , 3000/TIME_UNIT,     20000 , 3000/TIME_UNIT , COMP_G },  //	LA_SDVP_INDEX	,      //jkpark 2025-12-04 Pack Voltage Sensing Difference Protection detection	
{ FPDP  ,     1400 , 3000/TIME_UNIT, INT32_MAX , 3000/TIME_UNIT , COMP_L },  //	LA_PD_INDEX	    ,      //jkpark 2025-12-04 Power Down Protection detection	
{ FSHTP ,  -500000 , 3000/TIME_UNIT, INT32_MIN , 3000/TIME_UNIT , COMP_L },  //	LA_SHTP_INDEX   ,      //jkpark 2025-12-04 Short Protection detection	
//  LA_INDEX_MAX    
};

DetStateType g_DetState[ LA_INDEX_MAX ] = {
    { DET_S_NORMAL , 0 },
};

msStatus_t Detect_LinerValueClear(EnumLinerAlertType alertIndex)
{
	if( g_DetState[ alertIndex ].cntD > 0U){ g_DetState[ alertIndex ].cntD--; }
	if( g_DetState[ alertIndex ].cntR > 0U){ g_DetState[ alertIndex ].cntR--; }
    return MS_SUCCESS;
}

msStatus_t Detect_LinerValue(EnumLinerAlertType alertIndex, int32_t value, uint32_t *fDetStatus )
{
    msStatus_t      retValue    = MS_SUCCESS;
    DetFlagType     *pState     = &g_DetState[ alertIndex ].state;
    uint8_t         *pCntD      = &g_DetState[ alertIndex ].cntD;
    uint8_t         *pCntR      = &g_DetState[ alertIndex ].cntR;
    DetRefType      refData;

    if( alertIndex >= LA_INDEX_MAX ) return MS_ERROR;

    memcpy( &refData, &g_DetRef[alertIndex], sizeof( DetRefType ) );
    
    switch ( *pState )
    {
        case DET_S_NORMAL:
        {
    		if( (*pCntR) > 0U) (*pCntR)--;

            if (( refData.comp_type == COMP_G && value > refData.thDetEnter ) ||
                ( refData.comp_type == COMP_L && value < refData.thDetEnter))
            {
                if (*pCntD >= refData.cntDetE )
                {
                    *pState = DET_S_DETECT;
                    SET_BIT( *fDetStatus, refData.flagDetect );
                }
                else
                {
                    (*pCntD)++;
                }
            }
            else
            {
        		if( (*pCntD) > 0U ) (*pCntD)--;
            }
            break;
        }

        case DET_S_DETECT:
        {
    		if( (*pCntD) > 0U ) (*pCntD)--;
            
            if (( refData.comp_type == COMP_G && value < refData.thDetRelease ) ||
                ( refData.comp_type == COMP_L && value > refData.thDetRelease))
            {
                if (*pCntR >= refData.cntDetR )
                {
                    *pState = DET_S_NORMAL;
                    CLEAR_BIT( *fDetStatus, refData.flagDetect );
                }
                else
                {
                    (*pCntR)++;
                }
            }
            else
            {
        		if( (*pCntR) > 0U) (*pCntR)--;
            }
            break;
        }
        default:
        {
            retValue = MS_ERROR;
            //jkpark 2025-12-04 여기 들어오면 에러.
            break;
        }
    }
    return retValue;
}
#endif //DET_UPATE

#if EN_ORIDET 
#else
void DetChk_Occur(Enum_WFUnOver_Type eUnOv, 
								Enum_WF_Type eWF_Type, 
								Enum_WFPos_Type eWF_Pos,
								u16 u16CheckCount, s32 s32Value, s32 s32Spec)
{
	BOOL bOccured = FALSE;

	if(eUnOv == WF_OVER_Type){
		if(s32Value > s32Spec)	{g_u16OccurCheckCount[eWF_Type][eWF_Pos]++;}
			else					{g_u16OccurCheckCount[eWF_Type][eWF_Pos] = 0;}
		}else{
			if(s32Value < s32Spec)	{g_u16OccurCheckCount[eWF_Type][eWF_Pos]++;}
			else					{g_u16OccurCheckCount[eWF_Type][eWF_Pos] = 0;}

		}

		if(g_u16OccurCheckCount[eWF_Type][eWF_Pos] >= u16CheckCount){
			g_u16OccurCheckCount[eWF_Type][eWF_Pos] = u16CheckCount;
			bOccured = TRUE;
		}

		if(bOccured == TRUE){
			if(eWF_Type == WF_W_Type){g_u32WarnStatus |= (1 << eWF_Pos);}
			else				   {g_u32FaultStatus |= (1 << eWF_Pos);}
		g_u16OccurCheckCount[eWF_Type][eWF_Pos] = 0;
	}
}

void DetChk_Release(Enum_WFUnOver_Type eUnOv, 
										Enum_WF_Type eWF_Type, 
										Enum_WFPos_Type eWF_Pos, 
										u16 u16CheckCount, s32 s32Value, s32 s32Spec)
{
	BOOL bReleased = FALSE;

	if(eUnOv == WF_OVER_Type){
		if(s32Value >= s32Spec)	{g_u16ReleaseCheckCount[eWF_Type][eWF_Pos]++;}		//KKD 2025-12-01 Under Release Check
		else					{g_u16ReleaseCheckCount[eWF_Type][eWF_Pos] = 0;}
	}else{
		if(s32Value <= s32Spec)	{g_u16ReleaseCheckCount[eWF_Type][eWF_Pos]++;} 		//KKD 2025-12-01 Over Release Check
		else					{g_u16ReleaseCheckCount[eWF_Type][eWF_Pos] = 0;}
	}


	if(g_u16ReleaseCheckCount[eWF_Type][eWF_Pos] >= u16CheckCount){
		g_u16ReleaseCheckCount[eWF_Type][eWF_Pos] = u16CheckCount;
		bReleased = TRUE;
	}

	if(bReleased == TRUE){
		if(eWF_Type == WF_W_Type){g_u32WarnStatus &= ~(1 << eWF_Pos);}
		else					{g_u32FaultStatus &= ~(1 << eWF_Pos);}
		g_u16ReleaseCheckCount[eWF_Type][eWF_Pos] = 0;
	}
}

void DetChk_Default(sWF_Spec *psDetSpec)
{
	memset((u8*)&(psDetSpec[WF_W_Type].u8WarningFault), 0x00, sizeof(sWF_Spec));
	memset((u8*)&(psDetSpec[WF_F_Type].u8WarningFault), 0x00, sizeof(sWF_Spec));
	
	psDetSpec[WF_W_Type].u8WarningFault 	= WF_W_Type;
	psDetSpec[WF_W_Type].u16_Ovp 			= (2650);		// mV
	psDetSpec[WF_W_Type].u16_Uvp			= (1750);		// mV
	//psDetSpec[WF_W_Type].u16Pack_Ovp		= (7632);		// 100mV
	//psDetSpec[WF_W_Type].u16Pack_Uvp		= (4500);		// 100mV
	psDetSpec[WF_W_Type].s32_D_Ocp			= (900);		// 1mA
	psDetSpec[WF_W_Type].s32_C_Ocp			= (900);		// 1mA
	psDetSpec[WF_W_Type].s8_D_Otp			= (40);  		// 'C
	psDetSpec[WF_W_Type].s8_C_Utp			= (5);	
	psDetSpec[WF_W_Type].s8_D_Otp			= (55);  		// 'C
	psDetSpec[WF_W_Type].s8_D_Utp			= (-10);	
	psDetSpec[WF_W_Type].u8_Imbal			= (50);			// 10mV

	psDetSpec[WF_F_Type].u8WarningFault 	= WF_F_Type;
	psDetSpec[WF_F_Type].u16_Ovp			= (2700);		// mV	
	psDetSpec[WF_F_Type].u16_Uvp			= (1500);		// mV
	//psDetSpec[WF_F_Type].u16Pack_Ovp		= (5880); 		// 100mV
	//psDetSpec[WF_F_Type].u16Pack_Uvp		= (4200);		// 100mV 70%
	psDetSpec[WF_F_Type].s32_D_Ocp			= (900);		// 1mA
	psDetSpec[WF_F_Type].s32_C_Ocp			= (900);		// 1mA
	psDetSpec[WF_F_Type].s8_D_Otp			= (45);  		// 'C
	psDetSpec[WF_F_Type].s8_C_Utp			= (0);	
	psDetSpec[WF_F_Type].s8_D_Otp			= (60);     	// 'C'
	psDetSpec[WF_F_Type].s8_D_Utp			= (-20);
	psDetSpec[WF_F_Type].u8_Imbal			= (100);		// 10mV
	
	//write eep
}
#endif //EN_ORIDET
#endif

void Det_Proc(void)	                                                                 		//KKD 2025-11-28 신규 프로텍션 체크 기능 현재 구현 중~ (근데 기존께 더 간단할수도있음.)
{

#if EN_ORIDET
	RackPkt.falm = Alarm_Occur();
	RackPkt.falm = Alarm_Release();
	RackPkt.fdet = Det_Occur();
	RackPkt.fdet = Det_Release();
#else
#ifdef DET_UPATE    //2025-12-04   jkpark  DETECT 관련 수정.
    
    Detect_LinerValue( LA_COVP_INDEX, RackPkt.maxcv     , &RackPkt.fdet ); //jkpark 2025-12-04 Cell Over Voltage Protection detect
    Detect_LinerValue( LA_CUVP_INDEX, RackPkt.mincv     , &RackPkt.fdet );//jkpark 2025-12-04 Cell Under Voltage Protection detect
    
    Detect_LinerValue( LA_POVP_INDEX, RackPkt.pv[PVINP] , &RackPkt.fdet );//jkpark 2025-12-04 Pack Over Voltage Protection detect
    Detect_LinerValue( LA_PUVP_INDEX, RackPkt.pv[PVINP] , &RackPkt.fdet );//jkpark 2025-12-04 Pack Under Voltage Protection detect
    
    Detect_LinerValue( LA_COCP_INDEX, RackPkt.pi[IREAL] , &RackPkt.fdet );//jkpark 2025-12-04 Charge Over Current Protection detect
    Detect_LinerValue( LA_DOCP_INDEX, RackPkt.pi[IREAL] , &RackPkt.fdet );//jkpark 2025-12-04 Discharge Over Current Protection detect

    if( ( RackPkt.fbmssts & FCHGING ) != 0 )
    {
        Detect_LinerValue( LA_COTP_INDEX, RackPkt.maxth     , &RackPkt.fdet );//jkpark 2025-12-04 Charge Over TMEP Protection detect
        Detect_LinerValue( LA_CUTP_INDEX, RackPkt.minth     , &RackPkt.fdet );//jkpark 2025-12-04 Charge Under TMEP Protection detect
    }
    else
    {
        Detect_LinerValueClear( LA_COTP_INDEX );
        Detect_LinerValueClear( LA_CUTP_INDEX );
    }

    Detect_LinerValue( LA_DOTP_INDEX, RackPkt.maxth     , &RackPkt.fdet );//jkpark 2025-12-04 Discharge Over TMEP Protection detect
    Detect_LinerValue( LA_DUTP_INDEX, RackPkt.minth     , &RackPkt.fdet );//jkpark 2025-12-04 Discharge Under TMEP Protection detect



#else  //DET_UPATE
	/*OVP*/
	DetChk_Occur	(WF_OVER_Type, WF_W_Type,	WF_COVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.maxcv, (u32)g_sWF_Spec[WF_W_Type].u16_Ovp);		
	DetChk_Occur	(WF_OVER_Type, WF_F_Type,	WF_COVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.maxcv, (u32)g_sWF_Spec[WF_F_Type].u16_Ovp);
	DetChk_Release	(WF_OVER_Type, WF_W_Type,	WF_COVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.maxcv, (u32)g_sWF_Spec[WF_F_Type].u16_Ovp);	//Warning 해지시만

	/*UVP*/
	DetChk_Occur	(WF_UNDR_Type, WF_W_Type,	WF_CUVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.mincv, (u32)g_sWF_Spec[WF_W_Type].u16_Uvp);		
	DetChk_Occur	(WF_UNDR_Type, WF_F_Type,	WF_CUVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.mincv, (u32)g_sWF_Spec[WF_F_Type].u16_Uvp);
	DetChk_Release	(WF_UNDR_Type, WF_W_Type,	WF_CUVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.mincv, (u32)g_sWF_Spec[WF_F_Type].u16_Uvp);	//Warning 해지시만

	/*P_OVP*/
	DetChk_Occur	(WF_OVER_Type, WF_W_Type,	WF_P_OVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.pv[PVINP], (u32)g_sWF_Spec[WF_W_Type].u16_P_Ovp);		
	DetChk_Occur	(WF_OVER_Type, WF_F_Type,	WF_P_OVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.pv[PVINP], (u32)g_sWF_Spec[WF_F_Type].u16_P_Ovp);
	DetChk_Release	(WF_OVER_Type, WF_W_Type,	WF_P_OVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.pv[PVINP], (u32)g_sWF_Spec[WF_F_Type].u16_P_Ovp);	//Warning 해지시만

	/*P_UVP*/
	DetChk_Occur	(WF_UNDR_Type, WF_W_Type,	WF_P_UVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.pv[PVINP], (u32)g_sWF_Spec[WF_W_Type].u16_P_Uvp);		
	DetChk_Occur	(WF_UNDR_Type, WF_F_Type,	WF_P_UVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.pv[PVINP], (u32)g_sWF_Spec[WF_F_Type].u16_P_Uvp);
	DetChk_Release	(WF_UNDR_Type, WF_W_Type,	WF_P_UVP_Type, DET_CHECK_COUNT_DEF, (u32)RackPkt.pv[PVINP], (u32)g_sWF_Spec[WF_F_Type].u16_P_Uvp);	//Warning 해지시만
	/* */

#endif //DET_UPATE
#endif //EN_ORIDET
}
