#include "timer3.h"
#include "includes.h"

void InitTimer3(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	TIM_TimeBaseInitStructure.TIM_Period = 1000-1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 90-1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
}

void InitTimer3NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

extern u8 flag;

void TIM3_IRQHandler(void)  
{
	if (TIM_GetITStatus(TIM3,TIM_IT_Update)!= RESET) 
	{
		TAG_TIMER = ~TAG_TIMER;
		ControlTask();
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}
