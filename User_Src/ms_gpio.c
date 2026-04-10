/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(GPIO)                                                               *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_gpio.c
\author             KKD
\date               2025-11-21 
\brief              GPIO 동작을 위한 코드.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_gpio.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/


/* Private typedef --------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/
void Gpio_Led(void){
	GPIO_DOut_LYel_VIT_Toggle();

	if(RackPkt.fdet != 0U){
		GPIO_DOut_LRed_DET_Toggle();
		GPIO_DOut_ON_FLT(TRUE);
	}else if(RackPkt.falm != 0U){
		GPIO_DOut_ON_FLT(FALSE);
		GPIO_DOut_ON_ERR(TRUE);
	}else{
		GPIO_DOut_ON_ERR(FALSE);
		GPIO_DOut_ON_FLT(FALSE);
		GPIO_DOut_ON_NOR(TRUE);
	}

	//GPIO_DOut_LGre_CHG_Toggle();
	//GPIO_DOut_LOrg_DCH_Toggle();
}
BOOL Gpio_Ch_In(void)
{
	static u8 cnt;
	u8 sts;
	
	sts = GPIO_DIN_ON_CHG();
	return sts;
}

BOOL Gpio_Hv_In(void)
{
	static u8 cnt;
	u8 sts;
	sts = GPIO_DIN_ON_HVB();
	if(sts == TRUE){
		if(++cnt>200){
		}
	}
	return sts;
}

void Gpio_Fault_Out(void)
{
	if(RackPkt.fdet !=0u){
		GPIO_DOut_BMS_FULT(TRUE);
	}else{
		GPIO_DOut_BMS_FULT(FALSE);
	}
}

void Gpio_Proc(void)
{
	Gpio_Led();
	Gpio_Fault_Out();
	Gpio_Ch_In();
	Gpio_Hv_In();
}

void Gpio_Init(void)
{
	GPIO_DOut_EN422(TRUE);
	GPIO_DOut_EN485(TRUE);

	GPIO_DOut_GAS(TRUE);
	GPIO_DOut_CAB(TRUE);
	GPIO_DOut_IMD(TRUE);
	GPIO_DOut_MVB(TRUE);

	GPIO_DOut_ON_NOR(FALSE);
	GPIO_DOut_ON_ERR(FALSE);
	GPIO_DOut_ON_FLT(FALSE);

	GPIO_DOut_SPI1_CS1(FALSE);
	GPIO_DOut_SPI1_CS2(FALSE);
	GPIO_DOut_SPI2_CS1(FALSE);
	GPIO_DOut_SPI2_CS2(FALSE);

	GPIO_DOut_RlyOnSig(FALSE);
/*
	GPIO_DOut_MTOP_CON	(FALSE);
	GPIO_DOut_MBTM_CON(FALSE);
	GPIO_DOut_MPRE_RLY(FALSE);

	GPIO_DOut_BPRE_RLY(FALSE);
	GPIO_DOut_BTOP_RLY(FALSE);
	GPIO_DOut_BBTM_RLY(FALSE);
*/
}
