/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Mcu Adc)                                           			  *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                               		   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_adc.h
\author             KKD
\date               2025-11-24 
\brief              Mcu Adc
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_ADC_H
#define MS_ADC_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"
/* Exported define ---------------------------------------------------------------------------------*/
#define ADC_REF_MV        	(3300.0f)
#define ADC_15BIT_MAX     	(32767.0f)

#define ADC_LSB_15BIT_MV  	(ADC_REF_MV / ADC_15BIT_MAX)


//Int Power Supply
#define AUX33_GAIN        	(0.362987152f)
#define CONST_AUX33_15BIT 	(ADC_LSB_15BIT_MV / AUX33_GAIN)	// [mV / count]

#define AUX5_GAIN         	(0.315245827f)
#define CONST_AUX5_15BIT  	(ADC_LSB_15BIT_MV / AUX5_GAIN)	// [mV / count]

#define AUX13_GAIN        	(0.157612842f)
#define CONST_AUX13_15BIT 	(ADC_LSB_15BIT_MV / AUX13_GAIN)	// [mV / count]

#define AUX24_GAIN        	(0.084152501f)
#define CONST_AUX24_15BIT 	(ADC_LSB_15BIT_MV / AUX24_GAIN)	// [mV / count]


//PV
#define ADC_VREF_mV			(3300.0f)	
#define ADC_RESOLUTION_15B	(32767.0f)		/* 15Bit */

#define HV_R_HIGH			(3760000.0f)    /* 3.76 MΩ	HV -> ADC */
#define HV_R_LOW			(10000.0f)      /* 10 kΩ 	HV -> ADC */

#define HV_DIV_RATIO		((HV_R_HIGH + HV_R_LOW) / HV_R_LOW) //38.6

#define HV_GAIN_CAL_CPV        (0.754415858f)
#define CONST_CPV_15BIT  	(ADC_LSB_15BIT_MV / HV_GAIN_CAL_CPV)	// [mV / count]

#define HV_GAIN_CAL_VPV        (0.747386917f)
#define CONST_VPV_15BIT  	(ADC_LSB_15BIT_MV / HV_GAIN_CAL_VPV)	// [mV / count]

#define HV_GAIN_CAL_BTMS_VPV   (0.75535342f)   // 임시값, 실측 후 수정
#define CONST_TVPV_15BIT  	(ADC_LSB_15BIT_MV / HV_GAIN_CAL_BTMS_VPV)	// [mV / count]

#define ADC15_TO_HV_Cal(adc15, gain) \
                (ADC15_TO_HV_mV(adc15) * (gain)) // 600v : 81442.03009125 *


/* ADC(15bit) → ADC input voltage (mV) */ 
#define ADC15_TO_VADC_mV(adc15) \
    (((float)(adc15) * ADC_VREF_mV) / ADC_RESOLUTION_15B) //600v   :  (20950 * 3300) /32767

/* ADC(15bit) → High Voltage (mV) */
#define ADC15_TO_HV_mV(adc15) \
    (ADC15_TO_VADC_mV(adc15) * HV_DIV_RATIO)		//  2109.89715 * 38.6

#define HV_GAIN_CAL                 (1.000f)    /* Gain correction */
#define HV_OFFSET_CAL_mV            (0.0f)      /* Offset correction (mV) */

/* ADC(15bit) → Calibrated High Voltage (mV) */
#define ADC15_TO_HV_CAL_mV(adc15) \
    ((ADC15_TO_HV_mV(adc15) * HV_GAIN_CAL) + HV_OFFSET_CAL_mV)

#define HV_OV_800V_mV               (800000.0f)
#define HV_UV_600V_mV               (600000.0f)

#define ADC1_CH_NUM     (u8)4
#define ADC2_CH_NUM     (u8)3
#define ADC3_CH_NUM     (u8)2

#define ADC_AVG_COUNT   (u8)8
#define ADC_BUF_SIZE    (u8)(((u8)8 * ADC_AVG_COUNT) + (u8)1 )

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/

typedef enum{
    ID_ADIN0_24V = 0,
    ID_ADIN1_13V,
    ID_ADIN2_5V,
    ID_ADIN3_3V3,
    ID_ADIN4_CPV,
    ID_ADIN5_VPV,
    ID_ADIN6_TVPV,
    ID_ADIN10_PIL,
    ID_ADIN11_PIH,
    ID_ADC_NUM_MAX
}AdcIDType;

