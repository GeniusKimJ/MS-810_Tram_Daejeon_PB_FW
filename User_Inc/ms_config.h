#if 0	//info
/*=================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(uart2 - rs422(to MVB module))	        	                       *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2026-01-12                                                              	   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_uart2.h
\author             KKD
\date               2026-01-12 
\brief              uart2 - rs422(to MVB module).
*********************************************
* History:
* 2026-01-12     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_UART2_H
#define MS_UART2_H

/* Includes ---------------------------------------------------------------------------------------*/
#include <ms_main.h>

/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------*/
/* Private function prototypes --------------------------------------------------------------------*/
/* Private functions ------------------------------------------------------------------------------*/
#endif /* MS_UART2_H_ */

#endif


/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(bms define or struct)                                           			  *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                               		   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_config.h
\author             KKD
\date               2025-11-24 
\brief              bms define or struct
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_CONFIG_H
#define MS_CONFIG_H

typedef signed char			s8;			// 1 byte signed;   prefix: sb 
typedef signed short		s16;		// 2 byte signed;   prefix: ss 
typedef signed long			s32;		// 2 byte signed;   prefix: ss 
typedef signed long long	s64;		// 2 byte signed;   prefix: ss 

typedef unsigned char		u8;			// 1 byte signed;   prefix: sb 
typedef unsigned short		u16;		// 2 byte signed;   prefix: ss 
typedef unsigned long		u32;		// 4 byte signed;   prefix: ss 
typedef unsigned long long	u64;

typedef unsigned char		BOOL;

/* Includes ---------------------------------------------------------------------------------------*/
/* Exported define ---------------------------------------------------------------------------------*/
#define FABS(x)   			(((x) < 0.0f) ? -(x) : (x))
#define LABS_U16(x) 		((u16)((labs(x) > 65535L) ? 65535L : labs(x)))

//#define INT32_MIN           (-2147483648)       // -2^31     0x80000000
//#define INT32_MAX           (2147483647)        //  2^31 - 1 0x7FFFFFFF

/*
 //jkpark 2025-12-29 ms_monitor.h에 Define
#define SOH					(0x01U)				// Start of header
#define STX					(0x02U)				// Start of text
#define ETX					(0x03U)				// End of text
#define EOT					(0x04U)				// End of transmission
//#define CR				('\r')				// 0x0D Carriage return
//#define LF				('\n')			    // 0x0A Line feet
#define ACK					(0x06U)				// Acknowledge
#define NAK					(0x15U)				// Not Acknowledge

#define CMD_BOOT			(0x0BU)				// Boot Mode
#define CMD_REQ				(0x10U)				// Request Slave Data by Master
#define CMD_BMU				(0x11U)				// Request Master Data by Host
#define CMD_CMU				(0x12U)				// Request Slave Data by Host
#define CMD_RTC				(0x13U)				// RTC Local Time Setting
#define CMD_CAL				(0x17U)				// Calibration BMS
#define CMD_ESC				(0x18U)				// Escape test routine
#define CMD_ORDER			(0x19U)				// Manual Mode : mklee180906
#define CMD_MNM				(0x1BU)				// Manual Mode
#define CMD_EEP				(0x1EU)				// EEPROM
#define CMD_DETECT  		(0x1DU)				// Detection Setting

#define SCMD_BOOT_START		(0xB1U)				// Boot Start
#define SCMD_BOOT_SEND		(0xB2U)				// Boot Send
#define SCMD_BOOT_END		(0xB3U)				// Boot End
#define SCMD_BOOT_RTY		(0xB4U)				// Boot Retry
*/

#define NSLV				(u8)(1)				// half Slave Count		18EA
#define NSCV				(u8)(8)				// Slave Cell Count		8EA
#define NSTH				(u8)(4)				// Slave Temp Count		3EA
#define NSPI				(u8)(2)


#define NCV 				(u16)(NSCV*NSLV*NSPI) //8*18*2
#define NTH 				(u16)(NSTH*NSLV*NSPI)

