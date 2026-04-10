/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Battery alarm check)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_alarm.c
\author             KKD
\date               2025-11-21
\brief              Battery alarm check
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_alarm.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/

static u16 fAlm = 0;

u16 Alarm_Occur(void) {
	u32 chg		= ((RackPkt.fbmssts&FCHGING) !=0U) ? (u32)1:(u32)0;
	u32 dchg	= ((RackPkt.fbmssts&FDCHING) !=0U) ? (u32)1:(u32)0;

  	if(CellOVAD(RackPkt.maxcv) == 1U) 		{fAlm |= FCOVA;}	// Cell over voltage Protection Detection Alarm_Occur
  	if(CellUVAD(RackPkt.mincv) == 1U) 		{fAlm |= FCUVA;}	// Cell under voltage Protection Detection Alarm_Occur
  	if(PackOVAD(RackPkt.pv[PVINP]) == 1U) 	{fAlm |= FPOVA;} 	// Pack over voltage Protection Detection Alarm_Occur
  	if(PackUVAD(RackPkt.pv[PVINP]) == 1U) 	{fAlm |= FPUVA;}	// Pack under voltage Protection Detection Alarm_Occur
  	if(ChagOCAD(RackPkt.pi[0]) == 1U) 		{fAlm |= FCOCA;}	// Charge over current Protection Detection Alarm_Occur
  	if(DchgOCAD(RackPkt.pi[0]) == 1U) 		{fAlm |= FDOCA;}	// Discharge over current Protection Detection Alarm_Occur
  	if(ChagOTAD(RackPkt.maxth, chg))		{fAlm |= FCOTA;}	// Charge over TMEP Protection Detection Alarm_Occur
  	if(ChagUTAD(RackPkt.minth, chg))		{fAlm |= FCUTA;}	// Charge under TMEP Protection Detection Alarm_Occur
  	if(DchgOTAD(RackPkt.maxth, dchg)) 		{fAlm |= FDOTA;}	// Discharge over TMEP Protection Detection Alarm_Occur
  	if(DchgUTAD(RackPkt.minth, dchg)) 		{fAlm |= FDUTA;}	// Discharge under TMEP Protection Detection Alarm_Occur
  //if(AuxiOVAD(RackPkt.aux) == 1U) {
  //  fAlm |= FAOVA;    // Auxiliary over voltage Protection Detection Alarm_Occur
  //}
  //if(AuxiUVAD(RackPkt.aux) == 1U) {
  //  fAlm |= FAUVA;    // Auxiliary under voltage Protection Detection Alarm_Occur
  //}
  ///if(CellVImbalAD(RackPkt.diffcv) == 1U) {
    ///fAlm |= FIMBA;   // Cell imbalance Protection Detection Alarm_Occur
  //}
  //if(TempDifferAD(RackPkt.diffth) == 1U) {
  //  fAlm |= FTDFA;    // TEMP difference Protection Detection Alarm_Occur
  //}

  return fAlm;
}


