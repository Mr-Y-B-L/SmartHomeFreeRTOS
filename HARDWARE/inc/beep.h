#ifndef _BEEP_H_
#define _BEEP_H_

//单片机头文件
#include "stm32f10x.h"


#define BEEP_ON		1

#define BEEP_OFF	0

extern uint8_t Beep_Status;


void Beep_Init(void);

void Beep_Set(uint8_t status);


#endif
