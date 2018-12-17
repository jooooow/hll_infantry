#include "judge_system.h"

u8 usart6_rx_buf[200];
u8 usart6_rx_cnt = 0;
tFrame *frame;
float power,current = 0;

void InitJudgeSystemTask(void)
{
	InitUsart6();
	InitUsart6NVIC();
}

u8 ReadJudgeSystem(void)
{
	static u8 res = 0;
	static u8 verify_success = 0;
	static u8 is_start_new_frame = 0;
	
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET) 
	{
		res = USART6->DR;
		if(res == 0xA5 && usart6_rx_cnt > 9)
		{
			verify_success = DataVerify();
			is_start_new_frame = 0;
			usart6_rx_cnt = 0;
		}
		if(res == 0xA5 && is_start_new_frame == 0)
		{
			is_start_new_frame = 1;
		}
		
		if(is_start_new_frame == 1)
		{
			usart6_rx_buf[usart6_rx_cnt] = res;
			usart6_rx_cnt++;
		}
	}
	
	return verify_success;
}

