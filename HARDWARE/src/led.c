 
/*-------------------------------------------------*/
/*         	                                       */
/*           		 LED����           	 		   */
/*                                                 */
/*-------------------------------------------------*/

// Ӳ�����ӣ�
// PD2  LED1
// PA8  LED2
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�

LED_INFO led_info = {0};
/*-------------------------------------------------*/
/*����������ʼ��LED1��LED2����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led_init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOC, GPIO_Pin_13); 				//PD2 ����� 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
//	GPIO_Init(GPIOA, &GPIO_InitStructure); 
//	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 ����� 
}

/*-------------------------------------------------*/
/*��������LED1����                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led1_on(void)
{			
	GPIO_ResetBits(GPIOC, GPIO_Pin_13); 			    //PC13 �����
} 

/*-------------------------------------------------*/
/*��������LED1�ر�                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led1_off(void)
{		
	GPIO_SetBits(GPIOC, GPIO_Pin_13); 				//PC13 �����
}

/*
************************************************************
*	�������ƣ�	LED_Set
*
*	�������ܣ�	LED����
*
*	��ڲ�����	status������LED
*
*	���ز�����	��
*
*	˵����		��-LED_ON		��-LED_OFF
************************************************************
*/
void Led_Set(_Bool status)
{
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, status == LED_ON ? Bit_RESET : Bit_SET);		//���status����BEEP_ON���򷵻�Bit_SET�����򷵻�Bit_RESET
	
	led_info.Led_Status = status;

}





/*-------------------------------------------------*/
/*��������LED2����                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
//void led2_on(void)
//{			
//	GPIO_ResetBits(GPIOA, GPIO_Pin_8); 			    //PA8 �����
//} 
/*-------------------------------------------------*/
/*��������LED2�ر�                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
//void led2_off(void)
//{		
//	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 �����
//}

