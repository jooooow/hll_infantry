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
	
	unsigned char* reload_hook;  //reload_hookΪ1ʱreload
	unsigned char* descend_hook; //descend_hookΪ1ʱ��ΪNULLʱ�ŵݼ�
	
	void (*Start)(struct _SoftTimer* self);
	void (*Reload)(struct _SoftTimer* self);
	void (*OnTimeOut)(void);     //cntΪ0�Զ����ã���Ҫ�ֶ�reload
}SoftTimer;

void SoftTimerDescend(SoftTimer* self);
void SoftTimerReload(SoftTimer* self);

#endif

