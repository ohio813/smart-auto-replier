#pragma once

#include "..\resource.h"

#include <commctrl.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>

class CSelectUserDlg : public CDialogImpl<CSelectUserDlg>, public CMessageFilter
{
public:
	CSelectUserDlg(void);
	~CSelectUserDlg(void);
public:
	enum {IDD = IDD_SEL_USER};	
	BEGIN_MSG_MAP(CSelectUserDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnBtnOKClicked)
		COMMAND_ID_HANDLER(IDCANCEL, OnBtnCancelClicked)
		COMMAND_HANDLER(IDC_LIST_USERS, LBN_DBLCLK, OnLbnDblclkListUsers)
	END_MSG_MAP()
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnOKClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnCancelClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
private:
	void RebuildCL();
public:
	bool m_bAllOk;
	TCHAR m_szRetVal[MAX_PATH];
protected:
	CListBox m_listUsers;
public:
	LRESULT OnLbnDblclkListUsers(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};
