
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#pragma once


#define HEARTBEATINTERBALTOSER		5		//����������͵������������λ:��
#define HEARTBEATTOSERID			100		//����������������ļ�ʱ��ID
#define NODEPARSONINFOID			101		//���ӽڵ��ϵ���ּ�ʱ��ID
#define HEARTBEATINTERBALFROMSER	5		//���Է������������������λ:��
#define HEARTBEATOVERFROMSER		15000	//���Է�������������ʱʱ�䣬��λ:����
#define NODEPARSONOVER				10000	//���ӽڵ��ϵ��ʱ

/////////////////////////////////////////////////////////////////////
//�м������PC

//��Ϣ����
#define HEARTBEATMSGTYPE			0X01	//����
#define APPINENTMSGTYPE				0X11	//Ӧ��ʶ����Ϣ
#define DEVCONNECTMSGTYPE			0X12	//�豸�������������Ϣ
#define DEVDISCONNECTMSGTYPE		0X13	//�豸��������Ͽ���Ϣ
///////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
//Э������PC

//��Ϣ����
#define GETNODEINFOMSGTYPE			0X01	//��ȡ�ڵ���Ϣ
#define DRESTARTNODEMSGTYPE			0X05	//�����ڵ�
#define UNODEINFOMSGTYPE			0X21	//�ڵ���Ϣ
#define UEXTENDRESMSGTYPE			0X22	//�ϴ���չ����Դ����
#define DEXTENDRESMSGTYPE			0X23	//�´���չ����Դ����
#define UMOTHERRESMSGTYPE			0X24	//�ϴ�ĸ����Դ����
#define DMOTHERRESMSGTYPE			0X25	//�´�ĸ����Դ����
#define UNODEPARSONMSGTYPE			0X26	//�ڵ㸸�ӹ�ϵ
#define USENSORFIXMSGTYPE			0x29	//����������У׼
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
#define	ACKOK						0x00	//��ȷ����Ӧ��
#define ACKERR						0x01	//�������Ϣ
#define ACKUNKOWN					0x02	//δ֪����Ϣ
////////////////////////////////////////////////////////////////////

#pragma pack(push)
#pragma pack(1)

//��Ϣ��ͷ����Ϣͷ����Ϣ�����ỰID����Ϣ����
struct MsgHeader
{
	BYTE header;
	BYTE len;
	WORD id;
	BYTE type;
};

//Ӧ��ʶ����Ϣ
struct AppIdentMsg
{
	MsgHeader head;
	BYTE apptype;
	BYTE serheadbeatover;
	BYTE serheadbeatinte;
	BYTE sum;
};

//һ��Ӧ����Ϣ
struct CommACKMsg
{
	MsgHeader head;
	BYTE ack;
	BYTE sum;
};

//һ����Ϣ
struct CommMsg
{
	MsgHeader head;
	BYTE sum;
};

//�ڵ�һ����Ϣ
struct NodeCommMsg
{
	MsgHeader head;
	WORD addr;
	BYTE sum;
};

//�ڵ㸸�ӹ�ϵ��Ϣ
struct NodeParsonMsg
{
	MsgHeader head;
	WORD NodeAddr;
	WORD ParNodeAddr;
	BYTE sum;
} ;

//�ڵ���Ϣ��Ϣ
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
//����������У׼��Ϣ
struct SensorFixMsg
{
	MsgHeader head;
	WORD addr;
	SensorFixData fixdata;
	BYTE sum;
} ;

//�豸�������������Ϣ
struct DevConnectMsg
{
	MsgHeader head;
	WORD devsesionid;
// 	WORD ids;
// 	BYTE sum;
} ;

//�豸��������Ͽ���Ϣ
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