# Graduation-Project

## This is Faceless's Graduation-Project.

**Cradle_V0.07:**
	1、优化事件标志位的判断逻辑
	2、下一版本取消ov7725
**Cradle_V0.06:**  
	1、添加代码注释  
	2、重命名文件名，删除原有的readme.txt文件  
**Cradle_V0.05:**  
	1、调试完毕，等待APP联调  
**Cradle_V0.04:**   
	1、通信部分分发送和接收两个任务来运行，系统运行自己一个任务  
	2、等待重新调试  
**Cradle_V0.03:**  
	1、通信部分优化，调试功能完善  
	2、串口3接收数据跟发送的数据重叠，待解决！  
**Cradle_V0.02:**  
	1、逻辑功能优化，通信部分完善  
**Cradle_V0.01:**  
    1、增加.gitignore对OBJ中的编译文件屏蔽上传  
	2、大体功能逻辑基本完善，剩下通信部分未完善  
/*  
his:  
v0.1: The New Contents:  
	1、熟悉使用FreeRTOS中的任务、队列、信号量  
	2、熟悉使用ESP8266的TCP/IP协议模式  
	3、程序运行即开启配网模式，实现无限透传功能  
	4、使用按键进行任务挂起和恢复，并使用按键来进行ESP8266重连功能  
v0.2: The New Contents:  
	1、长按按键0 5s后进入Smartconfig配网模式  
	2、保证在配网模式过程中若误触发重新配网模式也可正常从头开始配网（使用任务挂起、恢复机制、局部标志位）  
	3、使用AT+CWSTARTSMART之后都要进行AT+CWSTOPSMART，在之前使用的话是保证在任务挂起时重新配网也能正常关闭  
	4、增加MQTT文件  
v0.3: The New Contents:  
	1、ESP8266发送配置信息需要延时500ms，保证来回数据传输正常  
	2、每次开机前都判断wifi是否已连接，若已连接则直接开启TCP透传模式，不需要重新配网  
	3、使用周期性软件定时器(1000ms)进行响应超时判断，上电30s内判断是否已联网，开启Smartconfig模式60s内判断是否成功（执行3次）  
v0.4: The New Contents:  
	1、将ESP8266_Rec_Task任务设置成阻塞状态，优先级规划为高优先级执行任务  
	2、取消二值信号量，使用事件标志位进行ESP8266的指令配置  
	3、创建Mqtt_Task任务，成功连上腾讯云物联网平台并订阅主题成功  
	4、增加10s与ESP8266无通信则提示连接失败，不进行连接  
v0.5:The New Contents:  
	1、增加MQTT心跳包、发布消息  
	2、用FreeRTOS使用cJSON文件需要注意将malloc、free替换成pvPortMalloc、vPortFree  
	3、使用char * str = cJSON_Print(cJSON *Item)函数，需要释放Item和返回值str，否则会导致内存泄露  
	4、使用char * str = cJSON_Parse(cJSON *Item)函数，需要释放Item和返回值str，否则会导致内存泄露  
	5、cJSON解析需要对MQTT发送过来的数据提取出有效载荷(Payload)    
*/