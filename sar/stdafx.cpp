#include "stdafx.h"

void OnRefreshSettings(void)
{
	SendMessage(HWND_BROADCAST, REFRESH_OPTS_MSG, 0, 0);
}