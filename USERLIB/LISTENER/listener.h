/*
  文件名：listener.h
	作者  ：hjm
	描述  ：监听各个模块在线/离线状态
	历史  ：2018-12-19 修改头文件引用方式，不再引用includes.h
	        2018-12-21 添加RELOAD宏
					2018-12-24 修改初始化函数
					           添加监听WAVE电机
										 添加ListenModule
										 添加OnModuleStateChanged
*/
#ifndef __LISTENER_H__
#define __LISTENER_H__

#include "sys.h"
#include "remoter.h"
#include "stdio.h"
#include "usart1.h"
#include "control_task.h"

#define GET_STATE(M)     (ONLINE_FLAG & (0x01 << M))
#define GET_OLD_STATE(M) (OLD_ONLINE_FLAG & (0x01 << M))

#define RELOAD_REMOTER   listen_cnt[REMOTER] = REMOTER_LOSS_TIME
#define RELOAD_MOTOR(a)  listen_cnt[a] = MOTOR_LOSS_TIME
#define RELOAD_GYRO			 listen_cnt[GYRO] = GYRO_LOSS_TIME
#define RELOAD_MAG		   listen_cnt[MAG] = MAG_LOSS_TIME

void InitListenerTask(void);
void ListenTask(void);
void ListenModule(void);
void OnModuleStateChanged(void);
void Listen(u8 index);
#endif
