#ifndef __YMODEM_H
#define __YMODEM_H
#include <stdint.h>


/****************************** GLOBAL FUNCTIONS ****************************/
#define PACKET_SEQNO_INDEX      (1)
#define PACKET_SEQNO_COMP_INDEX (2)

#define PACKET_HEADER           (3)     //包头
#define PACKET_TRAILER          (2)     //包尾(CRC16校验)
#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (256)
#define FILE_SIZE_LENGTH        (16)

#define SOH                     (0x01)  /* start of 128-byte data packet */
#define STX                     (0x02)  /* start of 1024-byte data packet */
#define EOT                     (0x04)  /* end of transmission */
#define ACK                     (0x06)  /* acknowledge */
#define NAK                     (0x15)  /* negative acknowledge */
#define CA                      (0x18)  /* two of these in succession aborts transfer */
#define CRC16                   (0x43)  /* 'C' == 0x43, request 16-bit CRC */

#define ABORT1                  (0x41)  //'A' == 0x41, 用户终止
#define ABORT2                  (0x61)  //'a' == 0x61, 用户终止

#define NAK_TIMEOUT             (0x100000)
#define MAX_ERRORS              (5)
/****************************** GLOBAL VARIABLES ******************************/
extern uint8_t file_name[FILE_NAME_LENGTH];
/****************************** GLOBAL FUNCTIONS ****************************/
extern int32_t Ymodem_Receive(uint8_t fu8_Channal, uint8_t *buf);



#endif
