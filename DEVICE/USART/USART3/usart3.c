#include "usart3.h"

struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;  

int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
	USART3->DR = (u8) ch;      
	return ch;
}

void Usart3Log(u8* data, u8 len)
{ 	
	u8 i;
	for(i = 0; i < len; i++)
	{
		while((USART3->SR&0X40)==0);
		USART3->DR = (u8)data[i];      
	}
}

void InitUsart3(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 230400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	//开启接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART3, ENABLE);
}

void InitUsart3NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

u8 usart3_recv_buf[64];
u8 usart3_recv_cnt = 0;
u8 is_start_new_frame = 0;
u8 usart3_recv_a_frame_flag = 0;

void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		//USART_SendData(USART3,USART3->DR);
		if(USART3->DR == '[')
		{
			is_start_new_frame = 1;
		}
		
		if(is_start_new_frame)
		{
			usart3_recv_buf[usart3_recv_cnt] = USART3->DR;
			//USART_SendData(USART3,USART3->DR);
			usart3_recv_cnt++;
		}
		
		if(USART3->DR == ']' && is_start_new_frame)
		{
			usart3_recv_a_frame_flag = 1;
			//USART_SendData(USART3,'k');
			is_start_new_frame = 0;
		}
	}
}