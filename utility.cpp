
#include "stdafx.h"
#include "utility.h"

#pragma  once

BYTE MyGetSum(BYTE *p,BYTE len)
{
	DWORD s=0;
	int i=0;
	for(i=0;i<len-1;i++)
		s+=*p++;

	return (BYTE)(s & 0X000000FF);
}

/*
以对话框形式显示错误消息。
来自MSDN

In(2):	(1)TCHAR* 对话框标题; 
		(2)int 错误代码，为NULL时，在函数内部调用GetLastError()。
		//
Out:	无。
*/
void MsgErrStr(TCHAR *s, int Err)
{
	LPVOID lpMsgBuf;
	DWORD err;
	if(NULL == Err)
		err=GetLastError();
	else
		err=Err;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	::MessageBox( NULL, (TCHAR*)lpMsgBuf, s, MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}
