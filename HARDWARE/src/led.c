 
/*-------------------------------------------------*/
/*         	                                       */
/*           		 LED控制           	 		   */
/*                                                 */
/*-------------------------------------------------*/

// 硬件连接：
// PD2  LED1
// PA8  LED2
#include "stm32f10x.h"  //包含需要的头文件
#include "led.h"        //包含需要的头文件

LED_INFO led_info = {0};
/*-------------------------------------------------*/
/*函数名：初始化LED1和LED2函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led_init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOC, GPIO_Pin_13); 				//PD2 输出高 
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
//	GPIO_Init(GPIOA, &GPIO_InitStructure); 
//	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 输出高 
}

/*-------------------------------------------------*/
/*函数名：LED1开启                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led1_on(void)
{			
	GPIO_ResetBits(GPIOC, GPIO_Pin_13); 			    //PC13 输出低
} 

/*-------------------------------------------------*/
/*函数名：LED1关闭                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led1_off(void)
{		
	GPIO_SetBits(GPIOC, GPIO_Pin_13); 				//PC13 输出高
}

/*
************************************************************
*	函数名称：	LED_Set
*
*	函数功能：	LED控制
*
*	入口参数：	status：开关LED
*
*	返回参数：	无
*
*	说明：		开-LED_ON		关-LED_OFF
************************************************************
*/
void Led_Set(_Bool status)
{
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, status == LED_ON ? Bit_RESET : Bit_SET);		//如果status等于BEEP_ON，则返回Bit_SET，否则返回Bit_RESET
	
	led_info.Led_Status = status;

}





/*-------------------------------------------------*/
/*函数名：LED2开启                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void led2_on(void)
//{			
//	GPIO_ResetBits(GPIOA, GPIO_Pin_8); 			    //PA8 输出低
//} 
/*-------------------------------------------------*/
/*函数名：LED2关闭                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void led2_off(void)
//{		
//	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 输出高
//}

