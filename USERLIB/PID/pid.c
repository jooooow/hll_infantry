#include "pid.h"

/*四轮PID结构体变量*/
extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;

/*
  函数名：CalcPositionPid
  描述  ：计算位置式PID
  参数  ：pid ：pid结构体变量
  返回值：无
*/
void CalcPositionPid(PositionPid_t* pid)
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
void CalcIncrementPid(IncrementPid_t* pid)
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
  函数名：ResetPositionPid
  描述  ：复位位置式pid
  参数  ：pid   -- 位置式pid结构体变量
          depth -- error清除深度
          tar   -- 新的目标值
*/
void ResetPositionPid(PositionPid_t* pid, u8 depth, u8 tar)
{
	switch(depth)
	{
		case 2:pid->old_error = 0;
		case 1:pid->error = 0;
		default:break;
	}
	pid->tar = tar;
	pid->integration = 0;
	pid->kp_output = 0;
	pid->ki_output = 0;
	pid->kd_output = 0;
	pid->output = 0;
}


/*
  函数名：ResetIncrementPid
  描述  ：复位增量式pid
  参数  ：pid   -- 增量式pid结构体变量
          depth -- error清除深度
          tar   -- 新的目标值
*/
void ResetIncrementPid(IncrementPid_t* pid, u8 depth, u8 tar)
{
	switch(depth)
	{
		case 3:pid->old_old_error = 0;
		case 2:pid->old_error = 0;
		case 1:pid->error = 0;
		default:break;
	}
	pid->tar = tar;
	pid->kp_output = 0;
	pid->ki_output = 0;
	pid->kd_output = 0;
	pid->output = 0;
}

/*
  函数名：PositionPid_tLimit
  描述  ：位置式PID输出限幅
  参数  ：pid -- 位置式pid结构体变量
  返回值：无
*/
void PositionPid_tLimit(PositionPid_t* pid, float min, float max)
{
	if(pid->output > max)
		pid->output = max;
	else if(pid->output < min)
		pid->output = min;
}

/*
  函数名：IncrementPid_tLimit
  描述  ：增量式PID输出限幅
  参数  ：pid -- 增量式pid结构体变量
  返回值：无
*/
void IncrementPid_tLimit(IncrementPid_t* pid, float min, float max)
{
	if(pid->output > max)
		pid->output = max;
	else if(pid->output < min)
		pid->output = min;
}


/*
  函数名：PositionPidLog
  描述  ：打印位置式PID值
  参数  ：pid -- 位置式pid结构体变量
  返回值：无
*/
void PositionPidLog(PositionPid_t* pid)
{
	printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\r\n",
	pid->tar,
	pid->cur,
	pid->error,
	pid->old_error,
	pid->output);
}


/*
  函数名：IncrementPidLog
  描述  ：打印增量式PID值
  参数  ：pid -- 增量式pid结构体变量
  返回值：无
*/
void IncrementPidLog(IncrementPid_t* pid)
{
	printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\r\n",
	pid->tar,
	pid->cur,
	pid->error,
	pid->old_error,
	pid->old_old_error,
	pid->output);
}
