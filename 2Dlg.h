// 2Dlg.h : ͷ�ļ�
//

#pragma once

#include "TimeOutSocket.h"
#include "TreeCtrlEx.h"
#include "resource.h"
#include "Message.h"
#include "DlgConnect.h"

#define RETRYTIME 2				//���ӻ�ȴ���ʱʱ�����Դ���

#define SOCKETBUFFERSIZE			4096			//socket����������
// #define SOCKETPORT					1777			//socket�˿�
// #define SOCKETSERVER				_T("192.168.0.134") //�м�������ڵ�ַ

// CMy2Dlg �Ի���
class CMy2Dlg : public CDialog
{
// ����
public:
	CMy2Dlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CMy2Dlg();

	CTimeOutSocket m_timesocket;
	CTreeCtrlEx *m_ptreeview;

	CString m_connectaddr,m_connectportstr;//�������ڵĵ�ַ�����Ӷ˿�
	UINT m_connectport;
	CDlgConnect m_connectdlg;

	char m_sbuffer[SOCKETBUFFERSIZE];
	char m_cstemp[SOCKETBUFFERSIZE];
	char *m_pcstemp,*m_pcstemp2;

	DWORD receivelen,leftlen;

	DWORD m_heartbeatfromser;		//���Է�����������ʱ��
	BOOL  m_bheartbeatfromserover;	//���Է������������Ƿ�ʱ

	MECGraphCtrl m_Graph;			//��ͼ
	WORD m_selectedid,m_selectednode;	//��ѡ���Ҳ���ڽ�������ʾ�������豸�ỰID�ͽڵ��ַ
	BYTE m_mboardtype;
	WORD m_extboardtype;
	pNodeExtInfo m_nodeextinfo;
	int m_plotchannel;

// �Ի�������
	enum { IDD = IDD_MY2_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	void DoExternData(const char*);		//�����ϴ�����չģ��������Ϣ
	void DoNodeInfoData(const char*);	//����ڵ���Ϣ��Ϣ
	void DoSensorFixData(const char*);	//����ڵ㴫����У׼������Ϣ
	void ShowPlot(BOOL);
	CString GetIDName(const BYTE ,const WORD);

	afx_msg void OnNMDblclkTreeNode(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedBtnSample();
};
