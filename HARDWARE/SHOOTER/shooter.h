/*
  文件名：shooter.h
	作者  ：hjm
	描述  ：控制发射机构
	历史  ：2012-12-21 新增文件
*/
#ifndef __SHOOTER_H__
#define __SHOOTER_H__

#include "sys.h"
#include "pwm.h"
#include "pid.h"

void InitShootTask(void);
void ShootTask(void);

#endif
