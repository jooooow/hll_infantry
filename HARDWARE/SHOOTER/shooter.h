/*
  文件名：shooter.h
	作者  ：hjm
	描述  ：控制发射机构
	历史  ：2018-12-21 新增文件
				  2019-01-18 
*/
#ifndef __SHOOTER_H__
#define __SHOOTER_H__

#define ROTATE_SPEED         3000
#define ROTATE_MAX_ABS_SPEED 6000

#define FRIC_STA_STOP          0
#define FRIC_STA_ROTATE        1
#define FRIC_TRIGGER_SPEED     1030
#define FRIC_FREEZE_SPEED      1020

#define WAVE_POSI    SetWaveTarSpeed(ROTATE_SPEED)
#define WAVE_NEGA    SetWaveTarSpeed(-ROTATE_SPEED)
#define WAVE_STOP		 SetWaveTarSpeed(0)

#define WAVE_NO_BLOCK		 0
#define WAVE_BLOCK       1

#define WAVE_STA_STOP        0
#define WAVE_STA_POSITIVE    1
#define WAVE_STA_NEGATIVE    2

/*堵转时间 单位2ms*/
#define BLOCK_TIME   150
/*反转时间 单位2ms*/
#define REVERSE_TIME 70

#include "sys.h"
#include "pwm.h"
#include "pid.h"
#include "control.h"
#include "can2.h"
#include "remoter.h"

void InitShootTask(void);
void ShootTask(void);

void R_ConvertRc2TargetOnoff(void);
void S_ConvertRc2TargetOnoff(void);
void A_ConvertRc2TargetOnoff(void);

void WavePositiveRotate(void);
void WaveNegativeRotate(void);
void WaveStop(void);
void SetWaveTarSpeed(short speed);

u8 WaveBlockDetect(PositionPid_t* wave_pid);
void OnIntoBlock(void);

void FricTask(void);
void FricRotate(void);
void FricStop(void);

#endif
