/*
  文件名：gimbal.h
	作者  ：hjm
	描述  ：计算云台pid
	        发送pid输出值给云台电调
	历史  ：2018-12-21 新增文件
          2018-12-24 增加R_ConvertRc2TargetAngle
					           增加S_ConvertRc2TargetAngle
										 增加A_ConvertRc2TargetAngle
*/
#ifndef __GIMBAL_H__
#define __GIMBAL_H__

#include "sys.h"
#include "pid.h"
#include "remoter.h"

void InitGimbalControlTask(void);
void GimbalControlTask(void);

void R_ConvertRc2TargetAngle(void);
void S_ConvertRc2TargetAngle(void);
void A_ConvertRc2TargetAngle(void);

#endif

