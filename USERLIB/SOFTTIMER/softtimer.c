#include "softtimer.h"

/*
*/
void SoftTimerDescend(SoftTimer* self)
{
	if(self->reload_hook != NULL && *(self->reload_hook))
	{
		self->Reload(self);
		*(self->reload_hook) = 0;
	}
		
	if(self->cnt > 0)
	{
		if(self->descend_hook == NULL)
		{
			self->cnt--;
		}
		else
		{
			if(*(self->descend_hook) == 1)
			{
				self->cnt--;
			}
			else
			{
				self->cnt = self->reload_cnt;
			}
		}
		if(self->cnt == 0)
			self->time_out_flag = 1;
	}
		
	if(self->OnTimeOut != NULL)
	{
		if(self->time_out_flag == 1)
		{
			//self->time_out_flag = 0;
			//self->Reload(self);
			self->OnTimeOut();
		}
	}
	self->old_time_out_flag = self->time_out_flag;
}

void SoftTimerReload(SoftTimer* self)
{
	self->time_out_flag = 0;
	self->cnt = self->reload_cnt;
}

