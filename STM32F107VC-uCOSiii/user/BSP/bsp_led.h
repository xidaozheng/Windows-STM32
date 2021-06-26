#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"


extern void bspLedInit(void);
extern void bspLedToggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif

