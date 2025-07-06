



//Ӳ������
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
//PWM����
//����value����Χ0-99������ռ�ձ�

void Fan_Adj(uint8_t value)
{
	
	TIM_SetCompare1(TIM1, 999); // ����ռ�ձȽϵ�ʱ��������Ȼ�޷�ֱ������
	delay_ms(20);
	TIM_SetCompare1(TIM1, value*10); // ����ռ�ձ�
	
	if(value>0)
		Fan_Status=1;
	else 
		Fan_Status=0;
}


void Fan_Set(uint8_t value)
{
	
	Fan_Adj(value);
}
