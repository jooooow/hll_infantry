#include "pid.h"
#include "stdlib.h"

/*四轮PID结构体变量*/
IncrementPid fl_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};
IncrementPid fr_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};
IncrementPid bl_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};
IncrementPid br_pid = {0,0,  9,0.4,0,  0,0,0,0,0,0,0};

/*
  函数名：CalcPositionPid
  描述  ：计算位置式PID
  参数  ：pid ：pid结构体变量
  返回值：无
*/
void CalcPositionPid(PositionPid* pid)
{
	pid->error = pid->tar - pid->cur;
	
	pid->kp_output = pid->kp * pid->error;
	pid->integration += pid->error;
	pid->ki_output = pid->ki * pid->integration;
	pid->kd_output = pid->kd * (pid->error - pid->old_error);
	
	pid->old_error = pid->error;
	pid->output = pid->kp_output + pid->ki_output + pid->kd_output;
}

/*
  函数名：CalcIncrementPid
  描述  ：计算增量式PID
  参数  ：pid ：pid结构体变量
  返回值：无
*/
void CalcIncrementPid(IncrementPid* pid)
{
	static float error_dev = 0;
	
	pid->error = pid->tar - pid->cur;
	if(abs(pid->error) < 20)
		pid->error = 0;
	
	error_dev = (pid->error - pid->old_error);
	pid->kp_output = pid->kp * error_dev;
	pid->ki_output = pid->ki * pid->error;
	pid->kd_output = pid->kd * (pid->error - 2 * pid->old_error + pid->old_old_error);
	
	pid->old_old_error = pid->old_error;
	pid->old_error = pid->error;
	pid->output += pid->kp_output + pid->ki_output + pid->kd_output;
}

/*
  函数名：PositionPidLimit
  描述  ：位置式PID输出限幅
  参数  ：pid ：pid结构体变量
  返回值：无
*/
void PositionPidLimit(PositionPid* pid, float min, float max)
{
	if(pid->output > max)
		pid->output = max;
	else if(pid->output < min)
		pid->output = min;
}

/*
  函数名：IncrementPidLimit
  描述  ：增量式PID输出限幅
  参数  ：pid ：pid结构体变量
  返回值：无
*/
void IncrementPidLimit(IncrementPid* pid, float min, float max)
{
	if(pid->output > max)
		pid->output = max;
	else if(pid->output < min)
		pid->output = min;
}
