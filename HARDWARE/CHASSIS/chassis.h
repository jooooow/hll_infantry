/*
  文件名：chassis.h
	作者  ：hjm
	描述  ：计算四轮pid
	        发送pid输出值给电调
	历史  ：2018-12-21 修改头文件引用方式，不再引用includes.h
	        2018-12-24 删除ConvertRc2TargetSpeed
					           添加R_ConvertRc2TargetSpeed
										 添加S_ConvertRc2TargetSpeed
										 添加A_ConvertRc2TargetSpeed
*/
#ifndef __CHASSIS_H__
#define __CHASSIS_H__

#include "sys.h"
#include "can1.h"
#include "pid.h"
#include "remoter.h"

void InitChassisControlTask(void);
void ChassisControlTask(void);

void R_ConvertRc2TargetSpeed(void);
void S_ConvertRc2TargetSpeed(void);
void A_ConvertRc2TargetSpeed(void);

void CalcChassisPID(void);
void LimitChassisOutput(void);



#endif