#define EN_DMA 1



#define FWVRSN	100		// 2024. 02. 24 	Misra-C 82EA
//#define ENCLK(m)	m?  (GPIOC->BSRR = GPIO_PIN_3) : (GPIOC->BSRR = GPIO_PIN_3 << 16U)

#define ENMN


#define	ON					(u8)1U
#define	OFF					(u8)0U
#define	HI					ON
#define	LO					OFF
#define	HIGH				ON
#define	LOW					OFF
#define	RISE				ON
#define	FALL				OFF
#define	START				ON
#define	STOP				OFF
#define	ENTER				ON
#define	EXIT				OFF
#define	OK					ON
#define	NG					OFF
#define	TXEN				ON
#define	TXDI				OFF
#define	EVEN				ON
#define	ODD					OFF

//#define	SET					(1)                                                 //jkpark 2026-01-07 SET은 사용하는 데가 많으므로 define하면 안됨.
//#define	CLR					(0)

#define	TRUE				(BOOL)(1U)
#define	FALSE				(BOOL)(0U)
#define	IN					(1)
#define	OUT					(0)
#define	EN					(1)
#define	DI					(0)
#define READ				(1)
#define WRITE				(0)


#define ICHGL				(0)
#define ICHGH				(1)
#define IDCHL				(2)
#define IDCHH				(3)
#define IREAL				(4)
#define I1SEC				(5)
#define CELLCAP				(20000)						// LTO HP
#define CONSTCRATE			(80000UL)					// , 80Ah
#define CONSTFULLCAP		(CONSTCRATE*3600UL)

//#define CONSTSOCTICK		(1U)						// 144s x 1 = 2m 24s
#define CONSTSOCTICK		(15U)						// Calculate Voltage Capacity every CONSTSOCTICK (unit : sec)
#define CONSTCNTVSOC		(144U * CONSTSOCTICK)		// Calculate VSOC 144 times. 144s
#define CONSTSOCRATE		(1e+11)

#define UBMIN				(2090)
#define UBMAX				(2033U)
#define UBOPR				(20U)
#define UBSTP				(5U)
//#define UBOPR				(50U)
//#define UBSTP				(50U)

/*************************** GPIO Macro define ****************************************************/
#define ENCLK(m)	m?  (GPIOC->BSRR = GPIO_PIN_6) : (GPIOC->BSRR = GPIO_PIN_6 << 16U)

/*************************** GPIO Macro define ****************************************************/


/// Port A
#define ad_m_ch0			ADC_CHANNEL_0
#define ad_pil_ch3			ADC_CHANNEL_3
#define ad_pih_ch4			ADC_CHANNEL_4
	
/// Port B
#define ad_pvou_ch8			ADC_CHANNEL_8
#define ad_pvin_ch9			ADC_CHANNEL_9

/// Port C
#define ad_12v_ch10			ADC_CHANNEL_10
#define ad_3_3v_ch12		ADC_CHANNEL_12
#define ad_5_0v_ch13		ADC_CHANNEL_13
#define ad_th1_ch14			ADC_CHANNEL_14
#define ad_th2_ch15			ADC_CHANNEL_15


//#define ENABLE_MENUAL
/**************************  STATE FLAG  *********************************************/
//[0]
#define	FPRERLY				(u16)(0x0001)		// Check PRE-DCK
#define	FDCKRLY				(u16)((1UL<<1))		// Check DCK
#define	FCKRLY				(u16)((1UL<<2))		// Check CK
#define	FLCKRLY     		(u16)((1UL<<3))		// Check L-CK
#define	FDCHING				(u16)((1UL<<4))		// Discharging
#define	FCHGING				(u16)((1UL<<5))		// Charging
#define	FNORM				(u16)((1UL<<6))		// Normal/FaUt -> Normal status
#define	FFAULT				(u16)((1UL<<7))		// Normal/FaUt -> FaUt status

