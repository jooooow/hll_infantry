#include "remoter.h"
#include "usart1.h"

RemoteController_t remote_controller     = {0,0,0,0,3,3,  0,0,0,0,0,  0, REMOTER_CONTROL,REMOTER_CONTROL};
RemoteController_t old_remote_controller = {0,0,0,0,3,3,  0,0,0,0,0,  0, REMOTER_CONTROL,REMOTER_CONTROL};

u8 is_remote_valid = 0;
extern u8 sbus_rx_buffer[25]; 
extern u8 is_remote_received;
extern u8 listen_cnt[8];

extern ControlMode_t mode;

extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;

/*
  函数名：InitRemoterTask
  描述  ：初始化遥控接收任务
  参数  ：无
  返回值：无
*/
void InitRemoterTask(void)
{
	memset((void*)sbus_rx_buffer,0,18);
}


/*
  函数名：InitRemoterTask
  描述  ：初始化遥控接收任务
  参数  ：无
  返回值：无
*/
void RemoteTask(void)
{
	/*重新装填遥控器的Listener*/
	RELOAD_REMOTER;
	
	/*检查数据合法性*/
	if(!RemoterDataCheck())
	{
		/*不合法 重置DMA*/
		ResetUsart1DMA();
		SetRemoteValue(&remote_controller,0,0,0,0,0,0);
		return;
	}
	
	/*合法 解析出通道值并赋值给remote_controller*/
	ParseRemoterData(&remote_controller);
	
	/*根据switch更新控制状态*/
	UpdateRobotControlMode();
	
	/*响应switch状态变化*/
	OnSwitchChanged();
	
#if (REMOTE_DATA_LOG == 1)
	/*打印遥控器通道值*/
	RemoterDataLog(&remote_controller);
#endif
}


/*
  函数名：ParseRemoterData
  描述  ：将sbus原始数据转化为遥控器数据
  参数  ：remoter -- 当前遥控器数据结构体变量
  返回值：无
*/
void ParseRemoterData(RemoteController_t* remoter)
{
	static short ch0 = 0;
	static short ch1 = 0;
	static short ch2 = 0;
	static short ch3 = 0;
	static short s1 = 0;
	static short s2 = 0;
	
	ch0 = (short)((sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8))&0x07ff) - 1024;
	ch1 = (short)(((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff) - 1024; 
	ch2 = (short)(((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | (sbus_rx_buffer[4] << 10)) & 0x07ff) - 1024;
	ch3 = (short)(((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff) - 1024;
	s1  = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2;                           
	s2  = ((sbus_rx_buffer[5] >> 4)& 0x0003);
	
	/**/
	SetRemoteValue(remoter,ch0,ch1,ch2,ch3,s1,s2);
}


/*
  函数名：SetRemoteValue
  描述  ：设置遥控器结构体变量的值
  参数  ：remoter -- 遥控器结构体变量
          ch0     -- 通道0
					ch1     -- 通道1
					ch2     -- 通道2
					ch3     -- 通道3
          s1      -- 拨码1
          s2      -- 拨码2
  返回值：无
*/
void SetRemoteValue(RemoteController_t* remoter, short ch0, short ch1, short ch2, short ch3, u8 s1, u8 s2)
{
	remoter->rc.ch0 = ch0;
	remoter->rc.ch1 = ch1;
	remoter->rc.ch2 = ch2;
	remoter->rc.ch3 = ch3;
	remoter->rc.s1  = s1;
	remoter->rc.s2  = s2;
}


/*
  函数名：RemoterDataCheck
  描述  ：检查sbus_rx合法性
  参数  ：无
  返回值：1 -- 合法
          0 -- 不合法
*/
u8 RemoterDataCheck(void)
{
	static RemoteController_t temp;
	
	temp.rc.ch0 = (short)((sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8))&0x07ff) - 1024;
	temp.rc.ch1 = (short)(((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff) - 1024; 
	temp.rc.ch2 = (short)(((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | (sbus_rx_buffer[4] << 10)) & 0x07ff) - 1024;
	temp.rc.ch3 = (short)(((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff) - 1024;
	temp.rc.s1  = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2;                           
	temp.rc.s2  = ((sbus_rx_buffer[5] >> 4)& 0x0003);
	
	is_remote_valid = 0;
	if(temp.rc.ch0 > 660 || temp.rc.ch0 < -660)
		return 0;
	if(temp.rc.ch1 > 660 || temp.rc.ch1 < -660)
		return 0;
	if(temp.rc.ch2 > 660 || temp.rc.ch2 < -660)
		return 0;
	if(temp.rc.ch3 > 660 || temp.rc.ch3 < -660)
		return 0;
	if(!(temp.rc.s1 == 1 || temp.rc.s1 == 2 || temp.rc.s1 == 3))
		return 0;
	if(!(temp.rc.s2 == 1 || temp.rc.s2 == 2 || temp.rc.s2 == 3))
		return 0;
	
	is_remote_valid = 1;
	return 1;
}

/*
  函数名：OnSwitchChanged
  描述  ：根据switch变化采取对应动作
  参数  ：无
  返回值：无
*/
void OnSwitchChanged(void)
{
	if(S1_CHANGED)
	{
		LED_RED_BLINK;
	}
	
	if(S2_CHANGED_TO(3,1))
	{
		LED_GREEN_BLINK;
	}
}


/*
  函数名：RemotorDataFilter
  描述  ：遥控器数据滤波
  参数  ：remoter -- 当前遥控器数据结构体变量
  返回值：无
*/
void RemoterDataFilter(RemoteController_t* remoter)
{
	//TO DO
}


/*
  函数名：RemoterDataLog
  描述  ：打印遥控器结构体变量值
  参数  ：remoter -- 遥控器结构体
  返回值：无
*/
void RemoterDataLog(RemoteController_t* remoter)
{
	static u8 log_cnt = 0;
	printf("remotor data[%d]_%d : %d\t%d\t%d\t%d\t%d\t%d\r\n",
	        log_cnt,
	        is_remote_valid,
	        remoter->rc.ch0,
	        remoter->rc.ch1,
	        remoter->rc.ch2,
	        remoter->rc.ch3,
	        remoter->rc.s1,
	        remoter->rc.s2);
          log_cnt++;
	if(log_cnt > 255)
		log_cnt = 0;
}

/*
  函数名：UpdateRobotControlMode
  描述  ：更新控制状态
  参数  ：无
  返回值：无
*/
void UpdateRobotControlMode(void)
{
	if(S1_CHANGED_TO(1,3) || S1_CHANGED_TO(2,3))
	{
		printf("**Control Mode [ switch to REMOTER_CONTROL mode ]\r\n\r\n");
		remote_controller.mode.cur_mode = REMOTER_CONTROL;
	}
	else if(S1_CHANGED_TO(3,1))
	{
		printf("**Control Mode [ switch to SOFTWARE_CONTROL mode ]\r\n\r\n");
		remote_controller.mode.cur_mode = SOFTWARE_CONTROL;
	}
	else if(S1_CHANGED_TO(3,2))
	{
		printf("**Control Mode [ switch to AUTO_CONTROL mode ]\r\n\r\n");
		remote_controller.mode.cur_mode = AUTO;
	}
}