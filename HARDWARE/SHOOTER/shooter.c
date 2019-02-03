#include "shooter.h"

#define WAVE_OUTPUT_MAX 15000

PositionPid_t wave_motor_pid = {0,0,  10,0.05,30,  0,0,0,  0,0,0,0,  CalcPositionPid};

short wave_motor_speed = 0;
extern short motor_speed_arr[7];
extern RemoteController_t remote_controller;
extern RemoteController_t old_remote_controller;
extern ShooterSignal_t wave_move_flag;

u8 fric_state = FRIC_STA_STOP;
u8 wave_rotate_direction = WAVE_STA_STOP;
u8 is_wave_block = WAVE_NO_BLOCK;
u8 old_is_wave_block = WAVE_NO_BLOCK;
u16 mid_speed = FRIC_FREEZE_SPEED;
u16 top_speed = FRIC_FREEZE_SPEED;

extern short wave_can2prep_output;

void InitShootTask(void)
{
	//TO DO
}

void ShootTask(void)
{
	/*根据控制模式选择相应的波轮正反转开关转化方法*/
	switch(remote_controller.mode.cur_mode)
	{
		/*遥控器控制模式*/
		case REMOTER_CONTROL:
				R_ConvertRc2TargetOnoff();
			break;
		/*软件控制模式*/
		case SOFTWARE_CONTROL:
				S_ConvertRc2TargetOnoff();
			break;
		/*自动控制模式*/
		case AUTO:
				A_ConvertRc2TargetOnoff();
			break;
		default:printf("CHASSIS [ invalid control mode! ]\r\n");break;
	}
	
	/*检测是否堵转*/
	is_wave_block = WaveBlockDetect(&wave_motor_pid);

	/*发生堵转则反转一段时间*/
	OnIntoBlock();
	
	/*获取当前转速*/
	wave_motor_pid.cur = motor_speed_arr[WAVE];
	
	/*计算PID*/
	wave_motor_pid.CalcPID(&wave_motor_pid,4000, 10);
	
	/*输出限幅*/
	PositionPid_tLimit(&wave_motor_pid,-WAVE_OUTPUT_MAX,WAVE_OUTPUT_MAX);
	
	/*设置波轮预备发送电流
		与云台预备电流一起发送*/
	wave_can2prep_output = wave_motor_pid.output;
	
	/*根据波轮状态改变摩擦轮转停*/
	FricTask();
}

/*
  函数名：R_ConvertRc2TargetOnoff
  描述  ：遥控器控制模式下，将遥控器switch转化为波轮正反转的开关
  参数  ：无
  返回值：无
*/
void R_ConvertRc2TargetOnoff(void)
{
	/*开始正转*/
	if(wave_move_flag.wave_begin_positive_flag)
	{
		wave_move_flag.wave_begin_positive_flag = 0;
		if(fric_state == FRIC_STA_ROTATE)
			WavePositiveRotate();
	}
	/*开始反转*/
	if(wave_move_flag.wave_begin_negative_flag)
	{
		wave_move_flag.wave_begin_negative_flag = 0;
		if(fric_state == FRIC_STA_ROTATE)
			WaveNegativeRotate();
	}
	/*停止*/
	if(wave_move_flag.wave_stop_flag)
	{
		wave_move_flag.wave_stop_flag = 0;
		WaveStop();
	}
	
	if(wave_move_flag.fric_begin_flag)
	{
		wave_move_flag.fric_begin_flag = 0;
		fric_state = FRIC_STA_ROTATE;
	}
	
	if(wave_move_flag.fric_stop_flag)
	{
		wave_move_flag.fric_stop_flag = 0;
		fric_state = FRIC_STA_STOP;
	}
}

/*
  函数名：S_ConvertRc2TargetOnoff
  描述  ：软件控制模式下，将遥控器switch转化为波轮正反转的开关
  参数  ：无
  返回值：无
*/
void S_ConvertRc2TargetOnoff(void)
{
	//TO DO
}

/*
  函数名：A_ConvertRc2TargetOnoff
  描述  ：自动控制模式下，将遥控器switch转化为波轮正反转的开关
  参数  ：无
  返回值：无
*/
void A_ConvertRc2TargetOnoff(void)
{
	//TO DO
}

