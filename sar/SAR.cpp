// IMRS.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "resource.h"

/// these dlg are necessary here..
#include "gui\settingsdlgholder.h"
#include "gui\EditReplyDlg.h" 
#include "gui\addruledlg.h"

/// let's advertise myselft and this plugin ;)
#define ICQPROTONAME		"SAR"
#define ICQPROTODECRSHORT   "Smart Auto Replier"
#define ICQPROTODECR		"Smart Auto Replier is able to reply on all incoming messages, making possible use of rules that are applied to specific contacts."
#define DEVNAME				"Shcherbyna Volodymyr"
#define DEVMAIL				"volodymyr@shcherbyna.com"
#define DEVCOPYRIGHT		"� 2005-2012 Shcheryna Volodymyr, www.shcherbyna.com"
#define DEVWWW				"http://code.google.com/p/smart-auto-replier/"
#define GLOB_HOOKS			2

/// global menu items strings...
#define MENU_ITEM_DISABLE_CAPTION	Translate("Disable Auto replier")
#define MENU_ITEM_ENABLE_CAPTION	Translate("Enable Auto replier")

//#define 
/// global data...
HINSTANCE		hInst = NULL;				/// hinstance 
PLUGINLINK		*pluginLink = NULL;			/// ptr to plugin object
CMessagesHandler *g_pMessHandler = NULL;	/// ptr to "main" global manager object
CSettingsDlgHolder *g_pSettingsDlg = NULL;
static HANDLE g_hHook[GLOB_HOOKS] = {NULL};	/// global hooks
CLISTMENUITEM g_mi = {NULL};				/// handle to menu item that is used to switch on/off plugin
CLISTMENUITEM g_miAUR2This = {NULL};		/// handle to menu item that is used to add user to aur 
bool g_bEnableMItem = true;					/// flag for menu item..
HANDLE g_hMenuItem;							/// global handle to menu service
CCrushLog CRUSHLOGOBJ;						/// global object that handles all crushes
LPTSTR g_strPluginName = ICQPROTONAME;		/// global string that represents plugin name...
INT	   g_nCurrentMode = 0;					/// current mode of a protocol == i duno which exactly protocol...
HANDLE g_hAUR2User = NULL;					/// handle to proto service of "autoreply to this user"

/// forming an info structure 
PLUGININFO pluginInfo =
{
	sizeof(PLUGININFO),
	ICQPROTODECRSHORT,
	PLUGIN_MAKE_VERSION(0, 0, 0, 9),
	ICQPROTODECR,
	DEVNAME,
	DEVMAIL,
	DEVCOPYRIGHT,
	DEVWWW,
	0,		/// "not transient" == saw this in many plugins, let it be here
	0		/// "doesn't replace anything built-in" == read above ;)
};

/// mapping into mirandaim.exe
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason,LPVOID lpvReserved)
{	
BEGIN_PROTECT_AND_LOG_CODE
	hInst = hinstDLL;
	DisableThreadLibraryCalls(hinstDLL);
	return TRUE;
END_PROTECT_AND_LOG_CODE
}

/// let miranda use ptr to info structure
extern "C" __declspec(dllexport) PLUGININFO* MirandaPluginInfo(DWORD dwMirandaVersion)
{
BEGIN_PROTECT_AND_LOG_CODE
	return &pluginInfo;
END_PROTECT_AND_LOG_CODE
}

/// handler of event when the options are choosed
static int OptionsInitialized(WPARAM wp, LPARAM lp)
{	
BEGIN_PROTECT_AND_LOG_CODE
	LPTSTR lpszGroup = Translate("Plugins");
	OPTIONSDIALOGPAGE optsDialog = {0};
	optsDialog.cbSize = sizeof(OPTIONSDIALOGPAGE);
	optsDialog.hInstance = hInst;	
	optsDialog.position = 910000000;
	optsDialog.pszTemplate = MAKEINTRESOURCE(IDD_SDLGHOLDER);
	TCHAR szMax[MAX_PATH] = {0};
	_tcscpy(szMax, Translate(ICQPROTODECRSHORT));
	optsDialog.pszTitle = szMax;
	optsDialog.pszGroup = lpszGroup;	
	optsDialog.pfnDlgProc = (DLGPROC)&CSettingsDlgHolder::FakeDlgProc;
	optsDialog.flags = ODPF_BOLDGROUPS/*|ODPF_EXPERTONLY*/; /// some lames are scaring that option...
	g_pSettingsDlg->m_bDestroying = false;
	CallService(MS_OPT_ADDPAGE, wp, reinterpret_cast<LPARAM>(&optsDialog));
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

/// pre building menu item for a contact..
static int AURContactPreBuildMenu(WPARAM wParam, LPARAM lParam)
{
	LPTSTR lpContactName = g_pMessHandler->GetContactName(reinterpret_cast<HANDLE>(wParam));
	CRulesStorage & storage = g_pMessHandler->getSettings().getStorage();
	bool bval = storage.RuleIsRegistered(lpContactName);

	if (bval)
	{
		g_miAUR2This.flags = CMIM_FLAGS | CMIF_GRAYED;
	}	
	else
	{
		g_miAUR2This.flags = CMIM_FLAGS;
	}

	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)g_hAUR2User, (LPARAM)&g_miAUR2This);

	return 0;
}

