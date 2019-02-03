/*
  文件名：pid.h
	作者  ：hjm
	描述  ：pid结构体定义
	        pid算法实现
	历史  ：2018-12-21 修改头文件引用方式，不再引用includes.h
*/
#ifndef __PID_H__
#define __PID_H__

#include "stdlib.h"
#include "math.h"
#include "sys.h"
#include "stdio.h"

#define abs2(x) ((x)>0? (x):(-(x))) 

typedef struct PPID_t
{
	float tar;
	float cur;
	
	float kp;
	float ki;
	float kd;
	
	float error;
	float old_error;
	float integration;
	
	float kp_output;
	float ki_output;
	float kd_output;
	float output;
	void(*CalcPID)(struct PPID_t* pid, float inte_max_abs, float error_min_abs);
}PositionPid_t;

typedef struct IPID_t
{
	float tar;
	float cur;
	
	float kp;
	float ki;
	float kd;
	
	float error;
	float old_error;
	float old_old_error;
	
	float kp_output;
	float ki_output;
	float kd_output;
	float output;
	void(*CalcPID)(struct IPID_t* pid);
}IncrementPid_t;

typedef struct GPID_t
{
	float P;
	float I;
	float D;
	
	float CurrentError;
	float LastError;
	float ThirdError;
	float ForthError;
	
	float Pout;
	float Iout;
	float Dout;
	float PIDout;
	
	float IMax;
	float PIDMax;
	
	float cur;
	float tar;
	void(*CalcPID)(struct GPID_t* pid);
}GimbalPID_t;

void CalcPositionPid(PositionPid_t* pid, float inte_max_abs, float error_min_abs);
void CalcIncrementPid(IncrementPid_t* pid);

void ResetPositionPid(PositionPid_t* pid, u8 depth, u8 tar);
void ResetIncrementPid(IncrementPid_t* pid, u8 depth, u8 tar);

void PositionPid_tLimit(PositionPid_t* pid, float min, float max);
void IncrementPid_tLimit(IncrementPid_t* pid, float min, float max);

void PositionPidLog(PositionPid_t* pid);
void IncrementPidLog(IncrementPid_t* pid);
void CalcPitchPid(GimbalPID_t* pid);
void CalcYawPid(GimbalPID_t* pid);

#endif
