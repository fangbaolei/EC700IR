///////////////////////////////////////////////////////////
//  CSWBaseFilter.cpp
//  Implementation of the Class CSWBaseFilter
//  Created on:      27-二月-2013 16:29:44
//  Original author: huanggc
///////////////////////////////////////////////////////////
#include "swpa.h"
#include "SWFC.h"
#include "SWBasePin.h"
#include "SWBaseFilter.h"
#include "SWBaseGraph.h"

/**
 * @brief 构造函数，需要指定此Filter输入输出的个数
 */
CSWBaseFilter::CSWBaseFilter(DWORD dwInCount, DWORD dwOutCount)
:m_dwInCount(dwInCount)
,m_dwOutCount(dwOutCount)
,m_pGraph(NULL)
,m_fsState(FILTER_NONE)
{
	m_pIn = new CSWBasePin*[GetInCount()];
	for(DWORD i = 0; i < GetInCount(); i++)
	{
		m_pIn[i] = NULL;
	}
	
	m_pOut = new CSWBasePin*[GetOutCount()];
	for(DWORD i = 0; i < GetOutCount(); i++)
	{
		m_pOut[i] = NULL;
	}
}


CSWBaseFilter::~CSWBaseFilter(){

	for(DWORD i = 0; i < GetInCount(); i++)
	{
		if(m_pIn[i])
		{
			m_pIn[i]->Release();
		}
	}
	delete []m_pIn;
	
	for(DWORD i = 0; i < GetOutCount(); i++)
	{
		if(m_pOut[i])
		{
			m_pOut[i]->Release();
		}
	}
	delete []m_pOut;
}

/**
 *@brief 模块的初始化函数
 *@param [in] pvParam 模块参数结构体指针
 *@return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseFilter::Initialize(PVOID pvParam)
{
	return E_NOTIMPL;
}

/**
 * @brief 设置Filter的名称
 * @param [in] pszName 设置Filter的名称
 * @return S_OK表示成功,其他值为错误代码
 */
HRESULT CSWBaseFilter::SetName(LPCSTR pszName)
{
	m_strName = "";
	if(pszName && swpa_strlen(pszName) > 0)
	{
		m_strName = pszName;
	}
	return S_OK;
}


/**
 * @brief 获取Filter的名称
 * @return 成功返回Filter的名称，失败返回NULL
 */
LPCSTR CSWBaseFilter::GetName(void)
{
	return m_strName;
}
	
	
/**
 * @brief 传送数据
 * @param [in] pdu数据协议单元
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseFilter::Deliver(CSWObject* obj){

	for(DWORD i = 0; i < GetOutCount(); i++)
	{
		GetOut(i)->Deliver(obj);
	}
	return S_OK;
}


/**
 * @brief 获得AM的指针
 * @return 返回AM指针
 */
CSWBaseGraph* CSWBaseFilter::GetGraph(){

	return  m_pGraph;
}


/**
 * @brief 获得输入的指针
 * @param [in] n 第几个输入
 * @return 成功返回输入指针，错误返回NULL
 */
CSWBasePin* CSWBaseFilter::GetIn(DWORD n){

	if(n < GetInCount())
	{
		if(NULL == m_pIn[n])
		{
			m_pIn[n] = CreateIn(n);
		}
		return m_pIn[n];
	}
	return NULL;
}


/**
 * @brief 获得输入的总数
 * @return 返回输入的总数
 */
DWORD CSWBaseFilter::GetInCount(){

	return  m_dwInCount;
}

/**
 *@brief 创建输入Pin
 *@param [in] n第几个输入
 *@return 返回输入Pin指针
 */
CSWBasePin* CSWBaseFilter::CreateIn(DWORD n)
{
	return new CSWBasePin(this, 0);
}


/**
 * @brief 获得输出的指针
 * @param [in] n 第几个输出
 * @return 成功返回输出指针，错误返回NULL
 */
CSWBasePin* CSWBaseFilter::GetOut(DWORD n){

	if(n < GetOutCount())
	{
		if(NULL == m_pOut[n])
		{
			m_pOut[n] = CreateOut(n);
		}
		return m_pOut[n];
	}
	return NULL;
}


/**
 * @brief 获得输出的总数
 * @return 返回输出的总数
 */
DWORD CSWBaseFilter::GetOutCount(){

	return  m_dwOutCount;
}

/**
 *@brief 创建输出Pin
 *@param [in] n第几个输出
 *@return 返回输入Pin指针
 */
CSWBasePin* CSWBaseFilter::CreateOut(DWORD n)
{
	return new CSWBasePin(this, 1);
}	
/**
 * @brief 返回Filter的状态
 */
FILTER_STATE CSWBaseFilter::GetState(){

	if(m_fsState == FILTER_RUNNING)
	{
		PIN_STATE bsState = PIN_RUNNING;
		for(DWORD i = 0; i < GetInCount() && bsState == PIN_RUNNING; i++)
		{	
			if(bsState != GetIn(i)->GetState())
			{
				bsState = GetIn(i)->GetState();
				break;
			}
		}
	
		for(DWORD i = 0; i < GetOutCount() && bsState == PIN_RUNNING; i++)
		{
			if(bsState != GetOut(i)->GetState())
			{
				bsState = GetOut(i)->GetState();
				break;
			}
		}
		switch(bsState)
		{
		case PIN_PAUSED  : m_fsState = FILTER_PAUSED; break;
		case PIN_STOPPED : m_fsState = FILTER_STOPPED;break;
		}
	}
	return  m_fsState;
}


/**
 * @brief 暂停Filter
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseFilter::Pause(){

	m_fsState = FILTER_PAUSED;
	for(DWORD i = 0; i < GetInCount(); i++)
	{
		GetIn(i)->Pause();
	}
	
	for(DWORD i = 0; i < GetOutCount(); i++)
	{
		GetOut(i)->Pause();
	}	
	return S_OK;
}


/**
 * @brief 接收输入的数据包
 * @param [in] pdu 接收数据包
 * @成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseFilter::Receive(CSWObject* obj){

	return Deliver(obj);
}


/**
 * @brief 运行Filter
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseFilter::Run(){

	m_fsState = FILTER_RUNNING;
	for(DWORD i = 0; i < GetInCount(); i++)
	{
		GetIn(i)->Run();
	}
	
	for(DWORD i = 0; i < GetOutCount(); i++)
	{
		GetOut(i)->Run();
	}
	return S_OK;
}


/**
 * @brief 调用AM发送命令
 * @param [in] dwID  dwID
 * @param [in] wParam 命令参数1
 * @param [in] lParam 命令参数2
 * @return 协议自定义返回类型
 */
HRESULT CSWBaseFilter::SendCommand(DWORD dwID, DWORD wParam, DWORD lParam){

	return  GetGraph() ? GetGraph()->OnCommand(this, dwID, wParam, lParam) : E_NOTIMPL;
}


/**
 * @brief 停止Filter
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBaseFilter::Stop(){

	m_fsState = FILTER_STOPPED;
	for(DWORD i = 0; i < GetInCount(); i++)
	{
		GetIn(i)->Stop();
	}
	
	for(DWORD i = 0; i < GetOutCount(); i++)
	{
		GetOut(i)->Stop();
	}
	return S_OK;
}

