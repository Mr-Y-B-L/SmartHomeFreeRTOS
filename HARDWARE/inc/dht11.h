
#ifndef __DHT11_H
#define __DHT11_H 
//PA0
#define DHT11_IO_IN()  {GPIOA->CRL &= 0XFFFFFFF0;GPIOA->CRL |= 8;}       //PA0控制DHT11，设置为输出
#define DHT11_IO_OUT() {GPIOA->CRL &= 0XFFFFFFF0;GPIOA->CRL |= 3;}       //PA0控制DHT11，设置为输入
//IO操作函数										   
#define DHT11_OUT(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_0, (BitAction)x)  //PA0控制DHT11
#define DHT11_DQ_IN     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)        //PA0控制DHT11	
   
void DHT11_Rst(void);//复位DHT11
char DHT11_Check(void);//检测DHT11
char DHT11_Read_Bit(void);//读取一位
char DHT11_Read_Byte(void);//读取一个字节
char dht11_read_data(char *temp, char *humi);//读取数据
char dht11_init(void);//初始化DHT11

#endif















