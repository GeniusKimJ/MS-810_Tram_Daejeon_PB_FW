/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2026-01-09                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2026 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_i2c.h
\author             jkpark
\date               2026-01-09 
\brief              i2c comm
*********************************************
* History:
* 2026-01-09     v0.01    jkpark    Create
*==================================================================================================*/
#ifndef MS_I2C_H
#define MS_I2C_H
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_config.h"

#define  HUMI_SHT3XDIS  //2026-01-09   jkpark  HUMI_SHT3XDIS센서 코드.
#ifdef HUMI_SHT3XDIS    //2026-01-09   jkpark  HUMI_SHT3XDIS센서 코드.
#include "HUMI_sht3xdis.h"
#endif //HUMI_SHT3XDIS

/* Exported define ---------------------------------------------------------------------------------*/
#define     I2C_RX_1_BUF_SIZE   500
#define     I2C_TX_1_BUF_SIZE   500
#define     I2C_RX_2_BUF_SIZE   500
#define     I2C_TX_2_BUF_SIZE   500

/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------*/
extern u8  g_RxBuf_1[I2C_RX_1_BUF_SIZE];
extern u8  g_TxBuf_1[I2C_TX_1_BUF_SIZE];
extern u8  g_RxBuf_2[I2C_RX_2_BUF_SIZE];
extern u8  g_TxBuf_2[I2C_TX_2_BUF_SIZE];

extern BufferInfoType g_BufInfoI2C_Rx1;
extern BufferInfoType g_BufInfoI2C_Tx1;
extern BufferInfoType g_BufInfoI2C_Rx2;
extern BufferInfoType g_BufInfoI2C_Tx2;

/* Exported function prototypes --------------------------------------------------------------------*/
msStatus_t I2C_Init( void );
msStatus_t I2C_Proc( void );



#endif //__MS_I2C_H__

