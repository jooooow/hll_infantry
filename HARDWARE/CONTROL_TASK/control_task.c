#include "control_task.h"

#define _MAX(a,b) (((a) > (b)) ? (a) : (b))
#define _MIN(a,b) (((a) < (b)) ? (a) : (b))

/*整车状态机*/
State_t robot_FSM = {INITING,INITING,0};

u8 init_done_flag = 0;

extern u16 ONLINE_FLAG;
extern u16 OLD_ONLINE_FLAG;
extern u8 listen_cnt[8];

void InitControlTask(void)
{
	InitUsart3();
	
	printf("  init..\r\n");
	
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
	
	/*初始化发射机构任务*/
	InitShootTask();
	
	InitLed();
	InitTimer3();
	InitTimer3NVIC();
	
	InitUsart1();
	InitUsart1DMA();
	InitUsartIDLENVIC();
	
	/*等待初始化完成*/
	delay_ms(3500);
	
	/*初始化完成*/
	init_done_flag = 1;
	
	/*初始化监听任务*/
	InitListenerTask();
	
	printf("  init done\r\n\r\n");
	
	/*开始任务循环*/
	START_CONTROL_TASK;
}

extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;

extern RemoteController_t remote_controller;
extern RemoteController_t old_remote_controller;

extern u8 wait_can_init_ok;
extern u8 is_remote_received;
extern u8 sbus_rx_buffer[25]; 


void ControlTask(void)
{
	static u8 control_cycle_cnt = 0;
	
	/*更新整车状态机*/
	UpdateRobotFSM();
	
	/*响应状态变化*/
	OnRobotStateSwitch();
	
	/*监听任务*/
	ListenTask();
	
	/*遥控器任务*/
	if(is_remote_received)
	{
		is_remote_received = 0;
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
		if(control_cycle_cnt % WAVE_CONTROL_CYCLE == 0)
		{
			/*发射任务*/
			ShootTask();
		}
	}
	
	/*历史保存*/
	SaveHistory();
	
	if(control_cycle_cnt >= _MAX(WAVE_CONTROL_CYCLE,_MAX(GIMBAL_CONTROL_CYCLE,CHASSIS_CONTROL_CYCLE)))
		control_cycle_cnt = 0;
}


/*
  函数名：UpdateRobotFSM
  描述  ：更新FSM
  参数  ：无
  返回值：无
*/
void UpdateRobotFSM(void)
{
	switch(robot_FSM.cur_state)
	{
		case INITING:
			/*模块初始化完成 -- 转移为NOMAL*/
			if(init_done_flag == 1)
			{
				robot_FSM.cur_state = NORMAL;
			}
			break;
		case NORMAL:
			/*有模块离线 -- 转移为PROLEM*/
			if((~ONLINE_FLAG & MODULE_MASK) != 0)
			{
				robot_FSM.cur_state = PROBLEM;
			}
			
			/*发生严重错误 -- 转移为BLOCK*/
			//TO DO
			break;
		case PROBLEM:
			/*模块全部上线 -- 转移为NORMAL*/
			if((~ONLINE_FLAG & MODULE_MASK) == 0)
			{
				robot_FSM.cur_state = NORMAL;
			}
			
			/*发生严重错误 -- 转移为BLOCK*/
			//TO DO
			break;
		case BLOCK:
			/*复位 或者 其他操作*/
			//TO DO
			break;
		default:break;
	}
}


/*
  函数名：OnRobotStateSwitch
  描述  ：响应FSM变化
  参数  ：无
  返回值：无
*/
void OnRobotStateSwitch(void)
{
	if(STATE_SWITCHD_TO(INITING,NORMAL))
	{
		printf("**FSM [ switch from INITING to NORMAL! ]\r\n\r\n");
	}
	else if(STATE_SWITCHD_TO(NORMAL,PROBLEM))
	{
		printf("**FSM [ switch from NORMAL to PROBLEM! ]\r\n\r\n");
	}
	else if(STATE_SWITCHD_TO(PROBLEM,NORMAL))
	{
		printf("**FSM [ switch from PROBLEM to NORMAL! ]\r\n\r\n");
	}
}


/*
  函数名：SaveHistory
  描述  ：保存本次变量作为下次的历史记录
          用来实现状态切换
  参数  ：无
  返回值：无
*/
void SaveHistory(void)
{
	robot_FSM.old_state = robot_FSM.cur_state;
	old_remote_controller = remote_controller;
	OLD_ONLINE_FLAG = ONLINE_FLAG;
}

