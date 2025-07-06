#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK nano STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
extern EventGroupHandle_t Event_Handle;
extern const int KEY_EVENT;

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
 	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);//ʹ��PORTA
	
	//KEY����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//KEY
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOE4,3
	
 //GPIOA.1	  �ж����Լ��жϳ�ʼ������ �½��ش��� //KEY
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);	  	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ʹ�ܰ���KEY���ڵ��ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;	//��ռ���ȼ�2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//�����ȼ�1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������
	if(key_up&&(KEY==0))
	{
		delay_xms(10);//ȥ���� 
		key_up=0;
		if(KEY==0)return KEY_PRES;
	}else if(KEY==1)key_up=1; 	    
 	return 0;// �ް�������
}

//void EXTI1_IRQHandler(void)
//{
//	delay_xms(10);//����
////	if(KEY==0)	 	 //KEY
////	{	
////		LED=!LED;
////	}
//	
//	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==0)
//	{
//		if(led_info.Led_Status==LED_ON)
//			Led_Set(LED_OFF);
//		else
//			Led_Set(LED_ON);
//	}
//	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE0�ϵ��жϱ�־λ  
//}

void EXTI1_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken=pdFALSE;
	uint32_t ulReturn;
	
	ulReturn = taskENTER_CRITICAL_FROM_ISR();
	UsartPrintf(USART_DEBUG, "key\r\n");
 if(EXTI_GetITStatus(EXTI_Line1) != RESET)
 {
 		xEventGroupSetBitsFromISR(Event_Handle,KEY_EVENT,&pxHigherPriorityTaskWoken);
	 portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
	 EXTI_ClearITPendingBit(EXTI_Line1);
 }
 taskEXIT_CRITICAL_FROM_ISR( ulReturn ); 
}
