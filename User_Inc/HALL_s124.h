/*=================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(HALL sensor s124 -adc )	        	                               *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025. 11. 20                                                                *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               HALL_s124.c
\author             KKD
\date               2025-11-21 
\brief              HALL sensor(Mcu Adc1_Ch10) - BTMS Current
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/

#ifndef HALL_S124_H_
#define HALL_S124_H_
/* Includes ---------------------------------------------------------------------------------------*/
//#include <ms_main.h>
#include "ms_adc.h"
#include "ms_process.h"


/* Exported define ---------------------------------------------------------------------------------*/
#define		OFFSETL				(0x3FFF);								// 16383
#define		CONSTPIL			((float_t)5000./32767./26.7*1e+3)		// Sensitivity : 26.7 / 5.86
#define		CONSTPIH			((float_t)5000./32767./4.0*1e+3)		// Sensitivity : 4.0 / 38.148
#define		ZEROCURR			(900L) 									// 0.900A
#define		DHABS124_MAX		(500000L) 								// 500.000A
#define		DHABS124_HILO		(70000L) 	
#define		CONSTPVIN			((3300./32767.)*(35.))					// Pack In Voltage 1.0mV Unit

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
#pragma pack(push,1)
typedef struct {
	u16 u16CurrAdc[2];
	u16 u16AdcCenter[4];
	u16 u16CurrSlope[4];
}sS124Data;
#pragma pack(pop)
/* Exported variables ---------------------------------------------------------*/
/* Exported function prototypes -----------------------------------------------*/
void HALL_s124_InitGain(void) ;
void HALL_s124_GetCurrent(void);


#endif /* HALL_S124_H_ */
