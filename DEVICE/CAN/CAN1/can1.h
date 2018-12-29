#ifndef __CAN1_H__
#define __CAN1_H__

#include "stm32f4xx.h"
#include "control.h"
#include "listener.h"

void InitCan1(void);
void InitCan1NVIC(void);

void Can1SendMsg2Chassis(short fl, short fr, short bl, short br);
void Can1SendMsg2WaveWheel(short current);

#endif
