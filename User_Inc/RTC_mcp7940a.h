/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Real time clock)                                           			  *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-11-24                                                               		   *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               RTC_mcp7940a.h
\author             KKD
\date               2025-11-24 
\brief              Real time clock
*********************************************
* History:
* 2025-11-24     v0.01    KKD    Create
*==================================================================================================*/
#ifndef RTC_MCP7940A_H
#define RTC_MCP7940A_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"

/* Exported define ---------------------------------------------------------------------------------*/
/*
 * MCP7940N Slave Address
 * 7bit address = 0x6F
 * I2C 8bit write = 0xDE, read = 0xDF
 */
#define MCP7940_I2C_ADDR_7BIT   (u16)(0x6F)
#define SLVMCP7940              (u16)(MCP7940_I2C_ADDR_7BIT<<1)

/* MCP7940N RTC Register length (00h~06h) */
#define MCP7940_RTCDATA_LEN 	(7U)

/*
 * MCP7940N SRAM
 * 0x20 ~ 0x5F : 64 Bytes usable
 *
 * 기존 DS1307 RAMADR = 0x08 을 그대로 재사용 불가 → MCP7940N은 0x20부터 시작
 * 하지만 상위 소스 구조를 유지하기 위해 MCP7940_SRAM_ADDR 고정 정의
 */
#define MCP7940_RTCWKDAY_ADDR   	(0x03U) 
#define MCP7940_SRAM_ADDR			(0x20U)										// Time Save Address
#define MCP7940_SRAM_ACAP			(MCP7940_SRAM_ADDR+4*sizeof(long))		// Capacity Save Address
#define MCP7940_SRAM_ACCA			(MCP7940_SRAM_ACAP+4*sizeof(long))		// One Cycle Discharge Capacity
#define MCP7940_SRAM_ACNT			(MCP7940_SRAM_ACCA+4*sizeof(long))		// Cycle count

#define MCP7940_VBATEN_BIT      	(1U << 3)   // Bit 3: VBATEN
#define RAMADR					    (0x08U)								// Ram Start Address
#define MCP7940_SRAM_SIZE   		(64U)

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
/* DS1307에서 사용한 구조체 그대로 사용 (28 bytes) */
typedef struct __attribute__((__packed__)) {
    u32  time;          	// (4)
    u32  bmsontime;     	// (4)
    u32  cap;          	 	// (4)
    u32  cyclecap;      	// (4)
    u16  cyccnt;        	// (2)
    u16  errcnt;        	// (2)
    u16  norcnt;        	// (2)
    u32  tot_dch_pw;    	// (4)
    u8   reserved[1];   	// (1)
    u8   crc;           	// (1)
} sMCP7940_Ram;           		// = 28 Bytes 동일
/* Exported variables ------------------------------------------------------------------------------*/
extern u16		g_u16MinuteCount;
extern const u8	TwiCRC[256];
extern u8  		MCP7940_RTC[MCP7940_RTCDATA_LEN];
extern u8  		Back7940_RTC[MCP7940_RTCDATA_LEN];
extern u8  		fCheckRtc;

extern sMCP7940_Ram RamPkt;

/* Exported function prototypes --------------------------------------------------------------------*/
u8		Bcd2Dec(u8 val);
u8		Dec2Bcd(u8 val);
u8		RTC_ChkComm(void);

void	RTC_SetLocalTime(u8 *rtc);
void	RTC_GetLocalTime(u8 *rtc);

void	RTC_GetSocToMcp7940(void);
void	RTC_SaveSocToMcp7940(u8 *rtc);
void	RTC_Init(void);


#endif //__RTC_MCP7940A_H__

