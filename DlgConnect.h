#pragma once

#include "resource.h"

// CDlgConnect �Ի���

class CDlgConnect : public CDialog
{
	DECLARE_DYNAMIC(CDlgConnect)

public:
	CDlgConnect(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgConnect();

// �Ի�������
	enum { IDD = IDD_DLG_CONNECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_connectaddr;
	CString m_connectport;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedOk();
};
