#include "listener.h"

u8 listen_cnt[8] = {0,0,0,0,0,0,0,0};

/*ONLINE_FLAG寄存器 表示模块在线/离线情况 1在线 0离线*/
u16 ONLINE_FLAG = 0;
u16 OLD_ONLINE_FLAG =0;

extern u8 is_remote_received;
extern u8 sbus_rx_buffer[25]; 
extern RemoteController_t remote_controller;
extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;

void InitListenerTask(void)
{
	listen_cnt[FL] = MOTOR_LOSS_TIME;
	listen_cnt[FR] = MOTOR_LOSS_TIME;
	listen_cnt[BL] = MOTOR_LOSS_TIME;
	listen_cnt[BR] = MOTOR_LOSS_TIME;
	listen_cnt[PITCH] = MOTOR_LOSS_TIME;
	listen_cnt[YAW] = MOTOR_LOSS_TIME;
	listen_cnt[WAVE] = MOTOR_LOSS_TIME;
	listen_cnt[REMOTER] = REMOTER_LOSS_TIME;
	ONLINE_FLAG = 0xFF;
	OLD_ONLINE_FLAG = 0xFF;
}


/*
  函数名：ListenTask
  描述  ：监听任务
          监听各模块状态
          并根据状态变化采取相对应操作
  参数  ：无
  返回值：无
*/
void ListenTask(void)
{
	/*监听各个模块*/
	ListenModule();
	
	/*响应模块状态变化 采取对应操作*/
	OnModuleStateChanged();
}


/*
  函数名：ListenModule
  描述  ：监听所有模块
  参数  ：无
  返回值：无
*/
void ListenModule(void)
{
	Listen(FL);
	Listen(FR);
	Listen(BL);
	Listen(BR);
	Listen(PITCH);
	Listen(YAW);
	Listen(WAVE);
	Listen(REMOTER);
}


/*
  函数名：OnModuleStateChanged
  描述  ：响应模块状态的变化
          根据状态变化采取对应措施
  参数  ：无
  返回值：无
*/
void OnModuleStateChanged(void)
{
	/*遥控器 连接/丢失情况采取相应操作*/
	if(GET_STATE(REMOTER) && !GET_OLD_STATE(REMOTER))
	{
#if (CONNECT_LOSS_LOG == 1)
		printf("-- remoter connect! --\r\n");
#endif
	}
	if(!GET_STATE(REMOTER) && GET_OLD_STATE(REMOTER))
	{
		/*丢失则重置DMA*/
		ResetUsart1DMA();
		SetRemoteValue(&remote_controller,0,0,0,0,0,0);
#if (CONNECT_LOSS_LOG == 1)
		printf("-- remoter loss! --\r\n");
#endif
	}
	
	
	/*FL 连接/丢失情况采取相应操作*/
	if(GET_STATE(FL) && !GET_OLD_STATE(FL))
  {
#if (CONNECT_LOSS_LOG == 1)
		printf("-- FL connect! --\r\n");
#endif
	}
	if(!GET_STATE(FL) && GET_OLD_STATE(FL))
	{
		ResetIncrementPid(&fl_pid,3,0);
#if (CONNECT_LOSS_LOG == 1)
		printf("-- FL loss! --\r\n");
#endif
	}
	
	
	/*FR 连接/丢失情况采取相应操作*/
	if(GET_STATE(FR) && !GET_OLD_STATE(FR))
  {
#if (CONNECT_LOSS_LOG == 1)
		printf("-- FR connect! --\r\n");
#endif
	}
	if(!GET_STATE(FR) && GET_OLD_STATE(FR))
	{
		ResetIncrementPid(&fr_pid,3,0);
#if (CONNECT_LOSS_LOG == 1)
		printf("-- FR loss! --\r\n");
#endif
	}
	
	
	/*BL 连接/丢失情况采取相应操作*/
	if(GET_STATE(BL) && !GET_OLD_STATE(BL))
  {
#if (CONNECT_LOSS_LOG == 1)
		printf("-- BL connect! --\r\n");
#endif
	}
	if(!GET_STATE(BL) && GET_OLD_STATE(BL))
	{
		ResetIncrementPid(&bl_pid,3,0);
#if (CONNECT_LOSS_LOG == 1)
		printf("-- BL loss! --\r\n");
#endif
	}
	
	
	/*BR 连接/丢失情况采取相应操作*/
	if(GET_STATE(BR) && !GET_OLD_STATE(BR))
  {
#if (CONNECT_LOSS_LOG == 1)
		printf("-- BR connect! --\r\n");
#endif
	}
	if(!GET_STATE(BR) && GET_OLD_STATE(BR))
	{
		ResetIncrementPid(&br_pid,3,0);
#if (CONNECT_LOSS_LOG == 1)
		printf("-- BR loss! --\r\n");
#endif
	}
	
	
	/*BR 连接/丢失情况采取相应操作*/
	if(GET_STATE(WAVE) && !GET_OLD_STATE(WAVE))
  {
#if (CONNECT_LOSS_LOG == 1)
		printf("-- WAVE connect! --\r\n");
#endif
	}
	if(!GET_STATE(WAVE) && GET_OLD_STATE(WAVE))
	{
		ResetIncrementPid(&br_pid,3,0);
#if (CONNECT_LOSS_LOG == 1)
		printf("-- WAVE loss! --\r\n");
#endif
	}
}

/*
  函数名：Listen
  描述  ：监听索引为index的模块
          如果listen_cnt[index]大于0则递减
          listen_cnt[index]等于0视为丢失
          listen_cnt被重新装填后视为重新连接
  参数  ：index -- 模块索引
  返回值：无
*/
void Listen(u8 index)
{
	if(listen_cnt[index] > 0)
	{
		if(!GET_STATE(index))
		{
			/*标记为CONNECT*/
			ONLINE_FLAG |= (0x01 << index);
		}
		listen_cnt[index]--;
	}
	
	if(listen_cnt[index] == 0 && GET_STATE(index))
	{
		/*标记为LOSS*/
		ONLINE_FLAG &= ~(0x01 << index);
	}
}
