#include "pwm.h"

void InitPWM(void)
{
	GPIO_InitTypeDef          gpio;
	TIM_TimeBaseInitTypeDef   tim;
	TIM_OCInitTypeDef         oc;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH ,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);
	
	gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOH,&gpio);
	
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource6, GPIO_AF_TIM12);
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource9, GPIO_AF_TIM12);
	//TIM12
	tim.TIM_Prescaler = 90-1;//90
	tim.TIM_CounterMode = TIM_CounterMode_Up;
	tim.TIM_Period = 2000;   //500hz
	tim.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM12,&tim);
	
	oc.TIM_OCMode = TIM_OCMode_PWM2;
	oc.TIM_OutputState = TIM_OutputState_Enable;
	oc.TIM_OutputNState = TIM_OutputState_Disable;
	oc.TIM_Pulse = 1000;
	oc.TIM_OCPolarity = TIM_OCPolarity_Low;
	oc.TIM_OCNPolarity = TIM_OCPolarity_High;
	oc.TIM_OCIdleState = TIM_OCIdleState_Reset;
	oc.TIM_OCNIdleState = TIM_OCIdleState_Set;
	
	TIM_OC1Init(TIM12,&oc);
	TIM_OC2Init(TIM12,&oc);
	
	TIM_OC1PreloadConfig(TIM12,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM12,TIM_OCPreload_Enable);
				
	TIM_ARRPreloadConfig(TIM12,ENABLE);
	
	TIM_Cmd(TIM12,ENABLE);
}

extern u16 mid_speed;
extern u16 top_speed;

void SetFricSpeed(u16 mid, u16 top)
{
	FricSpeedClose2(0,mid,5);
	FricSpeedClose2(1,top,5);
	TIM_SetCompare1(TIM12,mid_speed);
	TIM_SetCompare2(TIM12,top_speed);
}

void FricSpeedClose2(u8 id, u16 target, u16 dev)
{
	//mid
	if(id == 0)
	{
		if(mid_speed < target)
		{
			mid_speed += dev;
		}
		else if(mid_speed > target)
		{
			mid_speed = target;
		}
	}
	//top
	else if(id == 1)
	{
		if(top_speed < target)
		{
			top_speed += dev;
		}
		else if(top_speed > target)
		{
			top_speed = target;
		}
	}
}