u16 Alarm_Release(void) {
	u32 chg		= ((RackPkt.fbmssts&FCHGING) !=0U) ? (u32)1:(u32)0;
	u32 dchg	= ((RackPkt.fbmssts&FDCHING) !=0U) ? (u32)1:(u32)0;

	if(CellOVAR(RackPkt.maxcv) == 1U) 		{fAlm &= ~FCOVA;}	// Cell over voltage Protection release Alarm_Occur
  	if(CellUVAR(RackPkt.mincv) == 1U) 		{fAlm &= ~FCUVA;}   // Cell under voltage Protection release Alarm_Occur
  	if(PackOVAR(RackPkt.pv[PVINP]) == 1U)	{fAlm &= ~FPOVA;}   // Pack over voltage Protection release Alarm_Occur
  	if(PackUVAR(RackPkt.pv[PVINP]) == 1U) 	{fAlm &= ~FPUVA;}   // Pack under voltage Protection release Alarm_Occur
  	if(ChagOCAR(RackPkt.pi[0]) == 1U) 		{fAlm &= ~FCOCA;}   // Charge over current Protection release Alarm_Occur
  	if(DchgOCAR(RackPkt.pi[0]) == 1U) 		{fAlm &= ~FDOCA;}   // Discharge over current Protection release Alarm_Occur
  	if(ChagOTAR(RackPkt.maxth, chg)) 		{fAlm &= ~FCOTA;}	// Charge over TMEP Protection release Alarm_Occur
  	if(ChagUTAR(RackPkt.minth, chg)) 		{fAlm &= ~FCUTA;}	// Charge under TMEP Protection release Alarm_Occur
  	if(DchgOTAR(RackPkt.maxth, dchg)) 		{fAlm &= ~FDOTA;}	// Discharge over TMEP Protection release Alarm_Occur
  	if(DchgUTAR(RackPkt.minth, dchg)) 		{fAlm &= ~FDUTA; }	// Discharge under TMEP Protection release Alarm_Occur
  //if(AuxiOVAR(RackPkt.aux) == 1U) {
  //  fAlm &= ~FAOVA;   // Auxiliary over voltage Protection release Alarm_Occur
  //}
  //if(AuxiUVAR(RackPkt.aux) == 1U) {
  //  fAlm &= ~FAUVA;   // Auxiliary under voltage Protection release Alarm_Occur
  //}
  //if(CellVImbalAR(RackPkt.diffcv) == 1U) {
  //  fAlm &= ~FIMBA;   // Cell imbalance Protection Detection release Alarm_Occur
  //}
  //if(TempDifferAR(RackPkt.diffth) == 1U) {
  //  fAlm &= ~FTDFA;   // TEMP difference Protection Detection release Alarm_Occur
  //}

  return fAlm;
}

// Alarm_Occur Detection
u8 CellOVAD(u16 volt) {       // Cell over voltage Protection Alarm_Occur
  static u16 Dly_Cell_OVAD = 0U;
  u8 status = 0U;

  if(volt >= CELLOVAD) {
    if(Dly_Cell_OVAD >= DLYCOVAD) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Cell_OVAD;
    }
  } else {
    if(Dly_Cell_OVAD > 0U) {
        --Dly_Cell_OVAD;
    }
  }
  return status;
}

u8 CellUVAD(u16 volt) {       // Cell under voltage Protection Alarm_Occur
  static u16 Dly_Cell_UVAD = 0U;
  u8 status = 0U;

  if(volt <= CELLUVAD) {
    if(Dly_Cell_UVAD >= DLYCUVAD) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Cell_UVAD;
    }
  } else {
    if(Dly_Cell_UVAD > 0U) {
        --Dly_Cell_UVAD;
    }
  }
  return status;
}


u8 PackOVAD(u32 volt) {       // Pack over voltage Protection Alarm_Occur
  static u16 Dly_Pack_OVAD = 0U;
  u8 status = 0U;

  if(volt >= PACKOVAD) {
    if(Dly_Pack_OVAD >= DLYPOVAD) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Pack_OVAD;
    }
  } else {
    if(Dly_Pack_OVAD > 0U) {
      --Dly_Pack_OVAD;
    }
  }
  return status;
}


u8 PackUVAD(u32 volt) {       // Pack under voltage Protection Alarm_Occur
  static u16 Dly_Pack_UVAD = 0;
  u8 status = 0U;

  if(volt <= PACKUVAD) {
    if(Dly_Pack_UVAD >= DLYPUVAD) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Pack_UVAD;
    }
  } else {
    if(Dly_Pack_UVAD > 0U) {
       --Dly_Pack_UVAD;
    }
  }
  return status;
}

u8 ChagOCAD(s32 curr) {          // Charge over current Protection Alarm_Occur
  static u16 Dly_Chag_OCAD = 0;
  u8 status = 0U;

  if(curr >= CHAGOCAD) {
    if(Dly_Chag_OCAD >= DLYCOCAD) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Chag_OCAD;
    }
  } else {
    if(Dly_Chag_OCAD > 0U) {
      --Dly_Chag_OCAD;
    }
  }
  return status;
}

u8 DchgOCAD(s32 curr) {          // Discharge over current Protection Alarm_Occur
  static u16 Dly_Dchg_OCAD = 0;
  u8 status = 0U;

  if(curr <= DCHGOCAD) {
    if(Dly_Dchg_OCAD >= DLYDOCAD) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Dchg_OCAD;
    }
  } else {
    if(Dly_Dchg_OCAD > 0U) {
      --Dly_Dchg_OCAD;
    }
  }
  return status;
}

