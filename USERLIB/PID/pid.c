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
void CalcPositionPid(PositionPid_t* pid, float inte_max_abs, float error_min_abs)
{
	pid->error = pid->tar - pid->cur;
	
	if(fabs(pid->error) < error_min_abs)
		pid->error = 0;
	
	pid->kp_output = pid->kp * pid->error;
	pid->integration += pid->error;
	
	if(pid->integration > inte_max_abs)
		pid->integration = inte_max_abs;
	else if(pid->integration < -inte_max_abs)
		pid->integration = -inte_max_abs;
	
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

void CalcPitchPid(GimbalPID_t* pid)
{
	float index_i;
	static float  sump;
	pid->CurrentError = pid->tar - pid->cur;
	
	
	if(abs2(pid->CurrentError)<=2)pid->CurrentError=0;
	else if(pid->CurrentError>2)pid->CurrentError-=2;
	else if(pid->CurrentError<-2)pid->CurrentError+=2;
	
	pid->Pout = pid->P * pid->CurrentError;
	
	if(abs2(pid->CurrentError) > 100)
	{
		index_i = 0.0;
	}
	else if(abs2(pid->CurrentError) >= 50 && abs2(pid->CurrentError) <= 100)
	{	
		index_i = (100 - abs2(pid->CurrentError)) / 50;
		sump +=( pid->CurrentError)/2;
	}
	else 
	{
		index_i = 1.;
		sump += (pid->CurrentError)/2;
	}
	sump = sump > pid->IMax ? pid->IMax : sump;
	sump = sump < -pid->IMax ? -pid->IMax : sump; 
	pid->Iout = index_i * pid->I * sump;
	pid->Iout = pid->Iout > pid->IMax ? pid->IMax : pid->Iout;
	pid->Iout = pid->Iout < -pid->IMax ? -pid->IMax : pid->Iout;
	
	pid->Dout = pid->D * (pid->CurrentError + pid->LastError- pid->ThirdError-pid->ForthError);
	
	pid->PIDout = pid->Pout + pid->Iout  + pid->Dout;
	
	pid->PIDout = pid->PIDout > pid->PIDMax ? pid->PIDMax : pid->PIDout;
	pid->PIDout = pid->PIDout < -pid->PIDMax ? -pid->PIDMax : pid->PIDout;
	

  pid->ForthError = pid->ThirdError;
	pid->ThirdError = pid->LastError;
	pid->LastError = pid->CurrentError;
}

void CalcYawPid(GimbalPID_t* pid)
{
	static short sum, sump;
	float index_i;

	pid->CurrentError = pid->tar - pid->cur;
	
	if(abs2(pid->CurrentError)<=2)pid->CurrentError=0;
	else if(pid->CurrentError>2)pid->CurrentError-=2;
	else if(pid->CurrentError<-2)pid->CurrentError+=2;
	
	pid->Pout = pid->P * pid->CurrentError;
	
	if(abs2(pid->CurrentError) > 100)
	{
		index_i = 0;
	}
	else if(abs2(pid->CurrentError) >= 50 && abs2(pid->CurrentError) <= 100)
	{	
		index_i = (100 - abs2(pid->CurrentError)) / 100;
		sum += pid->CurrentError;
	}
	else 
	{
		index_i = 1.;
		sum += pid->CurrentError;
	}
	
	sum = sum > pid->IMax ? pid->IMax : sum;
	sum = sum < -pid->IMax ? -pid->IMax : sum; 
	pid->Iout = index_i * pid->I * sum;
	
	pid->Dout = pid->D * (pid->CurrentError - pid->LastError);
	
	pid->PIDout = pid->Pout + pid->Iout + pid->Dout;
	
	pid->PIDout = pid->PIDout > pid->PIDMax ? pid->PIDMax : pid->PIDout;
	pid->PIDout = pid->PIDout < -pid->PIDMax ? -pid->PIDMax : pid->PIDout;
	
	pid->LastError = pid->CurrentError;
}