/*
  函数名：WavePositiveRotate
  描述  ：波轮正转
  参数  ：无
  返回值：无
*/
void WavePositiveRotate(void)
{
	WAVE_POSI;
	wave_rotate_direction = WAVE_STA_POSITIVE;
}

/*
  函数名：WaveNegativeRotate
  描述  ：波轮反转
  参数  ：无
  返回值：无
*/
void WaveNegativeRotate(void)
{
	WAVE_NEGA;
	wave_rotate_direction = WAVE_STA_NEGATIVE;
}

/*
  函数名：WaveStop
  描述  ：波轮停
  参数  ：无
  返回值：无
*/
void WaveStop(void)
{
	WAVE_STOP;
	wave_rotate_direction = WAVE_STA_STOP;
}

/*
  函数名：SetWaveTarSpeed
  描述  ：设置波轮转速
  参数  ：speed -- 波轮目标转速(绝对值需要小于ROTATE_MAX_ABS_SPEED)
  返回值：无
*/
void SetWaveTarSpeed(short speed)
{
	if(abs(speed) <= ROTATE_MAX_ABS_SPEED)
		wave_motor_pid.tar = speed;
}

/*
  函数名：WaveBlockDetect
  描述  ：检测波轮是否堵转
	        根据pid输出电流，若其绝对值等于最大限流值，则计数器递减，到0则认为发生堵转
  参数  ：wave_pid -- 波轮速度环pid结构体变量
  返回值：1 -- 堵转
          0 -- 没堵转
*/
u8 WaveBlockDetect(PositionPid_t* wave_pid)
{
	static u16 time = BLOCK_TIME;
	
	/*输出电流达到最大值时，计数器开始递减*/
	if(abs(wave_pid->output) == WAVE_OUTPUT_MAX)
	{
		if(time > 0)
		{
			time--;
		}
	}
	else
	{
		time = BLOCK_TIME;
	}
	
	/*递减到0，认为发生堵转*/
	if(time == 0)
		return WAVE_BLOCK;
	else
		return WAVE_NO_BLOCK;
}

/*
  函数名：OnIntoBlock
  描述  ：响应堵转
				  若发生堵转，则反转一段时间，再切回原方向
  参数  ：无
  返回值：无
*/
void OnIntoBlock(void)
{
	static u8 is_into_block = 0;
	static u16 reserve_time = REVERSE_TIME;
	
	/*从非阻塞状态进入阻塞状态，并且当前不是开始反转状态，则电机反转*/
	if(!old_is_wave_block && is_wave_block && !is_into_block)
	{
		is_into_block = 1;
		printf("  wave block ni haita!\r\n");
		/*换向，开始反转*/
		if(wave_rotate_direction == WAVE_STA_POSITIVE)
		{
			printf("  into block : p->n\r\n");
			WaveNegativeRotate();
		}
		else if(wave_rotate_direction == WAVE_STA_NEGATIVE)
		{
			printf("  into block : n->p\r\n");
			WavePositiveRotate();
		}
	}
	
	/*开始反转，则反转计数器递减*/
	if(is_into_block)
	{
		reserve_time--;
		/*递减到0，停止反转*/
		if(reserve_time == 0)
		{
			/*换向，退出反转，返回原来转向*/
			if(wave_rotate_direction == WAVE_STA_POSITIVE)
			{
				printf("  reverse out : p->n\r\n");
				WaveNegativeRotate();
			}
			else if(wave_rotate_direction == WAVE_STA_NEGATIVE)
			{
				printf("  reverse out : n->p\r\n");
				WavePositiveRotate();
			}
			is_into_block = 0;
			reserve_time = REVERSE_TIME;
		}
	}
}

void FricTask(void)
{
	if(fric_state == FRIC_STA_ROTATE)
		FricRotate();
	else if(fric_state == FRIC_STA_STOP)
		FricStop();
}

void FricRotate(void)
{
	SetFricSpeed(1400,1400);
}

void FricStop(void)
{
	SetFricSpeed(FRIC_FREEZE_SPEED,FRIC_FREEZE_SPEED);
}