typedef enum{
    ID_ADC1_CH00 = 0,
    ID_ADC1_CH01,
    ID_ADC1_CH02,
    //ID_ADC1_CH03,                                                                     //jkpark 2026-01-15 STL에 겹치는 기능으로 내부 VREFINT 를 사용.
    //ID_ADC1_VREFINT = ID_ADC1_CH03,
    
    ID_ADC1_VREFINT,
    ID_ADC2_CH04,
    ID_ADC2_CH05,
    ID_ADC2_CH06,
    ID_ADC3_CH10,
    ID_ADC3_CH11,
    ID_ADC__NUM_MAX
}AdcIndexType;

/* Exported variables ------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
typedef struct{
	u16 supp3v3;
	u16 supp5v0;
	u16 supp13v;
	u16 supp24v;
	u16 s124_high;
	u16 s124_low;
	u16 cpv;
	u16 vpv;
	u16 tvpv;
	u16 cpvAdc;
	u16 vpvAdc;
	u16 tvpvAdc;
}sAdcData;


typedef struct{
	u16	ref_value;

	u16	s124_center;
	u16	s124_slope;
		
	u16	gain_pv_in;
	u16	gain_pv_ou;
	u16	gain_pv_bt;

	u16	gain_ax_24v;
	u16	gain_ax_13v;
	u16	gain_ax_5v;
	u16	gain_ax_3v;

}sAdcGain;



/* Private variables ------------------------------------------------------------------------------*/
extern sAdcData g_sAdcData;
extern sAdcGain g_sAdcGain;
extern u16 g_u16Adc1[ ADC1_CH_NUM ];                                                    //jkpark 2026-01-16 12Bit이므로 u16으로 설정해야함.
extern u16 g_u16Adc2[ ADC2_CH_NUM ];                                                    //jkpark 2026-01-16 12Bit이므로 u16으로 설정해야함.
extern u16 g_u16Adc3[ ADC3_CH_NUM ];                                                    //jkpark 2026-01-16 12Bit이므로 u16으로 설정해야함.

extern u32 g_u32Adc1_Buf[ ADC1_CH_NUM ][ ADC_BUF_SIZE ];
extern u32 g_u32Adc2_Buf[ ADC2_CH_NUM ][ ADC_BUF_SIZE ];
extern u32 g_u32Adc3_Buf[ ADC3_CH_NUM ][ ADC_BUF_SIZE ];
extern u32 g_u32Adc1_index;
extern u32 g_u32Adc2_index;
extern u32 g_u32Adc3_index;

extern u32 g_u32AdcSum[ ID_ADC__NUM_MAX ];



/* Exported function prototypes --------------------------------------------------------------------*/

//s32 ADC_GetStmAdc15bit(u32 rank, void);
//s32 ADC_GetStmAdc15bit(u32 rank, void);
void	ADC_UpdateStmAdc15bit( void );
s32		ADC_GetStmAdc15bit( AdcIDType adcID );
void 	ADC1_Ch10_GetPIL(void); 
void 	ADC1_Ch11_GetPIH(void);
void 	ADC1_Ch0_GetAd24V(void);
void 	ADC1_Ch1_GetAd13V(void); 
void 	ADC1_Ch2_GetAd5V0(void); 
void 	ADC1_Ch3_GetAd3V3(void) ;
void 	ADC2_Ch0_GetCPV(void) ;
void	ADC2_Ch1_GetAVPV(void) ;
void	ADC2_Ch2_GetBtmsVPV(void) ;
void    ADC_InitAll( void );

u32 	ADC_Get_PVIN(void);
u32 	ADC_Get_PVOUT(void);
u32 	ADC_Get_PV_BT(void);
u16 	ADC_GetAdc_PVIN(void);
u16 	ADC_GetAdc_PVOUT(void);
u16 	ADC_GetAdc_PV_BT(void);
void 	ADC_SetAdc_PVIN(u16 value);			
void 	ADC_SetAdc_PVOUT(u16 value);			
void	ADC_SetAdc_PV_BT(u16 value);		
void	Adc_DefaultV_Gain(sAdcGain *pstAdcGain);



#endif //__MS_ADC_H__
