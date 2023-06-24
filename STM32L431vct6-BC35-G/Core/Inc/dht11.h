#ifndef __DHT11_H__
#define __DHT11_H__

#include "stm32l4xx_hal.h"

//******************************外部函数定义************************************/
extern uint8_t DHT11_Read_Data(float *temp,float *humidity);
extern void DHT_Init(void);
#endif

