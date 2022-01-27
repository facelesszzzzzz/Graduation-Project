#ifndef __DATATYPE_H__
#define __DATATYPE_H__

typedef enum
{
    DEFAULT_PRIORITY = 0,
    TASK_INIT_PRIORITY,
    MAIN_RUNNING_PRIORITY,
    SEND_PRIORITY,
    ESP8266_PRIORITY,
}Task_Priority_e;

#endif // !__DATATYPE_H__
