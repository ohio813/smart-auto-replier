#include "stdafx.h"
#include "rulesstorage.h"
#include "crc32static.h"

extern HINSTANCE hInst;
extern CCrushLog CRUSHLOGOBJ;
extern CMessagesHandler * g_pMessHandler;

/// ctor...
CRulesStorage::CRulesStorage(void) : m_hFile(NULL)
{
	m_comItem.Header = NULL;
	m_comItem.Message = NULL;
BEGIN_PROTECT_AND_LOG_CODE
	MakeFullPath(m_szSettFileName, sizeof(m_szSettFileName), STORAGE_NAME);
	InitializeCriticalSection(&m_critSect);
END_PROTECT_AND_LOG_CODE
}

/// dtor...
CRulesStorage::~CRulesStorage(void)
{
BEGIN_PROTECT_AND_LOG_CODE	
	DeleteCriticalSection(&m_critSect);
	DeInit();
END_PROTECT_AND_LOG_CODE
}

bool CRulesStorage::Serialize(ISettingsStream *pSettings, DWORD & ObjectSize)
{
BEGIN_PROTECT_AND_LOG_CODE
	Flush();	
END_PROTECT_AND_LOG_CODE
	return true;
}

bool CRulesStorage::Deserialize(DWORD & ObjectSize)
{
BEGIN_PROTECT_AND_LOG_CODE
	Init();	
END_PROTECT_AND_LOG_CODE
	return false;
}

/// deiniting...
void CRulesStorage::DeInit(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	if (m_hFile && m_hFile != INVALID_HANDLE_VALUE)
		CloseHandle(m_hFile);
	m_hFile = NULL;

	/// now release memory...
	for (RulesHash::iterator it = m_hashTable.begin(); it != m_hashTable.end(); it++)
	{
		RULE_ITEM & i = it->second;
		VirtualFree (i.ContactName, NULL, MEM_RELEASE);
		VirtualFree (i.ReplyAction, NULL, MEM_RELEASE);
		VirtualFree (i.ReplyText, NULL, MEM_RELEASE);
		VirtualFree (i.RuleName, NULL, MEM_RELEASE);
	}

	VirtualFree (m_comItem.Header, NULL, MEM_RELEASE);
	m_comItem.Header = NULL;
	VirtualFree (m_comItem.Message, NULL, MEM_RELEASE);
	m_comItem.Message = NULL;
	
END_PROTECT_AND_LOG_CODE
}

/// writing a string into a file..
void CRulesStorage::RawWriteDataBufByChunk(LPTSTR str)
{
BEGIN_PROTECT_AND_LOG_CODE
	int nLength		= _tcslen(str);
	DWORD dwWritten = 0;
	BOOL bVal		= FALSE;
	
	bVal = WriteFile(m_hFile, &nLength, sizeof(nLength), &dwWritten, NULL);

	if (!bVal || dwWritten != sizeof(nLength))
		throw bVal;

	bVal = WriteFile(m_hFile, str, nLength, &dwWritten, NULL);
	if (!bVal || nLength != dwWritten)
	{
		throw bVal;
	}
END_PROTECT_AND_LOG_CODE
}

/// flushing all data from memory into a file...
void CRulesStorage::Flush(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	RulesHash::iterator it;

	SetFilePointer(m_hFile, NULL, NULL, FILE_BEGIN);
	SetEndOfFile(m_hFile); /// clear old data

	SetCommonMessages();

	for (it = m_hashTable.begin(); it != m_hashTable.end(); it++)
	{
		FlushItem(it->second);
	}

END_PROTECT_AND_LOG_CODE
}

