/**
\file               MISUM_Util.h
\author             jkpark
\date               2025-11-25 
\brief              공용으로 사용하는 함수
*/
/************************ (C) COPYRIGHT 2012 Misum ***************************
* History:
* 2025-11-25  v0.01    Create
********************************************************************************/
#ifndef MISUM_UTIL_H
#define MISUM_UTIL_H



#define ENTER_CRITICAL()   u32 primask = __get_PRIMASK(); __disable_irq()      //jkpark 2025-12-02 시스템 코드를 사용하므로 "stm32f4xx_hal.h"가 필요함.
#define EXIT_CRITICAL()    __set_PRIMASK(primask)                                   //jkpark 2025-12-02 시스템 코드를 사용하므로 "stm32f4xx_hal.h"가 필요함.


/* Includes ------------------------------------------------------------------*/
//#include <stdio.h>
//#include <string.h>
#include "stm32f4xx_hal.h"
#include "ms_main.h"



/* Exported define ------------------------------------------------------------*/
/* Exported macro -------------------------------------------------------------*/

//static inline u16 BYTE_15_08(u8 x){ return (u16)((u8)(((x) >> 8U) & 0xFFU ));}
//static inline u16 BYTE_07_00(u8 x){ return (u16)((u8)((x) & 0xFFU ));}



/* Exported typedef -----------------------------------------------------------*/
typedef enum
{
    ORDER_ASC = 0,
    ORDER_DESC,
    ORDER_UNKNOWN     /* 모든 값이 동일할 때 */
} order_t;


typedef union{
    char*   charType;
    short*  shortType;
    long*   longType;
}typeConvertor;

#pragma pack(push,1)
typedef struct{
    u8   *pBuffer;
    volatile int head;     // Write index
    volatile int tail;     // Read index
    volatile int count;    // Data count
    int bufSize;
}BufferInfoType;
#pragma pack(pop)
/* Exported variables ---------------------------------------------------------*/
/* Exported function prototypes -----------------------------------------------*/
static inline u16 BYTE_15_08(u16 x){    return (u16)((x >> 8U) & 0x00FFU);}
static inline u16 BYTE_07_00(u16 x){    return (u16)(x & 0x00FFU);}

msStatus_t Util_Buffer_Init(BufferInfoType *bufInfo, u8 *buffer, int bufSize );
msStatus_t Util_Buffer_Write( BufferInfoType *bufInfo, u8 *data, int dataSize, int *usedSize );
msStatus_t Util_Buffer_Read  (BufferInfoType *bufInfo, u8 *data, int reqSize, int *readSize );
msStatus_t Util_Buffer_Check(BufferInfoType *bufInfo, int dataSize, int *remainSize);

msStatus_t Util_GetBinaryRange(const int *arr, int n, int target, int *start, int *end);

void *Util_memcpy_s( void *dest, u32 destSize, void *src, u32 copySize );
void *Util_memset( void *dest, int c, u32 n );

u16 Util_UpdateFCS(u16 nFCS, u8 *pBlock,u32 nSize);
u16 Util_UpdateChecksum(u16 checkSum, u8 *pBuffer,u32 nBytes);

#endif //__MISUM__UTIL_H__

