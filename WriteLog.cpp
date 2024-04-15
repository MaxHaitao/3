

#include "stdafx.h"
#include "WriteLog.h"

#include <stdarg.h>

class CWriteLog
{
public:
	CWriteLog();
	virtual ~CWriteLog();
	void writelog(CString);

private:
	CFile m_logfile;
};

CWriteLog::CWriteLog()
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	CString filename;
	filename.Format(_T("log\\%04d%02d%02d-%02d%02d%02d.txt"),systime.wYear,systime.wMonth,systime.wDay,
		systime.wHour,systime.wMinute,systime.wSecond);
	::CreateDirectory(_T("log"),NULL);
	if(m_logfile.m_hFile == INVALID_HANDLE_VALUE)
	{
		m_logfile.Open(filename,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite);
		m_logfile.SeekToBegin();
#ifdef UNICODE 
		m_logfile.Write("\xff\xfe", 2);//UNICODE编码的头
#endif
	}
}

CWriteLog::~CWriteLog()
{
	if(m_logfile.m_hFile != CFile::hFileNull)
		m_logfile.Close();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//写日志文件函数
//输入：	日志记录
//输出：	无
//说明：	一条日志记录包含记录时间和记录信息，格式为 时:分:秒 记录内容
//			回车换行自动添加
void CWriteLog::writelog(CString text)
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	CString str;
	str.Format(_T("%02d:%02d:%02d  %s\r\n"),systime.wHour,systime.wMinute,systime.wSecond,text);
	m_logfile.Write(str,str.GetLength()*sizeof(_TCHAR));
}

//////////////////////////////
CWriteLog mywritelog;
//////////////////////////////
#define  MAX_BUF_LEN 4096

void WriteLog(LPCTSTR fmt,...)
{
	TCHAR   message[MAX_BUF_LEN];
	va_list cur_arg;
	va_start(cur_arg,fmt);
	_vsntprintf(message,MAX_BUF_LEN,fmt,cur_arg);
	va_end(cur_arg);
	mywritelog.writelog(message);
}