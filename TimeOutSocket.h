#pragma  once

class CMy2Dlg;


class CTimeOutSocket : public CSocket
{
public:
	BOOL m_bIsTimeOut;					//超时标志
	CMy2Dlg *m_mydlg;

protected:
	virtual BOOL OnMessagePending();

private:
	LONGLONG m_llDtStart;
	UINT  m_uTimeOut; 

public:
	BOOL SetTimeOut(UINT uTimeOut);  //设置超时时间，单位毫秒
	BOOL KillTimeOut();
	void GetDlg(CMy2Dlg *);
	int MySend(UINT timeout,const void* lpBuf, int nBufLen, int nFlags = 0);
	int MyReceive(UINT timeout,void* lpBuf, int nBufLen, int nFlags = 0);
	BOOL MyConnect(UINT timeout,LPCTSTR lpszHostAddress,UINT nHostPort);
	BOOL MyConnect(UINT timeout,const SOCKADDR* lpSockAddr,int nSockAddrLen);

	virtual void OnReceive(int nErrorCode);

};