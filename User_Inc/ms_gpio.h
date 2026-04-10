/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-21                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_gpio.h
\author             KKD
\date               2025-11-21 
\brief              GPIO 동작을 위한 코드.
*********************************************
* History:
* 2025-11-21     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_GPIO_H
#define MS_GPIO_H
/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_main.h>

/* Exported define ---------------------------------------------------------------------------------*/
#define tp1(m)	   	m?  GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET) : GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET)
#define tp4(m)	   	m?  GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET) : GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET)
#define tp5(m)	   	m?  GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET) : GPIO_WritePin(GPIOF, GPIO_PIN_10, GPIO_PIN_RESET)
#define tp6(m)	   	m?  GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET) : GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET)



/* Exported macro ----------------------------------------------------------------------------------*/
static inline u8 GPIO_ReadActiveLow(GPIO_TypeDef *port, u16 pin)
{
    return ((port->IDR & pin) != 0U) ? 0U : 1U;
}

static inline u8 GPIO_ReadActiveHigh(GPIO_TypeDef *port, u16 pin)
{
    return ((port->IDR & pin) != 0U) ? 1U : 0U;
}

static inline void GPIO_WritePin(GPIO_TypeDef *port, u16 pin, BOOL on)
{
    if (on != FALSE){
        port->BSRR = (u32)pin;
    }else{
        port->BSRR = ((u32)pin << 16U);
    }
}
static inline void GPIO_TogglePin(GPIO_TypeDef *port, u16 pin)
{
    port->ODR ^= pin;
}


/* Exported typedef --------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------*/
/* Exported function prototypes --------------------------------------------------------------------*/

/* ================= DIGITAL OUTPUT ================= */
/* RS-422 / RS-485 */
static inline void GPIO_DOut_EN422(BOOL on) 				{ GPIO_WritePin(GPIOF, GPIO_PIN_3, on); }
static inline void GPIO_DOut_EN485(BOOL on) 				{ GPIO_WritePin(GPIOB, GPIO_PIN_14, on); }

/* Sensor Power */
static inline void GPIO_DOut_GAS(BOOL on)   				{ GPIO_WritePin(GPIOF, GPIO_PIN_5, on); }
static inline void GPIO_DOut_CAB(BOOL on)   				{ GPIO_WritePin(GPIOF, GPIO_PIN_6, on); }
static inline void GPIO_DOut_IMD(BOOL on)   				{ GPIO_WritePin(GPIOF, GPIO_PIN_7, on); }
static inline void GPIO_DOut_MVB(BOOL on)   				{ GPIO_WritePin(GPIOF, GPIO_PIN_8, on); }

/* Ext led control */
static inline void GPIO_DOut_ON_NOR(BOOL on)   				{ GPIO_WritePin(GPIOE, GPIO_PIN_3, on); }
static inline void GPIO_DOut_ON_ERR(BOOL on)   				{ GPIO_WritePin(GPIOE, GPIO_PIN_4, on); }
static inline void GPIO_DOut_ON_FLT(BOOL on)   				{ GPIO_WritePin(GPIOE, GPIO_PIN_5, on); }

/* OUT_spi 2ea chip select */
static inline void GPIO_DOut_SPI1_CS1(BOOL on)   			{ GPIO_WritePin(GPIOE, GPIO_PIN_0,  on); }
static inline void GPIO_DOut_SPI1_CS2(BOOL on)				{ GPIO_WritePin(GPIOE, GPIO_PIN_1,  on); }
static inline void GPIO_DOut_SPI2_CS1(BOOL on)   			{ GPIO_WritePin(GPIOC, GPIO_PIN_14, on); }
static inline void GPIO_DOut_SPI2_CS2(BOOL on)   			{ GPIO_WritePin(GPIOC, GPIO_PIN_15, on); }


/* OUT_bms fault - hard wire signal */
static inline void GPIO_DOut_BMS_FULT(BOOL on)   			{ GPIO_WritePin(GPIOE, GPIO_PIN_2, on); }


/* Status LED */
static inline void GPIO_DOut_LYel_VIT(BOOL on) 				{ GPIO_WritePin(GPIOF, GPIO_PIN_12, on); }
static inline void GPIO_DOut_LRed_DET(BOOL on) 				{ GPIO_WritePin(GPIOF, GPIO_PIN_13, on); }
static inline void GPIO_DOut_LGre_CHG(BOOL on) 				{ GPIO_WritePin(GPIOF, GPIO_PIN_14, on); }
static inline void GPIO_DOut_LOrg_DCH(BOOL on) 				{ GPIO_WritePin(GPIOF, GPIO_PIN_15, on); }


/* FAN */
static inline void GPIO_DOut_B_FAN(BOOL on) 				{ GPIO_WritePin(GPIOB, GPIO_PIN_0, on); }
static inline void GPIO_DOut_S_FAN(BOOL on) 				{ GPIO_WritePin(GPIOB, GPIO_PIN_1, on); }


