#ifndef __KEY_H
#define __KEY_H
#include "sys.h"
#define KEY GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)//KEY 0

#define KEY_PRES 1 //KEY

void KEY_Init(void); //³õÊ¼»¯º¯Êý
u8 KEY_Scan(u8); //
//#define KEY_EVENT (0x01 << 0)
#endif
