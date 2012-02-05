#pragma once

/// base class for singletones in plugin
template <class T>
class IMSingeltone
{
public:	
	IMSingeltone(){};
	IMSingeltone(T * ptclass) {m_pThis = ptclass;}
	static void SetObject(T* pObj) {m_pThis = pObj;};
	static T* GetObject(){return m_pThis;};
protected:
	static T* m_pThis;
};