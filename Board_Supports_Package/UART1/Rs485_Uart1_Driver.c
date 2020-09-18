#include "Rs485_Uart1_Driver.h"


void Uart1_Init(uint32_t BaudRate, uint16_t DataBits, uint16_t Parity, uint16_t StopBits)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_485EN	  GPIOA.8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;			
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    //USART1_485TX    GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //USART1_485RX	  GPIOA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,ENABLE);//复位串口1
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1,DISABLE);//停止复位

    //USART 初始化设置
    USART_InitStructure.USART_BaudRate = BaudRate;//串口波特率
    USART_InitStructure.USART_WordLength = DataBits;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = StopBits;//一个停止位
    USART_InitStructure.USART_Parity = Parity;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
    USART_Init(USART1, &USART_InitStructure);//初始化串口1
    
    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;  //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		    //子优先级1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			    //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    
    USART_Cmd(USART1, ENABLE);//使能串口1
}



/********************************************************************************
* 函数名 : Uart1_SendData
* 功  能 : 串口发送数据
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void Uart1_SendData(uint8_t *buf,uint8_t len)
{
	uint8_t i=0;

    GPIO_SetBits(GPIOA, GPIO_Pin_8);    //设置为发送模式
    
  	for(i=0;i<len;i++)		//循环发送数据
	{
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); //等待发送结束
        USART_SendData(USART1, buf[i]); //发送数据
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET){} //等待发送结束

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);  //设置为接收模式
}
/********************************************************************************
* 函数名 : Uart1_PutChar
* 功  能 : 串口发送1byte数据
* 说  明 : none
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void Uart1_PutChar(uint8_t c)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_8);    //设置为发送模式
    
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET){} //等待发送结束
    USART_SendData(USART1, c); //发送数据
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET){} //等待发送结束

    GPIO_ResetBits(GPIOA, GPIO_Pin_8);  //设置为接收模式    
}
/********************************************************************************
* 函数名 : Uart1_PutString
* 功  能 : 串口发送字符串数据
* 说  明 : 对输入要求苛刻。输入必须为字符串；如果是字符串数组，注意末尾'\0'防止程序跑飞
* 入  参 : none
* 返  回 : none
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
void Uart1_PutString(uint8_t *s)
{
    while (*s != '\0')
    {
        Uart1_PutChar(*s);
        s++;
    }
}
