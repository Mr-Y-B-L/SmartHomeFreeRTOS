#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "usart.h"

void Mq2_Init(void) {
	
		GPIO_InitTypeDef GPIO_InitStructure;
		ADC_InitTypeDef ADC_InitStructure;
		
    // 开启GPIOA和ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // 配置PA5为模拟输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC1配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 配置ADC1的通道11为55.5个采样周期
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);

    // 启用ADC1
    ADC_Cmd(ADC1, ENABLE);

    // 初始化ADC校准寄存器   
    ADC_ResetCalibration(ADC1);
    // 等待校准寄存器初始化完成
    while(ADC_GetResetCalibrationStatus(ADC1));
    // 启动ADC校准
    ADC_StartCalibration(ADC1);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(ADC1));
    
    // 开始ADC转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



// 将ADC值转换为电压值
float adcValueToVoltage(void)
{
	uint16_t adc_value;
	
	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) 
	{
		adc_value = ADC_GetConversionValue(ADC1);
	}
	UsartPrintf(USART_DEBUG, "ADC_VALUE----%d\r\n",adc_value);
	
    return (float)adc_value * 3.3f / 4095.0f;
	  //return (float)adc_value * 6.6f / 4095.0f;
}

// 估计烟雾浓度，这里仅为示例，需要根据实际曲线调整
float estimateSmokeLevel(float voltage) {
    // 这里的计算需要根据MQ2传感器的实际曲线进行
    // 假设我们有一个简单的比例关系（非常粗略的估计）
    return voltage / 3.3f * 100.0f; // 假设电压越高，烟雾浓度越高
}


//int main(void) {
//    // 初始化ADC
//    ADC_Init();
//    
//    uint16_t adc_value;
//    
//    while (1) {
//        // 读取ADC转换值
//        if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
//            adc_value = ADC_GetConversionValue(ADC1);
//            // 这里可以添加代码，通过串口发送adc_value
//            // 例如: printf("ADC Value: %d\n", adc_value);
//        }
//    }
//}
