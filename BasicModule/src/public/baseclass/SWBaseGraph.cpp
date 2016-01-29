///////////////////////////////////////////////////////////
//  CSWBaseGraph.cpp
//  Implementation of the Class CSWBaseGraph
//  Created on:      27-二月-2013 16:29:45
//  Original author: huanggc
///////////////////////////////////////////////////////////
#include "swpa.h"
#include "SWFC.h"
#include "SWBasePin.h"
#include "SWBaseFilter.h"
#include "SWBaseGraph.h"


CSWBaseGraph::CSWBaseGraph(){

}


CSWBaseGraph::~CSWBaseGraph(){

	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		m_lstFilter.GetNext(pos)->Release();
	}
}

INT CSWBaseGraph::GetVersion(VOID)
{
#ifdef VERSION
	return VERSION;
#else
	return -1;
#endif
}

/**
 * @brief 添加一个Filter，只有添加Filter到AM去才能实现连接
 * @param [in] pszName 指定Filter的名称
 * @param [in] pFilter Filter对象
 * @return 成功返回S_OK,其他值代表错误代码
 */
HRESULT CSWBaseGraph::AddFilter(LPCSTR pszName, CSWBaseFilter* pFilter){

	if(NULL != pszName && NULL != pFilter)
	{
		CSWBaseFilter* pFindFilter = NULL;
		if(NULL != (pFindFilter = QueryFilter(pszName)))
		{
			pFindFilter->Release();
			return E_FAIL;
		}
			
		pFilter->AddRef();
		pFilter->SetName(pszName);
		pFilter->m_pGraph = this;
		m_lstFilter.AddTail(pFilter);
		
		return S_OK;
	}
	return E_FAIL;
}


/**
 * @brief 连接2个Pin
 * @param [in] pOut输出Pin
 * @param [in] pIn 输入Pin
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseGraph::Connect(CSWBasePin* pOut, CSWBasePin* pIn){

	if(NULL != pOut
	&& NULL != pIn
	&& NULL != pOut->GetFilter()
	&& NULL != pIn->GetFilter()
	&& this == pOut->GetFilter()->GetGraph()
	&& this == pIn->GetFilter()->GetGraph())
	{
		return pOut->Connect(pIn);
	}
	return E_FAIL;
}


/**
 * @brief 响应旗下的Filter的命令事件
 * @param [in] pFilter 触发此事件的Filter
 * @param [in] dwID  dwID
 * @param [in] wParam 命令参数1
 * @param [in] lParam 命令参数2
 * @return 协议自定义返回类型
 */
HRESULT CSWBaseGraph::OnCommand(CSWBaseFilter* pFilter, DWORD dwID, DWORD wParam, DWORD lParam){

	return  E_NOTIMPL;
}


/**
 * @brief 暂停整个Filter的运行，里面的实现是调用了旗下所有Filter的Pause函数
 * @return 成功返回S_OK，其他值代表错误代码
 */
HRESULT CSWBaseGraph::Pause(){

	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		m_lstFilter.GetNext(pos)->Pause();
	}
	return S_OK;
}


/**
 * @brief 查询AM下是否有此Filter,通过pszName查询
 * @param [in] pszName Filter的名称
 * @param [out] pFilter  保存查询到的Filter
 * @return 成功返回S_OK,其他值为错误代码
 */
CSWBaseFilter* CSWBaseGraph::QueryFilter(LPCSTR pszName){

	CSWBaseFilter* pFindFilter = NULL;
	bool fOk = false;
	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		pFindFilter = m_lstFilter.GetNext(pos);
		if(!swpa_strcmp(pFindFilter->GetName(), pszName))
		{
			pFindFilter->AddRef();
			fOk = true;
			break;
		}
	}
	return fOk ? pFindFilter : NULL;
}


/**
 * @brief 删除Filter
 * @param [in] pszName Filter名称
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseGraph::RemoveFilter(LPCSTR lpzName){

	CSWBaseFilter* pFindFilter = NULL;
	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		SW_POSITION posNow = pos;
		pFindFilter = m_lstFilter.GetNext(pos);
		if(!swpa_strcmp(pFindFilter->GetName(), lpzName))
		{
			pFindFilter->Release();
			m_lstFilter.RemoveAt(posNow);
			return S_OK;
		}
	}
	return E_FAIL;
}


/**
 * @brief  运行所有的Filter,其内部实现是调用了所有Filter的Run函数
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseGraph::Run(){

	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		m_lstFilter.GetNext(pos)->Run();
	}
	return S_OK;
}


/**
 * @brief  停止所有的Filter,其内部实现是调用了所有Filter的Stop函数
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseGraph::Stop(){

	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		CSWBaseFilter *pFilter = m_lstFilter.GetNext(pos);
		SW_TRACE_NORMAL("stop %s...\n", pFilter->GetName());
		pFilter->Stop();
	}
	return S_OK;
}

FILTER_STATE CSWBaseGraph::GetState()
{
	FILTER_STATE fs = FILTER_RUNNING;
	SW_POSITION pos = m_lstFilter.GetHeadPosition();
	while(m_lstFilter.IsValid(pos))
	{
		CSWBaseFilter *pFilter = m_lstFilter.GetNext(pos);
		if(FILTER_RUNNING != pFilter->GetState())
		{
			SW_TRACE_NORMAL("%s error.\n", pFilter->GetName());
			fs = pFilter->GetState();
		}
	}
	return fs;
}

