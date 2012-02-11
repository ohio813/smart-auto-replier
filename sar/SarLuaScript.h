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

#pragma once

#include "LuaBridge.h"
#include "LuaScript.h"

class CSarLuaScript : public CLuaScript
{
public:
	CSarLuaScript(CLuaBridge & luaBridge);
	~CSarLuaScript(void);

private:

	int ScriptCalling(CLuaBridge & luaBridge, int nFncNumber);
	int SendMessage(CLuaBridge & luaBridge);

	void HandleReturns (CLuaBridge & luaBridge, const char *strFunc){}

protected:
   int m_nFuncBaseIndex;
};