/// handler of clicking on menu item
/// aur to this user...
static int AUR2User(WPARAM wParam, LPARAM lParam)
{
	CAddRuleDlg dlg;
	RULE_ITEM item;
	item.ContactName = g_pMessHandler->GetContactName(reinterpret_cast<HANDLE>(wParam));
	item.ReplyAction = " ";
	item.ReplyText = " ... ";
	TCHAR rulename[MAX_PATH * 5] = {0};
	_tcscat(rulename, "reply to ");
	_tcscat(rulename, item.ContactName);
	item.RuleName = rulename;

	dlg.m_baur2thisMode = true;
	dlg.m_item = item;
	dlg.DoModal();
	if (dlg.m_bAddedOk)
	{	/// already added...
		OnRefreshSettings();
	}

	return 0;
}

/// handler of clicking on menu item enablr/disable plugin...
static int PluginMenuCommand(WPARAM wParam, LPARAM lParam)
{
BEGIN_PROTECT_AND_LOG_CODE
	if (!g_pMessHandler)
		return 0;

	if (g_bEnableMItem)
	{/// lets handle enabling of AUR feature
		g_pMessHandler->MakeAction(true); /// this will enable plugin
	}
	else
	{/// lets handle disabling of AUR feature
		g_pMessHandler->MakeAction(false); /// this will disable plugin
	}

	/// put here stuff to do
	TCHAR sz[MAX_PATH] = {0};
	_tcscpy(sz, (g_bEnableMItem == true ? MENU_ITEM_DISABLE_CAPTION : MENU_ITEM_ENABLE_CAPTION));
	g_mi.pszName = Translate(sz);
	g_mi.flags = CMIM_NAME;
#ifdef _DEBUG
	int nretval = 
#endif
	CallService(MS_CLIST_MODIFYMENUITEM, (WPARAM)g_hMenuItem, (LPARAM)&g_mi);

	g_bEnableMItem = !g_bEnableMItem;
END_PROTECT_AND_LOG_CODE
	return 0;
}

