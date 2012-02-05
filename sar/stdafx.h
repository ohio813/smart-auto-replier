#pragma once

#include "AggressiveOptimize.h"

#define WIN32_LEAN_AND_MEAN
#define _MERGE_RDATA_

/// windows and crt headers (win32 api)
#include <windows.h>
#include <string>
#include <prsht.h>
#include <crtdbg.h>
#include "resource.h"
#include <tchar.h>
#include <windowsx.h>

#include <commctrl.h>

/// shell's headers... (windows shell)
#include <shlwapi.h>
#include <shellapi.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

#pragma warning(disable:4312)
#pragma warning(disable:4267)
#pragma warning(disable:4356)

/// miranda's headers...
#include "../miranda32/newpluginapi.h"
#include "../miranda32/m_system.h"
#include "../miranda32/m_protomod.h"
#include "../miranda32/m_protosvc.h"
#include "../miranda32/m_clist.h"
#include "../miranda32/m_skin.h"
#include "../miranda32/m_database.h"
#include "../miranda32/m_options.h"
#include "../miranda32/m_langpack.h"

/// plugin's specific headers...
#include "messageshandler.h"	/// messages handler
#include "settingshandler.h"	/// settings handler
#include "GUI/optionsDlg.h"			/// options handler
#include "comlogging.h"			/// common loggin helper macroses
#include "crushlog.h"			/// crush log handler

#define REFRESH_OPTS_MSG	WM_USER + 666

#undef _WIN32_WINNT 
#define _WIN32_WINNT 0x0500

void OnRefreshSettings(void);