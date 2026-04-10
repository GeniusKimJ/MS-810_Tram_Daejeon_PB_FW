/**================================================================================================*
*       Header File                                                                                *
*==================================================================================================*
*       [Project]    : ms_810P(flash control)					                                          *
*       [Version]    : 1.0                                                                         *
*       [Start]      : 2025-12-22                                                               	*
*       [Inventor]   : www.misum.co.kr                                                             *
*       Copyright(C) 2025 Misum Systech Co.,Ltd. All Rights Reserved.                              *
*==================================================================================================*
** For Doxygen ******************************
\file               ms_flash.h
\author             KKD
\date               2025-12-22 
\brief              battery thermal management
*********************************************
* History:
* 2025-12-22     v0.01    KKD    Create
*==================================================================================================*/
/* Includes ---------------------------------------------------------------------------------------*/
#include "ms_flash.h"

/* Exported define ---------------------------------------------------------------------------------*/
/* Exported macro ----------------------------------------------------------------------------------*/
/* Exported typedef --------------------------------------------------------------------------------*/
/* Exported variables ------------------------------------------------------------------------------*/
u32 CalTotalChkSum;
u32 GetTotalChkSum;

/* Exported function prototypes --------------------------------------------------------------------*/
static u32 Flash_Stm32f407_GetSector(u32 address)
{ 
    if(address < 0x08004000UL) {return FLASH_SECTOR_0;}      // 16 KB
    if(address < 0x08008000UL) {return FLASH_SECTOR_1;}
    if(address < 0x0800C000UL) {return FLASH_SECTOR_2;}
    if(address < 0x08010000UL) {return FLASH_SECTOR_3;}
    if(address < 0x08020000UL) {return FLASH_SECTOR_4;}      // 64 KB
    if(address < 0x08040000UL) {return FLASH_SECTOR_5;}      // 128 KB
    if(address < 0x08060000UL) {return FLASH_SECTOR_6;}
    if(address < 0x08080000UL) {return FLASH_SECTOR_7;}
    if(address < 0x080A0000UL) {return FLASH_SECTOR_8;}
    if(address < 0x080C0000UL) {return FLASH_SECTOR_9;}
    if(address < 0x080E0000UL) {return FLASH_SECTOR_10;}
    return FLASH_SECTOR_11;
}

HAL_StatusTypeDef Flash_Stm32f407_SectorEraseByAddr(u32 any_addr_in_sector)
{
    FLASH_EraseInitTypeDef  erase_init;
    HAL_StatusTypeDef       status;
    u32                     SectorError;
    u32                     sector;
	//u32	*chkcum				= &GetTotalChkSum;

    sector = Flash_Stm32f407_GetSector(any_addr_in_sector);

    HAL_FLASH_Unlock();

    erase_init.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase_init.VoltageRange	= FLASH_VOLTAGE_RANGE_3;   // 2.7V~3.6V
    erase_init.Sector       = sector;
    erase_init.NbSectors    = 1;

    status = HAL_FLASHEx_Erase(&erase_init, (uint32_t *)&SectorError);

    HAL_FLASH_Lock();

    return status;
}



HAL_StatusTypeDef Flash_Stm32f407_Write(u32 address, const void *pdata, u32 word_cnt)
{
    HAL_StatusTypeDef status = HAL_OK;
    const u32 *pword = (const u32 *)pdata;

    HAL_FLASH_Unlock();

    for(u32 i = 0UL; i < word_cnt; i++) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, (u64)pword[i]);
        if(status != HAL_OK) {
            break;
        }
        address += 4UL;
    }

    HAL_FLASH_Lock();
    return status;
}

HAL_StatusTypeDef Flash_Stm32F407_Erase(u32 page_adres, u8 nb_pages) {
	FLASH_EraseInitTypeDef	FLASH_EraseInit;
	HAL_StatusTypeDef		status_typedef;
	u32						PageError;

	page_adres &= (u32)(FLASH_SECTOR1_ADDR-(u32)1);							// 0x0803F800

	HAL_FLASH_Unlock();

	FLASH_EraseInit.TypeErase	= FLASH_SECTOR_SIZE;					// 0x00U, Pages erase only
	//FLASH_EraseInit.Banks		= FLASH_BANK_1;								// Select banks to erase when Mass erase is enabled.
	FLASH_EraseInit.Sector 		= page_adres;								// PageAdress Min_Data = 0x08000000 and Max_Data = FLASH_BANKx_END
	FLASH_EraseInit.NbSectors	= nb_pages;									// NbPages: Number of pagess to be erased.

	status_typedef = HAL_FLASHEx_Erase(&FLASH_EraseInit, (uint32_t *)&PageError);		// Duration of time-22ms

	HAL_FLASH_Lock();

	return status_typedef;
}

