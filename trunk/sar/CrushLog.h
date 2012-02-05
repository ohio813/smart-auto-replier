#pragma once

/// crash item that is written in file...
typedef struct _CRUSHLOG_ITEM
{
	LPTSTR	CompilationDate;		/// compilation date of the source file
	LPTSTR	FileName;				/// file name where exception has occured
	LPTSTR	FunctionName;			/// function name where exception has occured
	INT		LineNumber;				/// line number
	INT		LastError;				/// last error
}CRUSHLOG_ITEM, *PCRUSHLOG_ITEM;

#define LOG_FILENAME		"sarcrushes.log"

/// class that handles all crushes
/// occured between BEGIN_LOG_AND_PRO.. and END_LOG..
class CCrushLog
{
public:	/// ctors, dtors section...
	CCrushLog(void);	
	~CCrushLog(void);
public:
	static void Init(void);		/// inits internal data..
	static void DeInit(void);	/// deinits internal data..
	static void SaveToLog(CRUSHLOG_ITEM & item);	/// saves data to log file
	static void InternalProtect(LPTSTR	FunctionName, LPTSTR FileName, 
								LPTSTR	CompilationDate, INT LineNumber,
								INT		LastError);
public:
	static TCHAR m_szLogPath[MAX_PATH];		/// path to log file
private:	
	static CRITICAL_SECTION m_critSect;		/// sync object...
};
