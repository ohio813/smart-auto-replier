#pragma once

class CCrc32Static
{
public:
	CCrc32Static();
	virtual ~CCrc32Static();
	static DWORD StringCrc32(LPCTSTR szString, DWORD &dwCrc32);
protected:
	static inline void CalcCrc32(const BYTE byte, DWORD &dwCrc32);
	static DWORD s_arrdwCrc32Table[256];
};
