/**
*@file 
*@brief
*
*/

#include "SWFC.h"
//#include "SWFilterStruct.h"

#include "SWExtendDataTransformFilter.h"

#define MAX_EXTEND_DATA_SIZE (3072*2) //附加数据有意思数据大小

CSWExtendDataTransformFilter::CSWExtendDataTransformFilter()
	: CSWBaseFilter(1,1)
	, m_fInited(FALSE)
	, m_dwQueueSize(5)
	, m_pSemaLock(NULL)
{
	//GetIn(0)->AddObject(CLASSID(CSWImage));
	m_pSemaLock = new CSWSemaphore(1, 1);

}

CSWExtendDataTransformFilter::~CSWExtendDataTransformFilter()
{
	//clear the callback 
	swpa_ipnc_setmetadatacallback(NULL, NULL);
	
	//m_mutexLock.Lock();
	if (NULL != m_pSemaLock)
	{
		m_pSemaLock->Pend();
		while (!m_lstExtendData.IsEmpty())
		{
			tExtendDataNode * pNode = m_lstExtendData.RemoveHead();
			if (pNode != NULL)
			{
				SAFE_MEM_FREE(pNode->pData);
			}
			SAFE_MEM_FREE(pNode);
		}
		m_pSemaLock->Post();

		SAFE_RELEASE(m_pSemaLock);
	}
	//m_mutexLock.Unlock();
	m_fInited = FALSE;
}



HRESULT CSWExtendDataTransformFilter::Initialize(PVOID pvParam)
{
	if (NULL == m_pSemaLock)
	{
		SW_TRACE_NORMAL("Err: no mem for m_pSemaLock.\n");
		return E_OUTOFMEMORY;
	}

	m_lstExtendData.SetMaxCount(m_dwQueueSize);

	if (FAILED(m_semaFrame.Create(0, m_dwQueueSize)))
	{
		SW_TRACE_NORMAL("Err: failed to create semaphore\n");
		return E_FAIL;
	}

	m_fInited = TRUE;

	return S_OK;
}



HRESULT CSWExtendDataTransformFilter::Run()
{
	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_NORMAL("Err: failed to run CSWExtendDataTransformFilter\n");
		return E_FAIL;
	}
	
	if (FAILED(m_cSendThread.Start(SendExtendDataProxy, this)))
	{
		SW_TRACE_NORMAL("Err: failed to start send thread\n");
		return E_FAIL;
	}

	//register data callback function
	swpa_ipnc_setmetadatacallback(ExtendDataCallback, this);
	

	return S_OK;
}

HRESULT CSWExtendDataTransformFilter::Stop()
{
	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop CSWExtendDataTransformFilter \n");
		return E_FAIL;
	}

	if (FAILED(m_cSendThread.Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop SendThread \n");
		return E_FAIL;
	}
	
	return S_OK;
}