u8 ChagOTAD(s16 temp, u32 fchag) {    // Charge over TMEP Protection Alarm_Occur
  static u16 Dly_Chag_OTAD = 0;
  u8 status = 0U;

  if(fchag == 0U) {
    status = 0U;
  } else {
    if(temp >= (s16)CHAGOTAD) {
      if(Dly_Chag_OTAD >= DLYCOTAD) {
        status = 1U;
      }
      if(status == 0U) {
        ++Dly_Chag_OTAD;
      }
    } else {
      if(Dly_Chag_OTAD > 0U) {
        --Dly_Chag_OTAD;
      }
    }
  }
  return status;
}

u8 ChagUTAD(s16 temp, u32 fchag) {    // Charge under TMEP Protection Alarm_Occur
  static u16 Dly_Chag_UTAD = 0;
  u8 status = 0U;

  if(fchag == 0U) {
    status = 0U;
  } else {
    if(temp <= CHAGUTAD) {
      if(Dly_Chag_UTAD >= DLYCUTAD) {
        status = 1U;
      }

      if(status == 0U) {
        ++Dly_Chag_UTAD;
      }
    } else {
      if(Dly_Chag_UTAD > 0U) {
        --Dly_Chag_UTAD;
      }
    }
  }
  return status;
}

u8 DchgOTAD(s16 temp, u32 fdchg) {    // Discharge over TMEP Protection Alarm_Occur
  static u16 Dly_Dchg_OTAD = 0;
  u8 status = 0U;

  if(temp >= DCHGOTAD) {
    if(Dly_Dchg_OTAD >= DLYDOTAD) {
             status = 1U;
    }

    if(status == 0U) {
      ++Dly_Dchg_OTAD;
    }
  } else {
    if(Dly_Dchg_OTAD > 0U) {
            --Dly_Dchg_OTAD;
      }
  }
  return status;
}

u8 DchgUTAD(s16 temp, u32 fdchg) {    // Discharge over TMEP Protection Alarm_Occur
  static u16 Dly_Dchg_UTAD = 0;
  u8 status = 0U;

  if(temp <= DCHGUTAD) {
    if(Dly_Dchg_UTAD >= DLYDUTAD) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Dchg_UTAD;
    }
  } else {
    if(Dly_Dchg_UTAD > 0U) {
      --Dly_Dchg_UTAD;
      }
  }
  return status;
}


u8 AuxiOVAD(u16 volt) {       // Auxiliary over voltage Protection Alarm_Occur
  static u16 Dly_Auxi_OVAD = 0;
  u8 status = 0U;

  if(volt >= AUXIOVAD) {
    if(Dly_Auxi_OVAD >= DLYAOVAD) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Auxi_OVAD;
    }
  } else {
    if(Dly_Auxi_OVAD > 0U) {
      --Dly_Auxi_OVAD;
    }
  }
  return status;
}


u8 AuxiUVAD(u16 volt) {       // Auxiliary under voltage Protection Alarm_Occur
  static u16 Dly_Auxi_UVAD = 0;
  u8 status = 0U;

  if(volt <= AUXIUVAD) {
    if(Dly_Auxi_UVAD >= DLYAUVAD) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Auxi_UVAD;
    }
  } else {
    if(Dly_Auxi_UVAD > 0U) {
       --Dly_Auxi_UVAD;
    }
  }
  return status;
}

u8 CellVImbalAD(s16 volt) {      // Cell imbalance Fault Alarm_Occur
  static u16 Dly_Cell_VImbalAD = 0;
  u8 status = 0U;

  if(volt >= (s16)CVIMBLAD) {
    if(Dly_Cell_VImbalAD >= DLYIMBAD) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Cell_VImbalAD;
    }
  } else {
    if(Dly_Cell_VImbalAD > 0U) {
      --Dly_Cell_VImbalAD;
    }
  }
  return status;
}

