/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_timer.c
\author             KKD
\date               2025-11-21 
\brief              Timer 동작을 위한 코드.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
  2025-12-05     v0.02    jkpark Timer 타이머 관련 함수 추가. 
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include "ms_timer.h"


/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef ---------------------CCF-----------------------------------------------------------*/
#define TIMx                             TIM1
#define TIMx_CLK_ENABLE                  __TIM1_CLK_ENABLE

#define TIMx_POLLING_INTERVAL             5 /* in ms. The max is 65 and the min is 1 */
#define TIMx_IRQn                        TIM3_IRQn
/* Private variables ------------------------------------------------------------------------------*/
SWTimerListType      timerElementList;
u8	g_f1ms;
u8	g_f10ms;


/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/


u32 DWT_Delay_Init(void) {
	u8 	status = 0;
	CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; 						// ~0x01000000;	Disable TRC
	CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; 						// 0x01000000;	Enable TRC

	DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; 									// ~0x00000001;	Disable clock cycle counter
	DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; 									// 0x00000001;	Enable  clock cycle counter
	DWT->CYCCNT = 0;														// Reset the clock cycle counter value

	 __ASM __IO ("NOP");													// 3 NO OPERATION instructions
	 __ASM __IO ("NOP");
	 __ASM __IO ("NOP");

	 if(DWT->CYCCNT > 0U) {
		status = 0U;														// Check if clock cycle counter has started
	 } else {
		status = 1U; 														// clock cycle counter not started
	 }

	 return status;
}

void Delay_us(__IO u32 microseconds) {	// delay_us
  u32 start = DWT->CYCCNT;
  u32 ticks = microseconds * (HAL_RCC_GetHCLKFreq() / 1000000UL);
  
  while ((DWT->CYCCNT - (u32)start) < ticks){;}
}

void Delay_ms(__IO u32 milliseconds) {	// delay_ms
    u32 cnt = 0;
    for( cnt = 0; cnt < milliseconds ; cnt++ ){
        Delay_us( 1000 );
    }
}


/*******************************************************************************
* Function Name  : TimerProcess
* Description    : timer인터럽트 루틴. 1mS 마다 timerElementList에 등록된 시간을 확인하고 함수를 수행함.
* Input          : timerElementList
* Output         : 등록된 함수를 수행.
* Return         : void
*******************************************************************************/
static volatile msStatus_t g_TimerFlag = MS_FALSE;
void TimerProcess( void )
{
    u32 pos;
    u8 enableValue;

    if( timerElementList.elementCount == 0 ){return;}

    if( g_TimerFlag == MS_TRUE ){return;}
    g_TimerFlag = MS_TRUE;

//    GPIO_SetBits(GPIOE, GPIO_Pin_3);

//==============================================================================
//  Timer...
//==============================================================================
    for( pos=0 ; pos<(u32)SW_TIMER_MAX ; pos++)
    {
        if(timerElementList.timerList[pos].timerEnable == (char)MS_ENABLE )
        {
            if(timerElementList.timerList[pos].setCount != 0)
            {
                timerElementList.timerList[pos].counter++;

                if(timerElementList.timerList[pos].counter >= timerElementList.timerList[pos].setCount)
                {
                    enableValue = (u8)timerElementList.timerList[pos].func();

                    timerElementList.timerList[pos].counter       =   0; 
                    timerElementList.timerList[pos].timerEnable   =   (char)enableValue;
                    if( enableValue == 0U ){
                        timerElementList.elementCount--;
                    }
                }
            }
        }
    }
    
    g_TimerFlag = MS_FALSE;
//    GPIO_ResetBits(GPIOE, GPIO_Pin_3);
}

/********************************************************************************
   Function Name  : TimerInit                                                    
   Description    : 타이머 관련 초기화.
   input          : 
   output         : 
   return         : 
********************************************************************************/
void TimerInit( void ){
    memset( &timerElementList, 0x00, sizeof( timerElementList ));
}

