/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	main.c
	*
	*	���ߣ� 		�����
	*
	*	���ڣ� 		2024-05-08
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		����FreeRTOS����ƽ̨�����ܳ���ϵͳ
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*			
	************************************************************
	************************************************************
	************************************************************
**/

/*------------------------------------------------------*/
/*                                                      */
/*            ����main��������ں���Դ�ļ�               */
/*                                                      */
/*------------------------------------------------------*/

#include "sys.h"
#include "delay.h"          // ��ʱ����ͷ�ļ�
#include "usart.h"          // USART��������

#include "FreeRTOS.h"       // FreeRTOS����ͷ�ļ�
#include "semphr.h"         // �ź���֧��
#include "queue.h"          // ����֧��
#include "event_groups.h"   // �¼���־��֧��

#include "wifi.h"           // WiFiͨ��ģ��
#include "mqtt.h"           // MQTTЭ��֧��
#include "control.h"        // ����ģ�飺LED���Ƶȹ���
#include "led.h"            // LED����
#include "dht11_u8.h"       // ��ʪ�ȴ���������
#include "beep.h"           // ����������
#include "mq2.h"            // ������������
#include "fan.h"            // ���ȿ�������
#include "fan_pwm.h"        // PWM��������

#include "oled.h"           // OLED��Ļ��ʾ
#include "key.h"            // ��������

#include "esp8266.h"        // ESP8266 WiFiģ��
#include "onenet.h"         // OneNetƽ̨֧��

/*-------------------------------------------------------------*/
/*          	WIFI������ONENET���ã����ã�			      	   */
/*-------------------------------------------------------------*/

const char SSID[] 			 = "ioqq";          //·��������
const char PASS[] 			 = "12345678";    //·��������

/*-------------------------------------------------------------*/
/*          ���������Լ�����ģ���ʼ״̬���ã����ã�		   	   */
/*-------------------------------------------------------------*/

const char *LED1_LABER  = "led1_flag";//LED1��ǩ�����͸�ONENET������������
const char *CMD_LED1ON  = "LED1ON";   //LED1��
const char *CMD_LED1OFF = "LED1OFF";  //LED1�ر�
char 	   *led1_flag   = "LED1OFF";  //LED1״̬����ʼ��Ϊ�ر�״̬

const char *LED2_LABER 	= "led2_flag";//LED2��ǩ
const char *CMD_LED2ON  = "LED2ON";   //LED2��
const char *CMD_LED2OFF = "LED2OFF";  //LED2�ر�
char 	   *led2_flag   = "LED2ON";   //LED2״̬����ʼ��Ϊ��״̬
/*ȫ�ֱ�������*/

u8 temp,humi,fan_adj;
float mq2_vol=0.0,smog_th=0.3;

/*---------------- ESP8266������Ϣ ------------------*/
#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"
/*-------------------------------------------------------------*/
/*               freerto����ͨ�ſ��ƣ��̶���			      	   */
/*-------------------------------------------------------------*/

SemaphoreHandle_t espSemaphoreHandle;

EventGroupHandle_t Event_Handle = NULL; 
const int KEY_EVENT=(0x01<<0);
const int MQ2_EVENT=(0x01<<1);

/*-------------------------------------------------------------*/
/*               ������������������1�����ã�		      	   */
/*-------------------------------------------------------------*/
//��ʼ����
TaskHandle_t StartTask_Handler;
void my_start_task(void *pvParameters);
//LED���� 
TaskHandle_t Led2_Task_Handler;
void my_led2_task(void *pvParameters);
//DHT11���� ��ʪ�ȴ�����
TaskHandle_t DHT11_Task_Handler;
void my_dht11_task(void *pvParameters);
//OneNet���� 
TaskHandle_t OneNet_Task_Handler;
void my_onenet_task(void *pvParameters);
//OneNet�������� 
TaskHandle_t OneNet_Rev_Handler;
void my_onenet_rev(void *pvParameters);
//�������� 
TaskHandle_t KEY_Task_Handler;
void KEY_task(void *pvParameters);
//����̽��
TaskHandle_t MQ2_Task_Handler;
void MQ2_task(void *pvParameters);
//��������
TaskHandle_t Process_Task_Handler;
void Process_task(void *pvParameters);


/*---------------------------------------------------------------*/
/*��������int main()                                             */
/*��  �ܣ�������							                         */
/*		  1.��ʼ��������ģ��  				     				 */
/*		  2.������ʼ�����ڿ�ʼ�����ﴴ��������������           */
/*		  3.�����������				       			 		     */
/*��  ������                          			   				 */
/*����ֵ����                                       			     */
/*---------------------------------------------------------------*/
void display_Init(void)
{
		OLED_Clear();
		OLED_ShowCHinese(0,0,1);//??
		OLED_ShowCHinese(18,0,2);//?��
		OLED_ShowCHinese(36,0,0);//��o
		OLED_ShowCHinese(72,0,3);//��?��??��
		
		OLED_ShowCHinese(0,3,4);//��a
		OLED_ShowCHinese(18,3,5);//?��
		OLED_ShowCHinese(36,3,0);//��o
		
		OLED_ShowString(72,3,"%",16);
		
		OLED_ShowCHinese(0,6,6);//����
		OLED_ShowCHinese(18,6,7);//��?
		OLED_ShowCHinese(36,6,0);//��o
//		OLED_Clear();
}
int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
	delay_init();	       //��ʱ������ʼ��
	Usart1_Init(115200);   //����1���ܳ�ʼ����������115200���봮��ס��ͨ��		
	Usart2_Init(115200);   //����2���ܳ�ʼ����������115200��wifiͨ��	
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
	while(OneNet_DevLink())			//����OneNET
	{
		ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
		delay_xms(500);
	}
	OLED_Clear();
	OLED_ShowString(0,4,"CONNECT Success",8);
	OneNET_Subscribe();
	//������ʼ����
	xTaskCreate((TaskFunction_t	) my_start_task,		//������
			    (const char* 	)"my_start_task",		//��������
				(uint16_t 		) 512,				  	//�����ջ��С
				(void* 		  	) NULL,				 	//���ݸ��������Ĳ���
				(UBaseType_t 	) 1, 				  	//�������ȼ�
				(TaskHandle_t*  ) &StartTask_Handler);	//������ƿ� 
			
	vTaskStartScheduler();  							//�����������
	while(1){
	}
}

