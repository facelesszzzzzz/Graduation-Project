#ifndef __ESP8266_H__
#define __ESP8266_H__

#define ESP8266_CONNECT_BIT     EVENT_GROUP_BIT(0)  
#define ESP8266_GOT_IP_BIT      EVENT_GROUP_BIT(1)

void ESP8266_Task_Init(void);

#endif // !__ESP8266_H__
