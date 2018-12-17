#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx.h"
#include "sys.h"

#define TAG_TIMER       PFout(1) 
#define TAG_TIMER_ON    (TAG_TIMER = 1)
#define TAG_TIMER_OFF   (TAG_TIMER = 0)

#define TAG_CHASSIS       PFout(0) 
#define TAG_CHASSIS_ON    (TAG_CHASSIS = 1)
#define TAG_CHASSIS_OFF   (TAG_CHASSIS = 0)

#define LED_GREEN PFout(14)
#define LED_RED   PEout(7)
#define LED_GREEN_BLINK (LED_GREEN = ~LED_GREEN)
#define LED_RED_BLINK   (LED_RED = ~LED_RED)

void InitLed(void);

#endif
