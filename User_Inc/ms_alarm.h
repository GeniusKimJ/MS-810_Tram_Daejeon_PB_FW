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
\file               ms_alarm.h
\author             KKD
\date               2025-11-21
\brief              Battery alarm check
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/

#ifndef MS_ALARM_H_
#define MS_ALARM_H_
/* Includes ---------------------------------------------------------------------------------------*/
//#include "ms_config.h"
#include "ms_det.h"

/* Exported define ---------------------------------------------------------------------------------*/

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------*/

/* Exported function prototypes --------------------------------------------------------------------*/
/*Alarm_Occur*/
u8 CellOVAD(u16 volt);                    // Cell over voltage Protection Det_Occur Alarm_Occur
u8 CellUVAD(u16 volt);                    // Cell under voltage Protection Det_Occur Alarm_Occur
u8 PackOVAD(u32 volt);                    // Pack over voltage Protection Det_Occur Alarm_Occur
u8 PackUVAD(u32 volt);                    // Pack under voltage Protection Det_Occur Alarm_Occur
u8 ChagOCAD(s32 curr);                    // Charge over current Protection Det_Occur Alarm_Occur
u8 DchgOCAD(s32 curr);                    // Discharge over current Protection Det_Occur Alarm_Occur
u8 ChagOTAD(s16 temp, u32 fchag);           // Charge over TMEP Protection Det_Occur Alarm_Occur
u8 ChagUTAD(s16 temp, u32 fchag);           // Charge under TMEP Protection Det_Occur Alarm_Occur
u8 DchgOTAD(s16 temp, u32 fdchg);           // Discharge over TMEP Protection Det_Occur Alarm_Occur
u8 DchgUTAD(s16 temp, u32 fdchg);           // Discharge under TMEP Protection Det_Occur Alarm_Occur
u8 AuxiOVAD(u16 volt);                    // Auxiliary over voltage Protection Det_Occur Alarm_Occur
u8 AuxiUVAD(u16 volt);                    // Auxiliary under voltage Protection Det_Occur Alarm_Occur
u8 CellVImbalAD(s16 volt);                  // Cell imbalance Protection Det_Occur Alarm_Occur
u8 TempDifferAD(s16 temp);                  // TEMP difference Protection Det_Occur Alarm_Occur
u8 DchgOC2ndAD(s32 curr);                 // Discharge over current 2'nd Protection Alarm_Occur

// Protection Alrams Det_Release
u8 CellOVAR(u16 volt);                    // Cell over voltage Protection release Alarm_Occur
u8 CellUVAR(u16 volt);                    // Cell under voltage Protection release Alarm_Occur
u8 PackOVAR(u32 volt);                    // Pack over voltage Protection release Alarm_Occur
u8 PackUVAR(u32 volt);                    // Pack under voltage Protection release Alarm_Occur
u8 ChagOCAR(s32 curr);                    // Charge over current Protection release Alarm_Occur
u8 DchgOCAR(s32 curr);                    // Discharge over current Protection release Alarm_Occur
u8 ChagOTAR(s16 temp, u32 fchag);           // Charge over TMEP Protection release Alarm_Occur
u8 ChagUTAR(s16 temp, u32 fchag);           // Charge under TMEP Protection release Alarm_Occur
u8 DchgOTAR(s16 temp, u32 fchag);           // Discharge over TMEP Protection release Alarm_Occur
u8 DchgUTAR(s16 temp, u32 fchag);           // Discharge under TMEP Protection release Alarm_Occur
u8 AuxiOVAR(u16 volt);                    // Auxiliary over voltage Protection release Alarm_Occur
u8 AuxiUVAR(u16 volt);                    // Auxiliary under voltage Protection release Alarm_Occur
u8 CellVImbalAR(s16 volt);                  // Cell imbalance Protection release Alarm_Occur
u8 TempDifferAR(s16 temp);                  // TEMP difference Protection release Alarm_Occur


u16  Alarm_Occur(void);
u16 Alarm_Release(void);


#endif /* MS_ALARM_H_ */
