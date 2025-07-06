#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "usart.h"

void Mq2_Init(void) {
	
		GPIO_InitTypeDef GPIO_InitStructure;
		ADC_InitTypeDef ADC_InitStructure;
		
    // ����GPIOA��ADC1��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    // ����PA5Ϊģ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC1����
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // ����ADC1��ͨ��11Ϊ55.5����������
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);

    // ����ADC1
    ADC_Cmd(ADC1, ENABLE);

    // ��ʼ��ADCУ׼�Ĵ���   
    ADC_ResetCalibration(ADC1);
    // �ȴ�У׼�Ĵ�����ʼ�����
    while(ADC_GetResetCalibrationStatus(ADC1));
    // ����ADCУ׼
    ADC_StartCalibration(ADC1);
    // �ȴ�У׼���
    while(ADC_GetCalibrationStatus(ADC1));
    
    // ��ʼADCת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}



// ��ADCֵת��Ϊ��ѹֵ
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

// ��������Ũ�ȣ������Ϊʾ������Ҫ����ʵ�����ߵ���
float estimateSmokeLevel(float voltage) {
    // ����ļ�����Ҫ����MQ2��������ʵ�����߽���
    // ����������һ���򵥵ı�����ϵ���ǳ����ԵĹ��ƣ�
    return voltage / 3.3f * 100.0f; // �����ѹԽ�ߣ�����Ũ��Խ��
}


//int main(void) {
//    // ��ʼ��ADC
//    ADC_Init();
//    
//    uint16_t adc_value;
//    
//    while (1) {
//        // ��ȡADCת��ֵ
//        if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) {
//            adc_value = ADC_GetConversionValue(ADC1);
//            // ���������Ӵ��룬ͨ�����ڷ���adc_value
//            // ����: printf("ADC Value: %d\n", adc_value);
//        }
//    }
//}
