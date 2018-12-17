#ifndef __REMOTER_H__
#define __REMOTER_H__

#include "includes.h"

typedef struct
{
	int16_t ch0;
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	uint8_t s1;
	uint8_t s2;
}Remoter;

typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t press_l;
	uint8_t press_r;
}Mouse;	

typedef	struct
{
	uint16_t value;
}Key;

typedef struct
{
	Remoter rc;
	Mouse mouse;
	Key key;
}RemoteController;

void InitRemoterTask(void);
void RemoteTask(void);
void BlinkRedLed(void);
void BlinkGreenLed(void);
void ParseRemoterData(RemoteController* remoter);
void RemoterDataFilter(RemoteController* remoter);
u8 RemoterDataCheck(void);
void SetRemoteValue(RemoteController* remoter, short ch0, short ch1, short ch2, short ch3, u8 s1, u8 s2);
void RemoterDataLog(RemoteController* remoter);

#endif
