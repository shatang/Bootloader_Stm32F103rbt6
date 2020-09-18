/*******************************************************************************
** 文件名: 		common.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.20
** 作者: 		liupeng
** 生成日期: 	2016-03-20
** 功能:		显示主菜单。主菜单上显示一系列操作，如加载二进制文件、
                执行应用程序以及禁止写保护(如果事先Flash被写保护)。
** 相关文件:	common.h，ymodem.h
** 修改日志：	2016-03-20   创建文档
*******************************************************************************/

/* 包含头文件 *****************************************************************/
#include "common.h"
#include "ymodem.h"
#include "Rs485_Uart1_Driver.h"
/* 变量 ----------------------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint32_t BlockNbr = 0, UserMemoryMask = 0;
__IO uint32_t FlashProtection = 0;

/********************************************************************************
* 函数名 : Int2Str
* 功  能 : 整形数据转到字符串
* 说  明 : none
* 入  参 : intnum:数据，str：转换为的字符串
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void Int2Str(uint8_t* str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}
/********************************************************************************
* 函数名 : Str2Int
* 功  能 : 字符串转到数据
* 说  明 : none
* 入  参 : inputstr:需转换的字符串，intnum：转好的数据
* 返  回 : 转换结果
            1：正确
            0：错误
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else//最多10为2输入
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }
        //超过10位无效，返回0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}
/********************************************************************************
* 函数名 : SerialKeyPressed
* 功  能 : 测试超级终端是否有按键按下
* 说  明 : key:按键
* 入  参 : none
* 返  回 : 1：正确 0：错误
* 设  计 : Shatang                    时  间 : 2020.06.18
* 修  改 : none                       时  间 : none
********************************************************************************/
uint32_t SerialKeyPressed(uint8_t fu8_Channel,uint8_t *key)
{
    if(Uart1_485 == fu8_Channel)
    {
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            *key = (uint8_t)USART1->DR;
            return 1;
        }
    }
    
    return 0;
}

typedef void (*PutChar_Callback)(uint8_t);
/********************************************************************************
* 函数名 : Serial_PutChar
* 功  能 : 串口发送一个字符
* 说  明 : none
* 入  参 : fu8_Channal:串口通道
           C:需发送的字符
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void Serial_PutChar(uint8_t fu8_Channal, uint8_t c)
{
    PutChar_Callback UartSend;

    if(Uart1_485 == fu8_Channal)UartSend = Uart1_PutChar;

    UartSend(c);
}
typedef void (*PutString_Callback)(uint8_t*);
/********************************************************************************
* 函数名 : Serial_PutString
* 功  能 : 串口发送一个字符串
* 说  明 : none
* 入  参 : *s:需发送的字符串
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void Serial_PutString(uint8_t fu8_Channal, uint8_t *s)
{
    PutString_Callback UartSend;
    
    if(Uart1_485 == fu8_Channal)UartSend = Uart1_PutString;

    UartSend(s);
}
