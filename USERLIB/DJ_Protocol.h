#ifndef _DJ_PROTOCOL_H_
#define _DJ_PROTOCOL_H_

#include "stm32f4xx.h"


/***********************************************
使用方法：
1、初始化串口； 实现串口发送函数
2、更改宏定义 USART_SendBuff ，将其改为实际串口发送函数名  xxxxxx   （该函数参数为 uchar *buff,uchar len    len为发送长度）
3、变量 tCmdID  cmdid = SelfDefinedData; //自定义数据 
				tSelfDefineInfo SelfDefineInfo = {.data1 = 1.133f,.data2 = 1.133f,.data3 = 1.143f};//自定义传输数据
   调用函数 Send_FrameData(cmdid,(uint8_t *)&SelfDefineInfo,sizeof(SelfDefineInfo));  //发送自定义数据至裁判系统
4、如果需要接收裁判系统发来的数据，需要实现串口中断接收函数，接收标志为 USART2_RX_STA，缓冲地址 USART2_RX_BUF
	然后在主程序调用 DataVerify() 函数，然后可以用另一个串口发送到串口数据观察（自行实现printf函数）。
	--------------------------------------------------------------------------2017/6/18
***********************************************/


// uart 通信配置，波特率 115200, 数据位 8, 停止位 1，检验位无，流控制无。
// --------------------------------------------------------------------------------
//| FrameHeader(5-Byte) | CmdID(2-Byte) | Data(n-Byte) | FrameTail(2-Byte, CRC16) |
// --------------------------------------------------------------------------------
//crc8 generator polynomial:G(x)=x8+x5+x4+1

#define USART_SendBuff   USART_Send_BUFF  //USART_Send_BUFF(uchar *buff,uchar len) 

/********************  结束更改  ***************************/
//0-3 bits
#define Strike_0   0x00 //0号装甲面（前）
#define Strike_1   0x01 //1号装甲面（左）
#define Strike_2   0x02 //2号装甲面（后）
#define Strike_3   0x03 //3号装甲面（右）
#define Strike_4   0x04 //4号装甲面（上1）
#define Strike_5   0x05 //5号装甲面（上2）
//4-7 bits
#define BC_Strike          0x00 //装甲伤害（受到攻击） BloodChanged    BC
#define BC_ShootSpeed      0x01 //子弹超速扣血
#define BC_ShootFreq       0x02 //子弹超频扣血
#define BC_PowerLimit      0x03 //功率超限
#define BC_ModuleOffline   0x04 //模块离线扣血
#define BC_CommonFoul      0x06 //普通犯规扣血
#define BC_Tarmac          0x0a //停机坪加血
#define BC_EngineerOuto    0x0b //工程机器人自动回血


typedef __packed struct
{
	uint8_t SOF;          //数据起始字节，固定为0xA5          
	uint16_t DataLength;  //数据长度
	uint8_t Seq;          //包序号
	uint8_t CRC8;         //帧头CRC校验
}tFrameHeader;//帧头

typedef enum 
{
	GameRobotState = 0x0001,     //比赛进程信息    发送频率 50 Hz
	RobotHurt=0x0002,  //实时血量变化数据
	ShootData=0x0003,             //实时射击数据
	PowerHeartData=0x0004,        //实时功率热量 (0×0004)
	RfidDetect=0x0005,   //场地交互数据(0x0005)
	GameResult=0x0006,   //比赛胜负数据(0x0006)
	GetBuff=0x0007,			//Buff获取数据(0X0007)
	GameRobotPos=0x0008,   //机器人位置朝向信息(0X0008)
	ShowData=0x0100,   //参赛队自定义数据(0x0100)
	Wrong = 0x1301 //枚举无效，只是为了使该枚举大小为2字节
}tCmdID;

//typedef __packed struct
//{
//	uint8_t flag;
//	uint32_t x;
//	uint32_t y;
//	uint32_t z;
//	uint32_t compass; //方位
//}tLocData; //位置信息

typedef __packed struct
{
	uint16_t stageRemainTime;       //比赛剩余时间（从倒计时三分钟开始计算，单位 s）
	uint8_t gameProgress; //当前比赛阶段
	uint8_t robotLevel;    //机器人当前等级
	uint16_t remainHP;    //机器人当前血量
	uint16_t maxHP;         //机器人满血量
}extGameRobotState_t; //比赛机器人状态（0x0001）

typedef __packed struct
{
	uint8_t armorType:4;
	uint8_t hurtType:4;
}extRobotHurt_t;//伤害数据（0x0002）

