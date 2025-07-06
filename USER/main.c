/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		杨璐语
	*
	*	日期： 		2024-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		基于FreeRTOS与云平台的智能厨务系统
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*			
	************************************************************
	************************************************************
	************************************************************
**/

/*------------------------------------------------------*/
/*                                                      */
/*            程序main函数，入口函数源文件               */
/*                                                      */
/*------------------------------------------------------*/

#include "sys.h"
#include "delay.h"          // 延时函数头文件
#include "usart.h"          // USART串口驱动

#include "FreeRTOS.h"       // FreeRTOS配置头文件
#include "semphr.h"         // 信号量支持
#include "queue.h"          // 队列支持
#include "event_groups.h"   // 事件标志组支持

#include "wifi.h"           // WiFi通信模块
#include "mqtt.h"           // MQTT协议支持
#include "control.h"        // 控制模块：LED控制等功能
#include "led.h"            // LED驱动
#include "dht11_u8.h"       // 温湿度传感器驱动
#include "beep.h"           // 蜂鸣器驱动
#include "mq2.h"            // 烟雾传感器驱动
#include "fan.h"            // 风扇控制驱动
#include "fan_pwm.h"        // PWM风扇驱动

#include "oled.h"           // OLED屏幕显示
#include "key.h"            // 按键驱动

#include "esp8266.h"        // ESP8266 WiFi模块
#include "onenet.h"         // OneNet平台支持

/*-------------------------------------------------------------*/
/*          	WIFI网络与ONENET配置（配置）			      	   */
/*-------------------------------------------------------------*/

const char SSID[] 			 = "ioqq";          //路由器名称
const char PASS[] 			 = "12345678";    //路由器密码

/*-------------------------------------------------------------*/
/*          控制命令以及控制模块初始状态设置（配置）		   	   */
/*-------------------------------------------------------------*/

const char *LED1_LABER  = "led1_flag";//LED1标签，发送给ONENET的数据流名称
const char *CMD_LED1ON  = "LED1ON";   //LED1打开
const char *CMD_LED1OFF = "LED1OFF";  //LED1关闭
char 	   *led1_flag   = "LED1OFF";  //LED1状态，初始化为关闭状态

const char *LED2_LABER 	= "led2_flag";//LED2标签
const char *CMD_LED2ON  = "LED2ON";   //LED2打开
const char *CMD_LED2OFF = "LED2OFF";  //LED2关闭
char 	   *led2_flag   = "LED2ON";   //LED2状态，初始化为打开状态
/*全局变量声明*/

u8 temp,humi,fan_adj;
float mq2_vol=0.0,smog_th=0.3;

/*---------------- ESP8266连接信息 ------------------*/
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
/*-------------------------------------------------------------*/
/*               freerto任务通信控制（固定）			      	   */
/*-------------------------------------------------------------*/

SemaphoreHandle_t espSemaphoreHandle;

EventGroupHandle_t Event_Handle = NULL; 
const int KEY_EVENT=(0x01<<0);
const int MQ2_EVENT=(0x01<<1);

/*-------------------------------------------------------------*/
/*               任务句柄及任务函数声明1（配置）		      	   */
/*-------------------------------------------------------------*/
//开始任务
TaskHandle_t StartTask_Handler;
void my_start_task(void *pvParameters);
//LED任务 
TaskHandle_t Led2_Task_Handler;
void my_led2_task(void *pvParameters);
//DHT11任务 温湿度传感器
TaskHandle_t DHT11_Task_Handler;
void my_dht11_task(void *pvParameters);
//OneNet任务 
TaskHandle_t OneNet_Task_Handler;
void my_onenet_task(void *pvParameters);
//OneNet接收任务 
TaskHandle_t OneNet_Rev_Handler;
void my_onenet_rev(void *pvParameters);
//按键任务 
TaskHandle_t KEY_Task_Handler;
void KEY_task(void *pvParameters);
//烟雾探测
TaskHandle_t MQ2_Task_Handler;
void MQ2_task(void *pvParameters);
//报警处理
TaskHandle_t Process_Task_Handler;
void Process_task(void *pvParameters);


