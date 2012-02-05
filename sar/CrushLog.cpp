#include "stdafx.h"
#include "crushlog.h"

extern HINSTANCE hInst;
extern CCrushLog CRUSHLOGOBJ;

/// init static members
TCHAR CCrushLog::m_szLogPath[MAX_PATH] = {0};
CRITICAL_SECTION CCrushLog::m_critSect;

/// ctor
CCrushLog::CCrushLog(void)
{/// nothing for now
BEGIN_PROTECT_AND_LOG_CODE
END_PROTECT_AND_LOG_CODE
}

/// d'ctor
CCrushLog::~CCrushLog(void)
{
BEGIN_PROTECT_AND_LOG_CODE
END_PROTECT_AND_LOG_CODE
}

/// deinit internal data...
void CCrushLog::DeInit(void)
{
	DeleteCriticalSection(&m_critSect);
}

/// init internal data...
void CCrushLog::Init(void)
{
	if (_tcslen(m_szLogPath) == 0) /// let's get path to log file
	{
		GetModuleFileName(hInst, m_szLogPath, sizeof(m_szLogPath) * sizeof(TCHAR));
		
		TCHAR *ptrPos = _tcsrchr(m_szLogPath, '\\');
		if (ptrPos)
		{
			ptrPos++;
			_tcscpy(ptrPos, LOG_FILENAME);
		}
		else
		{
			_RPT0(_CRT_WARN, "CCrushLog::CCrushLog - unable to form path");
		}
	}

	InitializeCriticalSection(&m_critSect);
}

/// provides forming data struct and saving it to log
void CCrushLog::InternalProtect(LPTSTR	FunctionName, LPTSTR FileName, 
								LPTSTR	CompilationDate, INT LineNumber,
								INT		LastError)
{
	CRUSHLOG_ITEM item = {0};				
	item.CompilationDate = CompilationDate;
	item.FileName = FileName;
	item.FunctionName = FunctionName;
	item.LineNumber = LineNumber;
	item.LastError = LastError;
	g_crushLog.SaveToLog(item);

	TCHAR tch[MAX_PATH * 3] = {0};
	_tcscpy(tch, Translate("Exception has occured please, analize or just send log file\n\r "));
	_tcscat(tch, CCrushLog::m_szLogPath);
	_tcscat(tch, Translate("\r\n to developer."));

	::MessageBox (NULL, tch, Translate("AutoReplier crush. Support: ICQ# 161762046"), MB_OK);
}

/// saving log information
void CCrushLog::SaveToLog(CRUSHLOG_ITEM & item)
{
	EnterCriticalSection(&m_critSect);

	HANDLE hFile = NULL;
	hFile = CreateFile (m_szLogPath, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{		
		LeaveCriticalSection(&m_critSect);
		return;
	}
	{
		DWORD dwRetVal = 0;
		DWORD dw = GetFileSize(hFile, &dwRetVal);
		SetFilePointer(hFile, dw, NULL, FILE_BEGIN);
	}

	int nSize =  _tcslen(item.CompilationDate);
		nSize += _tcslen(item.FileName);
		nSize += _tcslen(item.FunctionName);
		nSize *= 2;

	TCHAR *tsz = (TCHAR*)VirtualAlloc (NULL, nSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

	if (tsz)
	{
		_tcscpy(tsz, "******\n");
		_tcscat(tsz, item.CompilationDate);
		_tcscat(tsz, "\n");
		_tcscat(tsz, item.FileName);
		_tcscat(tsz, "\n");
		_tcscat(tsz, item.FunctionName);
		_tcscat(tsz, "\n");
		TCHAR tchtemp[MAX_PATH] = {0};
		_itot(item.LineNumber, tchtemp, 10);
		_tcscat(tsz, tchtemp);
		_tcscat(tsz, "\n");
		_itot(item.LastError, tchtemp, 10);
		_tcscat(tsz, tchtemp);
		_tcscat(tsz, "\n");
		DWORD dw = 0;
		WriteFile (hFile, tsz, _tcslen(tsz), &dw, NULL);
		VirtualFree (tsz, NULL, MEM_RELEASE);
	}

	CloseHandle (hFile);
	LeaveCriticalSection(&m_critSect);
}
