/*
 *  Smart Auto Replier (SAR) - auto replier plugin for Miranda IM
 *
 *  Copyright (C) 2004 - 2012 by Volodymyr M. Shcherbyna <volodymyr@shcherbyna.com>
 *
 *  This code is inspired by article of RichardS at http://www.codeproject.com/Articles/11508/Integrating-Lua-into-C
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
#include "SarLuaScript.h"

#include "../miranda32/m_protomod.h"
#include "../miranda32/m_protosvc.h"

extern INT g_nCurrentMode;

CSarLuaScript::CSarLuaScript(CLuaBridge & luaBridge) : CLuaScript(luaBridge)
{
	m_nFuncBaseIndex = RegisterFunction("SendMessage");
	RegisterFunction("GetMyStatus");
	RegisterFunction("SetMyStatus");	
}

CSarLuaScript::~CSarLuaScript(void)
{
}

int CSarLuaScript::ScriptCalling(CLuaBridge & luaBridge, int nFncNumber)
{
	switch (nFncNumber - m_nFuncBaseIndex)
	{
	case 0:
		return SendMessage(luaBridge);

	case 1:
		return GetMyStatus(luaBridge);

	case 2:
		return SetMyStatus(luaBridge);
	}

	return FALSE;
}

int CSarLuaScript::SendMessage(CLuaBridge & luaBridge)
{
    lua_State *pFunctionContext = (lua_State*)luaBridge;
	  
	const char * szMessage	= lua_tostring(pFunctionContext, -1);
	int hUserToken			= (int)lua_tonumber(pFunctionContext, -2);

	if (szMessage == NULL)
	{
		return FALSE;
	}

	int nRetVal = CallContactService((HANDLE)hUserToken, PSS_MESSAGE, 0, reinterpret_cast<LPARAM>(szMessage));
				
    return FALSE;
}

int CSarLuaScript::GetMyStatus(CLuaBridge & luaBridge)
{
	lua_State *pFunctionContext = (lua_State*)luaBridge;
	int nStatusMode = CallService(MS_CLIST_GETSTATUSMODE, (WPARAM)0, (LPARAM)0);

	nStatusMode = g_nCurrentMode;

	switch (nStatusMode)
	{
	case ID_STATUS_ONLINE:
		AddParam("Online");
		break;

	case ID_STATUS_OFFLINE:
		AddParam("Offline");
		break;

	case ID_STATUS_AWAY:
		AddParam("Away");
		break;

	case ID_STATUS_DND:
		AddParam("DND");
		break;

	case ID_STATUS_NA:
		AddParam("NA");
		break;

	case ID_STATUS_OCCUPIED:
		AddParam("Occupied");
		break;

	case ID_STATUS_FREECHAT:
		AddParam("FreeChat");
		break;

	case ID_STATUS_INVISIBLE:
		AddParam("Invisible");
		break;

	case ID_STATUS_ONTHEPHONE:
		AddParam("OnThePhone");
		break;

	case ID_STATUS_OUTTOLUNCH:
		AddParam("OutToLunch");
		break;

	default:
		AddParam("Unknown");
		break;
	}

	return TRUE;
}

int CSarLuaScript::SetMyStatus(CLuaBridge & luaBridge)
{
	int nNewStatus = 0;
	lua_State *pFunctionContext = (lua_State*)luaBridge;

	const char * szStatus = lua_tostring(pFunctionContext, -1);

	if (szStatus == NULL)
	{
		return FALSE;
	}

	if (strcmp(szStatus, "Online") == 0)
	{
		nNewStatus = ID_STATUS_ONLINE;
	}
	else if (strcmp(szStatus, "Offline") == 0)
	{
		nNewStatus = ID_STATUS_OFFLINE;
	}
	else if (strcmp(szStatus, "Away") == 0)
	{
		nNewStatus = ID_STATUS_AWAY;
	}
	else if (strcmp(szStatus, "DND") == 0)
	{
		nNewStatus = ID_STATUS_DND;
	}
	else if (strcmp(szStatus, "NA") == 0)
	{
		nNewStatus = ID_STATUS_NA;
	}
	else if (strcmp(szStatus, "Occupied") == 0)
	{
		nNewStatus = ID_STATUS_OCCUPIED;
	}
	else if (strcmp(szStatus, "FreeChat") == 0)
	{
		nNewStatus = ID_STATUS_FREECHAT;
	}
	else if (strcmp(szStatus, "Invisible") == 0)
	{
		nNewStatus = ID_STATUS_INVISIBLE;
	}
	else if (strcmp(szStatus, "OnThePhone") == 0)
	{
		nNewStatus = ID_STATUS_ONTHEPHONE;
	}
	else if (strcmp(szStatus, "OutToLunch") == 0)
	{
		nNewStatus = ID_STATUS_OUTTOLUNCH;
	}

	CallService(MS_CLIST_SETSTATUSMODE, (WPARAM)nNewStatus, (LPARAM)0);

	return FALSE;
}

void CSarLuaScript::HandleReturns(CLuaBridge & luaBridge, const char *szFunc)
{

}