/*******************************************************************************
** 文件名: 		common.h
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 5.20
** 作者: 		liupeng
** 生成日期: 	2016-03-20
** 功能:		common文件的头文件声明
** 相关文件:	string.h，stdio.h，stm32f10x.h，ymodem.h
** 修改日志：	2016-03-20   创建文档
*******************************************************************************/

/* 防止重定义 ----------------------------------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* 包含头文件 *****************************************************************/
#include "stdio.h"
#include "string.h"
#include "ymodem.h"


/* 类型声明 -----------------------------------------------------------------*/
typedef  void (*pFunction)(void);

typedef enum
{
    Uart1_485 = 0,

    Uart_NumMax,
}enum_Uart_t;

//计算上传文件大小 
#define FLASH_IMAGE_SIZE                   (uint32_t) (FLASH_SIZE - (ApplicationAddress - 0x08000000))

#define IS_AF(c)  ((c >= 'A') && (c <= 'F'))
#define IS_af(c)  ((c >= 'a') && (c <= 'f'))
#define IS_09(c)  ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)  IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)  IS_09(c)
#define CONVERTDEC(c)  (c - '0')

#define CONVERTHEX_alpha(c)  (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)   (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))

#define SerialPutString(x,y)    Serial_PutString(x,(uint8_t*)(y))



/* 函数声明 ------------------------------------------------------------------*/
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t GetIntegerInput(int32_t * num);

uint32_t SerialKeyPressed(uint8_t fu8_Channel,uint8_t *key);

uint8_t GetKey(void);


extern void Serial_PutChar(uint8_t fu8_Channal, uint8_t c);
extern void Serial_PutString(uint8_t fu8_Channal, uint8_t *s);


void GetInputString(uint8_t * buffP);

void SerialUpload(void);

#endif  /* _COMMON_H */

/*******************************文件结束***************************************/
