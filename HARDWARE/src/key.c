#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "led.h"

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK nano STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
extern EventGroupHandle_t Event_Handle;
extern const int KEY_EVENT;

//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
	EXTI_InitTypeDef EXTI_InitStructure;
 	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);//使能PORTA
	
	//KEY配置
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;//KEY
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOE4,3
	
 //GPIOA.1	  中断线以及中断初始化配置 下降沿触发 //KEY
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能按键KEY所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;	//抢占优先级2 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级1 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY_UP!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按
	if(key_up&&(KEY==0))
	{
		delay_xms(10);//去抖动 
		key_up=0;
		if(KEY==0)return KEY_PRES;
	}else if(KEY==1)key_up=1; 	    
 	return 0;// 无按键按下
}

//void EXTI1_IRQHandler(void)
//{
//	delay_xms(10);//消抖
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
//	EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位  
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
