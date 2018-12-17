#include "remoter.h"

RemoteController remote_controller     = {0,0,0,0,0,0,  0,0,0,0,0,  0};
RemoteController old_remote_controller = {0,0,0,0,0,0,  0,0,0,0,0,  0};

extern u8 sbus_rx_buffer[25]; 
extern u8 is_remote_received;
extern u8 remote_listen_cnt;
u8 is_remote_valid = 0;

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
	is_remote_received = 0;
	remote_listen_cnt = 0;
	
	/*检查合法性*/
	if(RemoterDataCheck())
	{
		/*合法 解析出通道值并赋值给remote_controller*/
		ParseRemoterData(&remote_controller);
		
		if(remote_controller.rc.s1 != old_remote_controller.rc.s1)
		{
			BlinkRedLed();
		}
		if(remote_controller.rc.s2 != old_remote_controller.rc.s2)
		{
			BlinkGreenLed();
		}
	}
	else
	{
		/*不合法 设置全0*/
		SetRemoteValue(&remote_controller,0,0,0,0,0,0);
	}
	
	/*打印遥控器通道值*/
	RemoterDataLog(&remote_controller);
}


/*
  函数名：ParseRemoterData
  描述  ：将sbus原始数据转化为遥控器数据
  参数  ：remoter -- 当前遥控器数据结构体变量
  返回值：无
*/
void ParseRemoterData(RemoteController* remoter)
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
	
	SetRemoteValue(remoter,ch0,ch1,ch2,ch3,s1,s2);
}


/*
  函数名：SetRemoteValue
  描述  ：设置遥控器结构体变量的值
  参数  ：remotor：遥控器结构体变量
          ch0 -- 通道0
					ch1 -- 通道1
					ch2 -- 通道2
					ch3 -- 通道3
          s1  -- 拨码1
          s2  -- 拨码2
  返回值：无
*/
void SetRemoteValue(RemoteController* remoter, short ch0, short ch1, short ch2, short ch3, u8 s1, u8 s2)
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
	static RemoteController temp;
	
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
  函数名：RemotorDataFilter
  描述  ：遥控器数据滤波
  参数  ：remoter -- 当前遥控器数据结构体变量
  返回值：无
*/
void RemoterDataFilter(RemoteController* remoter)
{
	//TO DO
}


/*
  函数名：RemoterDataLog
  描述  ：打印遥控器结构体变量值
  参数  ：remoter -- 遥控器结构体
  返回值：无
*/
void RemoterDataLog(RemoteController* remoter)
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

void BlinkRedLed(void)
{
	LED_RED_BLINK;
}

void BlinkGreenLed(void)
{
	LED_GREEN_BLINK;
}