u8 TempDifferAD(s16 temp) {        // TEMP difference Protection Alarm_Occur
  static u16 Dly_Temp_DifferAD = 0;
  u8 status = 0U;

  if(temp >= THDIFFAD) {
    if(Dly_Temp_DifferAD >= DLYTDFAD) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Temp_DifferAD;
    }
  } else {
    if(Dly_Temp_DifferAD > 0U) {
      --Dly_Temp_DifferAD;
    }
  }
  return status;
}


// Alarm_Occur Release
u8 CellOVAR(u16 volt) {
  static u16 Dly_Cell_OVAR = 0;
  u8 status = 0U;

  if(volt < CELLOVAR) {             // Pack Over Voltage Alarm_Occur
    if(Dly_Cell_OVAR >= DLYCOVAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Cell_OVAR;
    }
  } else {
    if(Dly_Cell_OVAR > 0U) {
       --Dly_Cell_OVAR;
    }
  }
  return status;
}

u8 CellUVAR(u16 volt) {
  static u16 Dly_Cell_UVAR = 0;
  u8 status = 0U;

  if(volt >= CELLUVAR) {              // Pack Over Voltage Alarm_Occur
    if(Dly_Cell_UVAR >= DLYCUVAR) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Cell_UVAR;
    }
  } else {
    if(Dly_Cell_UVAR > 0U) {
     --Dly_Cell_UVAR;
    }
  }
  return status;
}

u8 PackOVAR(u32 volt) {
  static u16 Dly_Pack_OVAR = 0;
  u8 status = 0U;

  if(volt < PACKOVAR) {             // Pack Over Voltage Alarm_Occur
    if(Dly_Pack_OVAR >= DLYPOVAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Pack_OVAR;
    }
  } else {
    if(Dly_Pack_OVAR > 0U) {
     --Dly_Pack_OVAR;
    }
  }
  return status;
}

u8 PackUVAR(u32 volt) {
  static u16 Dly_Pack_UVAR = 0;
  u8 status = 0U;

  if(volt >= PACKUVAR) {              // Pack Over Voltage Alarm_Occur
    if(Dly_Pack_UVAR >= DLYPUVAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Pack_UVAR;
    }
  } else {
    if(Dly_Pack_UVAR > 0U) {
      --Dly_Pack_UVAR;
    }
  }
  return status;
}


u8 ChagOCAR(s32 curr) {
  static u16 Dly_Chag_OCAR = 0;
  u8 status = 0U;

  if(curr <= CHAGOCAR) {              // Charge over current Protection Release Alarm_Occur
    if(Dly_Chag_OCAR >= DLYCOCAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Chag_OCAR;
    }
  } else {
    if(Dly_Chag_OCAR > 0U) {
      --Dly_Chag_OCAR;
    }
  }
  return status;
}

u8 DchgOCAR(s32 curr) {
  static u16 Dly_Dchg_OCAR = 0;
  u8 status = 0U;

  if(curr >= DCHGOCAR) {              // Discharge over current Protection Release Alarm_Occur
    if(Dly_Dchg_OCAR >= DLYDOCAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Dchg_OCAR;
    }
  } else {
    if(Dly_Dchg_OCAR > 0U) {
      --Dly_Dchg_OCAR;
    }
  }
  return status;
}

/*
u8 DchgOCAR2(s32 curr)   // 100ms
{
  static u8 Dly_Dchg_OCAR2 = 0U;
  u8 status = 0U;

  if(curr >= (-90000L))
  {             // Discharge over current Protection Release Alarm_Occur
    if(Dly_Dchg_OCAR2 >= (3000U/100U))
    {
      status = 1U;
    }
    ++Dly_Dchg_OCAR2;
  }
  else
  {
    if(Dly_Dchg_OCAR2 != 0U)
    {
      --Dly_Dchg_OCAR2;
    }
  }
  return status;
}
*/

u8 ChagOTAR(s16 temp, u32 fchag) {    // Charge over TMEP Protection Release Alarm_Occur
  static u16 Dly_Chag_OTAR = 0;
  u8 status = 0U;

  if(fchag == 0U) {
    status = 0U;
  } else {
    if(temp <= (s16)CHAGOTAR) {
      if(Dly_Chag_OTAR >= DLYCOTAR) {
        status = 1U;
      }
      if(status == 0U) {
        ++Dly_Chag_OTAR;
      }
    } else {
      if(Dly_Chag_OTAR > 0U) {
        --Dly_Chag_OTAR;
      }
    }
  }
  return status;
}

