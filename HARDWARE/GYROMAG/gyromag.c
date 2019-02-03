#include "gyromag.h"

#define DT  						  0.001f
#define GYRO_CALI_MAX_W   0.8f
#define GYRO_CALI_MIN_P   0.97f

Angle_t yaw   = {0.0f, 0.0f, 0.0f, 0.0f, DT};
Angle_t pitch = {0.0f, 0.0f, 0.0f, 0.0f, DT};

PositionPid_t gyro_pid = {0,0,  0.0003,0.0000001,0.1,  0,0,0,  0,0,0,0,  CalcPositionPid};

OffsetCalibrater gyro_offset_calibrater = {20,1500,  0.0f,0.30f,0.9f,0.01f,0.0f,0.0f,  GetGyroRawData,AverageCalibrate};

float yaw_mag = 0;

u8 angle_yaw_valid = 0;
u8 angle_pitch_valid = 0;

extern u8 listen_cnt[10];
extern float mpu6500_gyro_w_z;
float mpu6500_yaw = 0.0f;

void InitGyroMagTask(void)
{
	//InitGyro();
	while(!initMPU6500());
	
	
	delay_ms(1000);
	gyro_offset_calibrater.StartCalibrate(&gyro_offset_calibrater);
	//GyroCalibrate(&yaw,2000);
	
	InitMag();
}

void InitGyro(void)
{
	u8 id;
	delay_ms(1000);
	while(id != 0x71)
	{
		id = SetMPU9250();
		printf("  mpu9250 id : %d\r\n",id);
		delay_ms(100);
	}
	delay_ms(200);
	
	printf("  gyro calibrating.. \r\n  please don't move the gimbal.\r\n");
	GyroCalibrate(&yaw,1000);
	printf("  gyro calibrate done!\r\n\r\n");
}

void InitMag(void)
{
	Write_HMC5983(0x00, 0x9c);
	Write_HMC5983(0x01, 0x20);
	Write_HMC5983(0x02, 0x00);
}

u8 SetMPU9250(void)
{
	u8 id = MPU9250ReadReg(WHO_AM_I);
	delay_ms(200);
	MPU9250WriteReg(PWR_MGMT_1, 0x80);
	delay_ms(200);
	MPU9250WriteReg(PWR_MGMT_1,0X00);
	delay_ms(200);
	MPU9250WriteReg(SMPLRT_DIV, 0x00);
	delay_ms(200);
	MPU9250WriteReg(GYRO_CONFIG, 0x18);
	delay_ms(200);
	return id;
}

extern u8 is_start_accel_cali;
extern u8 is_start_gyro_cali;

void ReadGyroMag(void)
{
	if(is_start_gyro_cali)
	{
		gyro_offset_calibrater.StartCalibrate(&gyro_offset_calibrater);
		is_start_gyro_cali = 0;
	}
	RELOAD_GYRO;
	RELOAD_MAG;
	
	ReadGyro();
	ReadMag();
	
//	gyro_pid.cur = yaw.theta;
//	gyro_pid.tar = yaw_mag;
//	gyro_pid.CalcPID(&gyro_pid,180,0);
//	yaw.theta += gyro_pid.output;
	
	angle_pitch_valid = AnglePitchCheck();
	angle_yaw_valid = AngleYawCheck();
}

void ReadGyro(void)
{
	READ_MPU6500_GYRO();
	//mpu6500_yaw += mpu6500_gyro_w_z*0.001f;
	//yaw.w_raw = ReadGyroRaw();
	yaw.w_raw = mpu6500_gyro_w_z;
	
	//yaw.w_cur = yaw.w_raw - yaw.w_dev;
	yaw.w_cur = yaw.w_raw - gyro_offset_calibrater.offset_output;
	
	yaw.theta += yaw.w_cur * yaw.dt;
}

float ReadGyroRaw(void)
{
	static u8 BUF[6];
	static short g_z;
  BUF[0] = MPU9250ReadReg(GYRO_ZOUT_L); 
	delay_us(10);
  BUF[1] = MPU9250ReadReg(GYRO_ZOUT_H);
  g_z =	(BUF[1]<<8)|BUF[0];
	return g_z/16.4;
}

