#ifndef _FAN_H_
#define _FAN_H_

//单片机头文件
#include "stm32f10x.h"


#define FAN_ON	80

#define FAN_OFF	0

extern uint8_t Fan_Status;


void Fan_Init(void);

void Fan_Set(uint8_t status);

void Fan_Adj(uint8_t value);


#endif