/// handler that is invoked when mode is changed
static int OnStatusModeChanged(WPARAM wp, LPARAM lp)
{	
BEGIN_PROTECT_AND_LOG_CODE
	g_nCurrentMode = static_cast<INT>(wp);
	if (!g_pMessHandler)
		return FALSE;

	bool bNonOnOffLine = (wp != ID_STATUS_ONLINE &&
								  wp != ID_STATUS_OFFLINE &&
								  wp != ID_STATUS_FREECHAT);

	REPLYER_SETTINGS & s = g_pMessHandler->getSettings().getSettings();
	COMMON_RULE_ITEM & r = g_pMessHandler->getSettings().getStorage().getCommonRule();

	if (!bNonOnOffLine && s.bDisableWhenModeIsSet)
	{
		if (s.bEnabled)	/// plugin is enabled... we have a chance to disable it
		{
			bool bDisableIt = false;

			if (s.ModeDisValue == 0)
				bDisableIt = true;
			else if (s.ModeDisValue == 1 && wp == ID_STATUS_ONLINE)
				bDisableIt = true;
			else if (s.ModeDisValue == 2 && wp == ID_STATUS_FREECHAT)
				bDisableIt = true;

			if (bDisableIt)
			{	/// disable plugin...
				PluginMenuCommand(0, 0);
			}
		}
		return FALSE;
	}		

	if (s.bShowAURDlgWhenModeChanges)
	{
		CEditReplyDlg dlg;
		dlg.m_commRule = r;

		dlg.DoModal();
		if (dlg.m_bAllOk)
		{
			r = dlg.m_commRule;
		}
	}
	if (s.bEnableWhenModeIsSet)
	{
		if (!s.bEnabled)
		{			
			bool bEnable = false;
			if (s.ModeValue == 0 && bNonOnOffLine)
				bEnable = true;
			else if (s.ModeValue == 1 && wp == ID_STATUS_DND)
				bEnable = true;
			else if (s.ModeValue == 2 && wp == ID_STATUS_NA)
				bEnable = true;
			else if (s.ModeValue == 3 && wp == ID_STATUS_AWAY)
				bEnable = true;
			else if (s.ModeValue == 4 && wp == ID_STATUS_OCCUPIED)
				bEnable = true;

			if (bEnable) /// enabling plugin... (I realize that enabling and disabling is made via ass but i am lazy today)
				PluginMenuCommand(0, 0);
		}
	}

	return FALSE;
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

#ifndef _DEBUG
LPTOP_LEVEL_EXCEPTION_FILTER UnhandledExceptionFilter(
  LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter
)
{
	LPTOP_LEVEL_EXCEPTION_FILTER f = {0};
BEGIN_PROTECT_AND_LOG_CODE	
	throw f;	/// show msgbox to user and store inf to file	
END_PROTECT_AND_LOG_CODE
	return f;	/// let compiler shut ups...
}
#endif

/// loading plugin
extern "C" int __declspec(dllexport) Load(PLUGINLINK *link)
{
#ifndef _DEBUG
	SetUnhandledExceptionFilter(UnhandledExceptionFilter);
#endif
#ifdef _DEBUG
	//MessageBox (NULL, __FUNCTION__, __FILE__, MB_OK);
#endif
	CCrushLog::Init(); /// crushes manager should be inited first
BEGIN_PROTECT_AND_LOG_CODE	
	pluginLink = link;

#ifndef _DEBUG
	/// creates help file near plugin
	/// if it is absent...
	/// note: originally help is in recources of plugin
	CheckForHelpFile();
#endif
					 /// main manager...
	g_pMessHandler = new CMessagesHandler();
	g_pSettingsDlg = new CSettingsDlgHolder();

	if (g_pMessHandler)
	{	/// is inited
		REPLYER_SETTINGS & s = g_pMessHandler->getSettings().getSettings();
		if (s.bDisableWhenMirandaIsOn)
		{	
			s.bEnabled = false;
		}
		else
		{
			if (s.bEnabled)
				g_pMessHandler->HookEvents();
		}
		
		/// hook main events
		//HookEvent(MS_AWAYMSG_SHOWAWAYMSG, OnShow
		g_hHook[0] = HookEvent(ME_OPT_INITIALISE, OptionsInitialized);
		g_hHook[1] = HookEvent(ME_CLIST_STATUSMODECHANGE, OnStatusModeChanged);

		/// Yushenko Tak, Yanukovich - Mudak. (Ukranian revolution)
		/// 

		/// create menu item for disabling / enabling plugin
		CreateServiceFunction("AUR/MenuCommand", PluginMenuCommand);
		ZeroMemory(&g_mi, sizeof(g_mi));
		g_mi.cbSize = sizeof(g_mi);
		g_mi.position = -0x7FFFFFFF;
		g_mi.flags = 0;
		g_mi.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SAR_ICON)); //LoadSkinnedIcon(IDI_AUR_ICON/*SKINICON_OTHER_MIRANDA*/);
		bool bVal = g_pMessHandler->getSettings().getSettings().bEnabled;
		TCHAR sz[MAX_PATH] = {0};
		_tcscpy(sz, bVal ? MENU_ITEM_DISABLE_CAPTION : MENU_ITEM_ENABLE_CAPTION);
		g_mi.pszName = Translate(sz);
		g_mi.pszService = "AUR/MenuCommand";
		g_hMenuItem = (HANDLE)CallService(MS_CLIST_ADDMAINMENUITEM, 0, (LPARAM)&g_mi);
		g_bEnableMItem = !bVal;

		/// creating menu item for a contact		
		ZeroMemory(&g_miAUR2This, sizeof(g_miAUR2This));
		g_miAUR2This.cbSize = sizeof(g_miAUR2This);
		g_miAUR2This.position = -2000010000;
		g_miAUR2This.flags = 0;
		g_miAUR2This.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SAR_ICON));
		g_miAUR2This.pszContactOwner = NULL;
		g_miAUR2This.pszName = Translate("&Autoreply to this user");
		CreateServiceFunction("AUR/AURToThis", AUR2User);
		g_miAUR2This.pszService = "AUR/AURToThis";
		g_hAUR2User = (HANDLE)CallService(MS_CLIST_ADDCONTACTMENUITEM, 0, (LPARAM)&g_miAUR2This);
		HookEvent(ME_CLIST_PREBUILDCONTACTMENU, AURContactPreBuildMenu);
	}
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

/// unloading plugin
extern "C" int __declspec(dllexport) Unload(void)
{
BEGIN_PROTECT_AND_LOG_CODE
	for (size_t i = 0; i < GLOB_HOOKS; i++)
		UnhookEvent(g_hHook[i]);

	if (g_pSettingsDlg)
	{
		if (g_pSettingsDlg->IsWindow())
			g_pSettingsDlg->DestroyWindow();
		delete g_pSettingsDlg;
	}
	if (g_pMessHandler)
	{	/// unhook manager's events...
		g_pMessHandler->UnHookEvents();
		delete g_pMessHandler; /// call dtor
		g_pMessHandler = NULL;
	}	
END_PROTECT_AND_LOG_CODE

	CCrushLog::DeInit(); /// destroy crushes manager obj
	return FALSE;
}