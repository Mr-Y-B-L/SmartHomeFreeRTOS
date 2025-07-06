



//硬件驱动
#include "fan.h"
#include "delay.h"
#include "fan_pwm.h"



uint8_t Fan_Status=0;




void Fan_Init(void)
{

	RCC_Configuration();
  GPIO_Configuration();
	TIM1_Configuration();

}


extern u8 fan_adj;
//PWM调速
//输入value：范围0-99，设置占空比

void Fan_Adj(uint8_t value)
{
	
	TIM_SetCompare1(TIM1, 999); // 用于占空比较低时启动，不然无法直接启动
	delay_ms(20);
	TIM_SetCompare1(TIM1, value*10); // 设置占空比
	
	if(value>0)
		Fan_Status=1;
	else 
		Fan_Status=0;
}


void Fan_Set(uint8_t value)
{
	
	Fan_Adj(value);
}
