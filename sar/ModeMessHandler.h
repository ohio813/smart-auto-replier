#pragma once

class CModeMessHandler
{
public:
	CModeMessHandler(void);
	~CModeMessHandler(void);
public:
	static LPTSTR GetModeMessage(INT nMode);	
private:
	static LPTSTR GetDefaultMessage(INT status);
	static char *StatusModeToDbSetting(int status,const char *suffix);
};
