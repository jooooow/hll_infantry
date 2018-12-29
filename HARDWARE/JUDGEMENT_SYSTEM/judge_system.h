/*
  文件名：judge_system.h
	作者  ：hjm
	描述  ：读取裁判系统返回数据
	历史  ：2018-12-21 修改头文件引用方式，不再引用includes.h
*/
#ifndef __JUDGE_SYSTEM_H__
#define __JUDGE_SYSTEM_H__

#include "sys.h"
#include "usart6.h"
#include "DJ_Protocol.h"

void InitJudgeSystemTask(void);
u8 ReadJudgeSystem(void);

#endif
