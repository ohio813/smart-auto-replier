#pragma once

bool NotifyAboutWrongSettings(LPTSTR szFile);
bool MakeFullPath(LPTSTR lpRetval, DWORD dwSize, LPTSTR lpFile);
bool CheckForHelpFile();