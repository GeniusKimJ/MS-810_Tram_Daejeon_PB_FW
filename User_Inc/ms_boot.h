/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(boot)                  							 					*
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-12-22                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_boot.c
\author             KKD
\date               2025-12-22 
\brief              모니터 동작을 위한 코드.
*********************************************
* History:
* 2025-12-22     v0.01    KKD    Create
*==================================================================================================*/
#ifndef MS_BOOT_H
#define MS_BOOT_H

/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_can.h"
#include "ms_flash.h"
#include "ms_uart.h"





/* Exported define ---------------------------------------------------------------------------------*/
#define	FRSTMODE	0xFF
#define	BOOTMODE	0xBBBBBBBBUL
#define	BOOTPASS	0xAA
#define	BOOTADDR	0x0803F800
#define	APPLMODE	0xAA

#define	RXC_DFR0	0x230U                                                      // Data Frame 0
#define	RXC_DFR1	0x231U                                                      // Data Frame 1
#define	RXC_DFR2	0x232U                                                      // Data Frame 2
#define	RXC_DFR3	0x233U                                                      // Data Frame 3
#define	RXC_DFR4	0x234U                                                      // Data Frame 4
#define	RXC_DFR5	0x235U                                                      // Data Frame 5
#define	RXC_DFR6	0x236U                                                      // Data Frame 6
#define	RXC_DFR7	0x237U                                                      // Data Frame 7
#define	RXC_DFR8	0x238U                                                      // Data Frame 8

#define	RXC_BOOT	0x260U                                                      // Boot
#define	RXC_START	0x261U                                                      // Start
#define	RXC_END		0x263U                                                      // Request Slave Data by Master


#define	NUM_DFR		(RXC_DFR8-RXC_DFR0+1)
#define	VF_DFR		0x01FF                                                      // Data Frame -> 9ea
#define	NRXFRAME	(8+1)                                                       // Request Slave Data by Master



#define	ACK_BOOT	0x270U                                                      // Request Slave Data by Master
#define	ACK_START	0x271U                                                      // Request Slave Data by Master
#define	ACK_DATA	0x272U                                                      // Request Slave Data by Master
#define	ACK_END		0x273U                                                      // Request Slave Data by Master
#define	REQ_RTY		0x274U                                                      // Request Slave Data by Master
#define	ACK_OKNG	0x275U                                                      // Boot Ok/Ng

//#define	EEPROM64U     64UL                                                  // Eeprom Page bytes
//#define	FLASH64U    64UL                                                    // Flash Page bytes
#define	EEPVFY51      51                                                            // Eeprom Verify Reserved

#define	ARRAY_MAX	60


#define APP_START_ADDRESS     (0x2000UL)                                        // APP START Address

//CAN Bootloader Write Error
#define	ERXDLYM4		(1<<4)                                                  // PC Comm. Time Out
#define	ENUMPAG3		(1<<3)                                                  // Page Number
#define	ECHKSUM2		(1<<2)                                                  // Check Sum
#define	EEEPWRT1		(1<<1)                                                  // EEPROM Verify Error
#define	EADDRES0		(0x01)                                                  // Application Address Error

#define 	NREVPACK15		15
#define 	NREVCELL15		15
#define 	NREVDATA55		55
#define 	NREVBOOT50		50

#define	ENTER_CANBOOT		0x01U
#define	ENTER_UARTBOOT		0x02U



/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
#pragma pack(push,1)
typedef struct {
	u32	canid;                                                      // CAN ID
	u32	addr;                                                       // Data address
	u8	last;                                                       // reserved
	u16	pagechksum;                                                 // Page CheckSum
	u8	rxdat[512];                                 // Data
	//u8	rxdat[FLASH_SECTOR_SIZE];                                 // Data
} Pkt_Mem_Boot;

//typedef struct __attribute__((__packed__)) {
typedef struct {
	u32	boot;
	u32	totalchksum;
	u32	allpagecnt;
	u32	totaljudge;
} Pkt_Flash_Info;

/* Exported variables ------------------------------------------------------------------------------*/
extern u8		fBoot_mode;
extern u32		GetAllPageCnt;
extern u32		CalAllPageCnt;
extern Pkt_Mem_Boot		PktMemBoot[2];
extern Pkt_Flash_Info	PktFlashInfo[2];
/* Exported function prototypes --------------------------------------------------------------------*/


u8 Boot_FlashWritePage64(void);
void Boot_Function(void);


#endif //__MS_BOOT_H__
