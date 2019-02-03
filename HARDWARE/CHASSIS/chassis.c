#include "chassis.h"

/*遥控器通道值到目标转速缩放系数*/
#define MOTOR_PID_OUTPUT_LIMIT 10000
#define MOTOR_SPEED_SCALE  6

#define MAX_NEWYORK_MACHANICAL_ANGLE_ERROR 10
#define NEWYORK_MAX_MACHANICAL_ANGLE 800
#define NEWYORK_MIN_MACHANICAL_ANGLE -800
#define NEWYORK_MAX_ABS_PID_OUTPUT   550

#define CHASSIS_FOLLOW_MAX_ABS_PID_OUTPUT 750

const u16 yaw_initial_machanical_angle = 2680;


extern short motor_speed_arr[7];

IncrementPid_t fl_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0,  CalcIncrementPid};
IncrementPid_t fr_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0,  CalcIncrementPid};
IncrementPid_t bl_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0,  CalcIncrementPid};
IncrementPid_t br_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0,  CalcIncrementPid};

PositionPid_t follow_pid = {0,0,  0.9,0.001,0,  0,0,0,  0,0,0,0,  CalcPositionPid};
PositionPid_t newyork_pid = {0,0, 4.0,0.001,0,  0,0,0,  0,0,0,0,  CalcPositionPid};

extern RemoteController_t remote_controller;
extern RemoteController_t old_remote_controller;

/*
  函数名：InitChassisControlTask
  描述  ：初始化底盘任务
	参数  ：无
	返回值：无
*/
void InitChassisControlTask(void)
{
  //TO DO
}


/*
  函数名：ChassisControlTask
  描述  ：底盘任务
          根据模式选择通道转化方法
				  计算PID
  参数  ：无
  返回值：无
*/
void ChassisControlTask(void)
{
	/*根据控制模式选择相应的四轮目标速度转化方法*/
	switch(remote_controller.mode.cur_mode)
	{
		/*遥控器控制模式*/
		case REMOTER_CONTROL:
				R_ConvertRc2TargetSpeed();
			break;
		/*软件控制模式*/
		case SOFTWARE_CONTROL:
				S_ConvertRc2TargetSpeed();
			break;
		/*自动控制模式*/
		case AUTO:
				A_ConvertRc2TargetSpeed();
			break;
		default:printf("CHASSIS [ invalid control mode! ]\r\n");break;
	}
	
	/*获取当前转速*/
	fl_pid.cur = motor_speed_arr[FL];
	fr_pid.cur = motor_speed_arr[FR];
	bl_pid.cur = motor_speed_arr[BL];
	br_pid.cur = motor_speed_arr[BR];
	
	/*计算四轮PID*/
	CalcChassisPID();
	
	/*四轮PID输出限幅*/
	LimitChassisOutput();
	
	/*发送PID输出值给底盘电机*/
	Can1SendMsg2Chassis(fl_pid.output,fr_pid.output,bl_pid.output,br_pid.output);
	
	//IncrementPidLog(&fr_pid);
}


/*
  函数名：CalcChassisPID
  描述  ：计算四轮PID
  参数  ：无
  返回值：无
*/
void CalcChassisPID(void)
{
	fl_pid.CalcPID(&fl_pid);
	fr_pid.CalcPID(&fr_pid);
	bl_pid.CalcPID(&bl_pid);
	br_pid.CalcPID(&br_pid);
}


/*
  函数名：LimitChassisOutput
  描述  ：四轮PID输出限幅
  参数  ：无
  返回值：无
*/
void LimitChassisOutput(void)
{
	IncrementPid_tLimit(&fl_pid,-MOTOR_PID_OUTPUT_LIMIT,MOTOR_PID_OUTPUT_LIMIT);
	IncrementPid_tLimit(&fr_pid,-MOTOR_PID_OUTPUT_LIMIT,MOTOR_PID_OUTPUT_LIMIT);
	IncrementPid_tLimit(&bl_pid,-MOTOR_PID_OUTPUT_LIMIT,MOTOR_PID_OUTPUT_LIMIT);
	IncrementPid_tLimit(&br_pid,-MOTOR_PID_OUTPUT_LIMIT,MOTOR_PID_OUTPUT_LIMIT);
}


/*
	函数名：ChassisFollow
  描述  ：实现底盘跟随
  参数  ：无
  返回值：无
*/
void ChassisFollow(void)
{
	static short speed_fl = 0;
	static short speed_fr = 0;
	static short speed_bl = 0;
	static short speed_br = 0;
	static short follow_sp = 0;
	
	/*底盘跟随pid*/
	follow_pid.tar = yaw_initial_machanical_angle;
	follow_pid.cur = motor_speed_arr[YAW];
	follow_pid.CalcPID(&follow_pid,1000,7);
	PositionPid_tLimit(&follow_pid,-CHASSIS_FOLLOW_MAX_ABS_PID_OUTPUT,CHASSIS_FOLLOW_MAX_ABS_PID_OUTPUT);
	follow_sp = follow_pid.output;
	
	/*四轮速度分配*/
	speed_fl =  remote_controller.rc.ch0 + remote_controller.rc.ch1 + follow_sp + (remote_controller.rc.ch2/2.2)/**/;
	speed_fr =  remote_controller.rc.ch0 - remote_controller.rc.ch1 + follow_sp + (remote_controller.rc.ch2/2.2)/**/;
	speed_bl = -remote_controller.rc.ch0 + remote_controller.rc.ch1 + follow_sp + (remote_controller.rc.ch2/2.2)/**/;
	speed_br = -remote_controller.rc.ch0 - remote_controller.rc.ch1 + follow_sp + (remote_controller.rc.ch2/2.2)/**/;
	
	speed_fl *= MOTOR_SPEED_SCALE;
	speed_fr *= MOTOR_SPEED_SCALE;
	speed_bl *= MOTOR_SPEED_SCALE;
	speed_br *= MOTOR_SPEED_SCALE;
	
	fl_pid.tar = speed_fl;
	fr_pid.tar = speed_fr;
	bl_pid.tar = speed_bl;
	br_pid.tar = speed_br;
}

