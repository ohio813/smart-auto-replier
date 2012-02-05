#pragma once

#include "settingshandler.h"
#include "interfaces\ISingletone.h"

#include <map>
using namespace std;

typedef struct _CONTACT_METAINFO
{
	LARGE_INTEGER StartTime;
	LARGE_INTEGER DiffTime;
}CONTACT_METAINFO,*PCONTACT_METAINFO;

typedef std::map<HANDLE, CONTACT_METAINFO> ContactsMap;
typedef std::pair<HANDLE, CONTACT_METAINFO> ContactsPair;
const __int64 nano100SecInSec = (__int64)10000000; 

/// hooks def...
#define HOOKS_NUM	 1

INT WINAPI ContactsFetcherThread(LPVOID lp);

class CMessagesHandler : public IMSingeltone<CMessagesHandler>
{
	friend INT WINAPI ContactsFetcherThread(LPVOID lp);
public:
	CMessagesHandler(void);
	~CMessagesHandler(void);
public:
	void HookEvents(void);
	void UnHookEvents(void);
	void WriteToHistory(LPTSTR lpMsg, HANDLE hContact);
	CSettingsHandler & getSettings(void);
	bool AllowReply(HANDLE hContact);
	void MakeAction(bool bEnable);
	static LPTSTR GetContactName(HANDLE hContact);
	static int SendProtoMess(HANDLE hContact, LPCSTR szMess);	
protected:	// hooks procs
	static int EventAddHook(WPARAM wp, LPARAM lp);	
private:
	bool m_bInited;
private:
	HANDLE				m_hEvents[HOOKS_NUM];
	CSettingsHandler	m_settings;
	ContactsMap		    m_contacts;
	CRITICAL_SECTION	m_critSect;
	HANDLE				m_hFetcherThread;
	HANDLE				m_hEnableEvent;
	bool				m_bShouldWork;
};