#ifndef __TIMER_DEVICE_H
#define __TIMER_DEVICE_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void TimTaskCall(void);//回调函数类型定义
/*******************************************************************************
块注释：结构体定义声明
*******************************************************************************/
typedef struct{
	volatile uint32_t vu32Time_task_name;	//任务名称（组名称或上ID号码）
	volatile uint32_t vu32Time_Out;			//超时时间
	volatile uint32_t vu32Time_Count;		//任务启动为止到当前的时间
	TimTaskCall *pCallbackFunction;	        //超时回调函数指针
}struct_TaskData_t;							//定义添加执行任务所需要的数据	
/*******************************************************************************
块注释：定时器任务接口
*******************************************************************************/
typedef enum
{
    No_Task_ID = 0,
    
    PowerUpClrZero_TASK,
    BEEP_TASK,
    LED_COMx_TASK,
    LED_GPRS_TASK,
    CalAlarmInfo_TASK,
    
    TIM_TASK_NUM,
}enum_TimTask_t;



extern void SystemTimer_Init(void);
extern uint32_t SysTimeGet(void);
extern uint32_t ReadTimerCount(void);				        // 读取进入定时器中断的次数,用于时间计算
extern void SetTimerCount(uint32_t DesiredValue);		    // 设置进入次数的内部参数
//任务定时器部分
extern bool Timer_AddTask(uint32_t Name, uint32_t timeout, TimTaskCall* callbackfunction);//添加某个定时器任务
extern bool Timer_TaskClr(uint32_t Name);		            // 清除某个定时器任务	
extern bool Timer_TaskReset(uint32_t Name);					// 定时器任务复位，重新启动	
extern bool Timer_TaskClrALL(void);							// 清除所有定时器任务

#endif
