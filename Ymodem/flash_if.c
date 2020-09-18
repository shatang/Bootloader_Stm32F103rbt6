#include "flash_if.h"



static uint32_t GetSize_Sector(uint32_t Size)
{
    uint32_t pagenumber = 0x0;

    if ((Size % PAGE_SIZE) != 0)
    {
        pagenumber = (Size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = Size / PAGE_SIZE;
    }
    return pagenumber;
}

/********************************************************************************
* 函数名 :  InternalFLASH_Init
* 功  能 :  stm32的内部flash初始化(解锁)
* 说  明 :  none
* 入  参 :  none
* 返  回 :  none
* 设  计 :  Shatang                   时  间 :  2020.06.18
* 修  改 :  none                      时  间 :  none
********************************************************************************/
void FLASH_If_Init(void)
{ 
    FLASH_Unlock();

//    /* Clear pending flags (if any) */
//    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
}
/********************************************************************************
* 函数名 : FLASH_If_Finish
* 功  能 : stm32的内部flash关闭(上锁)
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.07.29
* 修  改 : none                       时  间 : none
********************************************************************************/
void FLASH_If_Finish(void)
{
//    /* Clear pending flags (if any) */
//    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
//                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);
    
    FLASH_Lock();
}
/********************************************************************************
* 函数名 :  FLASH_If_Erase
* 功  能 :  stm32的内部flash擦除
* 说  明 :  只针对APP文件地址的存储内容进行擦除
* 入  参 :  none
* 返  回 :  none
* 设  计 :  Shatang                   时  间 :  2020.06.18
* 修  改 :  none                      时  间 :  none
********************************************************************************/
uint32_t FLASH_If_Erase(uint32_t Flash_StartAddr)
{
    uint32_t i = 0;
    uint32_t UserSize_Sector = APPLICATION_END_ADDRESS - Flash_StartAddr;
    uint32_t PageNum = GetSize_Sector(UserSize_Sector);
    FLASH_Status FLASHStatus = FLASH_COMPLETE;
    
    for(i = 0; (i < PageNum) && (FLASHStatus == FLASH_COMPLETE); i++)
    {
        FLASHStatus = FLASH_ErasePage(Flash_StartAddr + (PAGE_SIZE * i));
    }
    
    return (0);
}
/********************************************************************************
* 函数名 :  FLASH_If_Write
* 功  能 :  stm32的内部flash写操作
* 说  明 :  none
* 入  参 :  none
* 返  回 :  none
* 设  计 :  Shatang                   时  间 :  2020.06.18
* 修  改 :  none                      时  间 :  none
********************************************************************************/
uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint32_t DataLength)
{
    uint32_t i = 0;

    for (i = 0; (i < DataLength) && (*FlashAddress <= (APPLICATION_END_ADDRESS-4)); i++)
    {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
           be done by word */ 
        if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
        {
            /* Check the written value */
            if (*(uint32_t*)*FlashAddress != *(uint32_t*)(Data+i))
            {
                /* Flash content doesn't match SRAM content */
                return(2);
            }
            /* Increment FLASH destination address */
            *FlashAddress += 4;
        }
        else
        {
            /* Error occurred while writing data in Flash memory */
            return (1);
        }
    }

    return (0);
}


//待定增加
//uint16_t FLASH_If_GetWriteProtectionStatus(void);
//uint32_t FLASH_If_DisableWriteProtection(void);
