#ifndef __ESP8266_H
#define __ESP8266_H	 
#include "sys.h"  
/**********************************************************************
*						WIFI指令说明
* 设置工作模式 1：station模式   2：AP模式  3：兼容 AP+station模式
* "AT+CWMODE=1";

* 让模块连接上自己的热点
* "AT+CWJAP=\"Faceless\",\"05720041018\"";

* =0：单路连接模式     =1：多路连接模式
* "AT+CIPMUX=0";

* 建立TCP连接  这四项分别代表了 要连接的ID号0~4   连接类型  远程服务器IP地址   远程服务器端口号
* "AT+CIPSTART=\"TCP\",\"192.168.43.1\",8900";

* 是否开启透传模式  0：表示关闭 1：表示开启透传
* "AT+CIPMODE=1";

* 透传模式下 开始发送数据的指令 这个指令之后就可以直接发数据了
*"AT+CIPSEND";
**********************************************************************/
#define CONNECT_10MS 200

void WIFI_Connect(void);
void ESP8266_Init(void);
u8 ESP8266_Send_Cmd(u8 *cmd,u8 *ack,u16 waittime);
u8* ESP8266_Check_Cmd(u8 *str);
void ESP8266_SendToApp(void);
void ESP8266_RecToAPP(void);



#endif













