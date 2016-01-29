// 该文件编码格式必须是WIN936
/**
* @file SWResultVideoTransformFilter.cpp
* @brief Implementation of CSWResultVideoTransformFilter
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-03-22
* @version 1.0
*/
	
#include "SWFC.h"
#include "SWResultVideoTransformFilter.h"
#include "SWTransmittingOpt.h"
#include "SWRecord.h"




CSWResultVideoTransformFilter::CSWResultVideoTransformFilter()
  : CSWBaseFilter(1, 1)
  , CSWMessage(0/*todo*/, 1/*todo*/)
  , m_dwQueueSize(4)
  , m_fInited(FALSE)
{
	GetIn(0)->AddObject(CLASSID(CSWRecord));
	GetOut(0)->AddObject(CLASSID(CSWRecord));
}


CSWResultVideoTransformFilter::~CSWResultVideoTransformFilter()
{
	if (!m_fInited)
	{
		return ;
	}

	//free the file list
	m_cRecordLock.Pend();
	while (!m_lstRecord.IsEmpty())
	{
		CSWRecord * pcRecord = (CSWRecord *)m_lstRecord.RemoveHead();
		SAFE_RELEASE(pcRecord);
	}	
	m_cRecordLock.Post();

	Stop();

	m_pcMemoryFactory = NULL;
	
	m_fInited = FALSE;
	
}


HRESULT CSWResultVideoTransformFilter::Initialize()
{
	
	m_pcMemoryFactory = CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY);
	if (NULL == m_pcMemoryFactory)
	{
		SW_TRACE_DEBUG("Err: failed to get m_pcMemoryFactory instance\n");
		return E_FAIL;
	}
	
	m_cRecordLock.Create(1, 1);

	m_lstRecord.SetMaxCount(m_dwQueueSize);

	m_cSemaQueueSync.Create(0, m_dwQueueSize);
	
	
	m_fInited = TRUE;
	SW_TRACE_DEBUG("---tobedeleted : init ok\n");

	return S_OK;
}