HRESULT CSWExtendDataTransformFilter::SendExtendData()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	SW_TRACE_DEBUG("Info: %s: %s() Running ..........\n",__FILE__, __FUNCTION__);

	
	BOOL fIsHaveConnect = FALSE;
	CHAR szExtendDataSockFile[128] = {"/tmp/extenddata.sock"};
	
	CSWTCPSocket cLocalTcpSock;
	HRESULT hr;
	if (FAILED(cLocalTcpSock.Create(TRUE)))
	{
        SW_TRACE_DEBUG("Info: ExtendData filter open %s failed.", szExtendDataSockFile);
		return E_FAIL;
	}

	do
	{
		hr = cLocalTcpSock.Bind(szExtendDataSockFile);
		if (FAILED(hr))
		{
			SW_TRACE_DEBUG("Info: ExtendData filter bind %s failed.",szExtendDataSockFile);
			swpa_thread_sleep_ms(2000);
			continue;
		}
	}while(S_OK != hr && FILTER_RUNNING == GetState());
	
	cLocalTcpSock.SetRecvTimeout(4000);
	cLocalTcpSock.SetSendTimeout(4000);
	if (FAILED(cLocalTcpSock.Listen()))
	{
		SW_TRACE_NORMAL("Err: failed to listen!\n");
		return E_FAIL;
	}
	CSWTCPSocket sockData;

	while (FILTER_RUNNING == GetState())
	{
		if (SUCCEEDED(m_semaFrame.Pend(200)))
		{
			//m_mutexLock.Lock();
			m_pSemaLock->Pend();
			tExtendDataNode* pNode = m_lstExtendData.RemoveHead();
			if (NULL == pNode || NULL == pNode->pData)
			{
				SW_TRACE_DEBUG("Extend data node is null............\n");
				m_pSemaLock->Post();
				continue;
			}
			//m_mutexLock.Unlock();
			m_pSemaLock->Post();

			if (FALSE == fIsHaveConnect)
			{
				SWPA_SOCKET_T outSock;
				if (FAILED(cLocalTcpSock.Accept(outSock)))
				{
					//SW_TRACE_NORMAL("Err: Accept failed...");
					SAFE_MEM_FREE(pNode->pData);
					SAFE_MEM_FREE(pNode);
					continue;
				}
				
				SW_TRACE_DEBUG("Info: ExtendData local socket got a connection...\n");
				sockData.Attach(outSock);
				sockData.SetSendTimeout(1000);
				sockData.SetSendBufferSize(32768);	//
				fIsHaveConnect = TRUE;
			}

			DWORD dwOutLengthSent = 0;

			typedef struct framehead
			{
				BYTE bySync;
				BYTE byType;
				BYTE byReserve[2]; 
				DWORD dwTimeStamp;
				DWORD dwDataLen;
							
#define EXTEND_HEAD_SYNC 0x47
#define EXTEND_DATA_TYPE 0x2
				framehead()
				{
					bySync = EXTEND_HEAD_SYNC;
					byType = EXTEND_DATA_TYPE;
					dwTimeStamp = 0;
					dwDataLen = 0;
				}
			}tDataHead;

			tDataHead head;
			head.dwDataLen = pNode->dwDataLen;
			head.dwTimeStamp = pNode->dwTimeStamp;
			HRESULT hr = sockData.Send((void*)&head, sizeof(head),&dwOutLengthSent);
			if (FAILED(hr) || dwOutLengthSent != sizeof(head))
			{
				SW_TRACE_DEBUG("Err: failed to send ExtendData head size via hr:0x%08x,Len:%d Sent:%d\n",
					hr,pNode->dwDataLen,dwOutLengthSent);
				sockData.Close();
				fIsHaveConnect = FALSE;
			}
						
			hr = sockData.Send((void*)pNode->pData, pNode->dwDataLen, &dwOutLengthSent);
			if (FAILED(hr) || dwOutLengthSent != pNode->dwDataLen)
			{
				SW_TRACE_DEBUG("Err: failed to send ExtendData size via hr:0x%08x,Len:%d Sent:%d\n",
					hr,pNode->dwDataLen,dwOutLengthSent);
				sockData.Close();
				fIsHaveConnect = FALSE;
			}
#if 0
			static INT nCount = 0;
			if (nCount++%100 == 0)
				SW_TRACE_NORMAL("CSWExtendDataTransformFilter send Extend Data ..............\n");
#endif
			
			SAFE_MEM_FREE(pNode->pData);
			SAFE_MEM_FREE(pNode);
			
		}
	}


	sockData.Close();
	cLocalTcpSock.Close();

	SW_TRACE_DEBUG("Info: %s: %s() exited\n", __FILE__, __FUNCTION__);
	
	return S_OK;
}


PVOID CSWExtendDataTransformFilter::SendExtendDataProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWExtendDataTransformFilter * pThis = (CSWExtendDataTransformFilter*)pvArg;
		pThis->SendExtendData();
	}

	return NULL;
}



INT CSWExtendDataTransformFilter::ProcessExtendData(VOID *pData, DWORD dwLen)
{
	m_pSemaLock->Pend();
	if (m_lstExtendData.IsFull())
	{
		//SW_TRACE_NORMAL("Err: CSWExtendDataTransformFilter ExtendData Queue is full, discards this data!\n");
		//m_mutexLock.Unlock();
		m_pSemaLock->Post();
	}
	else
	{
		tExtendDataNode* pNode = (tExtendDataNode*)swpa_mem_alloc(sizeof(tExtendDataNode));
		if (NULL == pNode)
		{
			SW_TRACE_NORMAL("Alloc extend data node mem failed!\n");
			m_pSemaLock->Post();
			return -1;
		}
		INT nDataSize = swpa_min(MAX_EXTEND_DATA_SIZE,dwLen);
		
		pNode->pData = (PBYTE)swpa_mem_alloc(nDataSize);
		if (NULL == pNode->pData)
		{
			SW_TRACE_NORMAL("Alloc extend data mem failed!\n");
			m_pSemaLock->Post();
			return -1;
		}
		pNode->dwDataLen = nDataSize;
		pNode->dwTimeStamp = CSWDateTime::GetSystemTick();
		swpa_memcpy(pNode->pData, pData, nDataSize);
#if 0
		static INT nCount = 0;
		if (nCount++%100 == 0)
			SW_TRACE_NORMAL("CSWExtendDataTransformFilter Process Extend Data ..............\n");
#endif
		m_lstExtendData.AddTail(pNode);
		//m_mutexLock.Unlock();
		m_pSemaLock->Post();

		m_semaFrame.Post();
	}
	return 0;
}

VOID CSWExtendDataTransformFilter::ExtendDataCallback(VOID *pvContext, VOID *pData, INT nLen)
{
	if (pvContext != NULL && pData != NULL && nLen > 0)
	{
		DWORD dwStart = CSWDateTime::GetSystemTick();
		CSWExtendDataTransformFilter* pThis = (CSWExtendDataTransformFilter *)pvContext;
		pThis->ProcessExtendData(pData, nLen);
		DWORD dwEnd = CSWDateTime::GetSystemTick();
		if (dwEnd - dwStart > 40)
		{
			SW_TRACE_NORMAL("...............ProcessExtendData takes:%d ..........\n",dwEnd-dwStart);
		}
	}
	return ;
}


