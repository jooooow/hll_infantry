#include "control_task.h"

//控制周期 单位ms
#define GIMBAL_CONTROL_CYCLE  1
#define CHASSIS_CONTROL_CYCLE 4

#define _max(a,b) (((a) > (b)) ? (a) : (b))
#define _min(a,b) (((a) < (b)) ? (a) : (b))

void InitControlTask(void)
{
	InitCan1();
	InitCan1NVIC();
	
	/*初始化遥控器任务*/
	InitRemoterTask();
	
	/*初始化底盘任务*/
	InitChassisControlTask();
	
	/*初始化云台任务*/
	InitGimbalControlTask();
	
	/*初始化读裁判系统任务*/
	//InitJudgeSystemTask();
	
	InitLed();
	InitTimer3();
	InitTimer3NVIC();
	
	InitUsart1();
	InitUsart1DMA();
	InitUsart1DMANVIC();
	
	InitUsart3();
	
	/*等待初始化完成*/
	delay_ms(1000);
}

extern IncrementPid fl_pid;
extern IncrementPid fr_pid;
extern IncrementPid bl_pid;
extern IncrementPid br_pid;
extern RemoteController remote_controller,old_remote_controller;
extern u8 wait_can_init_ok;
extern u8 is_remote_received;
extern u8 sbus_rx_buffer[25]; 

u8 remote_listen_cnt = 0;

void ControlTask(void)
{
	static u8 control_cycle_cnt = 0;
	
	/*监控遥控器*/
	RemoteListen();
	
	/*遥控任务*/
	if(is_remote_received)
	{
		RemoteTask();
	}
	
	/*云台&底盘任务*/
	if(wait_can_init_ok)
	{
		control_cycle_cnt++;
		if(control_cycle_cnt % GIMBAL_CONTROL_CYCLE == 0)
		{
			/*云台控制任务*/
			GimbalControlTask();
		}
		if(control_cycle_cnt % CHASSIS_CONTROL_CYCLE == 0)
		{
			/*底盘控制任务*/
			ChassisControlTask();
		}
	}
	
	/*历史保存*/
	SaveHistory();
	
	if(control_cycle_cnt >= _max(GIMBAL_CONTROL_CYCLE,CHASSIS_CONTROL_CYCLE))
		control_cycle_cnt = 0;
}


/*
  函数名：SaveHistory
  描述  ：保存本次变量作为下次的历史记录
  参数  ：无
  返回值：无
*/
void SaveHistory(void)
{
	old_remote_controller = remote_controller;
}


/*
  函数名：RemoteListen
  描述  ：监控遥控器是否丢失
          50ms没有接收到视为丢失
  参数  ：无
  返回值：无
*/
void RemoteListen(void)
{
	if(remote_listen_cnt < 50)
		remote_listen_cnt++;
	if(remote_listen_cnt == 50)
	{
		remote_listen_cnt++;
		printf("remote loss\r\n");
		memset(sbus_rx_buffer,0,18);
		/*丢失 设置全0*/
		SetRemoteValue(&remote_controller,0,0,0,0,0,0);
	}
}