/// n.c. 
bool CRulesStorage::IsRuleMatch(RULE_METAINFO & info, LPTSTR & strMess, LPTSTR & lpIncomingMsg)
{
BEGIN_PROTECT_AND_LOG_CODE
	EnterCriticalSection(&m_critSect);

	RulesHash::iterator iter;
	bool bExists = false;
	DWORD dwCRC32 = 0;
	CCrc32Static::StringCrc32(info.ContactName, dwCRC32);
	iter = m_hashTable.find(dwCRC32);

	CScriptsReader & screader = g_pMessHandler->getSettings().getScriptsReader();

	if (iter != m_hashTable.end())
	{
		/// I have to make copy of ReplyText,
		/// cause GetReturnedMessage will release ptr
		/// that is stored rule
		/// found this bug after 20 min. of debugging ;(
		strMess = reinterpret_cast<LPTSTR>(VirtualAlloc (NULL, _tcslen(iter->second.ReplyText), 
										   MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
		if (strMess == NULL) /// ooops...
		{
			screader.GetReturnMessage(info.ContactName, strMess, lpIncomingMsg);
			LeaveCriticalSection(&m_critSect);
			return false;
		}
		_tcscpy(strMess, iter->second.ReplyText);
		bExists = true;
		screader.GetReturnMessage(iter->second.ContactName, strMess, lpIncomingMsg);
	}
	else
		screader.GetReturnMessage(info.ContactName, strMess, lpIncomingMsg);

	LeaveCriticalSection(&m_critSect);
	return bExists;
END_PROTECT_AND_LOG_CODE
	return false;
}

/// adding rule into memory...
DWORD CRulesStorage::AddRuleItem(RULE_ITEM item, bool & bExists)
{
BEGIN_PROTECT_AND_LOG_CODE
	DWORD dwCRC32 = NULL;
	CCrc32Static::StringCrc32(item.ContactName, dwCRC32);
	RulesHash::iterator iter;
	iter = m_hashTable.find(dwCRC32);

	if (iter != m_hashTable.end())
	{
		bExists = true;
		return 0;
	}

	bExists = false;

	m_hashTable.insert(RulesHash::value_type(dwCRC32, item));
	return dwCRC32;
END_PROTECT_AND_LOG_CODE	
	return 0;
}

/// flushing item...
void CRulesStorage::FlushItem(RULE_ITEM & item)
{
BEGIN_PROTECT_AND_LOG_CODE
	RawWriteDataBufByChunk(item.RuleName);
	RawWriteDataBufByChunk(item.ContactName);
	RawWriteDataBufByChunk(item.ReplyText);
	RawWriteDataBufByChunk(item.ReplyAction);
END_PROTECT_AND_LOG_CODE
}

/// initing rules manager...
void CRulesStorage::Init(void)
{
#ifdef _DEBUG
	//MessageBox (NULL, __FUNCTION__, __FILE__, MB_OK);
#endif
BEGIN_PROTECT_AND_LOG_CODE
	if (m_hFile)
		CloseHandle (m_hFile);
	
	bool bFileExists = true;
	if (!PathFileExists(m_szSettFileName))
	{/// file does not exists...
	 /// need to fill it with common rules...
		bFileExists = false;
	}

	m_hFile = CreateFile (m_szSettFileName,
							GENERIC_READ|GENERIC_WRITE,  FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL, (bFileExists ? OPEN_ALWAYS : CREATE_ALWAYS), FILE_ATTRIBUTE_HIDDEN, NULL); 

	if (m_hFile == INVALID_HANDLE_VALUE)
	{/// show notification msg to user..
		throw INVALID_HANDLE_VALUE;
	}

	if (bFileExists == false)
	{
		ClearCommonMessages();
		m_comItem.Header = SETTINGS_DEF_HEADER;
		m_comItem.Message = SETTINGS_DEF_MESSAGE;
		SetCommonMessages();
	}

	DWORD dwSize = 0;
	dwSize = GetFileSize(m_hFile, &dwSize);
	DWORD	dwReaded = 0,
			dwCrc32 = 0;

	int nLength = 0;

	/// suppose that func is failed
	BOOL bReaded = FALSE;
	bool bFailed = false;

	if (bFileExists)
	{
		dwSize -= GetCommonMessages();
	}
	else
	{
		dwSize -= _tcslen(SETTINGS_DEF_HEADER);
		dwSize -= _tcslen(SETTINGS_DEF_MESSAGE);
		dwSize -= 2 * sizeof(int);
	}

	while (dwSize)
	{
		RULE_ITEM it;
		bReaded = ReadFile (m_hFile, &nLength, sizeof(nLength), &dwReaded, NULL);

		if (bReaded == FALSE || dwReaded != sizeof(nLength))
		{
			bFailed = true;
			break;
		}

		dwSize -= sizeof(nLength);
		it.RuleName = reinterpret_cast<LPTSTR>(VirtualAlloc(NULL, nLength, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE));
		if (it.RuleName == NULL)
		{
			bFailed = true;
			break;
		}

		bReaded = ReadFile (m_hFile, it.RuleName, nLength, &dwReaded, NULL);
		if (bReaded == FALSE || dwReaded != nLength)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		dwSize -= nLength;
		bReaded = ReadFile (m_hFile, &nLength, sizeof(nLength), &dwReaded, NULL);
		if (bReaded == FALSE || dwReaded != sizeof(nLength))
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}
		dwSize -= sizeof(nLength);
		it.ContactName = reinterpret_cast<LPTSTR>(VirtualAlloc(NULL, nLength, MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
		if (it.ContactName == NULL)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}
		bReaded = ReadFile (m_hFile, it.ContactName, nLength, &dwReaded, NULL);

		if (bReaded == FALSE || dwReaded != nLength)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		dwSize -= nLength;

		bReaded = ReadFile (m_hFile, &nLength, sizeof(nLength), &dwReaded, NULL);
		if (bReaded == FALSE || dwReaded != sizeof(nLength))
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		dwSize -= sizeof(nLength);
		it.ReplyText = reinterpret_cast<LPTSTR> (VirtualAlloc(NULL, nLength, MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
		if (it.ReplyText == NULL)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}
		bReaded = ReadFile (m_hFile, it.ReplyText, nLength, &dwReaded, NULL);
		if (bReaded == FALSE || dwReaded != nLength)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			VirtualFree (it.ReplyText, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		dwSize -= nLength;

		bReaded = ReadFile (m_hFile, &nLength, sizeof(nLength), &dwReaded, NULL);
		if (bReaded == FALSE || dwReaded != sizeof(nLength))
		{	
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			VirtualFree (it.ReplyText, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		dwSize -= sizeof(nLength);
		it.ReplyAction = reinterpret_cast<LPTSTR> (VirtualAlloc(NULL, nLength, MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
		if (it.ReplyAction == NULL)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			VirtualFree (it.ReplyText, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		bReaded = ReadFile (m_hFile, it.ReplyAction, nLength, &dwReaded, NULL);
		if (bReaded == FALSE || dwReaded != nLength)
		{
			VirtualFree (it.RuleName, 0, MEM_RELEASE);
			VirtualFree (it.ContactName, 0, MEM_RELEASE);
			VirtualFree (it.ReplyText, 0, MEM_RELEASE);
			VirtualFree (it.ReplyAction, 0, MEM_RELEASE);
			bFailed = true;
			break;
		}

		dwSize -= nLength;
		dwCrc32 = NULL;
		CCrc32Static::StringCrc32(it.ContactName, dwCrc32);
		m_hashTable.insert(RulesHash::value_type(dwCrc32, it) );	/// filling list
	}
	
	if (bFailed)
	{	/// upps...
		bool bdelete = NotifyAboutWrongSettings(m_szSettFileName);
		if (bdelete)
		{
			CloseHandle (m_hFile);
			m_hFile = NULL;
			DeleteFile (m_szSettFileName);
			DeInit();
			Init();
		}
	}

END_PROTECT_AND_LOG_CODE
}

/// reading an data from file storage...
UINT CRulesStorage::RawReadDataBufByChunk(LPTSTR & szData)
{
BEGIN_PROTECT_AND_LOG_CODE
	UINT nRetVal = 0;

	if (m_hFile == NULL || m_hFile == INVALID_HANDLE_VALUE)
	{
		return nRetVal;
	}	

	int nSizeOf = 0;
	DWORD dwReaded = 0;
	const int nsizeof = sizeof(nSizeOf);
	BOOL bReaded = ReadFile (m_hFile, &nSizeOf, nsizeof, &dwReaded, NULL);
	nRetVal += dwReaded;
	if (bReaded == FALSE || dwReaded != nsizeof)
	{
		return nRetVal;
	}
	
	szData = reinterpret_cast<LPTSTR>(VirtualAlloc (NULL, nSizeOf,  MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
	if (szData == NULL)
		return nRetVal + dwReaded;

	bReaded = ReadFile(m_hFile, szData, nSizeOf, &dwReaded, NULL);
	nRetVal += dwReaded;
	if (bReaded == FALSE || dwReaded != nSizeOf)
	{
		VirtualFree (szData, NULL, MEM_RELEASE);
		szData = NULL;
		return nRetVal;
	}

	return nRetVal;
END_PROTECT_AND_LOG_CODE
}

/// 
void CRulesStorage::SetCommonMessages(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	RawWriteDataBufByChunk(m_comItem.Header);
	RawWriteDataBufByChunk(m_comItem.Message);
END_PROTECT_AND_LOG_CODE
}

void CRulesStorage::ClearCommonMessages(void)
{
	if (m_comItem.Header)
	{
		VirtualFree (m_comItem.Header, NULL, MEM_RELEASE);
		m_comItem.Header = NULL;
	}
	if (m_comItem.Message)
	{
		VirtualFree (m_comItem.Message, NULL, MEM_RELEASE);
		m_comItem.Message = NULL;
	}
}

/// reads from storage common rules - header and 
/// message that is replyed to all users...
UINT CRulesStorage::GetCommonMessages(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	ClearCommonMessages();
	UINT nRetVal = 0;	
	nRetVal = RawReadDataBufByChunk(m_comItem.Header);

	if (nRetVal && m_comItem.Header)
	{
		nRetVal += RawReadDataBufByChunk(m_comItem.Message);
	}
	else
		return nRetVal;
	return nRetVal;
END_PROTECT_AND_LOG_CODE
#undef RAW_DATACLEAR	
	return 0;
}

void CRulesStorage::setCommonRule(COMMON_RULE_ITEM r)							/// n.c.
{
BEGIN_PROTECT_AND_LOG_CODE
	memcpy (&m_comItem, &r, sizeof(COMMON_RULE_ITEM));
END_PROTECT_AND_LOG_CODE
}

COMMON_RULE_ITEM & CRulesStorage::getCommonRule(void)			   /// n.c.
{
BEGIN_PROTECT_AND_LOG_CODE
	return m_comItem;
END_PROTECT_AND_LOG_CODE
}

/// getting hash table
RulesHash & CRulesStorage::getHashTable(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	return m_hashTable;
END_PROTECT_AND_LOG_CODE
}

/// remove rule by its id
bool CRulesStorage::RemReplyAction(DWORD dwCrc32)
{
	bool bRetVal = false;
BEGIN_PROTECT_AND_LOG_CODE	
	EnterCriticalSection(&m_critSect);
	RulesHash::iterator it;
	it = m_hashTable.find(dwCrc32);
	if (it != m_hashTable.end())
	{
		VirtualFree (it->second.ContactName, 0, MEM_RELEASE);
		VirtualFree (it->second.ReplyAction, 0, MEM_RELEASE);
		VirtualFree (it->second.ReplyText, 0, MEM_RELEASE);
		VirtualFree (it->second.RuleName, 0, MEM_RELEASE);

		m_hashTable.erase(it);
		bRetVal = true;
	}
	LeaveCriticalSection(&m_critSect);
END_PROTECT_AND_LOG_CODE
	return bRetVal;
}

bool CRulesStorage::RuleIsRegistered(LPTSTR lpContactName)
{
BEGIN_PROTECT_AND_LOG_CODE
	bool bRetVal = false;
	if (lpContactName == NULL)
		return bRetVal;
	DWORD dwCRC32 = NULL;
	CCrc32Static::StringCrc32(lpContactName, dwCRC32);
	RulesHash::iterator iter;

	EnterCriticalSection(&m_critSect);
	iter = m_hashTable.find(dwCRC32);

	if (iter != m_hashTable.end())
	{
		bRetVal = true;		
	}
	LeaveCriticalSection(&m_critSect);

	return bRetVal;
END_PROTECT_AND_LOG_CODE
	return false;
}