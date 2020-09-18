/* 包含头文件 *****************************************************************/
#include "download.h"
#include "common.h"

#include "Rs485_Uart1_Driver.h"
#include "Timer_Driver.h"


/* 变量声明 ------------------------------------------------------------------*/
uint8_t tab_1024[1024] = {0};


/********************************************************************************
* 函数名 : IAP_Init
* 功  能 : IAP功能所需底层驱动初始化
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.18
* 修  改 : none                       时  间 : none
********************************************************************************/
void IAP_Init(void)
{
	TIM3_PortInit(9, 7199);//1ms基准
    Uart1_Init(38400,USART_WordLength_8b,USART_Parity_No,USART_StopBits_1);//38400，8-n-1
}
/********************************************************************************
* 函数名 : IAP_Close
* 功  能 : IAP功能关闭
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.18
* 修  改 : none                       时  间 : none
********************************************************************************/
void IAP_Close(void)
{
    TIM_Cmd(TIM3, DISABLE);
    USART_Cmd(USART1, DISABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART1, DISABLE);
}



//static const char cStartTips[] 			= "\r\n\r\nready to upgrade\r\n准备升级\r\n\r\n";
//static const char cFinishThreadTips[] 	= "\r\n\r\nupgrade successfully ,please  re-power the indicator\r\n升级成功，建议您重新上电\r\n\r\n";

//static const char cErrorOverFlow[]		= "\r\nInsufficient storage space, file unavailable, upgrade failed\r\n升级文件超过存储空间，升级失败\r\n";
//static const char cErrorVerification[] 	= "\r\nverification failed, upgrade unsuccessful\r\n校验失败，升级不成功\r\n";
/********************************************************************************
* 函数名 : SerialDownload
* 功  能 : 通过串口接收一个文件
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void SerialDownload(uint8_t fu8_Channal)
{
    uint8_t Number[10] = "          ";
    int32_t Size = 0;

//    SerialPutString(fu8_Channal, cStartTips);
    SerialPutString(fu8_Channal, "\r\n\r\nWaiting for the file to be sent ... (press 'a' to abort)\r\n\r\n");
    Size = Ymodem_Receive(fu8_Channal, &tab_1024[0]);
    if (Size > 0)
    {
//        SerialPutString(fu8_Channal,cFinishThreadTips);
        SerialPutString(fu8_Channal,"\r\n\r\nUpdate Completed Successfully!");        
        SerialPutString(fu8_Channal, "\r\n\r\n Programming Completed Successfully!\r\n--------------------------------\r\n Name: ");
        SerialPutString(fu8_Channal, file_name);
        Int2Str(Number, Size);
        SerialPutString(fu8_Channal,"\r\n Size: ");
        SerialPutString(fu8_Channal, Number);
        SerialPutString(fu8_Channal, " Bytes\r\n");
        SerialPutString(fu8_Channal, "--------------------------------\r\n\r\n");
    }
    else if (Size == -1)
//        SerialPutString(fu8_Channal,cErrorOverFlow);
        SerialPutString(fu8_Channal, "\n\n\rThe image size is higher than the allowed space memory!\n\r");
    else if (Size == -2)
//        SerialPutString(fu8_Channal,cErrorVerification);
        SerialPutString(fu8_Channal, "\n\n\rVerification failed!\n\r");
    else if (Size == -3)
        SerialPutString(fu8_Channal, "\r\n\nAborted by user.\n\r");
    else
        SerialPutString(fu8_Channal, "\n\rFailed to receive the file!\n\r");
}

#define CHECK_VALID_CHAR        (' ')
#define CHECK_CHAR_LIMIT_LEN    (15)
#define TIME_DELAY_COUNT        (800)

static uint8_t Uart_Channel = 0;
/********************************************************************************
* 函数名 : Serial_GetDownloadChannel
* 功  能 : 获取当前串口下载文件的通道
* 说  明 : none
* 入  参 : none
* 返  回 : 当前串口下载文件的通道
* 设  计 : Shatang                    时  间 : 2020.06.23
* 修  改 : none                       时  间 : none
********************************************************************************/
uint8_t Serial_GetDownloadChannel(void)
{
    return Uart_Channel;
}
/********************************************************************************
* 函数名 : Serial_CheckDownloadChar
* 功  能 : 检查串口是否被按下，且发送空格字符' '
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.23
* 修  改 : none                       时  间 : none
********************************************************************************/
uint8_t Serial_CheckDownloadChar(void)
{
    uint8_t lu8_RxChar[Uart_NumMax] = {0};
    uint8_t lu8_RxLen[Uart_NumMax] = {0};
    uint32_t TickCache = 0;//使用while循环，故用局部变量
    
    TickCache = SysTimeGet();
    while(1)
    {      
        //串口2
        if(SerialKeyPressed(Uart1_485, &lu8_RxChar[Uart1_485]))
        {
            if (CHECK_VALID_CHAR == lu8_RxChar[Uart1_485])
            {
                lu8_RxChar[Uart1_485] = 0;
                if (++lu8_RxLen[Uart1_485] > CHECK_CHAR_LIMIT_LEN)
                {
                    lu8_RxLen[Uart1_485] = 0;
                    Uart_Channel = Uart1_485;
                    return 1;
                }
            }
            else
                lu8_RxLen[Uart1_485] = 0;
        }

        //检测超时
        if(SysTimeGet() - TickCache > TIME_DELAY_COUNT)return 0;
    }
    return 0;
}
