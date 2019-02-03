#ifndef __SPI_H__
#define __SPI_H__

#define SPI4_NOERROR     0
#define SPI4_UNRECHIEVED 1
#define SPI4_UNRECEIVED  2

#include "stm32f4xx.h"

void InitSPI4(void);
void InitSPI5(void);
u8 SPI4ReadWriteByte(u8 byte);
u8 SPI5ReadWriteByte(u8 TxData);

#endif