u8 ChagUTAR(s16 temp, u32 fchag) {    // Charge under TMEP Protection Release Alarm_Occur
  static u16 Dly_Chag_UTAR = 0;
  u8 status = 0U;

  if(fchag == 0U) {
    status = 0U;
  } else {
    if(temp >= CHAGUTAR) {
      if(Dly_Chag_UTAR >= DLYCUTAR) {
        status = 1U;
      }

      if(status == 0U) {
        ++Dly_Chag_UTAR;
      }
    } else {
      if(Dly_Chag_UTAR > 0U) {
         --Dly_Chag_UTAR;
      }
    }
  }
  return status;
}

u8 DchgOTAR(s16 temp, u32 fchag) {    // Discharge over TMEP Protection Release Alarm_Occur
    static u16 Dly_Dchg_OTAR = 0;
  u8 status = 0U;

  if(fchag == 0U) {
    status = 0U;
  } else {
    if(temp <= DCHGOTAR) {
      if(Dly_Dchg_OTAR >= DLYDOTAR) {
        status = 1U;
      }
      if(status == 0U) {
        ++Dly_Dchg_OTAR;
      }
    } else {
      if(Dly_Dchg_OTAR > 0U) {
        --Dly_Dchg_OTAR;
      }
    }
  }
  return status;
}

u8 DchgUTAR(s16 temp, u32 fchag) {    // Discharge under TMEP Protection Release Alarm_Occur
  static u16 Dly_Dchg_UTAR = 0;
  u8 status = 0U;

  if(fchag == 0U) {
    status = 0U;
  } else {
    if(temp >= DCHGUTAR) {
      if(Dly_Dchg_UTAR >= DLYDUTAR) {
        status = 1U;
      }
      if(status == 0U) {
        ++Dly_Dchg_UTAR;
      }
    } else {
      if(Dly_Dchg_UTAR > 0U) {
         --Dly_Dchg_UTAR;
      }
    }
  }
  return status;
}

u8 AuxiOVAR(u16 volt) {       // Auxiliary over voltage Protection Release Alarm_Occur
  static u16 Dly_Auxi_OVAR = 0;
  u8 status = 0U;

  if(volt <= AUXIOVAR) {
    if(Dly_Auxi_OVAR >= DLYAOVAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Auxi_OVAR;
    }
  } else {
    if(Dly_Auxi_OVAR > 0U) {
       --Dly_Auxi_OVAR;
      }
  }
  return status;
}

u8 AuxiUVAR(u16 volt) {       // Auxiliary over voltage Protection Release Alarm_Occur
  static u16 Dly_Auxi_UVAR = 0;
  u8 status = 0U;

  if(volt >= AUXIUVAR) {
    if(Dly_Auxi_UVAR >= DLYAUVAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Auxi_UVAR;
    }
  } else {
    if(Dly_Auxi_UVAR > 0U) {
       --Dly_Auxi_UVAR;
    }
  }
  return status;
}

u8 CellVImbalAR(s16 volt) {      // Cell imbalance Fault Release Alarm_Occur
  static u16 Dly_Cell_VImbalAR = 0;
  u8 status = 0U;

  if(volt <= (s16)CVIMBLAR) {
    if(Dly_Cell_VImbalAR >= DLYIMBAR) {
      status = 1U;
    }

    if(status == 0U) {
      ++Dly_Cell_VImbalAR;
    }
  } else {
    if(Dly_Cell_VImbalAR > 0U) {
       --Dly_Cell_VImbalAR;
    }
  }
  return status;
}

u8 TempDifferAR(s16 temp) {        // TEMP difference Protection Release Alarm_Occur
  static u16 Dly_Temp_DifferAR = 0;
  u8 status = 0U;

  if(temp <= THDIFFAR) {
    if(Dly_Temp_DifferAR >= DLYTDFAR) {
      status = 1U;
    }
    if(status == 0U) {
      ++Dly_Temp_DifferAR;
    }
  } else {
    if(Dly_Temp_DifferAR > 0U) {
      --Dly_Temp_DifferAR;
    }
  }
  return status;
}
