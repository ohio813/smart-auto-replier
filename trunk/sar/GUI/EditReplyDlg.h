#pragma once

#include "..\resource.h"

#include <commctrl.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>

class CEditReplyDlg : public CDialogImpl<CEditReplyDlg>, public CMessageFilter
{
public:
	CEditReplyDlg(void);
	~CEditReplyDlg(void);
public:
	enum {IDD = IDD_SET_AURMESSAGEDLG};	
	BEGIN_MSG_MAP(CEditReplyDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnBtnOKClicked)
		COMMAND_ID_HANDLER(IDCANCEL, OnBtnCancelClicked)
	END_MSG_MAP()
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBtnOKClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnCancelClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:
	REPLYER_SETTINGS m_settings;
	COMMON_RULE_ITEM m_commRule;
	bool	m_bAllOk;
protected:
	CEdit m_editReplyText;
	CEdit m_editReplyHeader;
};
