///////////////////////////////////////////////////////////
//  CSWObject.cpp
//  Implementation of the Class CSWObject
//  Created on:      28-二月-2013 14:09:46
//  Original author: zy
///////////////////////////////////////////////////////////

#include "SWObject.h"

/**
 * @brief 构造函数
 */
CSWObject::CSWObject(){

    m_dwRef = 1;
    swpa_mutex_create(&m_hMutex, NULL);
}

/**
 * @brief 析构函数
 */
CSWObject::~CSWObject(){

	swpa_mutex_delete(&m_hMutex);
}

/**
 * @brief 引用计数加1
 */
DWORD CSWObject::AddRef(){
    
  swpa_mutex_lock(&m_hMutex, -1);
  ++m_dwRef;
	swpa_mutex_unlock(&m_hMutex);
	return m_dwRef;
}

/**
 * @brief 引用计数减1，并在计数为0时销毁掉该对象
 * 
 * @return
 * - S_OK : 成功
 * - E_FAIL : 失败
 */
HRESULT CSWObject::Release(){

  swpa_mutex_lock(&m_hMutex, -1);
  int iRef = --m_dwRef;
	swpa_mutex_unlock(&m_hMutex);

  if ( iRef == 0 ) 
  {
      delete this;
  }
	return S_OK;
}

DWORD CSWObject::GetRefCount(){
	return m_dwRef;
}
