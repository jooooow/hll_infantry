#include "remoter.h"
#include "usart1.h"

RemoteController_t remote_controller     = {0,0,0,0,3,3,  0,0,0,0,0,  0, REMOTER_CONTROL,REMOTER_CONTROL};
RemoteController_t old_remote_controller = {0,0,0,0,3,3,  0,0,0,0,0,  0, REMOTER_CONTROL,REMOTER_CONTROL};
ShooterSignal_t wave_move_flag = {0,0,0,0,0};

u8 is_remote_valid = 0;
extern u8 sbus_rx_buffer[25]; 
extern u8 is_remote_received;
extern u8 listen_cnt[10];

extern u8 fric_state;
extern ControlMode_t mode;
extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;

u8 is_start_accel_cali = 0;
u8 is_start_gyro_cali = 0;
u8 s2_state = S2_STA_AUTO;

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
u8 is_waiba = 0;
SoftTimer timer3 = {50,50,0,0,  NULL,&is_waiba,  SoftTimerDescend,SoftTimerReload,NULL};
void RemoteTask(void)
{
	/*重新装填遥控器的Listener*/
	RELOAD_REMOTER;
	
	/*检查数据合法性*/
	if(!RemoterDataCheck())
	{
		/*不合法 重置DMA*/
		ResetUsart1DMA();
		SetRemoteValue(&remote_controller,0,0,0,0,0,0,0,0,0,0,0,0);
		return;
	}
	
	/*合法 解析出通道值并赋值给remote_controller*/
	ParseRemoterData(&remote_controller);
	
	/*根据switch更新控制状态*/
	UpdateRobotControlMode();
	
	/*响应switch状态变化*/
	OnSwitchChanged();
	
	if(remote_controller.rc.ch0 == 660 &&
		 remote_controller.rc.ch1 == -660 &&
	   remote_controller.rc.ch2 == -660 &&
	   remote_controller.rc.ch3 == -660)
		is_waiba = 1;
	else
		is_waiba = 0;
	timer3.Start(&timer3);
	if(timer3.time_out_flag)
	{
		printf("on waiba for 50x14ms\r\n");
		timer3.time_out_flag = 0;
		timer3.Reload(&timer3);
	}
	//RemoterDataLog(&remote_controller);
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
	static short x = 0;
	static short y = 0;
	static short z = 0;
	static unsigned short press_l = 0;
	static unsigned short press_r = 0;
	static unsigned short key = 0;
	
	ch0 = (short)((sbus_rx_buffer[0]| (sbus_rx_buffer[1] << 8))&0x07ff) - 1024;
	ch1 = (short)(((sbus_rx_buffer[1] >> 3) | (sbus_rx_buffer[2] << 5)) & 0x07ff) - 1024; 
	ch2 = (short)(((sbus_rx_buffer[2] >> 6) | (sbus_rx_buffer[3] << 2) | (sbus_rx_buffer[4] << 10)) & 0x07ff) - 1024;
	ch3 = (short)(((sbus_rx_buffer[4] >> 1) | (sbus_rx_buffer[5] << 7)) & 0x07ff) - 1024;
	s1  = ((sbus_rx_buffer[5] >> 4)& 0x000C) >> 2;                           
	s2  = ((sbus_rx_buffer[5] >> 4)& 0x0003);
	x   = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8);                  
	y   = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8);                 
	z   = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8);                 
	press_l = sbus_rx_buffer[12];                                           
	press_r = sbus_rx_buffer[13];   
	key = sbus_rx_buffer[14] | (sbus_rx_buffer[15] << 8);
	
	SetRemoteValue(remoter,ch0,ch1,ch2,ch3,s1,s2,x,y,z,press_l,press_r,key);
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
					x       -- 鼠标x
					y       -- 鼠标y
					z       -- 鼠标z
					p_l     -- 鼠标左键
					p_r     -- 鼠标右键
					key     -- 键盘按键
  返回值：无
