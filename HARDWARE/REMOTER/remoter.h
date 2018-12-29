/*
  文件名：remoter.h
	作者  ：hjm
	描述  ：遥控器结构体定义
	        检查接收到的遥控器原始数据合法性
					转化原始数据位遥控器通道值
					根据遥控器通道值设置遥控器结构体变量
	历史  ：2018-12-19 修改头文件引用方式，不再引用includes.h
	        2018-12-21 增加OnSwitchChanged函数
					           增加switch状态监测marco
					2018-12-24 添加UpdateRobotControlMode
					           RemoteController_t中添加ControlMode_t
*/
#ifndef __REMOTER_H__
#define __REMOTER_H__

#include "sys.h"
#include "stdio.h"
#include "led.h"
#include "string.h"
#include "control.h"
#include "pid.h"
#include "listener.h"

#define S1_VALUE       remote_controller.rc.s1
#define S2_VALUE       remote_controller.rc.s2
#define OLD_S1_VALUE   old_remote_controller.rc.s1
#define OLD_S2_VALUE   old_remote_controller.rc.s2

#define S1_EQUAL_1     (remote_controller.rc.s1 == 1)
#define S1_EQUAL_2     (remote_controller.rc.s1 == 2)
#define S1_EQUAL_3     (remote_controller.rc.s1 == 3)
#define S2_EQUAL_1     (remote_controller.rc.s2 == 1)
#define S2_EQUAL_2     (remote_controller.rc.s2 == 2)
#define S2_EQUAL_3     (remote_controller.rc.s2 == 3)

#define OLD_S1_EQUAL_1 (old_remote_controller.rc.s1 == 1)
#define OLD_S1_EQUAL_2 (old_remote_controller.rc.s1 == 2)
#define OLD_S1_EQUAL_3 (old_remote_controller.rc.s1 == 3)
#define OLD_S2_EQUAL_1 (old_remote_controller.rc.s2 == 1)
#define OLD_S2_EQUAL_2 (old_remote_controller.rc.s2 == 2)
#define OLD_S2_EQUAL_3 (old_remote_controller.rc.s2 == 3)

#define S1_CHANGED     (S1_VALUE != OLD_S1_VALUE)
#define S2_CHANGED     (S2_VALUE != OLD_S2_VALUE)

#define S1_CHANGED_TO(a,b) ((OLD_S1_VALUE == (a)) && (S1_VALUE == (b)))
#define S2_CHANGED_TO(a,b) ((OLD_S2_VALUE == (a)) && (S2_VALUE == (b)))


typedef struct
{
	int16_t ch0;
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	uint8_t s1;
	uint8_t s2;
}Remoter_t;

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t press_l;
	uint8_t press_r;
}Mouse_t;	

typedef	struct
{
	uint16_t value;
}Key_t;

typedef struct
{
	Remoter_t rc;
	Mouse_t mouse;
	Key_t key;
	ControlMode_t mode;
}RemoteController_t;

void InitRemoterTask(void);
void RemoteTask(void);

void UpdateRobotControlMode(void);

void OnRemoteModeChanged(void);

void ParseRemoterData(RemoteController_t* remoter);
void OnSwitchChanged(void);
void RemoterDataFilter(RemoteController_t* remoter);
u8 RemoterDataCheck(void);
void SetRemoteValue(RemoteController_t* remoter, short ch0, short ch1, short ch2, short ch3, u8 s1, u8 s2);
void RemoterDataLog(RemoteController_t* remoter);

#endif
