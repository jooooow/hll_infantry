/*
  文件名：control.h
	作者  ：hjm
	描述  ：定义各模块索引
	        定义各模块Listener重装时间
	        使能/失能打印调试
	历史  ：2018-12-19 新增文件
	        2018-12-21 修改REMOTER_LOSS_TIME
					           修改WAVE
										 修改REMOTER
				  2018-12-24 添加控制状态
					           添加整车状态
										 添加模块使能掩码
										 添加WAVE_CONTROL_CYCLE
										 添加ControlMode_t
										 添加State_t
*/

#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "stm32f4xx.h"

/*整车状态*/
#define INITING           0  //初始化中
#define NORMAL            1  //正常
#define PROBLEM           2  //故障(出现问题但不影响整车)
#define BLOCK             3  //阻塞(出现严重问题需要立刻停止)

/*控制状态*/
#define REMOTER_CONTROL   0  //遥控器控制
#define SOFTWARE_CONTROL  1  //软件控制
#define AUTO              2  //自动
#define FORBID            3  //禁止控制

/*各模块索引*/
#define FL                0  //左前轮      0x201
#define FR                1  //右前轮      0x202
#define BL                2  //左后轮      0x203
#define BR                3  //右后轮      0x204
#define PITCH             4  //云台PITCH轴 0x205
#define YAW               5  //云台YAW轴   0x206
#define WAVE              6  //拨轮所索引
#define REMOTER           7  //遥控器索引
#define GYRO              8	 //陀螺仪
#define MAG               9  //地磁仪

/*模块使能掩码*/
#define MODULE_MASK       0x7F

/*各模块Listener重装时间 单位ms*/
#define REMOTER_LOSS_TIME 28 //遥控器重装值
#define MOTOR_LOSS_TIME   100  //电机重装值
#define GYRO_LOSS_TIME    2
#define MAG_LOSS_TIME     2

/*电机控制周期 单位ms*/
#define GIMBAL_CONTROL_CYCLE  1
#define CHASSIS_CONTROL_CYCLE 4
#define WAVE_CONTROL_CYCLE    2

/*打印调试使能/失能*/
#define ONLINE_FLAG_LOG   0  //打印ONLINE标志寄存器
#define REMOTE_DATA_LOG   0  //打印遥控器数据
#define CONNECT_LOSS_LOG  1  //打印连接/丢失提示


typedef struct
{
	unsigned char cur_state;
	unsigned char old_state;
	unsigned char code;
}State_t;

typedef struct
{
	unsigned char cur_mode;
	unsigned char old_mode;
}ControlMode_t;

#endif