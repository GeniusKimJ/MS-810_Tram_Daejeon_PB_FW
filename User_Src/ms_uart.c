/**================================================================================================*
*       Source File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(Cell/Temp sensor - spi)                                             *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2026-01-12                                                                  *
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2026 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_uart.c
\author             jkpark
\date               2026-01-12 
\brief              Uart 관련
*********************************************
* History:
* 2026-01-12     v0.01    jkpark    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_Uart.h"
#include "ms_Util.h"  
/* Private define ---------------------------------------------------------------------------------*/
/* Private macro ----------------------------------------------------------------------------------*/
/* Private typedef --------------------------------------------------------------------------------*/
/* Private variables ------------------------------------------------------------------------------*/
u8	Tx1UartBuf[MAX_TX_BUFF];
u8	Rx1UartBuf[sizeof(Pkt_Uart1_Rxd)];
u8	UartRx1Cnt = 0;
u8 	Rx1DMABuf[sizeof(Pkt_Uart1_Rxd)];
//u8 		Rx1Debug[sizeof(Pkt_Uart1_Rxd)];
u8 	fRcvUart1Boot = FALSE;

//u8			dbgcnt = 0U;
//int16_t			dbgT[5] = {25};

//u8	g_u8Uart1DmaDataChkOK = 0;


/*prototypes --------------------------------------------------------------------*/


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){					/* min-10us */

	if(huart->Instance == USART1) {
	}
	if(huart->Instance == USART2) {
	}
	if(huart->Instance == UART4) {
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

	if(huart->Instance == USART1) {
	}

	if(huart->Instance == USART2) {			// 8.8us
		//CntRx2TxDisable = 2;													// 2ms
		//Delay_us(26U);														// 38400bps @ 1bit(26us)
		//enusart2_tx(TXDI);
	}

	if(huart->Instance == UART4) {
	}
}


//   Rx1DMABuf[]  = 19 00 03 04 XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX 01 02 08 00 11 00 00 00
//=> Rx1UartBuf[] = 01 02 08 00 11 00 00 00 19 00 03 04 XX XX XX XX ...
void Uart_GetRx1DmatoUartBuf(void) {
  static u16 prev1Position = 0, current1Position = 0;
  static u16 Com1_Receiving = 0, Com1_RxEnd = 0;
  static u16 Com1_RxSize;
  u16 New1Position = (u16)((huart1.RxXferSize)-(huart1.hdmarx->Instance->NDTR));
  u16 m;
  u32 error;

  error = HAL_UART_GetError(&huart1);                   // Noise error, Frame error
  if(error != HAL_UART_ERROR_NONE) {
    ///tp2_toggle();
    HAL_UART_Receive_DMA(&huart1, Rx1DMABuf, sizeof(Pkt_Uart1_Rxd));  // Noise error, Frame error Clear, ????
    ///huart1.ErrorCode = HAL_UART_ERROR_NONE;
  }    

  if(prev1Position != New1Position) {
    Com1_Receiving = 1U;
    prev1Position = New1Position;
  } else {
    if(Com1_Receiving == 1U) {
      Com1_RxEnd = 1U;
    }
  }

  if(Com1_RxEnd == 1U) {
    Com1_Receiving = 0U;
    Com1_RxEnd = 0U;
    if(current1Position != New1Position) {
      if(New1Position > current1Position) {
        Com1_RxSize = New1Position - current1Position;
        for(m = 0; m < Com1_RxSize; m++) {
           Rx1UartBuf[m] = Rx1DMABuf[current1Position+m];
        }
      } else {
        Com1_RxSize = (u16)sizeof(Rx1UartBuf) - current1Position;
        for(m = 0; m < Com1_RxSize; m++) {
          Rx1UartBuf[m] = Rx1DMABuf[current1Position+m];
        }
        if(New1Position > 0U) {
          for(m = 0; m < New1Position; m++) {
            Rx1UartBuf[Com1_RxSize+m] = Rx1DMABuf[m];
          }
          //Com1_RxSize += New1Position;
        }
      }
      current1Position = New1Position;
    }
  }
}

//=> Rx1UartBuf[] = 01 02 08 00 11 00 00 00 19 00 03 04 XX XX XX XX ...
//=> Rx1UartBuf[] = E1 01 01 01 02 08 00 11 00 00 00 19 00 03 04 XX XX XX XX ...
u8 Uart_ChkUartRx1Dma(void) {
  static u16 Len1Rxd, Rx1ChkSum;
  u16      calchksum;
  u16       ix;
  
  u16       m;
	u8       chk_packet = 0U;
  /// Check Noise Filter
  for(ix = 0; ix < (sizeof(Rx1UartBuf) - 12U); ix++) {          // min packet 12bytes, 32bytes - 12bytes = 20bytes
    if((Rx1UartBuf[ix] == SOH) && (Rx1UartBuf[1U+ix] == STX)) {
      //tp3_toggle();1\0
      // *((uint8_t *)&Len1Rxd+0U) = Rx1UartBuf[2U+ix];
      // *((uint8_t *)&Len1Rxd+1U) = Rx1UartBuf[3U+ix];
      Len1Rxd = Rx1UartBuf[2U+ix];
      Len1Rxd |= (u16)((u16)Rx1UartBuf[3U+ix] << 8);

      if(Len1Rxd < (sizeof(Rx1UartBuf) - 4U)) {           // min len 4bytes(SOH+STX+ETX+EOT)
        Rx1ChkSum = Rx1UartBuf[Len1Rxd+ix];
        Rx1ChkSum |= (u16)((u16)Rx1UartBuf[Len1Rxd+1U+ix] << 8);
        calchksum = 0U;
        for(m = 2U; m < Len1Rxd; m++) { // - crc(2)
          calchksum += Rx1UartBuf[m+ix];
        }
        if(Rx1ChkSum ==  calchksum) {
          if((Rx1UartBuf[Len1Rxd+2U+ix] == ETX) && (Rx1UartBuf[Len1Rxd+3U+ix] == EOT)) {
            /// E1 . 01 02 08 00 11 00 00 00 19 00 03 04 => 01 02 08 00 11 00 00 00 19 00 03 04
            for(m = 0U; m < (Len1Rxd+4U); m++) {        //  4bytes(SOH+STX+ETX+EOT)
              Rx1UartBuf[m] = Rx1UartBuf[ix+m];
            }
            chk_packet = 1U;
            break;
          }
        }
      }
    }
  }

  return chk_packet;
}





/**************************  End Of Source  ******************************************************/

