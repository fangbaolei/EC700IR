//// Buffer.cpp: implementation of the CBuffer1 class.
////
////////////////////////////////////////////////////////////////////////
//#include <memory.h>
//#include <malloc.h>
//#include "Buffer.h"
//
////////////////////////////////////////////////////////////////////////
//// Construction/Destruction
////////////////////////////////////////////////////////////////////////
//
//CBuffer::CBuffer()
//{
//	m_pBuffer = NULL ;
//	m_iSize = 0;
//	m_iMaxSize = 0;
//}
//
//CBuffer::CBuffer(int s)
//{
//	m_pBuffer = NULL;
//	Resize(s);
//}
//
//CBuffer::CBuffer(char * buffer, int size)
//{
//	m_pBuffer = NULL;
//	Resize(size);
//	memcpy(m_pBuffer, buffer, size);
//}
//
//const CBuffer & CBuffer::operator =(const CBuffer & b)
//{
//	Resize(b.m_iSize);
//	memcpy(m_pBuffer, b.m_pBuffer, b.m_iSize);
//	return * this;
//}
//
//CBuffer::~CBuffer()
//{  
//	Release();
//}
//
//void CBuffer::Resize(int size)
//{
//	if(size > 0 && m_iMaxSize < size)
//	{
//		m_iMaxSize = size;
//		m_pBuffer = (char *)realloc(m_pBuffer, m_iMaxSize);
//	}
//	m_iSize = size;
//}
//
//void CBuffer::Release(void)
//{
//	if(m_pBuffer)
//	{
//		free(m_pBuffer);
//		m_pBuffer = NULL;
//	}
//	m_iSize = 0;
//	m_iMaxSize = 0;
//}