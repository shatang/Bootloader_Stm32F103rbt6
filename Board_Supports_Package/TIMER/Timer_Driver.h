#ifndef __TIMER_DRIVER_H
#define __TIMER_DRIVER_H
#include "stm32f10x.h"

  
uint32_t SysTimeGet(void);
void TIM3_PortInit(uint16_t arr,uint16_t psc); 

#endif
