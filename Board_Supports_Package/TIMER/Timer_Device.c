#include "Timer_Device.h"
#include "Timer_Driver.h"

/****************************** PRIVATE VARIABLES *****************************/
static volatile uint32_t vu32_TimerTick = 0;				    // 系统时间(不可修改)
static volatile uint32_t vu32_TimerCounter = 0;	   			// 系统时间(可修改)
static struct_TaskData_t timer_task[TIM_TASK_NUM];	 	// 定时器任务的事件队列

void SystemTimer_Init(void)
{
    TIM3_PortInit(9, 7199);//1ms基准
}


void TIM3_IRQHandler(void)   //TIM3中断
{
    uint8_t i=0;
    
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
    {
        vu32_TimerTick++;
        vu32_TimerCounter++;

		for(i=0;i<TIM_TASK_NUM;i++) 	//任务检索
		{
			if(timer_task[i].vu32Time_Out != 0)	//如果任务存在
			{
				timer_task[i].vu32Time_Count++;	//对应任务时间计数
				//任务所设定的时间点到了，执行回调函数
				if(timer_task[i].vu32Time_Out == timer_task[i].vu32Time_Count)
				{
					timer_task[i].vu32Time_Count=0;     //复位计时时间
					if(timer_task[i].pCallbackFunction != NULL)  //回调函数存在
						timer_task[i].pCallbackFunction();     //执行回调函数
				}
			}
		}
        
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源 
    }
}

//================================ 任务定时器部分 =================================
/*************************************************************
函数名称:	bool Timer_AddTask(uint32_t , uint32_t ,  TimTaskCall*  )
功    能:	添加某个定时器任务
入口参数:	uint32_t 任务ID号  uint32_t 定时时间
			FUNCTION* 回调函数指针
调用程序:
返 回 值:	bool添加成功返回TRUE
设    计:	Jason			         时    间:   2011-7-28
修    改:                            时    间:
*************************************************************/
bool Timer_AddTask(uint32_t fu32_Name,  uint32_t timeout, TimTaskCall* callbackfunction)
{
	bool lb_AddSuccess=false;
	
    //为防止重复添加任务，添加任务前函数先清除同一名字的任务
	Timer_TaskClr(fu32_Name);
	//在任务列表中添加任务
	if((fu32_Name!=0)&&(timeout!=0))
	{
        if(timer_task[fu32_Name].vu32Time_task_name==0)  //如果该任务寄存器不存在任务，则添加
        {
            timer_task[fu32_Name].vu32Time_task_name = fu32_Name;
            timer_task[fu32_Name].vu32Time_Out=timeout;
            timer_task[fu32_Name].vu32Time_Count=0;
            timer_task[fu32_Name].pCallbackFunction=callbackfunction;
            lb_AddSuccess=true;
        }
	}
	return(lb_AddSuccess);
}
/*************************************************************
函数名称:	bool Timer_TaskClr(uint32_t ID)
功    能:	清除某个定时器任务	
入口参数:	uint32_t 任务ID号 
调用程序:
返 回 值:	bool清除成功返回TRUE
设    计:	Jason			         时    间:   2011-7-28
修    改:                            时    间:
*************************************************************/
bool Timer_TaskClr(uint32_t fu32_Name)
{
	bool lb_ClrSuccess=false;
    
	if(fu32_Name!=0)
	{   
        //根据任务名称查找相应任务并清除
        timer_task[fu32_Name].vu32Time_task_name=0;
        timer_task[fu32_Name].vu32Time_Out=0;
        timer_task[fu32_Name].vu32Time_Count=0;
        timer_task[fu32_Name].pCallbackFunction=NULL;
        lb_ClrSuccess=true;
	}
	return (lb_ClrSuccess);
}
/*************************************************************
函数名称:	bool Timer_TaskRest(uint32_t  )
功    能:	定时器任务复位，重新启动
入口参数:	uint32_t 任务ID号 
调用程序:
返 回 值:	bool复位成功返回TRUE
设    计:	Jason			         时    间:   2011-7-28
修    改:                            时    间:
*************************************************************/
bool Timer_TaskReset(uint32_t fu32_Name)
{
	bool lb_RstSuccess=false;
    
	if(fu32_Name!=0)
    {
        if(timer_task[fu32_Name].vu32Time_Out!=0)
        {
            timer_task[fu32_Name].vu32Time_Out=0;		
            lb_RstSuccess=true;
        }				
	}
	return(lb_RstSuccess);
}
/*************************************************************
函数名称:	bool Timer_TaskClrALL()
功    能:	清除所有定时器任务
入口参数:	
调用程序:
返 回 值:	bool清楚成功返回TRUE
设    计:	Jason			         时    间:   2011-7-28
修    改:                            时    间:
*************************************************************/
bool Timer_TaskClrALL(void)
{
	uint8_t i;
    
	for(i=0;i<TIM_TASK_NUM;i++)
	{
		timer_task[i].vu32Time_task_name=0;
		timer_task[i].vu32Time_Out=0;
		timer_task[i].vu32Time_Count=0;
		timer_task[i].pCallbackFunction=NULL;
	}
	return(true);
}
//============================== 定时器计数部分 ===============================
/*************************************************************
函数名称:	SysTimeGet
功    能:	获取系统时间
入口参数:	
调用程序:
返 回 值:	系统时间
设    计:	Jason			         时    间:   2011-7-28
修    改:                            时    间:
*************************************************************/
uint32_t SysTimeGet(void)
{
    return vu32_TimerTick;
}
/*************************************************************
函数名称:	uint32_t ReadTimerCount(void)
功    能:	读取进入定时器中断的次数,用于时间计算
入口参数:	void 
调用程序:
返 回 值:	uint32_t 进入定时器的次数
设    计:			         时    间:   2016-6-28
修    改:                    时    间:
*************************************************************/
uint32_t ReadTimerCount(void)   
{
	return vu32_TimerCounter; //返回 定时器时间计数
}
/*************************************************************
函数名称:	void SetTimerCount(uint32_t DesiredValue)
功    能:	设置进入次数的内部参数
入口参数:	uint32_t 设定值 
调用程序:
返 回 值:	
设    计:			         时    间:   2016-6-28
修    改:                    时    间:
*************************************************************/
void SetTimerCount(uint32_t fu32_DesiredValue)
{
	vu32_TimerCounter = fu32_DesiredValue; //设置 定时器时间计数
}