/*---------------------------------------------------------------*/
/*函数名：int main()                                             */
/*功  能：主函数							                         */
/*		  1.初始化各功能模块  				     				 */
/*		  2.创建开始任务（在开始任务里创建所有其他任务）           */
/*		  3.开启任务调度				       			 		     */
/*参  数：无                          			   				 */
/*返回值：无                                       			     */
/*---------------------------------------------------------------*/
void display_Init(void)
{
		OLED_Clear();
		OLED_ShowCHinese(0,0,1);//??
		OLED_ShowCHinese(18,0,2);//?è
		OLED_ShowCHinese(36,0,0);//￡o
		OLED_ShowCHinese(72,0,3);//é?ê??è
		
		OLED_ShowCHinese(0,3,4);//êa
		OLED_ShowCHinese(18,3,5);//?è
		OLED_ShowCHinese(36,3,0);//￡o
		
		OLED_ShowString(72,3,"%",16);
		
		OLED_ShowCHinese(0,6,6);//ì¨
		OLED_ShowCHinese(18,6,7);//μ?
		OLED_ShowCHinese(36,6,0);//￡o
//		OLED_Clear();
}
int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	delay_init();	       //延时函数初始化
	Usart1_Init(115200);   //串口1功能初始化，波特率115200，与串口住手通信		
	Usart2_Init(115200);   //串口2功能初始化，波特率115200，wifi通信	
	OLED_Init();
	led_init();		  	  
	Beep_Init();
	DHT11_Init();  		 
	Fan_Init();
	Mq2_Init();
	ESP8266_Init();
	OneNET_RegisterDevice();
	OLED_Clear();
	OLED_ShowString(0,5,"Connect MQTTs Server...",8);
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		delay_xms(500);
	OLED_Clear();
	OLED_ShowString(0,4,"Connect MQTTs Server Success",8);
	OLED_Clear();
	OLED_ShowString(0,4,"Device login...",8);
	while(OneNet_DevLink())			//接入OneNET
	{
		ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
		delay_xms(500);
	}
	OLED_Clear();
	OLED_ShowString(0,4,"CONNECT Success",8);
	OneNET_Subscribe();
	//创建开始任务
	xTaskCreate((TaskFunction_t	) my_start_task,		//任务函数
			    (const char* 	)"my_start_task",		//任务名称
				(uint16_t 		) 512,				  	//任务堆栈大小
				(void* 		  	) NULL,				 	//传递给任务函数的参数
				(UBaseType_t 	) 1, 				  	//任务优先级
				(TaskHandle_t*  ) &StartTask_Handler);	//任务控制块 
			
	vTaskStartScheduler();  							//开启任务调度
	while(1){
	}
}

/*---------------------------------------------------------------*/
/*函数名：void my_start_task(void *pvParameters)                 */
/*功  能：开始任务（配置）							             */
/*		  1.创建信号量，消息队列等任务通信方式   				     */
/*		  2.创建所有任务       			 						 */
/*		  3.删除本身       			 		    				 */
/*参  数：无                          			   				 */
/*返回值：无                                       			     */
/*---------------------------------------------------------------*/
void my_start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); 
	
	KEY_Init();
	display_Init();
	Event_Handle = xEventGroupCreate(); 
	espSemaphoreHandle = xSemaphoreCreateBinary();
	xSemaphoreGive(espSemaphoreHandle);
	
	//任务创建函数参数；1.任务函数 2.任务名称 3.任务堆栈大小 3.传递给任务函数的参数 4.任务优先级 5.任务控制块

	//创建led控制任务
//xTaskCreate(my_led2_task, 			"my_led2_task",				256, NULL, 3, &Led2_Task_Handler);  
	//创建DHT11任务，温湿度传感器
	xTaskCreate(my_dht11_task, 			"my_dht11_task", 			128, NULL, 5, &DHT11_Task_Handler);
	//创建OneNet任务
	xTaskCreate(my_onenet_task, 			"my_onenet_task", 			256, NULL, 4, &OneNet_Task_Handler);
	//创建OneNet接收任务
	xTaskCreate(my_onenet_rev, 			"my_onenet_rev", 			256, NULL, 5, &OneNet_Rev_Handler);
	//创建KEY任务
	xTaskCreate(KEY_task, 			"KEY_task", 			128, NULL, 5, &KEY_Task_Handler);
	//创建烟雾测量任务
	xTaskCreate(MQ2_task, 			"MQ2_task", 			256, NULL, 5, &MQ2_Task_Handler);
	//创建报警处理任务，风扇和蜂鸣器
	xTaskCreate(Process_task, 			"Process_task", 			256, NULL, 4, &Process_Task_Handler);
		
	vTaskDelete(StartTask_Handler); //删除开始任务
	taskEXIT_CRITICAL();            //退出临界区

}


/*---------------------------------------------------------------*/
//函数名：KEY_task(void *pvParameters)
//功能：实现控制LED进行连接
//参  数：无                          			   				 */
//返回值：无  
/*---------------------------------------------------------------*/

void KEY_task(void *pvParameters)
{
	EventBits_t r_event;
	while(1)
	{
		r_event = xEventGroupWaitBits(Event_Handle,KEY_EVENT,pdTRUE,pdFALSE,portMAX_DELAY);
		UsartPrintf(USART_DEBUG, "key\r\n");
		if((r_event&KEY_EVENT)!=0)
		{
			vTaskDelay(10);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0)
			{
				if(led_info.Led_Status==LED_ON)
					Led_Set(LED_OFF);
				else
					Led_Set(LED_ON);
				
				if(Fan_Status==1) 
					Fan_Set(FAN_OFF);
				else 
					Fan_Set(FAN_ON);
				Beep_Set(BEEP_OFF);
			}
		}
	}
}


