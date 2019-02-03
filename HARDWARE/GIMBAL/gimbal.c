#include "gimbal.h"

extern short motor_speed_arr[7];
extern RemoteController_t remote_controller;
extern RemoteController_t old_remote_controller;
extern Angle_t yaw;
extern u16 ONLINE_FLAG;
extern u16 OLD_ONLINE_FLAG;
extern u8 angle_yaw_valid;
extern u8 angle_pitch_valid;

extern short pitch_can2prep_output;
extern short yaw_can2prep_output;

float yaw_output = 0;
float pitch_output = 0;

/*19年的机械角反馈pid*/
GimbalPID_t yaw_pid = {20,0,0,   0,0,0,0, 0,0,0,0,  3000,5000,  0, INITIAL_YAW_ANGLE,  CalcYawPid};
GimbalPID_t pitch_pid = {45,0.32,40,  0,0,0,0, 0,0,0,0,  1000,3000,  0, INITIAL_PITCH_ANGLE,CalcPitchPid};

/*陀螺仪反馈yaw轴pid*/
PositionPid_t yaw_gyro_pid = {0,0,  1900,0,92000,  0,0,0,  0,0,0,0,  CalcPositionPid};

/*机械角反馈yaw轴pid*/
PositionPid_t yaw_machanical_pid = {2712,0,  40,0,0,  0,0,0,  0,0,0,0,  CalcPositionPid};

void InitGimbalControlTask(void)
{
	//TO DO
}

void GimbalControlTask(void)
{
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
	
	/*获取当前机械角*/
	pitch_pid.cur = motor_speed_arr[PITCH];
	yaw_pid.cur = motor_speed_arr[YAW];
	
	/*获取当前角度*/
	yaw_gyro_pid.cur = yaw.theta; 
	
	yaw_machanical_pid.cur = motor_speed_arr[YAW];
	
	CalcGimbalPid();
}


/*
  函数名：R_ConvertRc2TargetAngle
  描述  ：遥控器控制模式下，将通道转化为目标角度
  参数  ：无
  返回值：无
*/
void R_ConvertRc2TargetAngle(void)
{
	short gch3,gch2;
	
	/*遥控器通道限幅*/
	gch3 = remote_controller.rc.ch3;
	gch3 = gch3 > 660 ? 660 : gch3;
	gch3 = gch3 < -660 ? -660 : gch3;
	gch3 = gch3 / 180.0;
	
	/*目标角度累加*/
	yaw_gyro_pid.tar += -remote_controller.rc.ch2/2900.0;
	
	pitch_pid.tar -= gch3;
	
	/*目标角度限幅*/
	yaw_pid.tar   = yaw_pid.tar < INITIAL_YAW_ANGLE-500 ? INITIAL_YAW_ANGLE-500 : yaw_pid.tar;
	yaw_pid.tar   = yaw_pid.tar > INITIAL_YAW_ANGLE+1300 ? INITIAL_YAW_ANGLE+1300 : yaw_pid.tar;
	pitch_pid.tar = pitch_pid.tar < INITIAL_PITCH_ANGLE-800 ? INITIAL_PITCH_ANGLE-800 : pitch_pid.tar;
	pitch_pid.tar = pitch_pid.tar > INITIAL_PITCH_ANGLE+500 ? INITIAL_PITCH_ANGLE+500 : pitch_pid.tar;
}

/*
  函数名：S_ConvertRc2TargetAngle
  描述  ：软件控制模式下，将通道转化为目标角度
  参数  ：无
  返回值：无
*/
void S_ConvertRc2TargetAngle(void)
{
	yaw_gyro_pid.tar += remote_controller.mouse.x/40.0;
	pitch_pid.tar += remote_controller.mouse.y/30.0;
}


/*
  函数名：A_ConvertRc2TargetAngle
  描述  ：自动控制模式下，将通道转化为目标角度
  参数  ：无
  返回值：无
*/
extern u8 sight_received_a_frame_flag;
extern u8 sight_rx_buf[8];

PositionPid_t auto_find_yaw_pid = {0,0,  0.0006,0,0,  0,0,0,  0,0,0,0,  CalcPositionPid};

void A_ConvertRc2TargetAngle(void)
{
	if(S2_AT(3))
	{
		
//		auto_find_yaw_pid.tar = 480;
//		auto_find_yaw_pid.CalcPID(&auto_find_yaw_pid,1000,1);
//		PositionPid_tLimit(&auto_find_yaw_pid,-45.0f,45.0f);
//		
//		
//		yaw_gyro_pid.tar += auto_find_yaw_pid.output;
//		
//		if(yaw_gyro_pid.tar > 90)
//			yaw_gyro_pid.tar = 90;
//		else if(yaw_gyro_pid.tar < -90)
//			yaw_gyro_pid.tar = -90;
			R_ConvertRc2TargetAngle();
	}
	else if(S2_AT(1))
	{
		R_ConvertRc2TargetAngle();
	}
	else if(S2_AT(2))
	{
		
	}
}


void CalcGimbalPid(void)
{
	/*如果陀螺仪和地磁仪同时在线*/
	if(GET_STATE(GYRO) && GET_STATE(MAG))
	{
		/*检查pitch轴角度计算正确性，正确则使用陀螺仪角度反馈*/
		if(IS_PITCH_ANGLE_VALID)
		{
			pitch_pid.CalcPID(&pitch_pid);
			pitch_output = pitch_pid.PIDout;
		}
		/*不正确则使用机械角反馈*/
		else
		{
			pitch_pid.CalcPID(&pitch_pid);
			pitch_output = pitch_pid.PIDout;
		}
		
		/*检查yaw轴角度计算正确性，正确则使用陀螺仪角度反馈*/
		if(IS_YAW_ANGLE_VAILD)
		{
			//yaw_pid.CalcPID(&yaw_pid);
			//yaw_output = yaw_pid.PIDout;
			if(fabs(yaw_gyro_pid.integration) == 1000)
			{
				yaw_gyro_pid.tar = yaw_gyro_pid.cur;
				yaw_gyro_pid.integration = 0;
			}
			
			yaw_gyro_pid.CalcPID(&yaw_gyro_pid, 1000, 0);
			PositionPid_tLimit(&yaw_gyro_pid,-5000,5000);
			yaw_output = yaw_gyro_pid.output;
		}
		/*不正确则使用机械角反馈*/
		else
		{
			yaw_pid.CalcPID(&yaw_pid);
			yaw_output = yaw_pid.PIDout;
		}
	}
	/*陀螺仪或地磁仪不在线，则使用机械角反馈*/
	else
	{
		pitch_pid.CalcPID(&pitch_pid);
		yaw_pid.CalcPID(&yaw_pid);
		pitch_output = -pitch_pid.PIDout;
		yaw_output = yaw_pid.PIDout;
	}
	
	/*设置云台预备发送电流*/
	yaw_can2prep_output = yaw_output;
	pitch_can2prep_output = pitch_output;
	
	/*发送云台和波轮电流给CAN2*/
	Can2Send2Gimbal();
}
