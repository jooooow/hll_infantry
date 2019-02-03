#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f4xx.h"

void InitPWM(void);
void SetFricSpeed(u16 mid, u16 top);
void FricSpeedClose2(u8 id, u16 target, u16 dev);

#endif