u8 GyroCheck(void)
{
	
}

u8 MagCheck(void)
{
	
}

u8 AngleYawCheck(void)
{
	return 1;
}

u8 AnglePitchCheck(void)
{
	return 1;
}

void GyroCalibrate(Angle_t* angle,u16 cnt)
{
	u16 cali_cnt = 0;
	u16 valid_cnt = 0;
	while(1)
	{
		cali_cnt++;
		valid_cnt = GyroSingleCalibrate(angle, cnt);
		printf("  gyro cali_loop[%d] done, dev : %.4f\r\n",cali_cnt,yaw.w_dev);
		
		/*有效校准次数小于目标校准次数的1/2视为校准失败，则重新校准*/
		if(valid_cnt < cnt * GYRO_CALI_MIN_P)
		{
			printf("  gyro cali_loop[%d] failed, please don't move the gimbal!\r\n\r\n",cali_cnt);
		}
		else
		{
			break;
		}
		delay_ms(300);
	}
}

u16 GyroSingleCalibrate(Angle_t* angle,u16 cnt)
{
	float sum = 0;
	float res;
	u16 cnt_temp = cnt;
	while(cnt)
	{
		READ_MPU6500_GYRO();
		//res = ReadGyroRaw();
		res = mpu6500_gyro_w_z;
		
		/*校准时若出现较大幅度的角速度则舍弃*/
		if(fabs(res) > GYRO_CALI_MAX_W)
		{
			res = 0;
			cnt_temp--;
		}
		sum += res;
		cnt--;
		delay_ms(1);
	}
	angle->w_dev = (float)sum/cnt_temp;
	return cnt_temp;
}

u8 MPU6500_Write_Reg(u8 reg,u8 value)
{
	u8 status;
	MPU_6500_ENABLE;
	delay_200ns(1);
	status = SPI5ReadWriteByte(reg); 
	SPI5ReadWriteByte(value);
	MPU_6500_DISENABLE;
	delay_200ns(1);
	return(status);
}

u8 MPU6500_Read_Reg(u8 reg)
{
	u8 reg_val;
	MPU_6500_ENABLE;
	delay_200ns(1);
	SPI5ReadWriteByte(reg|0x80);
	reg_val=SPI5ReadWriteByte(0xff);
	MPU_6500_DISENABLE;
	delay_200ns(1);
	return(reg_val); 
}

u8 initMPU6500(void)
{
	if(MPU6500_Read_Reg(WHO_AM_I) == 0x70)			
	{		
		MPU6500_Write_Reg(PWR_MGMT_1,0X80);   		
		delay_ms(100);
		MPU6500_Write_Reg(SIGNAL_PATH_RESET,0X07);
		delay_ms(100);
		MPU6500_Write_Reg(PWR_MGMT_1,0X01);   		
		MPU6500_Write_Reg(PWR_MGMT_2,0X00);   		
		MPU6500_Write_Reg(CONFIG,0X02);						
		
		MPU6500_Write_Reg(SMPLRT_DIV,0X00);				
		MPU6500_Write_Reg(GYRO_CONFIG,0X18);  		
		
		MPU6500_Write_Reg(PWR_MGMT_1,0x00);
		MPU6500_Write_Reg(USER_CTRL ,0x20);
		MPU6500_Write_Reg(I2C_MST_CTRL,0x0d);	
		MPU6500_Write_Reg(I2C_SLV0_CTRL ,0x81);
		
		return 1;
	}
	else return 0;
}

float mpu6500_gyro_w_z = 0.0f;

void READ_MPU6500_GYRO(void)
{
	static u8 BUF[2];
	static short temp_gyro_w_z;
	
  BUF[0]=MPU6500_Read_Reg(GYRO_ZOUT_L);
  BUF[1]=MPU6500_Read_Reg(GYRO_ZOUT_H);
  temp_gyro_w_z =	(BUF[1]<<8)|BUF[0];
	mpu6500_gyro_w_z = temp_gyro_w_z/16.4;
}





short mag_data[3];

