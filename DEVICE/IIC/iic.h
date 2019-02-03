#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"

#define RCC_IIC 		0x00000010
#define SDA_OUT() 	{GPIOF->MODER |= 0x01;}
#define SDA_IN() 	  {GPIOF->MODER &= (~0x03);}
#define SCL_OUT() 	{GPIOF->MODER |= 0x04;}
#define IIC_SDA 	PFout(0)
#define IIC_SCL 	PFout(1)
#define READ_SDA	PFin(0)

void InitIIC(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(u8);
u8 IIC_Read_Byte(u8);

#endif

