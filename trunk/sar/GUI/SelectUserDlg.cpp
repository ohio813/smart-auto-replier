#include "stdafx.h"
#include "selectuserdlg.h"
#include "selectuserdlg.h"

extern LPTSTR g_strPluginName;
extern CMessagesHandler * g_pMessHandler;


CSelectUserDlg::CSelectUserDlg(void) : m_bAllOk(false)
{
}

CSelectUserDlg::~CSelectUserDlg(void)
{
}

BOOL CSelectUserDlg::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

LRESULT CSelectUserDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{	
BEGIN_PROTECT_AND_LOG_CODE
	TranslateDialogDefault(m_hWnd);
	m_listUsers = GetDlgItem(IDC_LIST_USERS);	
	RebuildCL();
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

void CSelectUserDlg::RebuildCL()
{
BEGIN_PROTECT_AND_LOG_CODE
	HANDLE hContact= reinterpret_cast<HANDLE> (CallService(MS_DB_CONTACT_FINDFIRST, 0, 0));
	char* szContactName = NULL;
	char* szProto = NULL;
	DWORD wId = 0;
	while (hContact != NULL)
	{
		szContactName = g_pMessHandler->GetContactName(hContact);		
		szProto = (char*)CallService(MS_PROTO_GETCONTACTBASEPROTO, (WPARAM)hContact, 0);
		if (szProto)
		{
			size_t nPos = -1;
			if (szContactName)
			{
				nPos = m_listUsers.AddString(szContactName);
			}

			/*DBVARIANT db = {0};
			DBGetContactSetting(hContact, szProto, "uin", &db);
			DBGetContactSetting(hContact, szProto, "UIN", &db);*/
			//m_listUsers.SetItemData(nPos, )
		}

		hContact = reinterpret_cast<HANDLE> (CallService(MS_DB_CONTACT_FINDNEXT,(WPARAM) hContact, 0));
	}
END_PROTECT_AND_LOG_CODE
}

LRESULT CSelectUserDlg::OnBtnOKClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
BEGIN_PROTECT_AND_LOG_CODE
	INT nsel = m_listUsers.GetCurSel();	
	m_bAllOk = (nsel != -1);
	if (m_bAllOk == true)
	{
		m_listUsers.GetText(nsel, m_szRetVal);
		EndDialog(0);
	}
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

LRESULT CSelectUserDlg::OnBtnCancelClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
BEGIN_PROTECT_AND_LOG_CODE
	m_bAllOk = false;
	EndDialog(0);
END_PROTECT_AND_LOG_CODE
	return FALSE;
}

LRESULT CSelectUserDlg::OnLbnDblclkListUsers(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{	
	return OnBtnOKClicked(wNotifyCode, wID, hWndCtl, bHandled);
}
