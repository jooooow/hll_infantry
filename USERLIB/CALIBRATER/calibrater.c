#include "calibrater.h"
#include "delay.h"

u8 AverageCalibrate(OffsetCalibrater* self)
{
	u8  temp_max_loop_cnt = self->max_loop_cnt;
	u16 temp_single_loop_cnt = self->single_loop_cnt;
	u16 invalid_cnt = 0;
	float sum = 0.0f;
	
	while(temp_max_loop_cnt)
	{
		temp_single_loop_cnt = self->single_loop_cnt;
		invalid_cnt = 0;
		sum = 0.0f;
		while(temp_single_loop_cnt)
		{
			self->new_data = self->GetData();
			if(self->new_data > self->filter_max || self->new_data < self->filter_min)
				invalid_cnt++;
			else
				sum += self->new_data; 
			temp_single_loop_cnt--;
			delay_ms(1);
		}
		if(invalid_cnt > self->single_loop_cnt * self->max_invalid_rate)
		{
			self->invalid_rate = (float)invalid_cnt / self->single_loop_cnt;
			printf("calibrate loop[%d] failed! offset : %.4f\tinvalid_rate : %.4f\r\n\r\n",self->max_loop_cnt - temp_max_loop_cnt + 1,sum / (self->single_loop_cnt - invalid_cnt),self->invalid_rate);
		}
		else
		{
			self->offset_output = sum / (self->single_loop_cnt - invalid_cnt);
			printf("calibrate loop[%d] successed! offset : %.4f\r\n\r\n",self->max_loop_cnt - temp_max_loop_cnt + 1,self->offset_output);
			return 1;
		}
		temp_max_loop_cnt--;
		delay_ms(100);
	}
	printf("calibrate failed!\r\n");
	return 0;
}
