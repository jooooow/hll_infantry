#ifndef __CAN_H__
#define __CAN_H__

#include "includes.h"

#define FL       0  //左前轮      0x201
#define FR       1  //右前轮      0x202
#define BL       2  //左后轮      0x203
#define BR       3  //右后轮      0x204
#define PITCH    4  //云台PITCH轴 0x205
#define YAW      5  //云台YAW轴   0x206

void InitCan1(void);
void InitCan1NVIC(void);

void Can1SendMsg2Chassis(short fl, short fr, short bl, short br);

#endif
