#pragma once

#include "..\resource.h"

#include <commctrl.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>

#include "optionsdlg.h"

/// dialog that holds all 
/// gui dialogs..
class CSettingsDlgHolder : public CDialogImpl<CSettingsDlgHolder>,
						   public CMessageFilter, 
						   public IMSingeltone<CSettingsDlgHolder>
{
public: /// ctors
	CSettingsDlgHolder(void);
		/// dctors
	~CSettingsDlgHolder(void);
public:
	enum {IDD = IDD_SDLGHOLDER};	
	BEGIN_MSG_MAP(CSettingsDlgHolder)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(PSM_CHANGED, OnSettingsChanged)
	END_MSG_MAP()
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSettingsChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
private:
	void OnNotifyParents(LPNMHDR lpnmhdr, int nCode);
private:
	bool m_bTabSelected;
public:
	static BOOL CALLBACK FakeDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
	CTabCtrl m_tabCtrl;			/// holder of all settings dlg
	CStatic m_statusMsg;
	COptionsDlg m_generalOpt;	/// general options dlg
public:
	bool m_bDestroying;
};