*/
void SetRemoteValue(RemoteController_t* remoter, short ch0, short ch1, short ch2, short ch3, u8 s1, u8 s2, short x, short y, short z, u8 p_l, u8 p_r, u16 key)
{
	remoter->rc.ch0 = ch0;
	remoter->rc.ch1 = ch1;
	remoter->rc.ch2 = ch2;
	remoter->rc.ch3 = ch3;
	remoter->rc.s1  = s1;
	remoter->rc.s2  = s2;
	remoter->mouse.x = x;
	remoter->mouse.y = y;
	remoter->mouse.z = z;
	remoter->mouse.press_l = p_l;
	remoter->mouse.press_r = p_r;
	remoter->key.value = key;
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
  temp.mouse.x = sbus_rx_buffer[6] | (sbus_rx_buffer[7] << 8);                  
	temp.mouse.y   = sbus_rx_buffer[8] | (sbus_rx_buffer[9] << 8);                 
	temp.mouse.z   = sbus_rx_buffer[10] | (sbus_rx_buffer[11] << 8);                 
	temp.mouse.press_l = sbus_rx_buffer[12];                                           
	temp.mouse.press_r = sbus_rx_buffer[13];   
	temp.key.value = sbus_rx_buffer[14] | (sbus_rx_buffer[15] << 8);
	
	is_remote_valid = 0;
	
	/*检验合法性*/
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
	if(!(temp.mouse.x >= -32768 && temp.mouse.x <= 32767))
		return 0;
	if(!(temp.mouse.y >= -32768 && temp.mouse.y <= 32767))
		return 0;
	if(!(temp.mouse.z >= -32768 && temp.mouse.z <= 32767))
		return 0;
	if(!(temp.mouse.press_l == 1 || temp.mouse.press_l == 0))
		return 0;
	if(!(temp.mouse.press_r == 1 || temp.mouse.press_r == 0))
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
		wave_move_flag.wave_begin_positive_flag = 1;
	}
	else if(S2_CHANGED_TO(1,3) || S2_CHANGED_TO(2,3))
	{
		LED_GREEN_BLINK;
		wave_move_flag.wave_stop_flag = 1;
	}
	else if(S2_CHANGED_TO(3,2))
	{
		LED_GREEN_BLINK;
		
		if(S1_AT(3))
		{
			if(fric_state == FRIC_STA_STOP)
				wave_move_flag.fric_begin_flag = 1;
			else if(fric_state == FRIC_STA_ROTATE)
				wave_move_flag.fric_stop_flag = 1;
		}
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
	printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\r\n",
	        is_remote_valid,
	        remoter->rc.ch0,
	        remoter->rc.ch1,
	        remoter->rc.ch2,
	        remoter->rc.ch3,
	        remoter->rc.s1,
	        remoter->rc.s2,
					remoter->mouse.x,
					remoter->mouse.y,
					remoter->mouse.z,
					remoter->mouse.press_l,
					remoter->mouse.press_r,
				  remoter->key.value
	);
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
		printf("**Control Mode [ switch to AUTO / CALI mode ]\r\n\r\n");
		remote_controller.mode.cur_mode = AUTO;
	}
	
	if(S1_AT(2))
	{
		if(S2_CHANGED_TO(3,1))
		{
			printf("  [switch to NEWYORK mode]\r\n");
			s2_state = S2_STA_NEWY;
		}
		else if(S2_CHANGED_TO(3,2))
		{
			printf("  [switch to CALI mode]\r\n");
			s2_state = S2_STA_CALI;
		}
		else if(S2_CHANGED_TO(1,3) || S2_CHANGED_TO(2,3))
		{
			/*这里暂时清除两个校准开始标志，实际山应该由校准函数校准完毕后清除*/
			is_start_gyro_cali = 0;
			is_start_accel_cali = 0;
			/******************************************************************/
			
			printf("  [switch to AUTO mode]\r\n");
			s2_state = S2_STA_AUTO;
		}
	}
	
	/*AUTO/CALI模式下的校准模式*/
	if(s2_state == S2_STA_CALI)
	{
		/*判断是否进入陀螺仪校准*/
		if(is_start_gyro_cali == 0 && IsAtTheChannelValueFor14xtMs(660,-660,-660,-660,50))
		{
			is_start_gyro_cali = 1;
			printf("into gyro cali\r\n");
		}
	}
}

/*
	函数名 ：IsAtTheChannelValueFor14xtMs
  描述   ：遥控器通道是否已维持特定值一段时间
           时间周期为14ms
  参数   ：ch0 -- 通道0
           ch1 -- 通道1
           ch2 -- 通道2
           ch3 -- 通道3
           t   -- 持续时间周期数
  返回值 ：0   -- 还没持续到目标时间
           1   -- 已经持续到目标时间
*/
u8 IsAtTheChannelValueFor14xtMs(short ch0, short ch1, short ch2, short ch3, u16 t)
{
	static u8 flag = 0;
	static u8 old_flag = 0;
	static u8 ret = 0;
	static u16 t_cnt = 0;
	
	if(remote_controller.rc.ch0 == ch0 &&
		 remote_controller.rc.ch1 == ch1 &&
	   remote_controller.rc.ch2 == ch2 &&
	   remote_controller.rc.ch3 == ch3)
	{
		if(t_cnt < t)
		{
			t_cnt++;
			if(t_cnt == t)
			{
				flag = 1;
			}
		}
	}
	else
	{
		flag = 0;
		t_cnt = 0;
	}
	
	if(flag == 1 && old_flag == 0)
		ret = 1;
	else
		ret = 0;
	
	old_flag = flag;
	return ret;
}