/********************************************************************************
   Function Name  : TimerAdd                                                    
   Description    : Timer를 추가하는 함수.                              
   input          : 동작할 간격(interval),동작할 함수(func), 등록된 TimerID(pElementID)
   output         : interval간격으로 func를 동작하도록 timer 루틴에 추가한다.   
   return         : 추가된 Timer ID.  
********************************************************************************/
msStatus_t TimerAdd( long interval, msStatus_t (*func)(void), int *pElementID){
    int iElementID = 0;

    if(timerElementList.elementCount >= (int)(SW_TIMER_MAX-1)){
        return MS_E_TIMER_ELEMENT_FULL;
    }

    if( interval <= (long)0 ){
        return MS_E_TIMER_UNVALUABLE_COUNT;
    }

    if( func == NULL ){
        return MS_E_TIMER_UNVALUABLE_FUNCTION;
    }

    for( iElementID = 0 ; iElementID < SW_TIMER_MAX ; iElementID++ ){
        if( timerElementList.timerList[ iElementID].timerEnable == (char)MS_DISABLE ){
            timerElementList.elementCount++;

            timerElementList.timerList[ iElementID ].timerEnable  = (char)MS_ENABLE;
            timerElementList.timerList[ iElementID ].setCount     = (long)(interval*(long)T1MS);
            timerElementList.timerList[ iElementID ].func         = func ;
            timerElementList.timerList[ iElementID ].counter      = (long)0;

            if( pElementID != NULL ) {
		*pElementID = iElementID;
		}
            return MS_SUCCESS;
        }
    }

    return MS_E_TIMER_ELEMENT_FULL;
}

/********************************************************************************
   Function Name  : TimerRestart                                                
   Description    : Timer를 다시시작하는 함수.                     
   input          : ?ㅼ ??? Timer ID(elementID)                             
   output         : ?대?elementID? Count媛 0遺???ㅼ ?????               
   return         :                                                  
********************************************************************************/
msStatus_t TimerRestart( int elementID ){

    if(    (timerElementList.timerList[elementID].setCount       == (long)0) 
        || (timerElementList.timerList[elementID].timerEnable    == (char)0 )){
        return MS_E_TIMER_UNUSED_ID;
    }

    timerElementList.timerList[elementID].counter      = (long)0;

    return MS_SUCCESS;
}

/********************************************************************************
   Function Name  : TimerDelete                                                
   Description    : Timer를 삭제하는 간략한 함수.                    
   input          : 삭제할 Timer ID(elementID)                                      
   output         : 해당 elementID의 Timer를 삭제한다.
   return         :                                                    
********************************************************************************/
msStatus_t TimerDelete( int elementID ){

    if( timerElementList.timerList[elementID].timerEnable != (char)0 ){
        timerElementList.elementCount--;
        timerElementList.timerList[elementID].timerEnable  = (char)0;
        timerElementList.timerList[elementID].setCount     = (long)0;
        timerElementList.timerList[elementID].counter      = (long)0;
        timerElementList.timerList[elementID].func         = NULL;

        if(timerElementList.elementCount < 0 ) {
				timerElementList.elementCount = 0;
        }
    }else{
        return MS_ERROR;
    }
    
    return MS_SUCCESS;
}

u32 GetTick_us(void) {
    return DWT->CYCCNT / (SystemCoreClock / (u32)1000000);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	tp1(1);
	GPIO_DOut_Tp3_toggle();
	GPIO_DOut_Tp5_toggle();
	GPIO_DOut_Tp6_toggle();
	tp4(1);
  	//Can_TxProc(McuCan_Ext_Type);
	//static u8 s10ms = 0;
	if (htim->Instance == TIM1)
	{
	    // TIM1 주기 처리
	    TimerProcess();
	}
	else if (htim->Instance == TIM10)
	{
	    // TIM10 주기 처리
	}
	if(++g_sSysTick.u16CanTimer>=(u16)MAX_SYSTICK){
		g_sSysTick.u16CanTimer = 0;
	}
	g_f1ms = 1;
	if(++g_f10ms>=10U){
		g_f10ms = 0;
		tp4(0);
		g_sSysTick.b10ms_Task = TRUE;

	}

	if(++g_sSysTick.u16SysTick>=(u16)MAX_SYSTICK){
		g_sSysTick.u16SysTick = 0;
		tp1(0);

	}   
}



