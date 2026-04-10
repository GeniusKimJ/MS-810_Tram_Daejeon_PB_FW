/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(FIRE sensor - check input pin)                                      *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               FIRE_phsc220xcr.h
\author             KKD
\date               2025-11-24 
\brief              FIRE sensor - check input pin
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef FIRE_PHSC220XCR_H
#define FIRE_PHSC220XCR_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"

/* Exported define ---------------------------------------------------------------------------------*/
#define MAX_LHD_CHK_COUNT		(30)                                                   //KKD 2025-12-04 3sec (100 x 30)
/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef enum{
    LHD_STATE_NORMAL = 0,
    LHD_STATE_OPEN,
    LHD_STATE_SHORT
} eLhdState;


/* Exported variables ------------------------------------------------------------------------------*/
/* Exported function prototypes --------------------------------------------------------------------*/

void Fire_Init(void);
void Fire_OpenShortCheck(void);
void Fire_Proc(void);
BOOL Fire_Open(void);
BOOL Fire_Short(void);
#endif //__FIRE_PHSC220XCR_H__

