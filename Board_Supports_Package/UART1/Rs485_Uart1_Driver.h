#ifndef __RS485_UART1_DRIVER_H
#define __RS485_UART1_DRIVER_H
#include <stdint.h>
#include "stm32f10x.h"

extern void Uart1_Init(uint32_t BaudRate, uint16_t DataBits, uint16_t Parity, uint16_t StopBits);
extern void Uart1_SendData(uint8_t *buf,uint8_t len);
extern void Uart1_PutChar(uint8_t c);
extern void Uart1_PutString(uint8_t *s);

#endif