/* Relay */
static inline void GPIO_DOut_MTOP_CON(BOOL on) 				{ GPIO_WritePin(GPIOE, GPIO_PIN_7, on); }
static inline void GPIO_DOut_MBTM_CON(BOOL on) 				{ GPIO_WritePin(GPIOE, GPIO_PIN_8, on); }
static inline void GPIO_DOut_MPRE_RLY(BOOL on) 				{ GPIO_WritePin(GPIOE, GPIO_PIN_9, on); }

static inline void GPIO_DOut_BPRE_RLY(BOOL on) 				{ GPIO_WritePin(GPIOE, GPIO_PIN_10, on); }
static inline void GPIO_DOut_BTOP_RLY(BOOL on) 				{ GPIO_WritePin(GPIOE, GPIO_PIN_11, on); }
static inline void GPIO_DOut_BBTM_RLY(BOOL on) 				{ GPIO_WritePin(GPIOE, GPIO_PIN_12, on); }


/* Debug TP */
static inline void GPIO_DOut_TP1(BOOL on) 					{ GPIO_WritePin(GPIOA, GPIO_PIN_7, on); }
static inline void GPIO_DOut_TP2(BOOL on) 					{ GPIO_WritePin(GPIOB, GPIO_PIN_13, on); }
static inline void GPIO_DOut_TP3(BOOL on) 					{ GPIO_WritePin(GPIOB, GPIO_PIN_12, on); }
static inline void GPIO_DOut_TP4(BOOL on) 					{ GPIO_WritePin(GPIOB, GPIO_PIN_15, on); }
static inline void GPIO_DOut_TP5(BOOL on) 					{ GPIO_WritePin(GPIOF, GPIO_PIN_10, on); }
static inline void GPIO_DOut_TP6(BOOL on) 					{ GPIO_WritePin(GPIOG, GPIO_PIN_14, on); }


/* ================= LED TOGGLE ================= */
static inline void GPIO_DOut_LYel_VIT_Toggle(void) { GPIO_TogglePin(GPIOF, GPIO_PIN_12); }
static inline void GPIO_DOut_LRed_DET_Toggle(void) { GPIO_TogglePin(GPIOF, GPIO_PIN_13); }
static inline void GPIO_DOut_LGre_CHG_Toggle(void) { GPIO_TogglePin(GPIOF, GPIO_PIN_14); }
static inline void GPIO_DOut_LOrg_DCH_Toggle(void) { GPIO_TogglePin(GPIOF, GPIO_PIN_15); }


/* ================= RLY TOGGLE ================= */
static inline void GPIO_DOut_Tp1_toggle(void)					{ GPIO_TogglePin(GPIOA, GPIO_PIN_7); }
static inline void GPIO_DOut_Tp2_toggle(void)					{ GPIO_TogglePin(GPIOB, GPIO_PIN_13); }
static inline void GPIO_DOut_Tp3_toggle(void)					{ GPIO_TogglePin(GPIOB, GPIO_PIN_12); }
static inline void GPIO_DOut_Tp4_toggle(void)					{ GPIO_TogglePin(GPIOB, GPIO_PIN_15); }
static inline void GPIO_DOut_Tp5_toggle(void) 					{ GPIO_TogglePin(GPIOF, GPIO_PIN_10); }
static inline void GPIO_DOut_Tp6_toggle(void) 					{ GPIO_TogglePin(GPIOG, GPIO_PIN_14); }


static inline void GPIO_DOut_Rly_toggle(void)					{ GPIO_TogglePin(GPIOD, GPIO_PIN_0); }
static inline void GPIO_DOut_RlyOnSig(BOOL on) 					{ GPIO_WritePin(GPIOD, GPIO_PIN_0, on); }


/* ================= DIGITAL INPUT ================= */
/* Fuse */
static inline u8 GPIO_DIN_MTOP_FUS(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_2); }
static inline u8 GPIO_DIN_MBTM_FUS(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_3); }
static inline u8 GPIO_DIN_BBTM_FUS(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_6); }

/* Relay feedback */
static inline u8 GPIO_DIN_MTOP_CON(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_4); }
static inline u8 GPIO_DIN_MBTM_CON(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_5); }
static inline u8 GPIO_DIN_BTOP_RLY(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_7); }
static inline u8 GPIO_DIN_BBTM_RLY(void) { return GPIO_ReadActiveLow(GPIOG, GPIO_PIN_8); }

/* External */
static inline u8 GPIO_DIN_ON_CHG(void)   { return GPIO_ReadActiveLow(GPIOD, GPIO_PIN_9); }
static inline u8 GPIO_DIN_ON_HVB(void)   { return GPIO_ReadActiveLow(GPIOD, GPIO_PIN_10); }

/* Fire sensor */
static inline u8 GPIO_DIN_LHD_Open(void)  { return GPIO_ReadActiveHigh(GPIOD, GPIO_PIN_12); }
static inline u8 GPIO_DIN_LHD_Short(void) { return GPIO_ReadActiveHigh(GPIOD, GPIO_PIN_13); }

BOOL Gpio_Hv_In(void);
BOOL Gpio_Ch_In(void);
void Gpio_Fault_Out(void);
void Gpio_Led(void);
void Gpio_Proc(void);
void Gpio_Init(void);

#endif //__MS_GPIO_H__
