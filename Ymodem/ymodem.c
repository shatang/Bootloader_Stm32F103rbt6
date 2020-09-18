#include "ymodem.h"
#include "flash_if.h"
#include "common.h"

/****************************** GLOBAL VARIABLES ******************************/
uint8_t file_name[FILE_NAME_LENGTH];    //传输文件名


/********************************************************************************
* 函数名 :  Receive_Byte
* 功  能 :  规定时间内，接收到一个字节
* 说  明 :  接口化，用于ymodem.c文件调用
* 入  参 :  none
* 返  回 :  none
* 设  计 :  Shatang                   时  间 :  2020.06.18
* 修  改 :  none                      时  间 :  none
********************************************************************************/
static int32_t Receive_Byte (uint8_t fu8_Channel,uint8_t *c, uint32_t fu32_timeout)
{
	while(fu32_timeout -- > 0)
    {
        if(SerialKeyPressed(fu8_Channel,c) == 1)return 0;
    }
	
    return -1;
}
/********************************************************************************
* 函数名 :  Send_Byte
* 功  能 :  发送一个字节
* 说  明 :  接口化，用于ymodem.c文件调用
* 入  参 :  none
* 返  回 :  none
* 设  计 :  Shatang                   时  间 :  2020.06.18
* 修  改 :  none                      时  间 :  none
********************************************************************************/
static uint32_t Send_Byte (uint8_t fu8_Channel,uint8_t c)
{
    Serial_PutChar(fu8_Channel,c);
    return 0;
}
/********************************************************************************
* 函数名 : Receive_Packet
* 功  能 : 从发送端接收一个数据包
* 说  明 : none
* 入  参 : data ：数据指针
           length：长度
           timeout ：超时时间

* 返  回 : 接收的结果
            0: 正常返回
            -1: 超时或者数据包错误
            1: 用户取消
* 设  计 : Shatang                    时  间 : 2020.06.22
* 修  改 : none                       时  间 : none
********************************************************************************/
static int32_t Receive_Packet (uint8_t fu8_Channal, uint8_t *data, int32_t *length, uint32_t timeout)
{
    uint16_t i, packet_size;
    uint8_t c;
    *length = 0;
    if (Receive_Byte(fu8_Channal, &c, timeout) != 0)
    {
        return -1;
    }
    switch (c)
    {
        case SOH:
            packet_size = PACKET_SIZE;
            break;
        case STX:
            packet_size = PACKET_1K_SIZE;
            break;
        case EOT:
            return 0;
        case CA:
            if((Receive_Byte(fu8_Channal, &c, timeout) == 0) && (c == CA))
            {
                *length = -1;
                return 0;
            }
            else
            {
                return -1;
            }
        case ABORT1:
        case ABORT2:
            return 1;
        default:
            return -1;
    }
    *data = c;
    for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
    {
        if (Receive_Byte(fu8_Channal, data + i, timeout) != 0)
        {
            return -1;
        }
    }
    if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
    {
        return -1;
    }
    *length = packet_size;
    return 0;
}

/********************************************************************************
* 函数名 : Ymodem_Receive
* 功  能 : 通过 ymodem协议接收一个文件
* 说  明 : none
* 入  参 : buf: 首地址指针
* 返  回 : 文件大小(长度)
* 设  计 : Shatang                    时  间 : 2020.06.23
* 修  改 : none                       时  间 : none
********************************************************************************/
int32_t Ymodem_Receive(uint8_t fu8_Channal, uint8_t *buf)
{
    uint8_t packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD] = {0};
    uint8_t file_size[FILE_SIZE_LENGTH] = {0};
    uint8_t *file_ptr;
    uint8_t *buf_ptr;

    int32_t i = 0; 
    
    int32_t session_begin = 0;  //会话开始标志位
    int32_t session_done = 0;   //会话结束标志位
    int32_t file_done = 0;      //文件传输完成标志位
    int32_t packet_length = 0;      //数据包的数据区长度
    int32_t packets_received = 0;   //数据包接收个数
    int32_t errors;                 //数据包接收错误次数
    int32_t size = 0;           //最终文件大小
    
	uint32_t RamSource = 0;
    
    uint32_t FlashDestination = 0;
    
    //初始化Flash地址变量
    FlashDestination = APPLICATION_START_ADDRESS;

    for (session_done = 0, errors = 0, session_begin = 0; ;)
    {
        for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
        {
            switch (Receive_Packet(fu8_Channal, packet_data, &packet_length, NAK_TIMEOUT))
            {
                case 0:
                    errors = 0;
                    switch (packet_length)
                    {
                        //发送端终止
                        case -1:
                            Send_Byte(fu8_Channal, ACK);
                            return 0;
                        //结束传输
                        case 0:
                            Send_Byte(fu8_Channal, ACK);
                            file_done = 1;
                            break;
                        //正常的数据包
                        default:
                            if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))
                            {
                                Send_Byte(fu8_Channal, NAK);
                            }
                            else
                            {
                                if (packets_received == 0)
                                {
                                    //文件名数据包
                                    if (packet_data[PACKET_HEADER] != 0)
                                    {
                                        //文件名数据包有效数据区域
                                        for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
                                        {
                                            file_name[i++] = *file_ptr++;
                                        }
                                        file_name[i++] = '\0';
                                        for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
                                        {
                                            file_size[i++] = *file_ptr++;
                                        }
                                        file_size[i++] = '\0';
                                        Str2Int(file_size, &size);

                                        //测试数据包是否过大
                                        if (size > (APPLICATION_SIZE - 1))
                                        {
                                            //结束
                                            Send_Byte(fu8_Channal, CA);
                                            Send_Byte(fu8_Channal, CA);
                                            return -1;
                                        }

                                        //计算需要擦除Flash的页
                                        FLASH_If_Erase(APPLICATION_START_ADDRESS);
                                        Send_Byte(fu8_Channal, ACK);
                                        Send_Byte(fu8_Channal, CRC16);
                                    }
                                    //文件名数据包空，结束传输
                                    else
                                    {
                                        Send_Byte(fu8_Channal, ACK);
                                        file_done = 1;
                                        session_done = 1;
                                        break;
                                    }
                                }
                                //数据包
                                else
                                {
                                    uint8_t lu8_Index = 0;
                                    
                                    memcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
                                    RamSource = (uint32_t)buf;

                                    //把接收到的数据编写到Flash中
                                    if (FLASH_If_Write(&FlashDestination, (uint32_t*) RamSource, (uint16_t) packet_length/4)  == 0)
                                    {
                                        Send_Byte(fu8_Channal,ACK);
                                    }
                                    else /* An error occurred while writing to Flash memory */
                                    {
                                        //结束
                                        Send_Byte(fu8_Channal,CA);
                                        Send_Byte(fu8_Channal,CA);
                                        return -2;
                                    }
                                }
                                packets_received ++;
                                session_begin = 1;
                            }
                   }
                break;
                case 1:
                    Send_Byte(fu8_Channal, CA);
                    Send_Byte(fu8_Channal, CA);
                    return -3;
                default:
                    if (session_begin > 0)
                    {
                        errors ++;
                    }
                    if (errors > MAX_ERRORS)
                    {
                        Send_Byte(fu8_Channal, CA);
                        Send_Byte(fu8_Channal, CA);
                        return 0;
                    }
                    Send_Byte(fu8_Channal, CRC16);
                    break;
            }
            if (file_done != 0)
            {
                break;
            }
        }
        if (session_done != 0)
        {
            break;
        }
    }
    return (int32_t)size;
}
