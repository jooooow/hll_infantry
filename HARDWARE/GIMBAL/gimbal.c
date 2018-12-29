#include "gimbal.h"

extern short motor_speed_arr[7];
extern RemoteController_t remote_controller;

PositionPid_t pitch_pid = {0,0,  0,0,0,  0,0,0,0,0,0,0};
PositionPid_t yaw_pid   = {0,0,  0,0,0,  0,0,0,0,0,0,0};

void InitGimbalControlTask(void)
{
	//TO DO
}

void GimbalControlTask(void)
{
	//TO DO
	
	/*根据控制模式选择相应的云台目标角度转化方法*/
	switch(remote_controller.mode.cur_mode)
	{
		/*遥控器控制模式*/
		case REMOTER_CONTROL:
				R_ConvertRc2TargetAngle();
			break;
		/*软件控制模式*/
		case SOFTWARE_CONTROL:
				S_ConvertRc2TargetAngle();
			break;
		/*自动控制模式*/
		case AUTO:
				A_ConvertRc2TargetAngle();
			break;
		default:printf("GIMBAL [ invalid control mode! ]\r\n");break;
	}
}


/*
  函数名：R_ConvertRc2TargetAngle
  描述  ：遥控器控制模式下，将通道转化为目标角度
  参数  ：无
  返回值：无
*/
void R_ConvertRc2TargetAngle(void)
{
	//TO DO
}


/*
  函数名：S_ConvertRc2TargetAngle
  描述  ：软件控制模式下，将通道转化为目标角度
  参数  ：无
  返回值：无
*/
void S_ConvertRc2TargetAngle(void)
{
	//TO DO
}


/*
  函数名：A_ConvertRc2TargetAngle
  描述  ：自动控制模式下，将通道转化为目标角度
  参数  ：无
  返回值：无
*/
void A_ConvertRc2TargetAngle(void)
{
	//TO DO
}


