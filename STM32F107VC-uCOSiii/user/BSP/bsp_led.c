#include "bsp_led.h"

void bspLedInit(void)
{
	GPIO_InitTypeDef GPIO_InitStrutor;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStrutor.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_InitStrutor.GPIO_Pin 	= GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStrutor.GPIO_Speed	= GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStrutor);
	
	GPIO_SetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14);
}
