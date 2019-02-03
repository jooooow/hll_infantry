#ifndef __USART3_H__
#define __USART3_H__

#include "stm32f4xx.h"
#include "sys.h"
#include "stdio.h"

void InitUsart3(void);
void InitUsart3NVIC(void);
void Usart3Log(u8* data, u8 len);

#endif
