#include "can1.h"

void InitCan1(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
	CAN_InitTypeDef        can;
	CAN_FilterInitTypeDef  can_filter;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_CAN1);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_CAN1);
    
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
  can.CAN_Prescaler = 3;   //CAN BaudRate 45/(1+9+5)/3=1Mbps
  CAN_Init(CAN1, &can);

	can_filter.CAN_FilterNumber = 0;
	can_filter.CAN_FilterMode=CAN_FilterMode_IdMask;
	can_filter.CAN_FilterScale=CAN_FilterScale_32bit;
	can_filter.CAN_FilterIdHigh= 0x0000;
	can_filter.CAN_FilterIdLow = 0x0000;
	can_filter.CAN_FilterMaskIdHigh = 0x0000;
	can_filter.CAN_FilterMaskIdLow  = 0x0000;
	can_filter.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;
	can_filter.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&can_filter);
  
  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
  CAN_ITConfig(CAN1,CAN_IT_TME,ENABLE); 
}

void InitCan1NVIC(void)
{
	NVIC_InitTypeDef       nvic;
	
	nvic.NVIC_IRQChannel = CAN1_RX0_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 2;
  nvic.NVIC_IRQChannelSubPriority = 1;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);
}

short motor_speed_arr[7] = {0,0,0,0,0,0,0};
unsigned int can_rx_cnt_arr[7] = {0,0,0,0,0,0,0};

u8 wait_can_init_ok = 0;
extern u16 ONLINE_FLAG;
extern u8 listen_cnt[10];

void CAN1_RX0_IRQHandler(void)
{
	static u16 can_cnt = 100;
	CanRxMsg can1_rx_msg;
	if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
	{
		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
		CAN_Receive(CAN1, CAN_FIFO0, &can1_rx_msg);	
		
		/*等待数据稳定*/
		if(can_cnt > 0)
		{
			can_cnt--;
			if(can_cnt == 0)
				wait_can_init_ok++;
		}
		
		if(can1_rx_msg.StdId >= 0x201 && can1_rx_msg.StdId <= 0x207)
		{
			switch(can1_rx_msg.StdId)
			{
				case 0x201:
					can_rx_cnt_arr[FL]++;
				  RELOAD_MOTOR(FL);
					motor_speed_arr[FL] = (can1_rx_msg.Data[2]<<8)|(can1_rx_msg.Data[3]);
					break;
				case 0x202:
					can_rx_cnt_arr[FR]++;
				  RELOAD_MOTOR(FR);
					motor_speed_arr[FR] = (can1_rx_msg.Data[2]<<8)|(can1_rx_msg.Data[3]);
					break;
				case 0x203:
					can_rx_cnt_arr[BL]++;
				  RELOAD_MOTOR(BL);
					motor_speed_arr[BL] = (can1_rx_msg.Data[2]<<8)|(can1_rx_msg.Data[3]);
					break;
				case 0x204:
					can_rx_cnt_arr[BR]++;
				  RELOAD_MOTOR(BR);
					motor_speed_arr[BR] = (can1_rx_msg.Data[2]<<8)|(can1_rx_msg.Data[3]);
					break;
//				case 0x207:
//					can_rx_cnt_arr[WAVE]++;
//					RELOAD_MOTOR(WAVE);
//				  motor_speed_arr[WAVE] = (can1_rx_msg.Data[2]<<8)|(can1_rx_msg.Data[3]);
//				  break;
				default:break;
			}
		}
  }
}


/*
  函数名：Can1SendMsg2Chassis
  描述  ：发送电流值给四个轮子
  参数  ：fl -- 左前轮电流值
				  fr -- 右前轮电流值
          bl -- 左后轮电流值
          br -- 右后轮电流值
  返回值：无
*/
void Can1SendMsg2Chassis(short fl, short fr, short bl, short br)
{
		CanTxMsg tx_message;
    tx_message.StdId = 0x200;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    tx_message.Data[0] = (uint8_t)(fl >> 8);
    tx_message.Data[1] = (uint8_t)fl;
    tx_message.Data[2] = (uint8_t)(fr >> 8);
    tx_message.Data[3] = (uint8_t)fr;
    tx_message.Data[4] = (uint8_t)(bl >> 8);
    tx_message.Data[5] = (uint8_t)bl;
    tx_message.Data[6] = (uint8_t)(br >> 8);
    tx_message.Data[7] = (uint8_t)br;
    CAN_Transmit(CAN1,&tx_message);
}

void Can1SendMsg2WaveWheel(short current)
{
	CanTxMsg tx_message;
    tx_message.StdId = 0x1FF;
    tx_message.IDE = CAN_Id_Standard;
    tx_message.RTR = CAN_RTR_Data;
    tx_message.DLC = 0x08;
    tx_message.Data[0] = 0;
    tx_message.Data[1] = 0;
    tx_message.Data[2] = 0;
    tx_message.Data[3] = 0;
    tx_message.Data[4] = (uint8_t)(current >> 8);
    tx_message.Data[5] = (uint8_t)current;
    tx_message.Data[6] = 0;
    tx_message.Data[7] = 0;
    CAN_Transmit(CAN1,&tx_message);
}
