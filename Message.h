
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#pragma once


#define HEARTBEATINTERBALTOSER		5		//向服务器发送的心跳间隔，单位:秒
#define HEARTBEATTOSERID			100		//向服务器发送心跳的计时器ID
#define NODEPARSONINFOID			101		//父子节点关系保持计时器ID
#define HEARTBEATINTERBALFROMSER	5		//来自服务器的心跳间隔，单位:秒
#define HEARTBEATOVERFROMSER		15000	//来自服务器的心跳超时时间，单位:毫秒
#define NODEPARSONOVER				10000	//父子节点关系超时

/////////////////////////////////////////////////////////////////////
//中间服务与PC

//消息代码
#define HEARTBEATMSGTYPE			0X01	//心跳
#define APPINENTMSGTYPE				0X11	//应用识别消息
#define DEVCONNECTMSGTYPE			0X12	//设备与服务器连接消息
#define DEVDISCONNECTMSGTYPE		0X13	//设备与服务器断开消息
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//协调器与PC

//消息代码
#define GETNODEINFOMSGTYPE			0X01	//获取节点信息
#define DRESTARTNODEMSGTYPE			0X05	//重启节点
#define UNODEINFOMSGTYPE			0X21	//节点信息
#define UEXTENDRESMSGTYPE			0X22	//上传扩展板资源数据
#define DEXTENDRESMSGTYPE			0X23	//下传扩展板资源数据
#define UMOTHERRESMSGTYPE			0X24	//上传母板资源数据
#define DMOTHERRESMSGTYPE			0X25	//下传母板资源数据
#define UNODEPARSONMSGTYPE			0X26	//节点父子关系
#define USENSORFIXMSGTYPE			0x29	//传感器参数校准
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
#define	ACKOK						0x00	//正确接收应答
#define ACKERR						0x01	//错误的消息
#define ACKUNKOWN					0x02	//未知的消息
////////////////////////////////////////////////////////////////////

#pragma pack(push)
#pragma pack(1)

//消息包头：消息头，消息长，会话ID，消息代码
struct MsgHeader
{
	BYTE header;
	BYTE len;
	WORD id;
	BYTE type;
};

//应用识别消息
struct AppIdentMsg
{
	MsgHeader head;
	BYTE apptype;
	BYTE serheadbeatover;
	BYTE serheadbeatinte;
	BYTE sum;
};

//一般应答消息
struct CommACKMsg
{
	MsgHeader head;
	BYTE ack;
	BYTE sum;
};

//一般消息
struct CommMsg
{
	MsgHeader head;
	BYTE sum;
};

//节点一般消息
struct NodeCommMsg
{
	MsgHeader head;
	WORD addr;
	BYTE sum;
};

//节点父子关系消息
struct NodeParsonMsg
{
	MsgHeader head;
	WORD NodeAddr;
	WORD ParNodeAddr;
	BYTE sum;
} ;

//节点信息消息
struct NodeInfoMsg
{
	MsgHeader head;
	WORD addr;
	UINT IEEE[2];
	char RSSI;
	BYTE LQI;
	WORD PANID;
	BYTE mboardtype;
	WORD extboardtype;
	BYTE sum;
} ;

struct SensorFixData
{
	WORD moduletype;
	WORD maxdata;
	WORD zerodata;
	WORD mindata;
} ;
//传感器参数校准消息
struct SensorFixMsg
{
	MsgHeader head;
	WORD addr;
	SensorFixData fixdata;
	BYTE sum;
} ;

//设备与服务器连接消息
struct DevConnectMsg
{
	MsgHeader head;
	WORD devsesionid;
// 	WORD ids;
// 	BYTE sum;
} ;

//设备与服务器断开消息
struct DevDisConnectMsg
{
	MsgHeader head;
	WORD devsesionid;
	BYTE sum;
} ;

////////

struct ADControl
{
	BYTE channel;
	BYTE precision;
	WORD speed;
	WORD mode;
	WORD interval;
	WORD nums;
	BYTE upnums;
} ;

struct MotherResData
{
	MsgHeader head;
	WORD addr;
	BYTE mboardtype;
	WORD rescode;
} ;

struct ExternResData
{
	MsgHeader head;
	WORD addr;
	WORD extcode;
} ;

/////////

#pragma pack(pop)

typedef struct _NodeExtInfo
{
	DWORD lifetime;
	WORD ADDR;
	WORD PANID;
	UINT IEEE[2];
	BYTE LQI;
	BYTE mboardtype;
	WORD extboardtype;
	char RSSI;
	SensorFixData fixdata[3];
} NodeExtInfo,*pNodeExtInfo;


#endif