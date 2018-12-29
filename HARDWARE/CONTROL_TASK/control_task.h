/*
  文件名：control_task.h
	作者  ：hjm
	描述  ：初始化各个模块
	        模块状态监测
	        模块任务切换
					历史保存
	历史  ：2018-12-19 修改头文件引用方式，不再引用includes.h
	        2018-12-21 添加OnelineFlagLog
					           添加State_t
					           添加UpdateRobotFSM
										 添加OnRobotStateSwitch
					2018-12-23 删除OnelineFlagLog
					           添加UpdateRobotFSM
										 添加OnRobotStateSwitch
					2018-12-24 删除UpdateRobotControlMode
*/
#ifndef __CONTROL_TASK_H__
#define __CONTROL_TASK_H__

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "timer3.h"
#include "usart1.h"
#include "usart3.h"
#include "usart6.h"
#include "can1.h"
#include "pid.h"
#include "remoter.h"
#include "listener.h"
#include "chassis.h"
#include "gimbal.h"
#include "shooter.h"

#define STATE_SWITCHD_TO(a,b) ((robot_FSM.old_state == (a)) && (robot_FSM.cur_state == (b)))
#define START_CONTROL_TASK TIM_Cmd(TIM3,ENABLE)

void UpdateRobotFSM(void);
void OnRobotStateSwitch(void);

void InitControlTask(void);
void ControlTask(void);
void SaveHistory(void);

void ChassisMotorListen(void);
void GimbalMotorListen(void);

#endif
