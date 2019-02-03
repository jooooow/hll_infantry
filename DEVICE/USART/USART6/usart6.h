#ifndef __USART6_H__
#define __USART6_H__

#include "sys.h"
#include "stm32f4xx.h"

void InitUsart6(void);
void InitUsart6DMA(void);
void InitUsart6NVIC(void);
void InitUsart6DMANVIC(void);

#endif