//[1]
#define	FBMSOK				(u16)((1UL<<8))		// BMS On
#define	FKEYPWR				(u16)((1UL<<9))		// Power key 24V
//#define	FMSCKEY			(u16)((1UL<<10))	// Maconkey
#define	FFWKEY				(u16)((1UL<<11))	// Debug key, FW writing key
#define	FPVNG				(u16)((1UL<<12))	// PackV Not Out
//#define	FPWDOWN			(u16)((1UL<<13))	// Power Down
//#define	FRLYNG			(u16)((1UL<<14))	// Relay chk NG
//#define	FFUSE			(u16)((1UL<<15))	// FUSE

/**************************  COMM STATE FLAG  *********************************************/
#define	FCOMSLV				(u8)(0x01)			// Communication Slave
#define	FCOMHST				(u8)((1U<<1))		// Communication host
#define	FCOMCHR				(u8)((1U<<2))		// Communication charger
#define	FCOMPC				(u8)((1U<<3))		// Communication PC
#define	FAFEIC				(u8)((1U<<4))		// LTC6813 AFC IC B(SPI), 0001 0000
#define	FCOMRTC				(u8)((1U<<5))		// Communication RTC(I2c)
#define	FCOMEEP				(u8)((1U<<6))		// Communication EEPROM(I2c)
#define	FCOMTCMS			(u8)((1U<<7))		// system ready : relay on command

#define	FPRDFAIL			(u8)((1U<<7))		// pre-discharge fail
#define	DCHGING				(-1000)				// Discharging Current(1mA)
#define	CHAGING				(1000L)				// Charge Current(1mA)

/**************************  RELAY  *********************************************/
/*
#define FDET_PDCK			(u32)(FCUVP|FPUVP|FDOCP|FCOTP|FDOTP|FCUTP|FDUTP|FCOMF)
#define FDET_DCK			(u32)(FCUVP|FPUVP|FDOCP|FCOTP|FDOTP|FCUTP|FDUTP|FCOMF)
#define FDET_CK				(u32)(FCOVP|FPOVP|FCOCP|FCOTP|FDOTP|FCUTP|FDUTP|FCOMF)
#define FDET_LCK			(u32)(FCOVP|FPOVP|FCOCP|FCOTP|FDOTP|FDUTP|FCOMF)
#define RELAY_ALL			(u8)(0x0FU)
#define	FBMS_STS			(u32)((FPWDOWN))

// frlyctr
#define	FPDCK				(1U<<PDCK)			// pre discharge relay
#define	FDCK				(1U<<DCK)			// discharge relay
#define	FCK					(1U<<CK)			// charge relay
#define	FLCK				(1U<<LCK)			// lowtemp charge relay

#define PDCK				(u8)0U
#define DCK					(u8)1U
#define CK					(u8)2U
#define LCK					(u8)3U
#define RLYON100MS			(u8)(2)
*/


/**************************  VOLTAGE  *********************************************/
#define AVGTH				(4 + 2)				// Average Temperature (Reject Max & Min Data)
#define AVGVCV				(4 + 2)				// Average CV(Reject Max & Min Data)
#define AVGVPV				(4 + 2)				// Average PV(Reject Max & Min Data)
#define AVGPI				(8 + 2)				// Average CV(Reject Max & Min Data)
#define AVGAX				(4 + 2)				// Aux (Reject Max & Min Data)

#define ADSEL_PIH 			(0)
#define ADSEL_VPV 			(1)
#define ADSEL_VCV 			(2)
#define ADSEL_IREF			(3)

#define PVINP				(0U)
#define PVOUT				(1U)
#define PVBTM				(2U)

/**************************  CURRENT  *********************************************/
//#define ZEROCURR			(1000)
#define S24CURRL			(70000L)

//#define	CURRL			(60000UL)
#define CURRL				(60000L)			// TEST
#define DCHCDB				(-500L)
#define CHGCDB				(500L)

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
 //jkpark 2025-12-02 MISRA-C에서  TRUE, FALS를 권장하지 않아 enum을 선언.