/*---------------------------------------------------------------*/
/*��������void my_start_task(void *pvParameters)                 */
/*��  �ܣ���ʼ�������ã�							             */
/*		  1.�����ź�������Ϣ���е�����ͨ�ŷ�ʽ   				     */
/*		  2.������������       			 						 */
/*		  3.ɾ������       			 		    				 */
/*��  ������                          			   				 */
/*����ֵ����                                       			     */
/*---------------------------------------------------------------*/
void my_start_task(void *pvParameters)
{
	taskENTER_CRITICAL(); 
	
	KEY_Init();
	display_Init();
	Event_Handle = xEventGroupCreate(); 
	espSemaphoreHandle = xSemaphoreCreateBinary();
	xSemaphoreGive(espSemaphoreHandle);
	
	//���񴴽�����������1.������ 2.�������� 3.�����ջ��С 3.���ݸ��������Ĳ��� 4.�������ȼ� 5.������ƿ�

	//����led��������
//xTaskCreate(my_led2_task, 			"my_led2_task",				256, NULL, 3, &Led2_Task_Handler);  
	//����DHT11������ʪ�ȴ�����
	xTaskCreate(my_dht11_task, 			"my_dht11_task", 			128, NULL, 5, &DHT11_Task_Handler);
	//����OneNet����
	xTaskCreate(my_onenet_task, 			"my_onenet_task", 			256, NULL, 4, &OneNet_Task_Handler);
	//����OneNet��������
	xTaskCreate(my_onenet_rev, 			"my_onenet_rev", 			256, NULL, 5, &OneNet_Rev_Handler);
	//����KEY����
	xTaskCreate(KEY_task, 			"KEY_task", 			128, NULL, 5, &KEY_Task_Handler);
	//���������������
	xTaskCreate(MQ2_task, 			"MQ2_task", 			256, NULL, 5, &MQ2_Task_Handler);
	//���������������񣬷��Ⱥͷ�����
	xTaskCreate(Process_task, 			"Process_task", 			256, NULL, 4, &Process_Task_Handler);
		
	vTaskDelete(StartTask_Handler); //ɾ����ʼ����
	taskEXIT_CRITICAL();            //�˳��ٽ���

}


/*---------------------------------------------------------------*/
//��������KEY_task(void *pvParameters)
//���ܣ�ʵ�ֿ���LED��������
//��  ������                          			   				 */
//����ֵ����  
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
//��������void my_onenet_task(void *pvParameters)
//���ܣ����Ʒ�����OneNet��������
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
		if (xSemaphoreTake(espSemaphoreHandle, portMAX_DELAY) == pdTRUE)//������һ��ȫ�ֵ�ESP8266buff
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
/*��������void my_dht11_task(void *pvParameters)                 */
/*��  �ܣ�DHT11���� ��ʪ�ȴ����������ã�							 */
/*		  1.��⻷����ʪ������      							     */
/*		  2.��������ʪ�����ݷ��봫����������Ϣ����       			 */
/*��  ������                          			   				 */
/*����ֵ����                                       			     */
/*��  ���������������Լ�PING������30S����ģʽ�¼�����ʱִ�д�����*/
/*		  �����������   									     */
/*---------------------------------------------------------------*/
void my_dht11_task(void *pvParameters)
{
	
	while(1)
	{
		char buf[3];
		DHT11_Read_Data(&temp, &humi);//��ȡ��ʪ��ֵ
		//oled��ʾ��ʪ��
		sprintf(buf,"%2d",temp);
		OLED_ShowString(54,0,buf,16);//�¶�
		sprintf(buf,"%2d",humi);
		OLED_ShowString(54,3,buf,16);//ʪ��
		if(led_info.Led_Status)
			OLED_ShowCHinese(54,6,8);//��
		else
			OLED_ShowCHinese(54,6,9);//��
		delay_ms(3000);//��ʱ3s
	}
} 

/*---------------------------------------------------------------*/
//��������void MQ2_task(void *pvParameters);
//���ܣ����������ʹ���¼�����б�������򿪷���
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
//��������void Process_task(void *pvParameters);
//���ܣ����������ʹ���¼�����б�������򿪷���
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
/*��������void my_led2_task(void *pvParameters)                  */
/*��  �ܣ�LED�������ã�									     */
/*		  1.LED2����ִ��       							         */
/*��  ������                          			   				 */
/*����ֵ����                                       			     */
/*��  ������������ʱ����еƹⱨ����*/
/*		  �����������   									     */
/*---------------------------------------------------------------*/
void my_led2_task(void *pvParameters)
{
	EventBits_t event;
	while(1)
	{
		event = xEventGroupWaitBits(Event_Handle,MQ2_EVENT,pdTRUE,pdFALSE,portMAX_DELAY);
		led1_on();		
		vTaskDelay(500);	//��ʱ500ms
		led1_off();
		vTaskDelay(500);	//��ʱ500ms
	}
}

