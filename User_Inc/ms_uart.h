/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2026-01-12                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2026 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_uart.h
\author             jkpark
\date               2026-01-12 
\brief              Uart관련 헤더.
*********************************************
* History:
* 2026-01-12     v0.01    jkpark    Create
*==================================================================================================*/
#ifndef MS_UART_H
#define MS_UART_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_main.h"

/* Exported define ---------------------------------------------------------------------------------*/
#define MAX_TX_BUFF			(u16)(2000)


#define SOH					0x01U		// Start of header
#define STX					0x02U		// Start of text
#define ETX					0x03U		// End of text
#define EOT					0x04U		// End of transmission
//#define CR				'\r'		// 0x0D Carriage return
#define LF					'\n'		// 0x0A Line feet
#define ACK					0x06U		// Acknowledge
#define NAK					0x15U		// Not Acknowledge

#define CMD_BOOT			0x0BU		// Boot Mode
#define CMD_REQ				0x10U		// Request Slave Data by Master
#define CMD_BMU				0x11U		// Request Master Data by Host
#define CMD_CMU				0x12U		// Request Slave Data by Host
#define CMD_RTC				0x13U		// RTC Local Time Setting
#define CMD_CAL				0x17U		// Calibration BMS
#define CMD_ESC				0x18U		// Escape test routine
#define CMD_ORDER			0x19U		// Manual Mode : mklee180906
#define CMD_MNM				0x1BU		// Manual Mode
#define CMD_EEP				0x1EU		// EEPROM
#define CMD_DETECT  0x1DU				// Det_Occur Setting


#define SCMD_BOOT_START		0xB1U		// Boot Start
#define SCMD_BOOT_SEND		0xB2U		// Boot Send
#define SCMD_BOOT_END		0xB3U		// Boot End
#define SCMD_BOOT_RTY		0xB4U		// Boot Retry
/*
#define	NCV					(SNCV*NSLV)	// Total Cell Num
#define	NTH					(SNTH*NSLV)	// Total Temperature Num
#define	NTHM				(1U)		// V102 TEMP ���� ����
#define	NSLV				(5U)		// Slave Count

#define SNCV				(5U)		// Slave Cell V Count
#define SNTH				(1U)		// Slave Cell T Count
*/
#define NRACK				(1)
#define NMAST				(NRACK + 1)

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
typedef struct __attribute__((__packed__)) {
	uint8_t		soh;					// 1
	uint8_t		stx;					// 1	
	uint16_t 	len;					// 2 Data length
	uint8_t		cmd;					// 1
	uint8_t		id;						// 1
	uint8_t		scmd;					// 1
	uint8_t		mode;					// 1 8 
	uint8_t		data[4+4+64+64];		// 136 144 Receive data
	uint8_t		reserve[8];				// 8 152 ChkSum(2),ETX(1), EOT(1)
} Pkt_Uart1_Rxd;


typedef struct __attribute__((__packed__)) {
	uint8_t		ack;
	uint8_t		stx;
	uint16_t	len;					// Data length
	uint8_t		cmd;					// command
	uint8_t		id;						// Id
	uint8_t		scmd;
	uint8_t		mode;
	uint16_t	chksum;
	uint8_t		etx;
	uint8_t		eot;
} Pkt_Uart1_Txd;

typedef struct __attribute__((__packed__)) {
//typedef struct {

	uint8_t		ack;
	uint8_t		stx;
	uint16_t 	len;					// Data length
	uint8_t		cmd;					// command
	uint8_t		id;						// Id
	uint8_t		scmd;
	uint8_t		mode;
	uint32_t	total_page_cnt;
	uint32_t 	total_checksum;
	uint16_t 	fw_ver;
	uint8_t 	mfr_year;
	uint8_t		mfr_month;
	uint8_t		mfr_day;
	uint16_t 	mfr_sn;
	uint16_t 	chksum;
	uint8_t		etx;
	uint8_t		eot;
} Pkt_Uart1_TxBootEnd;


