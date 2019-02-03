#ifndef __CAN2_H__
#define __CAN2_H__

#include "stdint.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx.h"
#include "led.h"
#include "pid.h"
#include "sys.h"
#include "control.h"

void InitCan2(void);
void Can2Send2Gimbal(void);

#endif 