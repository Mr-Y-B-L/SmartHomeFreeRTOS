
#ifndef __LED_H
#define __LED_H
typedef struct
{

	_Bool Led_Status;

} LED_INFO;

#define LED_ON		1

#define LED_OFF	0

extern LED_INFO led_info;

void led_init(void);               //≥ı ºªØ	

void led1_on(void);              
void led1_off(void);  


void Led_Set(_Bool status);
//void led2_on(void);             
//void led2_off(void);    

#endif
