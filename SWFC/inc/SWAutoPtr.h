#ifndef __SW_AUTO_PTR_H__
#define __SW_AUTO_PTR_H__
#include "SWObject.h"

template<class T>class CSWAutoPtr
{
public:
	CSWAutoPtr(T *t = NULL)
	{
		m_pT = NULL;
		Reset(t);
	}
	
	virtual ~CSWAutoPtr()
	{
		Release();
	}
	
	T& operator*()
	{
		return *m_pT;
	}
	
	T* operator->()
	{
		return Get();
	}
	
	T* Get()
	{
		return m_pT;
	}
	
	VOID Reset(T* t)
	{
		Release();
		m_pT = t;
	}
	
	VOID Release()
	{
		if(m_pT)
		{
			m_pT->Release();
			m_pT = NULL;
		}
	}
private:
	T *m_pT;
};
#endif