#include "stdafx.h"
#include "comfunc.h"

extern LPTSTR g_strPluginName;
extern HINSTANCE hInst;

/// show to user msgbox that settings 
/// file is corrupted...
bool NotifyAboutWrongSettings(LPTSTR szFile)
{
	TCHAR szTxt[MAX_PATH * 2] = {0};
	_tcscpy(szTxt, "Settings file: ");
	_tcscat(szTxt, szFile);
	_tcscat(szTxt, " is corrupted !\n");
	_tcscat(szTxt, "As a result plugin can crush.\n");
	_tcscat(szTxt, "Reset settings to default ?");

	DWORD dw = MessageBox (NULL, Translate(szTxt), g_strPluginName, MB_YESNO);
	return (dw == IDYES);
}

/// forming path
bool MakeFullPath(LPTSTR lpRetval, DWORD dwSize, LPTSTR lpFile)
{
	GetModuleFileName(hInst, lpRetval, dwSize);
	TCHAR *ptrPos = _tcsrchr(lpRetval, '\\');
	if (ptrPos)
	{
		ptrPos++;
		_tcscpy (ptrPos, lpFile);
	}
	else
	{
		_RPT0(_CRT_WARN, "MakeFullPath - unable to form path");
		return false;
	}
	return true;
}

/// checks the help file near plugin and if 
/// it's not present there:
/// creates it..
bool CheckForHelpFile()
{
	TCHAR tchFile[MAX_PATH + 1] = {0};
	MakeFullPath(tchFile, MAX_PATH, READMEFILE); 
	if (PathFileExists(tchFile))
	{	/// todo check it's size... ? not sure to do that..
		return true;
	}
	else
	{
		HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(IDR_REAME), RT_HTML);
		if (hRes == NULL)
		{
			return false;
		}
		DWORD dwSize = SizeofResource(hInst, hRes);
		HGLOBAL hResLoad = LoadResource(hInst, hRes);
		if (hResLoad == NULL || dwSize == NULL)
		{
			return false;
		}
		LPVOID ptrData = LockResource(hResLoad);
		if (ptrData == NULL)
		{
			FreeResource(hResLoad);
			return false;
		}
		HANDLE hFile = CreateFile (tchFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
								CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == NULL || hFile == INVALID_HANDLE_VALUE)
		{
			FreeResource(hResLoad);
			return false;
		}
		DWORD dwreaded = 0;
		BOOL b = WriteFile(hFile, ptrData, dwSize,&dwreaded, NULL);
		if (b == FALSE || dwreaded != dwSize)
		{
			FreeResource(hResLoad);
			CloseHandle (hFile);
			return false;
		}

		FreeResource(hResLoad);
		CloseHandle (hFile);
		return true;
	}

}