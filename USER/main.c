#include "control_task.h"
#include "control.h"

extern short motor_speed_arr[7];
extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;
extern PositionPid_t yaw_gyro_pid;

extern Angle_t yaw;
extern GimbalPID_t pitch_pid,yaw_pid;

extern RemoteController_t remote_controller;
extern RemoteController_t old_remote_controller;
extern PositionPid_t wave_motor_pid;
extern u8 is_wave_block;

extern u16 mid_speed;
extern u16 top_speed;

extern float yaw_mag;

extern short mag_data[3];
extern float mpu6500_yaw;

extern PositionPid_t yaw_machanical_pid;
extern PositionPid_t yaw_gyro_pid;

extern unsigned int can_rx_cnt_arr[7];
extern u16 temp_dev_angle_tar;
u8 yaw_com_report_arr[4] = {0xFF,0x00,0x00,0xFE};

static void ReportYaw2Com(void);

extern u8 usart3_recv_buf[64];
extern u8 usart3_recv_cnt;
extern u8 usart3_recv_a_frame_flag;
extern PositionPid_t auto_find_yaw_pid;
	
int main()
{
	u8 i;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*初始化控制任务*/
	InitControlTask();

	while(1)
	{
//		if(usart3_recv_a_frame_flag)
//		{
//			LED_GREEN_BLINK;
//			for(i = 0; i < usart3_recv_cnt; i++)
//			{
//				while((USART3->SR&0X40)==0);
//				USART_SendData(USART3,usart3_recv_buf[i]);
//			}
//			usart3_recv_a_frame_flag = 0;
//			usart3_recv_cnt = 0;
//		}
		
		//printf("%.4f\t%.4f\t%.4f\r\n",auto_find_yaw_pid.tar,yaw_gyro_pid.tar,auto_find_yaw_pid.output);
		//printf("u16 temp_dev_angle_tar : %d\r\n",temp_dev_angle_tar);
		//printf("%.4f\t%.4f\t%.4f\r\n",remote_controller.rc.ch2/10.0,yaw_gyro_pid.tar,yaw_gyro_pid.cur);
		//ReportYaw2Com();
		//printf("%d\t%d\t%d\t%d\r\n",can_rx_cnt_arr[0],can_rx_cnt_arr[1],can_rx_cnt_arr[2],can_rx_cnt_arr[3]);
		printf("%.4f\t%.4f\t%.4f\r\n",yaw_gyro_pid.tar,yaw_gyro_pid.cur,yaw_gyro_pid.output);
		//printf("yaw : %.2f\t%.2f\r\n",yaw_machanical_pid.cur,yaw_machanical_pid.output);
		//printf("yaw : %.4f\t yaw_w : %.4f\r\n",yaw.theta,yaw.w_cur);
		//printf("mpu6500_yaw : %.4f\r\n",mpu6500_yaw);
		//printf("%d,%d\r\n",motor_speed_arr[YAW],motor_speed_arr[PITCH]);
		//printf("is_wave_block : %d\t",is_wave_block);
		//printf("%.2f\t%.2f\t%.2f\t%.2f\r\n",wave_motor_pid.cur,wave_motor_pid.cur,wave_motor_pid.error,wave_motor_pid.output);
		//printf("motor_speed_arr[WAVE] : %d\r\n",motor_speed_arr[WAVE]);
		//printf("mid : %d\ttop : %d\r\n",mid_speed,top_speed);
		//printf("mag_yaw : %.4f\tgyrp_yaw : %.4f\r\n",yaw_mag,yaw.theta);
		//printf("%d;%d\r\n",mag_data[0],mag_data[1]);
		delay_ms(5);
	}
	return 0;
}

static void ReportYaw2Com(void)
{
	u8 i;
	yaw_com_report_arr[1] = ((short)yaw.theta >> 8) & 0xFF;
	yaw_com_report_arr[2] = (short)yaw.theta & 0xFF;
	for(i = 0; i < 4; i++)
	{
		USART_SendData(USART3,yaw_com_report_arr[i]);
		while((USART3->SR&0X40)==0);
	}
}