typedef struct __attribute__((__packed__)) {
	uint8_t		ack;					// 1
	uint8_t		stx;					// 2
	uint16_t	len;					// 4 Data length
	uint8_t		cmd;					// 5 command,
	uint8_t		id;						// 6 Id
	uint8_t		scmd;					// 7
	uint8_t		mode;					// 8

	uint8_t		flagbmu[8];				//	9~16

	int32_t		i_mst;					// 20 Current(1mA Unit)
	int16_t		temp[NTH];				// 22	v
	uint32_t	packin;					// 26	v
	uint32_t	packout;				// 30	

	uint16_t	aux;					// 32	v
	uint16_t	soc;					// 34 state of charge 	v
	uint8_t 	Mf_year;				// 35
	uint8_t 	Mf_month;				// 36
	uint8_t 	Mf_day;					// 37
	uint16_t	sr_num;					// 39
	int8_t		rtc[7];					// 40~46  Local Time(RTC)

	uint16_t	chksum;					// 48
	uint8_t		etx;					// 49
	uint8_t		eot;					// 50
} Pkt_Uart1_Bmu;

typedef struct __attribute__((__packed__)) { 		//288byte
	uint8_t		ack;
	uint8_t		stx;
	uint16_t	len;					// Data length
	uint8_t		cmd;					// command
	uint8_t		id;						// Id
	uint8_t		scmd;
	uint8_t		mode;

	uint16_t	reserve[2];				// reserve 4 byte

	uint32_t	fbalance;				// 4 balancing flg
	uint16_t	fid;					// 2 slvid
	uint8_t		fafecom;				// 1 slvid
	uint8_t		fslvsts;				// 1 LTC COM OK/NG

	int32_t		i_mst;					// Current(1mA Unit)
	int16_t		temp[NTH];
	uint32_t	vpack;
	uint16_t	cell[NCV];
	uint16_t	cellir[NCV];

	uint16_t	chksum;
	uint8_t		etx;
	uint8_t		eot;
} Pkt_Uart1_Cmu;

typedef struct __attribute__((__packed__)) {
	uint8_t		ack;
	uint8_t		stx;
	uint16_t	len;					// Data length
	uint8_t		cmd;					// command
	uint8_t		id;						// Id
	uint8_t		scmd;
	uint8_t		mode;

	uint8_t	eepdata[64];				// reserve 4 byte

	uint16_t	chksum;
	uint8_t		etx;
	uint8_t		eot;
} Pkt_Uart1_Eep;

typedef struct __attribute__((__packed__)) {
	uint8_t		ack;
	uint8_t		stx;
	uint16_t	len;					// Data length
	uint8_t		cmd;					// command
	uint8_t		id;						// Id
	uint8_t		scmd;
	uint8_t		mode;
	int32_t		data[4];				// Receive data
	uint16_t	chksum;
	uint8_t		etx;
	uint8_t		eot;
} Pkt_Uart1_Cal;

typedef struct __attribute__((__packed__)) {
	uint8_t		ack;
	uint8_t		stx;
	uint16_t	len;					// Data length
	uint8_t		cmd;					// command
	uint8_t		id;						// Id
	uint8_t		scmd;
	uint8_t		mode;
	int32_t		data[4];				// Receive data
	uint16_t	chksum;
	uint8_t	etx;
	uint8_t	eot;
} Pkt_Uart1_Mnl;						// manual mode
typedef struct __attribute__((__packed__)) {
	uint8_t		ack;
	uint8_t		stx;
	uint16_t	len;				// Data length
	uint8_t		cmd;				// command
	uint8_t		id;					// Id
	uint8_t		scmd;
	uint8_t		mode;
	uint16_t	chksum;
	uint8_t		etx;
	uint8_t		eot;
} Pkt_Uart1_Det;

/* Exported variables ------------------------------------------------------------------------------*/
extern u8	Tx1UartBuf[MAX_TX_BUFF];
extern u8	Rx1UartBuf[sizeof(Pkt_Uart1_Rxd)];
extern u8	UartRx1Cnt;
extern u8 	Rx1DMABuf[sizeof(Pkt_Uart1_Rxd)];
extern u8 	fRcvUart1Boot;
//extern u8	fMnbalstart;
//extern u8	fMnrlystart;
extern u8	mnrly;

/* Exported function prototypes --------------------------------------------------------------------*/
void 	Uart_GetRx1DmatoUartBuf(void);
u8		Uart_ChkUartRx1Dma(void);




#endif //__MS_UART_H__

