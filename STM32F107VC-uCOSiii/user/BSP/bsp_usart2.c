#include <includes.h>

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	NVIC_InitStructure.NVIC_IRQChannel 					= USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd				= ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		= 0;
	
	NVIC_Init(&NVIC_InitStructure);
}

void BSP_usart2Config(uint32_t baud)
{
	GPIO_InitTypeDef 	GPIO_InitStrutor;
	USART_InitTypeDef 	USART_InitStructor;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);		//������뿪������ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);									//������ƣ�������Ҫ��ӳ��
	
	GPIO_InitStrutor.GPIO_Mode 	= GPIO_Mode_AF_PP;	//�����������
	GPIO_InitStrutor.GPIO_Pin 	= GPIO_Pin_5;		//USART2_TX
	GPIO_InitStrutor.GPIO_Speed	= GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOD, &GPIO_InitStrutor);
	
	GPIO_InitStrutor.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;	//��������
	GPIO_InitStrutor.GPIO_Pin 	= GPIO_Pin_6;				//USART2_RX	
	GPIO_Init(GPIOD, &GPIO_InitStrutor);
	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_InitStructor.USART_BaudRate				= baud;
	USART_InitStructor.USART_HardwareFlowControl	= USART_HardwareFlowControl_None;
	USART_InitStructor.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructor.USART_Parity					= USART_Parity_No;
	USART_InitStructor.USART_StopBits				= USART_StopBits_1;
	USART_InitStructor.USART_WordLength				= USART_WordLength_8b;
	
	USART_Init(USART2, &USART_InitStructor);

	NVIC_Configuration();
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
}

void BSP_usart2SendByte(uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(USART2, ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	
}

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
	/* ����һ���ֽ����ݵ����� */
	USART_SendData(USART2, (uint8_t) ch);
	
	/* �ȴ�������� */
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		

	return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
	while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART2);
}

