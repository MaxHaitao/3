#include "stdafx.h"
#include "TimeOutSocket.h"
#include "2Dlg.h"

//虚拟回调函数，在 等待操作完成 时由 CSocket 类进行调用。
//此函数提供处理传入消息的机会。
//此过程检查用 SetTimeOut 调用函数设置的定时器的 WM_TIMER 消息。
//如果收到消息，则调用 CancelBlockingCall 函数
BOOL CTimeOutSocket::OnMessagePending()
{
	if( m_llDtStart )
	{
		LARGE_INTEGER lldtEnd;
		::QueryPerformanceCounter(&lldtEnd);  
		LARGE_INTEGER llFrq;
		::QueryPerformanceFrequency(&llFrq);
		double dbDealy=(double)(lldtEnd.QuadPart-m_llDtStart)*1000/llFrq.QuadPart;
		if( dbDealy>m_uTimeOut )
		{
			CancelBlockingCall();
			m_bIsTimeOut=TRUE;
			return FALSE;  // No need for idle time processing.
		}
	}

	return CSocket::OnMessagePending();
}

//设置超时时间，单位毫秒。若参数为0，则为不设置超时时间。
BOOL CTimeOutSocket::SetTimeOut(UINT uTimeOut)
{
	if(0 == uTimeOut)
		m_llDtStart=0;
	else
	{
		LARGE_INTEGER llCnt;
		::QueryPerformanceCounter(&llCnt);
		m_llDtStart=llCnt.QuadPart; 
		m_uTimeOut=uTimeOut;
		m_bIsTimeOut=FALSE;
	}

	return TRUE;
}

BOOL CTimeOutSocket::KillTimeOut()
{
	m_llDtStart=0;			//表明取消计时
	return TRUE;
}

int CTimeOutSocket::MySend(UINT timeout,const void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: 在此添加专用代码和/或调用基类
	SetTimeOut(timeout);
	int r;
	r=Send(lpBuf, nBufLen, nFlags);
	int err=GetLastError();
	KillTimeOut();
	if(m_bIsTimeOut)
	{
		//MessageBox(NULL,_T("发送超时"),_T("注意"),MB_OK);
		return -1;
	}

	if(r == SOCKET_ERROR)
	{
		MsgErrStr(_T("2 Send Error"),err);
	}
	return r;
}

int CTimeOutSocket::MyReceive(UINT timeout,void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: 在此添加专用代码和/或调用基类
	SetTimeOut(timeout);
	int r;
	r=Receive(lpBuf, nBufLen, nFlags);
	int err=GetLastError();
	KillTimeOut();
	if(m_bIsTimeOut)
	{
		MessageBox(NULL,_T("2 Receive TimeOut"),_T("2 Receive"),MB_OK);
		return -1;
	}
	if(r == SOCKET_ERROR)
	{
		MsgErrStr(_T("2 Receive Err"),err);
		return -1;
	}
	return r;
}
BOOL CTimeOutSocket::MyConnect(UINT timeout,LPCTSTR lpszHostAddress,UINT nHostPort)
{
	if(!SetTimeOut(timeout))
		MessageBox(NULL,_T("2 SetTimer Err"),_T("2 Connect"),MB_OK);
	BOOL r=FALSE;
	r=Connect(lpszHostAddress,nHostPort);
	DWORD nErrorCode=GetLastError();
	KillTimeOut();
	if(m_bIsTimeOut)
	{
		MessageBox(NULL,_T("2 Connect TimeOut"),_T("2 Connect"),MB_OK);
		return FALSE;
	}
	if(0 == r)
	{
		MsgErrStr(_T("2 Connect Error"),nErrorCode);
	}

	return r;
}

BOOL CTimeOutSocket::MyConnect(UINT timeout,const SOCKADDR* lpSockAddr,int nSockAddrLen)
{
	if(!SetTimeOut(timeout))
		MessageBox(NULL,_T("2 SetTimer Err"),_T("2 Connect"),MB_OK);
	BOOL r=FALSE;
	r=Connect(lpSockAddr,nSockAddrLen);
	DWORD nErrorCode=GetLastError();
	KillTimeOut();
	if(m_bIsTimeOut)
	{
		MessageBox(NULL,_T("2 Connect TimeOut"),_T("2 Connect"),MB_OK);
		return FALSE;
	}
	if(0 == r)
	{
		MsgErrStr(_T("2 Connect Error"),nErrorCode);
	}
	return r;
}


void CTimeOutSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	m_mydlg->MyOnReceive();
	CSocket::OnReceive(nErrorCode);
}
void CTimeOutSocket::GetDlg(CMy2Dlg * dlg)
{
	m_mydlg=dlg;
}
