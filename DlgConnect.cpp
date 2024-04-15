// DlgConnect.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgConnect.h"


// CDlgConnect 对话框

IMPLEMENT_DYNAMIC(CDlgConnect, CDialog)

CDlgConnect::CDlgConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnect::IDD, pParent)
{
	if(m_connectaddr.IsEmpty())
		m_connectport=_T("1777");
	if(m_connectaddr.IsEmpty())
		m_connectaddr=_T("192.168.0.134");
}

CDlgConnect::~CDlgConnect()
{
}

void CDlgConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CONNECTADDR, m_connectaddr);
	DDX_Text(pDX, IDC_EDIT_CONNECTPORT, m_connectport);
}


BEGIN_MESSAGE_MAP(CDlgConnect, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgConnect::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgConnect 消息处理程序

BOOL CDlgConnect::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (WM_KEYDOWN == pMsg->message)
	{
		if (VK_RETURN == pMsg->wParam)
		{
			UINT nID = GetFocus()->GetDlgCtrlID(); 
			switch( nID) 
			{ 
			case IDC_EDIT_CONNECTADDR:
				GetDlgItem(IDC_EDIT_CONNECTPORT)->SetFocus();
				return 1;
			case IDC_EDIT_CONNECTPORT:
				GetDlgItem(IDOK)->SetFocus();
				return 1;
			default: 
				return TRUE; 
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgConnect::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	OnOK();
}