typedef __packed struct
{
	uint8_t bulletType; //弹丸类型
	uint8_t bulletFreq;  //子弹实时射频（发 /s）
	float bulletSpeed;   //弹丸射速(m/s)
}extShootData_t;    //实时射击信息 (0×0003)

typedef __packed struct
{
	float chassisVolt; //底盘输出电压
	float chassisCurrent; //底盘输出电流
	float chassisPower; //底盘输出功率
	float chassisPowerBuffer; //底盘功率缓冲
	uint16_t shooterHeat0; //17mm枪口热量
	uint16_t shooterHeat1; //42mm枪口热量
}extPowerHeartData_t;    //实时功率热量 (0×0004)

typedef __packed struct
{
	uint8_t cardType;  //卡类型
	uint8_t cardldx;   //卡索引号
}extRfidDetect_t;   //场地交互数据(0x0005)

typedef __packed struct
{
	uint8_t winner;

}extGameResult_t;   //比赛胜负数据(0x0006)

typedef __packed struct
{
	uint8_t buffType;  //Buff类型
	uint8_t buffAddition;   //加成百分比
}extGetBuff_t;   //Buff获取数据(0X0007)

typedef __packed struct
{
	float x;  //位置x坐标值
	float y;  //位置y坐标值
	float z;  //位置z坐标值
	float yaw;  //枪口朝向角度值
}extGameRobotPos_t;   //机器人位置朝向信息(0X0008)

typedef __packed struct
{
	float data1;
	float data2;
	float data3;
	uint8_t mask;
}extShowData_t;   //参赛队自定义数据(0x0100).

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	__packed union 
	{
		extGameRobotState_t       GameRobotState;    //比赛进程信息 （0x0001）
		extRobotHurt_t   RobotHurt; //实时血量变化数据（0x0002）
		extShootData_t   ShootData; //实时射击信息 (0×0003)
		extPowerHeartData_t   PowerHeartData ;    //实时功率热量 (0×0004)
		extRfidDetect_t    RfidDetect;   //场地交互数据(0x0005)
		extGameResult_t    GameResult;   //比赛胜负数据(0x0006)
		extGetBuff_t   GetBuff;   //Buff获取数据(0X0007)
		extGameRobotPos_t    GameRobotPos;   //机器人位置朝向信息(0X0008)
		extShowData_t     ShowData;   //参赛队自定义数据(0x0100).
	}Data;
	uint16_t        CRC16;         //之前所有数据CRC校验   注意此数据和之前的数据可能不连续，所以不要直接使用，若需要直接使用，必须在此赋值
}tFrame;  //数据帧

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extGameRobotState_t     GameRobotState;   
	uint16_t        CRC16;         //数据CRC校验
}extGameRobotState_tFrame;  //比赛进程信息帧 （0x0001）

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extRobotHurt_t   RobotHurt;   
	uint16_t        CRC16;         //数据CRC校验
}extRobotHurt_tFrame; //实时血量变化数据（0x0002）

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extShootData_t   ShootData;  
	uint16_t        CRC16;         //数据CRC校验
}extShootData_tFrame;    //实时射击信息 (0×0003)

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extPowerHeartData_t   PowerHeartData ;   
	uint16_t        CRC16;         //数据CRC校验
}extPowerHeartData_tFrame;  ////实时功率热量 (0×0004)

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extRfidDetect_t    RfidDetect;   
	uint16_t        CRC16;         //数据CRC校验
}extRfidDetect_tFrame;    //场地交互数据(0x0005)

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extGameResult_t   GameResult;   
	uint16_t        CRC16;         //数据CRC校验
}extGameResult_tFrame;    //比赛胜负数据(0x0006)

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extGetBuff_t   GetBuff;   
	uint16_t        CRC16;         //数据CRC校验
}extGetBuff_tFrame;    //Buff获取数据(0X0007)

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extGameRobotPos_t   GameRobotPos;   
	uint16_t        CRC16;         //数据CRC校验
}extGameRobotPos_tFrame;    //机器人位置朝向信息(0X0008)

typedef __packed struct
{
	tFrameHeader    FrameHeader;
	tCmdID          CmdID;
	extShowData_t     ShowData;   
	uint16_t        CRC16;         //数据CRC校验
}extShowData_tFrame;    //参赛队自定义数据(0x0100).

u8 DataVerify(void);
// 使用方法 发送用户数据 Send_FrameData(SelfDefinedDara, userMessage,tSelfDefineInfo(userMessage)); 
void Send_FrameData(tCmdID cmdid, uint8_t * pchMessage,uint8_t dwLength); 


#endif

