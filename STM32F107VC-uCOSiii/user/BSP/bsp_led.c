#include "bsp_led.h"

void bspLedInit(void)
{
	GPIO_InitTypeDef GPIO_InitStrutor;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStrutor.GPIO_Mode 	= GPIO_Mode_Out_PP;
	GPIO_InitStrutor.GPIO_Pin 	= GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStrutor.GPIO_Speed	= GPIO_Speed_10MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStrutor);
	
	GPIO_ResetBits(GPIOD, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

void bspLedToggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	static int toggle = 0;
	
	if(toggle == 0)
	{
		GPIO_SetBits(GPIOx, GPIO_Pin);
		toggle = 1;
	}
	else
	{
		GPIO_ResetBits(GPIOx, GPIO_Pin);
		toggle = 0;		
	}
}
