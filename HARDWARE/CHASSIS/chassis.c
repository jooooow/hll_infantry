#include "chassis.h"

/*遥控器通道值到目标转速缩放系数*/
#define MOTOR_PID_OUTPUT_LIMIT 10000
#define MOTOR_SPEED_SCALE  6

extern short motor_speed_arr[7];

IncrementPid_t fl_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};
IncrementPid_t fr_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};
IncrementPid_t bl_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};
IncrementPid_t br_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};

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
	CalcIncrementPid(&fl_pid);
	CalcIncrementPid(&fr_pid);
	CalcIncrementPid(&bl_pid);
	CalcIncrementPid(&br_pid);
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
  函数名：ConvertRc2TargetSpeed
  描述  ：遥控器控制模式下，将遥控器通道转化为四个轮子目标转速
  参数  ：无
  返回值：无
*/
void R_ConvertRc2TargetSpeed(void)
{
	static short speed_fl = 0;
	static short speed_fr = 0;
	static short speed_bl = 0;
	static short speed_br = 0;
	
	/*四轮速度分配*/
	speed_fl =  remote_controller.rc.ch0 + remote_controller.rc.ch1 + remote_controller.rc.ch2;
	speed_fr =  remote_controller.rc.ch0 - remote_controller.rc.ch1 + remote_controller.rc.ch2;
	speed_bl = -remote_controller.rc.ch0 + remote_controller.rc.ch1 + remote_controller.rc.ch2;
	speed_br = -remote_controller.rc.ch0 - remote_controller.rc.ch1 + remote_controller.rc.ch2;
	
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
void A_ConvertRc2TargetSpeed(void)
{
	//TO DO 
}