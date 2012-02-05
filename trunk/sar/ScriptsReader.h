#pragma once

#include <list>

/// class that converts all scripts
/// into an actions or raw text

/// replacing commands
#define SETTINGS_SCRIPT_USERNAME			"%username%"
#define SETTINGS_SCRIPT_CURRDATA			"%currdate%"
#define SETTINGS_SCRIPT_MODEMESSAGE			"%modemessage%"
#define SETTINGS_SCRIPT_INCOMMINGMESSAGE	"%incomingmessage%"
#define SETTINGS_SCRIPT_OWNNICKNAME			"%ownnickname%"

/// actios commands

typedef struct _ACTION
{
	LPTSTR ActionCode;
}ACTION, *PACTION;

typedef std::list<ACTION> ACTIONS;

class CScriptsReader
{
public:
	CScriptsReader(void);
	~CScriptsReader(void);
public:
	ACTIONS ParceString(LPTSTR & strMessage);
	bool GetReturnMessage(LPSTR lpContactName, LPSTR & lpMsg, LPTSTR & lpIncomingMsg);
	static bool Replace(LPSTR & lpString, LPSTR strReplWhat, LPSTR & strReplWith);
private:
	void InternalInit(void);
private:
};