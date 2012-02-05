#pragma once 

/// define hlobal object..
#ifndef CRUSHLOGOBJ
	#define CRUSHLOGOBJ	g_crushLog
#endif

/// this macro should be placed at the begining of any func of plugin..
#ifndef BEGIN_PROTECT_AND_LOG_CODE	
		#define BEGIN_PROTECT_AND_LOG_CODE		try {			\
												SetLastError(0);
#endif

/// this macro should be placed at the end of any func of plugin..
#ifndef END_PROTECT_AND_LOG_CODE
		#define END_PROTECT_AND_LOG_CODE		}	\
		catch (...)									\
		{											\
			CCrushLog::InternalProtect(__FUNCTION__, __FILE__, __DATE__, __LINE__,	GetLastError());\
		}
#endif
