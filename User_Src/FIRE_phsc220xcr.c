/*=================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(FIRE sensor - check input pin)	        	                       *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025. 11. 20                                                                *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               FIRE_phsc220xcr.c
\author             KKD
\date               2025-11-21 
\brief              화재 감지 GPIO Input로 확인.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <FIRE_phsc220xcr.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
static eLhdState g_eLhdState = LHD_STATE_NORMAL;

/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/
void Fire_Init(void)
{
    g_eLhdState = LHD_STATE_NORMAL;
}

void Fire_OpenShortCheck(void)
{
	static u8 opencnt  = 0;
	static u8 shortcnt = 0;
	static u8 normalcnt = 0;
	//BOOL result = 0;


	BOOL bopen = GPIO_DIN_LHD_Open();
	BOOL bshort = GPIO_DIN_LHD_Short();

	if((bopen == TRUE) && (bshort == FALSE)){
	   if((++opencnt) >= (u8)MAX_LHD_CHK_COUNT){
		   opencnt = (u8)MAX_LHD_CHK_COUNT;
		   g_eLhdState = LHD_STATE_OPEN;
	   }
	   shortcnt = 0;
	   normalcnt = 0;
	}else if((bopen == FALSE) && (bshort == TRUE)){
	   if(++shortcnt >= (u8)MAX_LHD_CHK_COUNT){
		   shortcnt = MAX_LHD_CHK_COUNT;
		   g_eLhdState = LHD_STATE_SHORT;
	   }
	   opencnt = 0;
	   normalcnt = 0;
	}else if((bopen == FALSE) && (bshort == FALSE)){
	   if(++normalcnt >= (u8)MAX_LHD_CHK_COUNT){
		   normalcnt = MAX_LHD_CHK_COUNT;
		   g_eLhdState = LHD_STATE_NORMAL;
	   }
	   opencnt = 0;
	   shortcnt = 0;
	}
}

void Fire_Proc(void)
{
	Fire_OpenShortCheck();
	
	if(g_eLhdState == LHD_STATE_OPEN){
		// Line Open Fault
	}
	
	if(g_eLhdState == LHD_STATE_SHORT){
		// Real Fire Fault
	}
	
	if(g_eLhdState == LHD_STATE_NORMAL){
		// Normal(Idle)
	}   
}

BOOL Fire_Open(void)
{
	BOOL state = FALSE;
	if(g_eLhdState == LHD_STATE_OPEN){
		state = TRUE;
	}state = FALSE;
	return state;
}

BOOL Fire_Short(void)
{
	BOOL state = FALSE;
	if(g_eLhdState == LHD_STATE_SHORT){
		state = TRUE;
	}state = FALSE;
	return state;
}