HRESULT CSWResultVideoTransformFilter::Run()
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not init yet\n");
		return E_FAIL;
	}
	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_DEBUG("Err: Failed to run this filter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThread.Start(AddVideoProxy, this)))
	{
		SW_TRACE_DEBUG("Err: failed to start AddVideo thread\n");
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CSWResultVideoTransformFilter::Stop()
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not init yet\n");
		return E_FAIL;
	}
	
	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_DEBUG("Err: Failed to stop this filter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThread.Stop()))
	{
		SW_TRACE_DEBUG("Err: failed to stop AddVideo thread\n");
		return E_FAIL;
	}

	return S_OK;
}



PVOID CSWResultVideoTransformFilter::AddVideoProxy(PVOID pvArg)
{
	if (NULL != pvArg)
	{
		CSWResultVideoTransformFilter* pThis = (CSWResultVideoTransformFilter*)pvArg;
		pThis->AddVideo();
	}
}


HRESULT CSWResultVideoTransformFilter::AddVideo()
{
	SW_TRACE_DEBUG("Info: running...\n");
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not init yet\n");
		return E_FAIL;
	}
	
	while (FILTER_RUNNING == GetState())
	{
		if (FAILED(m_cSemaQueueSync.Pend(1000)))
		{
			continue;
		}
		
		m_cRecordLock.Pend();
		DWORD dwCount = m_lstRecord.GetCount();
		SW_POSITION pos = m_lstRecord.GetHeadPosition();
		m_cRecordLock.Post();

		SW_TRACE_DEBUG("-----tobedeleted dwCount = %d\n", dwCount);

		while (dwCount > 0)
		{
			SW_POSITION prevpos = pos;
			m_cRecordLock.Pend();
			if (!m_lstRecord.IsValid(pos))
			{
				pos = m_lstRecord.GetHeadPosition();
				prevpos = pos;
			}
			CSWRecord *pRecord = (CSWRecord*)m_lstRecord.GetNext(pos);
			m_cRecordLock.Post();
			
			if (NULL != pRecord)
			{
				DWORD dwCurTick = CSWDateTime::GetSystemTick();				
				if (dwCurTick < pRecord->GetRefTime() + 5000)
				{
					swpa_thread_sleep_ms(200);
					continue;
				}
					
				CSWImage * pVideo = NULL;
				DWORD dwVideoCount = pRecord->GetVideoCount();
				DWORD dwStartTick = 0;
				if (0 == dwVideoCount)
				{
					dwStartTick = (pRecord->GetRefTime()/1000)*1000 - 5000;
				}
				else
				{
					pRecord->GetVideo(dwVideoCount-1, &pVideo);
					dwStartTick = (pVideo->GetRefTime()/1000)*1000 + 1000;
				}

				SW_TRACE_DEBUG("-----tobedeleted record(%x) dwVideoCount = %d, tick = %d\n", pRecord, dwCount, dwStartTick);
				
				BOOL fOk = TRUE;
				for (DWORD i=dwStartTick, dwID = dwVideoCount; dwID<10; i+=1000)
				{	
					pVideo = NULL;
					pRecord->GetVideo(dwID, &pVideo);
					if (NULL == pVideo)
					{
						if (FAILED(CSWMessage::SendMessage(MSG_H264_QUEUE_GET_VIDEO, WPARAM(i), LPARAM(&pVideo)))
							|| (NULL == pVideo))
						{
							//try to get the next one
							if (FAILED(CSWMessage::SendMessage(MSG_H264_QUEUE_GET_VIDEO, WPARAM(i+1000), LPARAM(&pVideo)))
								|| (NULL == pVideo))
							{
								dwCurTick = CSWDateTime::GetSystemTick();

								if (dwCurTick > i + 30000) //no video available any more, so output the record immediately
								{
									SW_TRACE_NORMAL("Warning: can not get video file #%d, send the incompletable record.\n");
									break;
								}
								else if (dwCurTick < i)
								{
									fOk = FALSE;
									break; //try to get it next loop
								}
								else
								{
									fOk = FALSE;
									SW_TRACE_DEBUG("Err: failed to get video, skip this one\n");
									continue; //skip this one
								}
							}
							else
							{
								i += 1000;
							}
						}

						if (NULL != pVideo)
						{
							pRecord->SetVideo(dwID, pVideo);
							dwID ++;
						}
					}
					swpa_thread_sleep_ms(100);
				}

				if (fOk)
				{
					m_cRecordLock.Pend();
					m_lstRecord.RemoveAt(prevpos);
					m_cRecordLock.Post();

					SW_TRACE_DEBUG("-----tobedeleted deliver record(%x) \n", pRecord);
					Deliver(pRecord);
					SAFE_RELEASE(pRecord);
					
					dwCount --;
				}
			}
			else
			{
				m_cRecordLock.Pend();
				m_lstRecord.RemoveAt(prevpos);
				m_cRecordLock.Post();

				dwCount --;
			}
			swpa_thread_sleep_ms(100);
		}	
	}

	SW_TRACE_DEBUG("Info: exited...\n");
	return S_OK;
}




HRESULT CSWResultVideoTransformFilter::Receive(CSWObject * pObj)
{
	
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	if (IsDecendant(CSWRecord, pObj))
	{
		SW_TRACE_DEBUG("Info: received a record\n");

		CSWRecord * pRecord = (CSWRecord *)pObj;
		if (!pRecord->IsNormal())
		{
			m_cRecordLock.Pend();
			if (m_lstRecord.IsFull())
			{
				SW_TRACE_DEBUG("Warning: record queue is full_%d, discards this record.\n", m_lstRecord.GetCount());
				//CSWRecord * pRecord = (CSWRecord *)m_lstRecord.RemoveHead();
				//SAFE_RELEASE(pRecord);

				Deliver(pRecord);
			}
			else
			{
				SAFE_ADDREF(pRecord);
				m_lstRecord.AddTail(pRecord);
				m_cSemaQueueSync.Post();
			}
			m_cRecordLock.Post();
		}
		else
		{
			Deliver(pRecord);
		}

	}

	return S_OK;
}





