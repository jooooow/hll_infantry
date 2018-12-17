#include "chassis.h"

/*遥控器通道值到目标转速缩放系数*/
#define CHASSIS_MOTOR_SPEED_SCALE 6
#define CHASSIS_MOTOR_SPEED_LIMIT 10000

extern short chassis_motor_speed[6];

extern IncrementPid fl_pid;
extern IncrementPid fr_pid;
extern IncrementPid bl_pid;
extern IncrementPid br_pid;

extern RemoteController remote_controller;
extern RemoteController old_remote_controller;


/*
  函数名：InitChassisControlTask
  描述  ：初始化底盘任务
	参数  ：无
	返回值：无
*/
void InitChassisControlTask(void)
{

}


/*
  函数名：ChassisControlTask
  描述  ：底盘任务
  参数  ：无
  返回值：无
*/
void ChassisControlTask(void)
{
	/*获取当前转速*/
	fl_pid.cur = chassis_motor_speed[FL];
	fr_pid.cur = chassis_motor_speed[FR];
	bl_pid.cur = chassis_motor_speed[BL];
	br_pid.cur = chassis_motor_speed[BR];
	
	/*将遥控器通道转化为四个轮子目标转速*/
	ConvertRc2TargetSpeed();
	
	/*计算四轮PID*/
	CalcChassisPID();
	
	/*四轮PID输出限幅*/
	LimitChassisOutput();
	
	/*发送PID输出值给底盘电机*/
	Can1SendMsg2Chassis(fl_pid.output,fr_pid.output,bl_pid.output,br_pid.output);
}


/*
  函数名：ConvertRc2TargetSpeed
  描述  ：将遥控器通道转化为四个轮子目标转速
  参数  ：无
  返回值：无
*/
void ConvertRc2TargetSpeed(void)
{
	static short speed_fl = 0;
	static short speed_fr = 0;
	static short speed_bl = 0;
	static short speed_br = 0;
	
	speed_fl =  remote_controller.rc.ch0 + remote_controller.rc.ch1 + remote_controller.rc.ch2;
	speed_fr = -remote_controller.rc.ch1 + remote_controller.rc.ch2 + remote_controller.rc.ch0;
	speed_bl =  remote_controller.rc.ch1 + remote_controller.rc.ch2 - remote_controller.rc.ch0;
	speed_br = -remote_controller.rc.ch1 + remote_controller.rc.ch2 - remote_controller.rc.ch0;
	
	speed_fl *= CHASSIS_MOTOR_SPEED_SCALE;
	speed_fr *= CHASSIS_MOTOR_SPEED_SCALE;
	speed_bl *= CHASSIS_MOTOR_SPEED_SCALE;
	speed_br *= CHASSIS_MOTOR_SPEED_SCALE;
	
	fl_pid.tar = speed_fl;
	fr_pid.tar = speed_fr;
	bl_pid.tar = speed_bl;
	br_pid.tar = speed_br;
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
	IncrementPidLimit(&fl_pid,-CHASSIS_MOTOR_SPEED_LIMIT,CHASSIS_MOTOR_SPEED_LIMIT);
	IncrementPidLimit(&fr_pid,-CHASSIS_MOTOR_SPEED_LIMIT,CHASSIS_MOTOR_SPEED_LIMIT);
	IncrementPidLimit(&bl_pid,-CHASSIS_MOTOR_SPEED_LIMIT,CHASSIS_MOTOR_SPEED_LIMIT);
	IncrementPidLimit(&br_pid,-CHASSIS_MOTOR_SPEED_LIMIT,CHASSIS_MOTOR_SPEED_LIMIT);
}

