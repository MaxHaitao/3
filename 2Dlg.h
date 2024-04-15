// 2Dlg.h : 头文件
//

#pragma once

#include "TimeOutSocket.h"
#include "TreeCtrlEx.h"
#include "resource.h"
#include "Message.h"
#include "DlgConnect.h"

#define RETRYTIME 2				//连接或等待超时时，重试次数

#define SOCKETBUFFERSIZE			4096			//socket缓冲区长度
// #define SOCKETPORT					1777			//socket端口
// #define SOCKETSERVER				_T("192.168.0.134") //中间服务所在地址

// CMy2Dlg 对话框
class CMy2Dlg : public CDialog
{
// 构造
public:
	CMy2Dlg(CWnd* pParent = NULL);	// 标准构造函数
	~CMy2Dlg();

	CTimeOutSocket m_timesocket;
	CTreeCtrlEx *m_ptreeview;

	CString m_connectaddr,m_connectportstr;//服务所在的地址及连接端口
	UINT m_connectport;
	CDlgConnect m_connectdlg;

	char m_sbuffer[SOCKETBUFFERSIZE];
	char m_cstemp[SOCKETBUFFERSIZE];
	char *m_pcstemp,*m_pcstemp2;

	DWORD receivelen,leftlen;

	DWORD m_heartbeatfromser;		//来自服务器心跳的时间
	BOOL  m_bheartbeatfromserover;	//来自服务器的心跳是否超时

	MECGraphCtrl m_Graph;			//绘图
	WORD m_selectedid,m_selectednode;	//所选择的也即在界面上显示出来的设备会话ID和节点地址
	BYTE m_mboardtype;
	WORD m_extboardtype;
	pNodeExtInfo m_nodeextinfo;
	int m_plotchannel;

// 对话框数据
	enum { IDD = IDD_MY2_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void MyOnReceive();
	BOOL BeginConnectToSer();
	void InitUI();
	void DoExternData(const char*);		//处理上传的扩展模块数据消息
	void DoNodeInfoData(const char*);	//处理节点信息消息
	void DoSensorFixData(const char*);	//处理节点传感器校准参数消息
	void ShowPlot(BOOL);
	CString GetIDName(const BYTE ,const WORD);

	afx_msg void OnNMDblclkTreeNode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedBtnSample();
};
