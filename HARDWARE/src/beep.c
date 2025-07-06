



//硬件驱动
#include "beep.h"

uint8_t Beep_Status;

/*
************************************************************
*	函数名称：	Beep_Init
*
*	函数功能：	蜂鸣器初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Beep_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//打开GPIOB的时钟
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;				//设置为输出
	gpio_initstruct.GPIO_Pin = GPIO_Pin_9;						//将初始化的Pin脚
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率
	
	GPIO_Init(GPIOB, &gpio_initstruct);							//初始化GPIO
	
	Beep_Set(BEEP_OFF);											//初始化完成后，关闭蜂鸣器

}

/*
************************************************************
*	函数名称：	Beep_Set
*
*	函数功能：	蜂鸣器控制
*
*	入口参数：	status：开关蜂鸣器
*
*	返回参数：	无
*
*	说明：		开-BEEP_ON		关-BEEP_OFF
************************************************************
*/
void Beep_Set(uint8_t status)
{
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_9, status == BEEP_ON ? Bit_RESET : Bit_SET);		//如果status等于BEEP_ON，则返回Bit_SET，否则返回Bit_RESET
	
	Beep_Status = status;

}
