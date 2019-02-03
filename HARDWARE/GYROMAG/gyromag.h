/*
  文件名：gyromag.h
	作者  ：hjm
	描述  ：陀螺仪和地磁仪初始化与读取
	历史  ：2019-01-15 新增文件
*/

#ifndef __GYROMAG_H__
#define __GYROMAG_H__

#define MPU_9250_ENABLE  PEout(4) = 0
#define MPU_9250_DISABLE PEout(4) = 1

#define MPU_6500_DISENABLE  GPIOF->BSRRL = GPIO_Pin_6;
#define MPU_6500_ENABLE  GPIOF->BSRRH = GPIO_Pin_6;

#define IS_YAW_ANGLE_VAILD        (angle_yaw_valid == 1)
#define IS_PITCH_ANGLE_VALID      (angle_pitch_valid = 1)

#define GYRO_XOUT_H  				0x43
#define GYRO_XOUT_L  				0x44
#define GYRO_ZOUT_H  				0x47
#define GYRO_ZOUT_L  				0x48
#define PWR_MGMT_1   				0x6B
#define PWR_MGMT_2   				0x6C
#define CONFIG       				0x1A
#define SMPLRT_DIV   				0x19
#define GYRO_CONFIG  				0x1B
#define INT_PIN_CFG  				0x37
#define I2C_MST_CTRL 				0x24
#define USER_CTRL   				0x6A
#define I2C_MST_DELAY_CTRL 	0x67
#define I2C_SLV0_CTRL 		 	0x27
#define SIGNAL_PATH_RESET  	0x68
#define WHO_AM_I      		 	0x75

#define HMC5983_ADDR				0x3C

#include "spi.h"
#include "iic.h"
#include "math.h"
#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "control.h"
#include "listener.h"
#include "calibrater.h"

typedef struct
{
	float theta;
	float w_cur;
	float w_raw;
	float w_dev;
	float dt;
}Angle_t;

void InitGyroMagTask(void);
void InitGyro(void);
void InitMag(void);
u8 SetMPU9250(void);

void ReadGyroMag(void);
void ReadGyro(void);
float ReadGyroRaw(void);
void ReadMag(void);
float ReadMagRaw(void);

u8 GyroCheck(void);
u8 MagCheck(void);

u8 AngleYawCheck(void);
u8 AnglePitchCheck(void);

void GyroCalibrate(Angle_t* angle,u16 cnt);
u16 GyroSingleCalibrate(Angle_t* angle,u16 cnt);

u8 MPU9250WriteReg(u8 reg,u8 value);
u8 MPU9250ReadReg(u8 reg);

void Write_HMC5983(u8 add, u8 da);
u8 Read_HMC5983(u8 REG_Address);
void Multiple_read_HMC5983(u8*BUF);
void HMC_Get_Data(short*DAT);

u8 MPU6500_Write_Reg(u8 reg,u8 value);
u8 MPU6500_Read_Reg(u8 reg);
u8 initMPU6500(void);
void READ_MPU6500_GYRO(void);

float GetGyroRawData(void);

#endif

