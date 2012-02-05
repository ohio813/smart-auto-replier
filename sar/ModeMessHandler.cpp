#include "stdafx.h"
#include "modemesshandler.h"

/// TODO: add comments and change char* to TCHAR* !!!

/// ctor
CModeMessHandler::CModeMessHandler(void)
{
}

/// dtor
CModeMessHandler::~CModeMessHandler(void)
{
}

char * CModeMessHandler::StatusModeToDbSetting(int status,const char *suffix)
{
    LPTSTR prefix;
	static TCHAR str[64] = {0};

	switch(status) {
		case ID_STATUS_AWAY: prefix = "Away";	break;
		case ID_STATUS_NA: prefix = "Na";	break;
		case ID_STATUS_DND: prefix = "Dnd"; break;
		case ID_STATUS_OCCUPIED: prefix = "Occupied"; break;
		case ID_STATUS_FREECHAT: prefix = "FreeChat"; break;
		case ID_STATUS_ONLINE: prefix = "On"; break;
		case ID_STATUS_OFFLINE: prefix = "Off"; break;
		case ID_STATUS_INVISIBLE: prefix = "Inv"; break;
		case ID_STATUS_ONTHEPHONE: prefix = "Otp"; break;
		case ID_STATUS_OUTTOLUNCH: prefix = "Otl"; break;
		default: 
			return NULL;
	}
	lstrcpy(str, prefix); 
	_tcscat(str, suffix);

	return str;
}

LPTSTR CModeMessHandler::GetDefaultMessage(INT status)
{
	switch (status)
	{
		case ID_STATUS_AWAY:	return Translate("I've been away since %time%.");
		case ID_STATUS_NA:		return Translate("Give it up, I'm not in!");
		case ID_STATUS_OCCUPIED: return Translate("Not right now.");
		case ID_STATUS_DND: return Translate("Give a guy some peace, would ya?");
		case ID_STATUS_FREECHAT: return Translate("Well, I would talk to you if Miranda IM supported chat");
		case ID_STATUS_ONLINE: return Translate("Yep, I'm here.");
		case ID_STATUS_OFFLINE: return Translate("Nope, not here.");
		case ID_STATUS_INVISIBLE: return Translate("I'm hiding from the mafia.");
		case ID_STATUS_ONTHEPHONE: return Translate("That'll be the phone.");
		case ID_STATUS_OUTTOLUNCH: return Translate("Mmm...food.");
	}
	return NULL;
}

/// returnes mode message
/// looks like ret. val should be free(), but
/// read notes in the place where this func is used
LPTSTR CModeMessHandler::GetModeMessage(INT statusMode)
{
	DBVARIANT dbv = {0};
	INT nretVal = {0};	/// suppose failure..

	nretVal = DBGetContactSetting(NULL, "SRAway", StatusModeToDbSetting(statusMode, "Msg"), &dbv);
	if (nretVal) /// unable to get away msg...
	{
		dbv.pszVal = _tcsdup(GetDefaultMessage(statusMode));
		if (dbv.pszVal == NULL) /// maybe will occure ;)
		{
			DBGetContactSetting(NULL, "SRAway", StatusModeToDbSetting(statusMode, "Default"), &dbv);
		}
	}

	int i;
	TCHAR substituteStr[128] = {0};
	for(i = 0; dbv.pszVal[i]; i++)
	{
		if (dbv.pszVal[i] != '%')
			continue;
		if (!_tcsncicmp(dbv.pszVal+i, "%time%", 6))
			GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, NULL, NULL, substituteStr, sizeof(substituteStr) / sizeof (TCHAR));
		else if (!_tcsncicmp(dbv.pszVal+i, "%date%", 6))
			GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, NULL, NULL, substituteStr, sizeof(substituteStr) / sizeof(TCHAR));
		else
			continue;
		if (_tcslen(substituteStr) > 6) 
			dbv.pszVal = (LPTSTR)realloc(dbv.pszVal, _tcslen(dbv.pszVal) + 1 + _tcslen(substituteStr) - 6);
		MoveMemory(dbv.pszVal + i + _tcslen(substituteStr), dbv.pszVal + i + 6, _tcslen(dbv.pszVal) - i - 5);
		CopyMemory(dbv.pszVal + i, substituteStr, _tcslen(substituteStr));
	}

	return dbv.pszVal;
}