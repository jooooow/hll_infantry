#ifndef __CALIBRATER_H__
#define __CALIBRATER_H__

#include "stm32f4xx.h"

typedef struct _OffsetCalibrater
{
	unsigned char  max_loop_cnt;
	unsigned short single_loop_cnt;
	
	float new_data;
	float filter_min;
	float filter_max;
	float max_invalid_rate;
	float invalid_rate;
	float offset_output;
	
	float (*GetData)(void);
	unsigned char (*StartCalibrate)(struct _OffsetCalibrater* self);
}OffsetCalibrater;

unsigned char AverageCalibrate(OffsetCalibrater* self);

#endif

