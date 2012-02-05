/*
 *  Smart Auto Replier (SAR) - auto replier plugin for Miranda IM
 *
 *  Copyright (C) 2005 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>
 *
 *      This file is part of SAR.
 *
 *  SAR is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SAR is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SAR.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "editreplydlg.h"

extern LPTSTR g_strPluginName;

CEditReplyDlg::CEditReplyDlg(void)
{
}

CEditReplyDlg::~CEditReplyDlg(void)
{
}

BOOL CEditReplyDlg::PreTranslateMessage(MSG* pMsg)
{
	return IsDialogMessage(pMsg);
}

LRESULT CEditReplyDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{	
	TranslateDialogDefault(m_hWnd);
	m_editReplyText = GetDlgItem(IDC_ED_TEXT);
	m_editReplyHeader = GetDlgItem(IDC_ED_HEADER);

	if (m_commRule.Header)
	{
		m_editReplyHeader.SetWindowText(m_commRule.Header);
	}
	if (m_commRule.Message)
	{
		m_editReplyText.SetWindowText(m_commRule.Message);
	}

	CenterWindow(GetDesktopWindow());	

	return FALSE;
}

LRESULT CEditReplyDlg::OnBtnOKClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nLength = m_editReplyHeader.GetWindowTextLength();

	if (nLength > SETTINGS_HEADER_MAXVALENGTH)
	{
		MessageBox (Translate("header is too big"), g_strPluginName, MB_OK);
		//m_editReplyHeader.SetWindowText(m_szHeader);
		return FALSE;
	}

	VirtualFree ((LPVOID)m_commRule.Header , 0, MEM_RELEASE);

	nLength++;
	m_commRule.Header = reinterpret_cast<char*>(VirtualAlloc (NULL, nLength, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE));
	if (!m_commRule.Header)
		return FALSE;
	m_editReplyHeader.GetWindowText((LPTSTR)m_commRule.Header, nLength);

	VirtualFree ((LPVOID)m_commRule.Message, 0, MEM_RELEASE);

	nLength = m_editReplyText.GetWindowTextLength();

	if (nLength > SETTINGS_MESSAGE_MAXVALENGTH)	
	{
		MessageBox (Translate("message is too big"), g_strPluginName, MB_OK);

		return FALSE;
	}
	nLength++;
	m_commRule.Message = reinterpret_cast<char*>(VirtualAlloc (NULL, nLength, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE));

	if (!m_commRule.Message)
		return FALSE;
	
	m_editReplyText.GetWindowText((LPTSTR)m_commRule.Message, nLength);

	m_bAllOk = true;
	EndDialog(1);
	return FALSE;
}

LRESULT CEditReplyDlg::OnBtnCancelClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bAllOk = false;
	EndDialog(0);
	return FALSE;
}