/*---------------------------------------------------------------*/
//函数名：void my_onenet_task(void *pvParameters)
//功能：与云服务器OneNet进行连接
/*---------------------------------------------------------------*/

void my_onenet_task(void *pvParameters)
{
	unsigned char *dataPtr = NULL;
	while(1)
	{
		if (xSemaphoreTake(espSemaphoreHandle, portMAX_DELAY) == pdTRUE)
		{
		OneNet_SendData();
		ESP8266_Clear();
		xSemaphoreGive(espSemaphoreHandle);
		}
		delay_ms(1000);
	}
}

void my_onenet_rev(void *pvParameters)
{
	unsigned char *dataPtr = NULL;
	while(1)
	{
		if (xSemaphoreTake(espSemaphoreHandle, portMAX_DELAY) == pdTRUE)//共享了一个全局的ESP8266buff
		{
			dataPtr = ESP8266_GetIPD(3);
			if(dataPtr != NULL)
				OneNet_RevPro(dataPtr);
			xSemaphoreGive(espSemaphoreHandle);
		}
		delay_ms(50);
	}
}

/*---------------------------------------------------------------*/
/*函数名：void my_dht11_task(void *pvParameters)                 */
/*功  能：DHT11任务 温湿度传感器（配置）							 */
/*		  1.检测环境温湿度数据      							     */
/*		  2.将环境温湿度数据放入传感器数据消息队列       			 */
/*参  数：无                          			   				 */
/*返回值：无                                       			     */
/*其  他：服务器连接以及PING心跳包30S发送模式事件发生时执行此任务，*/
/*		  否则挂起任务   									     */
/*---------------------------------------------------------------*/
void my_dht11_task(void *pvParameters)
{
	
	while(1)
	{
		char buf[3];
		DHT11_Read_Data(&temp, &humi);//读取温湿度值
		//oled显示温湿度
		sprintf(buf,"%2d",temp);
		OLED_ShowString(54,0,buf,16);//温度
		sprintf(buf,"%2d",humi);
		OLED_ShowString(54,3,buf,16);//湿度
		if(led_info.Led_Status)
			OLED_ShowCHinese(54,6,8);//亮
		else
			OLED_ShowCHinese(54,6,9);//亮
		delay_ms(3000);//延时3s
	}
} 

/*---------------------------------------------------------------*/
//函数名：void MQ2_task(void *pvParameters);
//功能：检测烟雾，并使用事件组进行报警，与打开风扇
/*---------------------------------------------------------------*/
void MQ2_task(void *pvParameters)
{
	uint8_t smog_lock=0;
	while(1)
	{
		mq2_vol=adcValueToVoltage();
		UsartPrintf(USART_DEBUG, "mq2: %.2f\r\n",mq2_vol);
		if(mq2_vol>smog_th && smog_lock==0)
		{	
			xEventGroupSetBits(Event_Handle, MQ2_EVENT);
			smog_lock=1;
		}
		if(mq2_vol<smog_th && smog_lock==1)
		{
			smog_lock = 0;
		}
		
		delay_ms(1000);
	}
}	
/*---------------------------------------------------------------*/
//函数名：void Process_task(void *pvParameters);
//功能：检测烟雾，并使用事件组进行报警，与打开风扇
/*---------------------------------------------------------------*/

void Process_task(void *pvParameters)
{
	EventBits_t event;
	while(1)
	{
		event = xEventGroupWaitBits(Event_Handle,MQ2_EVENT,pdTRUE,pdFALSE,portMAX_DELAY);
		Fan_Set(FAN_ON);
		Beep_Set(BEEP_ON);
		Led_Set(LED_ON);
		delay_ms(200);
	}
}



/*---------------------------------------------------------------*/
/*函数名：void my_led2_task(void *pvParameters)                  */
/*功  能：LED任务（配置）									     */
/*		  1.LED2任务执行       							         */
/*参  数：无                          			   				 */
/*返回值：无                                       			     */
/*其  他：出现烟雾时候进行灯光报警，*/
/*		  否则挂起任务   									     */
/*---------------------------------------------------------------*/
void my_led2_task(void *pvParameters)
{
	EventBits_t event;
	while(1)
	{
		event = xEventGroupWaitBits(Event_Handle,MQ2_EVENT,pdTRUE,pdFALSE,portMAX_DELAY);
		led1_on();		
		vTaskDelay(500);	//延时500ms
		led1_off();
		vTaskDelay(500);	//延时500ms
	}
}

