
#ifndef __DHT11_H
#define __DHT11_H 
//PA0
#define DHT11_IO_IN()  {GPIOA->CRL &= 0XFFFFFFF0;GPIOA->CRL |= 8;}       //PA0����DHT11������Ϊ���
#define DHT11_IO_OUT() {GPIOA->CRL &= 0XFFFFFFF0;GPIOA->CRL |= 3;}       //PA0����DHT11������Ϊ����
//IO��������										   
#define DHT11_OUT(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)x)  //PA0����DHT11
#define DHT11_DQ_IN     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)        //PA0����DHT11	
   
void DHT11_Rst(void);//��λDHT11
char DHT11_Check(void);//���DHT11
char DHT11_Read_Bit(void);//��ȡһλ
char DHT11_Read_Byte(void);//��ȡһ���ֽ�
char dht11_read_data(char *temp, char *humi);//��ȡ����
char dht11_init(void);//��ʼ��DHT11

#endif















