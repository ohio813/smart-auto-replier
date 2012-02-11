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

CSarLuaScript::CSarLuaScript(CLuaBridge & luaBridge) : CLuaScript(luaBridge)
{
	m_nFuncBaseIndex = RegisterFunction("SendMessage");
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
		return SendMessage(luaBridge);
	}

	return 0;
}

int CSarLuaScript::SendMessage(CLuaBridge & luaBridge)
{
    lua_State *pFunctionContext = (lua_State*)luaBridge;
	  
	const char * szMessage	= lua_tostring(pFunctionContext, -1);
	int hUserToken			= (int)lua_tonumber(pFunctionContext, -2);

	int nRetVal = CallContactService((HANDLE)hUserToken, PSS_MESSAGE, 0, reinterpret_cast<LPARAM>(szMessage));
				
    return 0;
}