/*
  函数名 ：ChassisTranslation
  说明   ：底盘只能平移
  参数   ：无
  返回值 ：无
*/
void ChassisTranslation(void)
{
	static short speed_fl = 0;
	static short speed_fr = 0;
	static short speed_bl = 0;
	static short speed_br = 0;
	static short follow_sp = 0;
	
	/*四轮速度分配*/
	speed_fl =  remote_controller.rc.ch0 + remote_controller.rc.ch1;
	speed_fr =  remote_controller.rc.ch0 - remote_controller.rc.ch1;
	speed_bl = -remote_controller.rc.ch0 + remote_controller.rc.ch1;
	speed_br = -remote_controller.rc.ch0 - remote_controller.rc.ch1;
	
	speed_fl *= MOTOR_SPEED_SCALE;
	speed_fr *= MOTOR_SPEED_SCALE;
	speed_bl *= MOTOR_SPEED_SCALE;
	speed_br *= MOTOR_SPEED_SCALE;
	
	fl_pid.tar = speed_fl;
	fr_pid.tar = speed_fr;
	bl_pid.tar = speed_bl;
	br_pid.tar = speed_br;
}

/*
  函数名：ConvertRc2TargetSpeed
  描述  ：遥控器控制模式下，将遥控器通道转化为四个轮子目标转速
  参数  ：无
  返回值：无
*/
void R_ConvertRc2TargetSpeed(void)
{
	ChassisFollow();
}


/*
  函数名：ConvertRc2TargetSpeed
  描述  ：软件控制模式下，将遥控器通道转化为四个轮子目标转速
  参数  ：无
  返回值：无
*/
void S_ConvertRc2TargetSpeed(void)
{
	//TO DO
}


/*
  函数名：ConvertRc2TargetSpeed
  描述  ：自动控制模式下，将遥控器通道转化为四个轮子目标转速
  参数  ：无
  返回值：无
*/
u16 temp_dev_angle_tar;
void A_ConvertRc2TargetSpeed(void)
{
	//只转云台
	if(S2_AT(3))
	{
		ChassisTranslation();
	}
	
	//校准模式
	if(S2_AT(2))
	{
		ChassisFollow();
	}
	
	/*扭腰模式*/
	else if(S2_AT(1))
	{
		NewYork();
	}
}

/*
  函数名 ：NewYork
  描述   ：实现扭腰
  参数   ：无
  返回值 ：无
*/
void NewYork(void)
{
	static short speed_fl = 0;
	static short speed_fr = 0;
	static short speed_bl = 0;
	static short speed_br = 0;
	static short follow_sp = 0;
	static u16 dev_angle_tar = yaw_initial_machanical_angle;
	
	dev_angle_tar = GetNewYorkTargetManchanicalAngle();
	temp_dev_angle_tar = dev_angle_tar;
	newyork_pid.tar = dev_angle_tar;
	newyork_pid.cur = motor_speed_arr[YAW];
	newyork_pid.CalcPID(&newyork_pid,1000,7);
	PositionPid_tLimit(&newyork_pid,-NEWYORK_MAX_ABS_PID_OUTPUT,NEWYORK_MAX_ABS_PID_OUTPUT);
	follow_sp = newyork_pid.output;
	
	/*四轮速度分配*/
	speed_fl =  remote_controller.rc.ch0 + remote_controller.rc.ch1 + follow_sp;
	speed_fr =  remote_controller.rc.ch0 - remote_controller.rc.ch1 + follow_sp;
	speed_bl = -remote_controller.rc.ch0 + remote_controller.rc.ch1 + follow_sp;
	speed_br = -remote_controller.rc.ch0 - remote_controller.rc.ch1 + follow_sp;
	
	speed_fl *= MOTOR_SPEED_SCALE;
	speed_fr *= MOTOR_SPEED_SCALE;
	speed_bl *= MOTOR_SPEED_SCALE;
	speed_br *= MOTOR_SPEED_SCALE;
	
	fl_pid.tar = speed_fl;
	fr_pid.tar = speed_fr;
	bl_pid.tar = speed_bl;
	br_pid.tar = speed_br;
}

/*
  函数名 ：GetNewYorkTargetManchanicalAngle
  描述   ：获取扭腰时的yaw轴目标机械角
					 当误差绝对值小于MAX_NEWYORK_MACHANICAL_ANGLE_ERROR时换向
  参数   ：无
  返回值 ：无
*/
u16 GetNewYorkTargetManchanicalAngle(void)
{
	static u8 is_achieved = 0;
	static u8 old_is_achieved = 0;
	static u8 direction = 0;
	
	if(fabs(newyork_pid.error) < MAX_NEWYORK_MACHANICAL_ANGLE_ERROR && is_achieved == 0)
		is_achieved = 1;
	
	if(old_is_achieved == 0 && is_achieved == 1)
	{
		is_achieved = 0;
		direction = !direction;
	}
	
	old_is_achieved = is_achieved;
	
	if(direction)
		return yaw_initial_machanical_angle + NEWYORK_MAX_MACHANICAL_ANGLE;
	else
		return yaw_initial_machanical_angle + NEWYORK_MIN_MACHANICAL_ANGLE;
}