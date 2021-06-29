#ifndef __BSP_USART2_H
#define __BSP_USART2_H

#include "stm32f10x.h"

extern void BSP_usart2Config(uint32_t baud);
extern void BSP_usart2SendByte(uint8_t ch);

#endif