void ReadMag(void)
{
	HMC_Get_Data(mag_data);
	mag_data[0]+=210;
	mag_data[1]+=-130;
	yaw_mag = (atan2(mag_data[1],mag_data[0]) * (180/3.1415926));
	//ReadMagRaw();
}

float ReadMagRaw(void)
{
	return 0.0f;
}

u8 MPU9250ReadReg(u8 reg)
{
	u8 reg_val;
	MPU_9250_ENABLE;
	delay_200ns(1);
	SPI4ReadWriteByte(reg|0x80);
	reg_val = SPI4ReadWriteByte(0xff);
	MPU_9250_DISABLE;
	delay_200ns(1);
	return(reg_val);
}

u8 MPU9250WriteReg(u8 reg,u8 value)
{
	u8 status;
	MPU_9250_ENABLE;
	delay_200ns(1);
	status = SPI4ReadWriteByte(reg);
	SPI4ReadWriteByte(value);
	MPU_9250_DISABLE;
	delay_200ns(1);
	return(status);
}


void Write_HMC5983(u8 add, u8 da)
{
	IIC_Start();                  
	IIC_Send_Byte(HMC5983_ADDR);   
	IIC_Wait_Ack();
	
	IIC_Send_Byte(add);    
	IIC_Wait_Ack();
	
	IIC_Send_Byte(da);       
	IIC_Wait_Ack();
	
	IIC_Stop();              
}

u8 Read_HMC5983(u8 REG_Address)
{   
	u8 REG_data;
  IIC_Start();                      
  IIC_Send_Byte(HMC5983_ADDR);      
	IIC_Wait_Ack();

  IIC_Send_Byte(REG_Address);       
	IIC_Wait_Ack();

  IIC_Start();                      
  IIC_Send_Byte(HMC5983_ADDR+1);    
	IIC_Wait_Ack();

  REG_data=IIC_Read_Byte(0);        
	IIC_Stop();                          
  return REG_data; 
}


void Multiple_read_HMC5983(u8*BUF)
{   
	u8 i;
  IIC_Start();                         
  IIC_Send_Byte(HMC5983_ADDR);         
	IIC_Wait_Ack();
  IIC_Send_Byte(0x03);                 
	IIC_Wait_Ack();
  IIC_Start();                         
  IIC_Send_Byte(HMC5983_ADDR+1);       
	IIC_Wait_Ack();
	for (i=0; i<6; i++)                   
  {
		if (i == 5)
		{
			BUF[i] = IIC_Read_Byte(0);    
		}
		else
		{
			BUF[i] = IIC_Read_Byte(1);         
    }
  }
	IIC_Stop();                          
}

void HMC_Get_Data(short* DAT)
{
	u8 HMC5983_buf[6];
	static u16 last_DAT[3];
	Multiple_read_HMC5983(HMC5983_buf);     
	DAT[0] = (short)HMC5983_buf[0] << 8 | HMC5983_buf[1]; 
	DAT[1] = (short)HMC5983_buf[4] << 8 | HMC5983_buf[5]; 
	DAT[2] = (short)HMC5983_buf[2] << 8 | HMC5983_buf[3]; 

	if(DAT[0]>2048||DAT[0]<-2048||DAT[1]>2048||DAT[1]<-2048||(DAT[0]==-1&&DAT[1]==-1))
	{
		DAT[0]=last_DAT[0];
		DAT[1]=last_DAT[1];
	}
	else
	{
		last_DAT[0]=DAT[0]=DAT[0] + 14;
		last_DAT[1]=DAT[1]=DAT[1] - 22;
	}
}

float GetGyroRawData(void)
{
	static u8 BUF[2];
	static short temp_gyro_w_z;
	
  BUF[0]=MPU6500_Read_Reg(GYRO_ZOUT_L);
  BUF[1]=MPU6500_Read_Reg(GYRO_ZOUT_H);
  temp_gyro_w_z =	(BUF[1]<<8)|BUF[0];
	mpu6500_gyro_w_z = temp_gyro_w_z/16.4;
	
	return mpu6500_gyro_w_z;
}