typedef enum
{
	MS_ERROR   = 0,
	MS_FALSE   = 0,
	MS_DISABLE = 0,
    MS_SUCCESS = 1,
	MS_TRUE    = 1,
	MS_ENABLE  = 1,
    MS_E_TIMER_ELEMENT_FULL        ,
    MS_E_TIMER_UNVALUABLE_COUNT    ,
    MS_E_TIMER_UNVALUABLE_FUNCTION ,
    MS_E_TIMER_UNUSED_ID           ,    
    MS_E_MAX
} msStatus_t;

typedef enum
{
	McuAdc_PW24V_Type					= 0,
	McuAdc_PW13V_Type,
	McuAdc_PW5V0_Type,
	McuAdc_PW3V3_Type,
	MAX_McuAdc_Type
}eMcuAdc_Type;

typedef enum{
	HvAdc_CPV_Type						= 0,
	HvAdc_VPV_Type,
	HvAdc_TPV_Type,
	MAX_HvAdc_Type
}eHvAdc_Type;


#pragma pack(push,1)
typedef struct{
	u8	soh;
	u8	stx;
	u16	len;				// Data length
	u8	cmd;				// command
	u8	id;					// Id
	u8	scmd;
	u8	mode;
	s32	data[4];			// Receive data
	u16	chksum;
	u8	etx;
	u8	eot;
} Pkt_Cal_Txd;

typedef struct{
	u16 len;				// Data length
	u8	cmd;				//
	u8	id;					//
	u8	scmd;				//
	u8	mode;				//
	u8	data[16];			// Receive data
	u8	reserve[8];			// CAN Frame 8
} Pkt_Cal_Rxd;
typedef struct 
{
	u32	fbalance;			// 4 balancing flg
	u8	fid;				// 1 slvid
	u8	ffansts;			// 1 slvid
	u16	fltccom;			// 2 LTC COM OK/NG
	u32	vpack;
	s32	i_mst;				// Current(1mA Unit)
	u16	aux;				// 32

	u32	sumcv;				// 4
	u16	avgcv;				// 2 	Avg Cell Volt
	u16	maxcv;				// 2 	Max Cell Volt
	u8	maxcvn;				// 1 	Max Cell Volt Number
	u16	mincv;				// 2 	Min Cell Volt
	u8	mincvn;				// 1 	Min Cell Volt Number
	s16	diffcv;				// 2

	s16	avgth;				// 2
	s16	maxth;				// 2
	u8	maxthn;				// 1
	s16	minth;				// 2
	u8	minthn;				// 1
	s16	diffth;				// 2

	s16	temp[NTH];
	u16	cell[NCV];
} Pkt_Tray;


typedef struct
{
	u32	fdet;				// 4
	u16	falm;				// 2
	u8	fcomsts;			// 1
	u8	fcomcmd;			// 1
	u8	frlyctr;			// 1	relay control
	u16	fbmssts;			// 2
	u8 	frlysts;			// 1	relay status
	u32	fcomslv;			// 4    Master  Slave

	u32	pv[4];				//
	s32	pi[6];				// 0 : chg low, 1 : chg hi, 2 : dch low, 3 : dch hi, 4 : use, 5 : pi1sec

	u32	tot_chg_pw;			//
	u32	tot_dch_pw;			//
	u32	fullcap;			// (4) Cap
	u32	ratefullcap;		// (4) Cap

	u16	dlyvsoc;

	u32	sumcv;				// 4 	Avg Cell Volt
	u16	avgcv;				// 2 	Avg Cell Volt
	u16	maxcv;				// 2 	Max Cell Volt
	u8	maxcvtrayn;			// 1 	Max Cell Volt Number
	u16	maxcvn;				// 1 	Max Cell Volt Number
	u16	mincv;				// 2 	Min Cell Volt
	u8	mincvtrayn;			// 1 	Min Cell Volt Number
	u16	mincvn;				// 1 	Min Cell Volt Number
	s16	diffcv;				// 2

	s16	avgth;				// 2
	s16	maxth;				// 2
	u8	maxthtrayn;			// 1
	u8	maxthn;				// 1
	s16	minth;				// 2
	u8	minthtrayn;			// 1
	u8	minthn;				// 1
	s16	diffth;				// 2

	u32	bmsontime;			// 4 BMS On Time
	u32	datacap;			// 4 Cap
	u32	cyclecap;			// 4 Full Cap Or One Charge Cap
	u16	cyccnt;				// 2 cycle count
	u16	errcnt;				// 2 Error Data Cnt
	u16	norcnt;				// 2 Normal Data Cnt

	u16	aux;				// 16
	u16	soc;				// state of charge
	u16	soh;				// state of health
	
	u16	sr_num;
	u8	rtc[7];				// Local Time(RTC)
	u8 	Mf_year;
	u8 	Mf_month;
	u8 	Mf_day;

	u8	hostip[4];
	u16	balontime[NCV];
	Pkt_Tray TrayPkt[1];

	u8	hvbat_on;
	u8	charge_on;

	
} Pkt_Rack;



