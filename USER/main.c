#include "includes.h"
  
extern short chassis_motor_speed[6];
extern IncrementPid fl_pid;
extern IncrementPid fr_pid;
extern IncrementPid bl_pid;
extern IncrementPid br_pid;
extern u8 is_recv_gyro_data;
extern u16 recv_gyro_data_cnt;
extern u8 gyro_buf[11];
extern u8 gyro_rx_buf[11];
u16 raw_yaw;
float yaw;

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*初始化控制任务*/
	InitControlTask();

	while(1)
	{
		
	}
	return 0;
}
