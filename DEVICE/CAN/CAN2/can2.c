#include "can2.h"
#include "listener.h"

extern short motor_speed_arr[7];
short pitch_can2prep_output = 0;
short yaw_can2prep_output = 0;
short wave_can2prep_output = 0;

void InitCan2(void)
{
    CAN_InitTypeDef        can;
    CAN_FilterInitTypeDef  can_filter;
    GPIO_InitTypeDef       GPIO_InitStructure;
    NVIC_InitTypeDef       nvic;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);

	
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
		nvic.NVIC_IRQChannel = CAN2_RX0_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 2;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
		
		CAN_DeInit(CAN2);
    CAN_StructInit(&can);
		
		can.CAN_TTCM = DISABLE;
    can.CAN_ABOM = DISABLE;    
    can.CAN_AWUM = DISABLE;    
    can.CAN_NART = DISABLE;    
    can.CAN_RFLM = DISABLE;    
    can.CAN_TXFP = ENABLE; 

		
    can.CAN_Mode = CAN_Mode_Normal;
    can.CAN_SJW  = CAN_SJW_1tq;
    can.CAN_BS1 = 8;
    can.CAN_BS2 = 4;
    can.CAN_Prescaler = 3;   //CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN2, &can);

		can_filter.CAN_FilterNumber=14;
		can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
		can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
		can_filter.CAN_FilterIdHigh=0x0000;
		can_filter.CAN_FilterIdLow=0x0000;
		can_filter.CAN_FilterMaskIdHigh=0x0000;
		can_filter.CAN_FilterMaskIdLow=0x0000;
		can_filter.CAN_FilterFIFOAssignment=0;
		can_filter.CAN_FilterActivation=ENABLE;
		CAN_FilterInit(&can_filter);
    
    CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);
    CAN_ITConfig(CAN2,CAN_IT_TME,ENABLE); 	
}

void CAN2_TX_IRQHandler(void) //CAN TX
{
  if (CAN_GetITStatus(CAN2,CAN_IT_TME)!= RESET) 
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_TME);
  }
}


extern CanRxMsg rx_message;
extern char usart3_tx_buffer[80];
extern GimbalPID_t yaw_pid, pitch_pid;
extern u8 wait_can_init_ok;
extern u8 listen_cnt[10];

short yaw_machanical_angle = 0;
short old_yaw_machanical_angle = 0;
short yaw_machanical_round = 0;

void CAN2_RX0_IRQHandler(void)
{ 	
	static u16 can_cnt = 100;
	CanRxMsg rx_message;
	if (CAN_GetITStatus(CAN2,CAN_IT_FMP0)!= RESET)
	{ 
		CAN_ClearITPendingBit(CAN2, CAN_IT_FF0);
		CAN_ClearFlag(CAN2, CAN_FLAG_FF0); 
		CAN_Receive(CAN2, CAN_FIFO0, &rx_message);
		
		if(can_cnt > 0)
		{
			can_cnt--;
			if(can_cnt == 0)
				wait_can_init_ok++;
		}
		
		switch(rx_message.StdId)
		{
			case 0x205:
				RELOAD_MOTOR(YAW);
				yaw_machanical_angle = (rx_message.Data[0]<<8)|(rx_message.Data[1]);	
				
				if(yaw_machanical_angle - old_yaw_machanical_angle > 4096)
					yaw_machanical_round--;
				else if(yaw_machanical_angle - old_yaw_machanical_angle < -4096)
					yaw_machanical_round++;
				
				motor_speed_arr[YAW] = yaw_machanical_angle + yaw_machanical_round * 8192;
				old_yaw_machanical_angle = yaw_machanical_angle;
		  break;
			case 0x206:
				RELOAD_MOTOR(PITCH);
				motor_speed_arr[PITCH] = (rx_message.Data[0]<<8)|(rx_message.Data[1]);	
			break;
			case 0x207:
				RELOAD_MOTOR(WAVE);
				motor_speed_arr[WAVE] = (rx_message.Data[2]<<8)|(rx_message.Data[3]);	
			break;
		}
	}
}

void Can2Send2Gimbal(void)
{
    CanTxMsg tx_message1; 
    tx_message1.StdId = 0x1FF;
    tx_message1.IDE = CAN_Id_Standard;
    tx_message1.RTR = CAN_RTR_Data;
    tx_message1.DLC = 0x08;
    tx_message1.Data[0] = (u8)(yaw_can2prep_output >> 8);		
    tx_message1.Data[1] = (u8)yaw_can2prep_output;
    tx_message1.Data[2] = (u8)(pitch_can2prep_output >> 8);		
    tx_message1.Data[3] = (u8)pitch_can2prep_output;
		tx_message1.Data[4] = (u8)(wave_can2prep_output >> 8);
		tx_message1.Data[5] = (u8)wave_can2prep_output;
		tx_message1.Data[6] = 0;
		tx_message1.Data[7] = 0;
    CAN_Transmit(CAN2,&tx_message1);	
}