typedef struct{
	BOOL bInitProc;
	BOOL bAdbmsProc_Cv[NSPI];
	BOOL bAdbmsProc_Th[NSPI];
	BOOL bAdbmsProc_Bal;
	BOOL bDataProc;
	BOOL bSocProc;
	BOOL bStateProc;
	BOOL bMeasProc;
	BOOL bCtlProc;
	BOOL bMvbProc;
	BOOL b10ms_Task;
	BOOL bCanProc;
	BOOL bSysProc;
	BOOL bTimeProc;
	BOOL bMnProc;

	u16 u16SysTick;
	u16 u16CanTimer;
}sSystemTick;

#ifdef ENMN

typedef struct 
{
	BOOL mn_start;
	s32	mn_set_curr;	

	u16	mn_type;

	u16	mn_set_det;	//0 ~ 32 det
	u8	mn_set_msckey;
	u16	mn_set_soc;
	u8	mn_set_clr_save_det;
	u8	mn_set_clr_read_det;
	u8	mn_set_rlysts;
	
	u16	mn_set_cv;
	u16	mn_set_cvpos;
	s16	mn_set_th;
	u8	mn_set_thpos;

	u8	mn_set_bal_start;
	u8	mn_set_bal_slv_pos;
	u8	mn_set_bal_spi_pos;
	u8	mn_set_bal_flag[NSPI][NSLV];

	u8	mn_remain;

	u16	mn_cv[NCV];
	s16	mn_th[NTH];

	u8	mn_set_rtc_start;
	u8 	mn_set_rtc[7];

	u16	mn_set_pv_in;
	u16	mn_set_pv_ou;
	u16	mn_set_pv_bt;

	u8	mn_btms_cmd;
	u8	mn_btms_subcmd;

	s32	offset_cv;
	s32	offset_th;
	s32	offset_curr;

	u16	offset_pv_in;
	u16	offset_pv_ou;
	u16	offset_pv_bt;


	BOOL bset_cal;
	u8	mn_calpos;
	u32	mn_ref_data;
	u32	mn_cal_data;
	u32	mn_set_caldata;
	float mn_cal_ratio;
	float mn_cal_offset;
}sMn;
#endif
#pragma pack(pop)

extern s16 	DataTh[NSTH];
extern s32 	DataPIH;				// Current High Data
extern s32 	DataPIL;				// Current Low Data
extern u32	DataPvDet[2];					// Test Set PV Value


extern float	RatPI[4];														// charge low, charge high, discharge low, discharge high
extern float	GabPI[4];														// pack in, pack out discharge, pack out charge, pack volt reserved
extern float	RatPV[4];														// charge low, charge high, discharge low, discharge high
extern float	GabPV[4];														// pack in, pack out discharge, pack out charge, pack volt reserved
extern float	RatAX;
extern float	GabAX;



/* Exported function prototypes --------------------------------------------------------------------*/



#endif //__MS_CONFIG_H__

