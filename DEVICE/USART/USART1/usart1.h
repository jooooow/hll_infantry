#ifndef __USART1_H__
#define __USART1_H__

#include "stm32f4xx.h"

void InitUsart1(void);
void InitUsart1DMA(void);
void InitUsart1DMANVIC(void);

void InitUsartIDLENVIC(void);
void ResetUsart1DMA(void);

#endif
