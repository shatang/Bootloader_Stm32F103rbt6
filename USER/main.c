#include "stm32f10x.h"
#include "download.h"
#include "flash_if.h"


typedef void (*pFunction)(void);
/********************************************************************************
* 函数名 : IAP_LoadApp
* 功  能 : Bootloader跳转至APP
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.23
* 修  改 : none                       时  间 : none
********************************************************************************/
void IAP_LoadApp(uint32_t appxaddr)
{
    pFunction JumpApp;
    
	if(((*(__IO uint32_t*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		JumpApp=(pFunction)*(__IO uint32_t*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		__set_MSP(*(__IO uint32_t*)appxaddr);				    //初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		JumpApp();								                //跳转到APP.
	}
}

int main(void)
{
    uint8_t i = 0;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
    IAP_Init();
    FLASH_If_Init();

    if(Serial_CheckDownloadChar())
    {
        SerialDownload(Serial_GetDownloadChannel());
    }

    FLASH_If_Finish();
    IAP_Close();
    IAP_LoadApp(APPLICATION_START_ADDRESS); //程序跳转
    
    while(1){}
}

