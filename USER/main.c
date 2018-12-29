#include "control_task.h"
#include "control.h"

extern short motor_speed_arr[7];
extern IncrementPid_t fl_pid;
extern IncrementPid_t fr_pid;
extern IncrementPid_t bl_pid;
extern IncrementPid_t br_pid;

extern RemoteController_t remote_controller;

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*初始化控制任务*/
	InitControlTask();

	while(1)
	{
#if (ONLINE_FLAG_LOG == 1)
		/*打印ONLINE_FLAG以显示当前模块状态*/
		OnelineFlagLog();
		delay_ms(5);
#endif
	}
	return 0;
}
