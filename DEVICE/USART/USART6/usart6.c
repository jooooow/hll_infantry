#include "usart6.h"

u8 gyro_rx_buf[11];

void InitUsart6(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);
 
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOG,&GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	
	USART_Cmd(USART6, ENABLE);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	USART_DMACmd(USART6,USART_DMAReq_Rx,ENABLE);
}

void InitUsart6DMA(void)
{
	DMA_InitTypeDef   dma;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE);
	dma.DMA_Channel= DMA_Channel_5;
	dma.DMA_PeripheralBaseAddr = (uint32_t)&(USART6->DR);
	dma.DMA_Memory0BaseAddr = (uint32_t)gyro_rx_buf;
	dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
	dma.DMA_BufferSize = 11;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Circular;
	dma.DMA_Priority = DMA_Priority_VeryHigh;
	dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	dma.DMA_MemoryBurst = DMA_Mode_Normal;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	
	DMA_Init(DMA2_Stream1,&dma);
	DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA2_Stream1,ENABLE);
}

void InitUsart6NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void InitUsart6DMANVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void DMA2_Stream1_IRQHandler(void)
{   
  if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1))
  {
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1);
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
	}
}

void usart6_output(u8* data, u8 len)
{ 	
	u8 i;
	for(i = 0; i < len; i++)
	{
		while((USART6->SR&0X40)==0);
		USART6->DR = (u8)data[i];      
	}
}
