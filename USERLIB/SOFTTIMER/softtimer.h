#ifndef __SOFTTIMER_H__
#define __SOFTTIMER_H__

#include "stm32f4xx.h"
#include "stdlib.h"

#define new_delay(a) {a,a,0,0,  NULL,NULL,  SoftTimerDescend,SoftTimerReload,  NULL};
#define Delay_t SoftTimer

typedef struct _SoftTimer
{
	unsigned short cnt;
	unsigned short reload_cnt;
	unsigned char time_out_flag;
	unsigned char old_time_out_flag;
	
	unsigned char* reload_hook;  //reload_hook为1时reload
	unsigned char* descend_hook; //descend_hook为1时或为NULL时才递减
	
	void (*Start)(struct _SoftTimer* self);
	void (*Reload)(struct _SoftTimer* self);
	void (*OnTimeOut)(void);     //cnt为0自动调用，需要手动reload
}SoftTimer;

void SoftTimerDescend(SoftTimer* self);
void SoftTimerReload(SoftTimer* self);

#endif

