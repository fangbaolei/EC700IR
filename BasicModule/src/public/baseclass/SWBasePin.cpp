///////////////////////////////////////////////////////////
//  CSWBasePin.cpp
//  Implementation of the Class CSWBasePin
//  Created on:      27-二月-2013 16:29:45
//  Original author: huanggc
///////////////////////////////////////////////////////////
#include "SWFC.h"
#include "SWBasePin.h"
#include "SWBaseFilter.h"
#include "SWBaseGraph.h"


/**
 * @brief 构造函数，需要输入参数指定是输入还是输出Pin
 * @param [in] pFilter Filter指针
 * @param [in] dwDir 方向，0表示输入, 1表示输出
 */
CSWBasePin::CSWBasePin(CSWBaseFilter * pFilter, DWORD dwDir)
:m_pFilter(pFilter)
,m_dwDirection(dwDir)
,m_pIn(NULL)
,m_State(PIN_NONE)
{
}


CSWBasePin::~CSWBasePin(){

}

/**
 * @brief 添加此接口的数据格式，只有符合此格式的数据单元才能经过此接口
 * @param [in] dwObjectID 对象的id号
 * @return 成功返回S_OK,其他值为错误代码
 */
HRESULT CSWBasePin::AddObject(DWORD dwObjectID){

	m_lstObjectID.AddTail(dwObjectID);
	return S_OK;
}	


/**
 * @brief 连接另一个Pin,这里要求只有输出能够连接输入,其他的连接方式都是不允许的
 * @param [in] pIn 输入
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBasePin::Connect(CSWBasePin * pIn){

	if(NULL != pIn
	&& NULL == m_pIn
	&& 1 == m_dwDirection
	&& 0 == pIn->m_dwDirection
	&& GetFilter() != pIn->GetFilter()
	&& GetFilter()->GetGraph() == pIn->GetFilter()->GetGraph())
	{
		DWORD dwOutID, dwInID;
		SW_POSITION posOut = m_lstObjectID.GetHeadPosition(); 
		while(m_lstObjectID.IsValid(posOut))
		{ 
			dwOutID = m_lstObjectID.GetNext(posOut);
			SW_POSITION posIn = pIn->m_lstObjectID.GetHeadPosition();
			while(pIn->m_lstObjectID.IsValid(posIn))
			{
				dwInID = pIn->m_lstObjectID.GetNext(posIn);
				if(dwOutID == dwInID)
				{
					m_lstConnectID.AddTail(dwInID);
				}
			}
		}
		if(!m_lstConnectID.IsEmpty())
		{
			m_pIn = pIn;
			pIn->m_pIn = this;
			return S_OK;
		}
	}
	return E_FAIL;
}


/**
 * @brief 传送数据
 * @param [in] pdu数据协议单元
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBasePin::Deliver(CSWObject *obj){

	if(NULL != m_pIn && 0 == m_pIn->m_dwDirection && NULL != obj)
	{
		SW_POSITION posOut = m_lstConnectID.GetHeadPosition(); 
		while(m_lstConnectID.IsValid(posOut))
		{ 
			INT id = m_lstConnectID.GetNext(posOut);
			if(obj->IsDescendant(id))
			{
				return m_pIn->Receive(obj);
			}
		}		
	}
	return E_FAIL;
}


/**
 * @brief 获得Filter指针
 * @return 成功返回Filter指针，失败返回NULL
 */
CSWBaseFilter* CSWBasePin::GetFilter(){

	return  m_pFilter;
}


/**
 * @brief 获得连接此Pin的另一个Pin
 * @return 成功返回Pin指针，失败返回NULL
 */
CSWBasePin* CSWBasePin::GetPin(){

	return  m_pIn;
}


/**
 * @brief 返回Pin的状态
 * @return 返回Pin的状态
 */
PIN_STATE CSWBasePin::GetState(){

	return  m_State;
}


/**
 * @brief 暂停Pin
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBasePin::Pause(){

	m_State = PIN_PAUSED;
	return S_OK;
}


/**
 * @brief 接收输入的数据包
 * @param [in] pdu 接收数据包
 * @成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBasePin::Receive(CSWObject *obj){

	return  GetFilter() ? GetFilter()->Receive(obj) : E_FAIL;
}


/**
 * @brief 运行Pin
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBasePin::Run(){

	m_State = PIN_RUNNING;
	return S_OK;
}


/**
 * @brief 停止Pin
 * @return 成功返回S_OK，其他值为错误代码
 */
HRESULT CSWBasePin::Stop(){

	m_State = PIN_STOPPED;
	return S_OK;
}

