#ifndef __SW_OBJECT_LIST_H__
#define __SW_OBJECT_LIST_H__
#include <stdarg.h>

class CSWObjectList : public CSWList<CSWObject *>
{
CLASSINFO(CSWObjectList, CSWObject)
public:
	CSWObjectList()
	{
		SetMaxCount(10);
		m_syncSEM.Create(0, 10);
	}
	virtual ~CSWObjectList()
	{
	}
	/**
	 *@brief 将对象压入队列,压入成功之后对对象进行AddRef,故外部调用此函数之后，还需自行Release
	 *@param [in] pObj 对象指针
	 *@return 成功返回S_OK，错误返回E_FAIL
	 */
	HRESULT Push(CSWObject *pObj)
	{
		m_rwMutex.WLock();
		SW_POSITION pos = AddTail(pObj);
		m_rwMutex.Unlock();
		if(NULL == pos)
		{
			return E_FAIL;
		}
		m_syncSEM.Post();
		return S_OK;
	}
	
	/**
	 *@brief 从队列头节点移出对象
	 *@param [in] dwTimeOut 当队列为空时，等到的超时时间，默认是0，立即返回
	 *@return 成功返回对象指针，失败返回NULL，注意用完此对象需要自行Release
	 */	 
	CSWObject* Pop(DWORD dwTimeOut = 0)
	{
		CSWObject *pObj = NULL;
		if(S_OK == m_syncSEM.Pend(dwTimeOut))
		{
			m_rwMutex.WLock();
			if(FALSE == IsEmpty())
			{
				pObj = RemoveHead();
			}		
			m_rwMutex.Unlock();
		}
		return pObj;
	}
	
	/**
	 *@brief 遍历链表,通过回调函数将对象传递到外部
	 *@param [in] OnObject 回调函数,第一个obj是链表节点，后续的变参是外部传入,返回S_OK继续遍历,返回E_FAIL停止遍历
	 *@param [in] n,(n<10)说明变参中有多少个指针对象
	 *@return 成功返回S_OK，失败返回E_FAIL
	 */
	HRESULT EnumObject(HRESULT (*OnObject)(void* pvParam, CSWObject *obj, ...), void* pvParam, DWORD n, ...)
	{
		void** prgObj = new void*[n];
		
		va_list marker;
		va_start(marker, n);
		for(int i = 0; i < n; i++)
		{
			prgObj[i] = va_arg(marker, CSWObject*);
		}
		va_end(marker);
		
		m_rwMutex.RLock();
		SW_POSITION pos = GetHeadPosition();
		bool fOk = true;
		while(IsValid(pos) && true == fOk)
		{
			switch(n)
			{
			case 0 : fOk = (S_OK == OnObject(pvParam, GetNext(pos))); break;
			case 1 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0])); break;
			case 2 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1])); break;
			case 3 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2])); break;
			case 4 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3])); break;
			case 5 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3], prgObj[4])); break;
			case 6 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3], prgObj[4], prgObj[5])); break;
			case 7 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3], prgObj[4], prgObj[5], prgObj[6])); break;
			case 8 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3], prgObj[4], prgObj[5], prgObj[6], prgObj[7])); break;
			case 9 : fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3], prgObj[4], prgObj[5], prgObj[6], prgObj[7], prgObj[8])); break;
			case 10: fOk = (S_OK != OnObject(pvParam, GetNext(pos), prgObj[0], prgObj[1], prgObj[2], prgObj[3], prgObj[4], prgObj[5], prgObj[6], prgObj[7], prgObj[8], prgObj[9])); break;
			default: break;
			}			
		}
		m_rwMutex.Unlock();
		
		delete []prgObj;
		return S_OK;
	}
private:
	CSWRWMutex m_rwMutex;
	CSWSemaphore m_syncSEM;
};
#endif

