#include "stdafx.h"
#include "TimeOutSocket.h"
#include "2Dlg.h"

//����ص��������� �ȴ�������� ʱ�� CSocket ����е��á�
//�˺����ṩ��������Ϣ�Ļ��ᡣ
//�˹��̼���� SetTimeOut ���ú������õĶ�ʱ���� WM_TIMER ��Ϣ��
//����յ���Ϣ������� CancelBlockingCall ����
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

//���ó�ʱʱ�䣬��λ���롣������Ϊ0����Ϊ�����ó�ʱʱ�䡣
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
	m_llDtStart=0;			//����ȡ����ʱ
	return TRUE;
}

int CTimeOutSocket::MySend(UINT timeout,const void* lpBuf, int nBufLen, int nFlags)
{
	// TODO: �ڴ����ר�ô����/����û���
	SetTimeOut(timeout);
	int r;
	r=Send(lpBuf, nBufLen, nFlags);
	int err=GetLastError();
	KillTimeOut();
	if(m_bIsTimeOut)
	{
		//MessageBox(NULL,_T("���ͳ�ʱ"),_T("ע��"),MB_OK);
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
	// TODO: �ڴ����ר�ô����/����û���
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
	// TODO: �ڴ����ר�ô����/����û���

	m_mydlg->MyOnReceive();
	CSocket::OnReceive(nErrorCode);
}
void CTimeOutSocket::GetDlg(CMy2Dlg * dlg)
{
	m_mydlg=dlg;
}
