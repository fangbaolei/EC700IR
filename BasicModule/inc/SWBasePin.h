///////////////////////////////////////////////////////////
//  CSWBasePin.h
//  Implementation of the Class CSWBasePin
//  Created on:      27-二月-2013 16:29:45
//  Original author: huanggc
///////////////////////////////////////////////////////////

#if !defined(EA_D326A1EB_5AD6_445a_90BC_65F4369EF27C__INCLUDED_)
#define EA_D326A1EB_5AD6_445a_90BC_65F4369EF27C__INCLUDED_

#include "SWObject.h"

/**
 * @brief 输入输出类
 */
class CSWBaseGraph;
class CSWBaseFilter;
class CSWBasePin;

enum PIN_STATE{PIN_NONE, PIN_RUNNING, PIN_PAUSED, PIN_STOPPED};
enum FILTER_STATE{FILTER_NONE, FILTER_RUNNING, FILTER_PAUSED, FILTER_STOPPED};

class CSWBasePin : virtual public CSWObject
{
	CLASSINFO(CSWBasePin,CSWObject)
	friend class CSWBaseGraph;
	friend class CSWBaseFilter;
public:	
	/**
	 * @brief 构造函数，需要输入参数指定是输入还是输出Pin
	 * @param [in] pFilter Filter指针
	 * @param [in] dwDir 方向，0表示输出，1表示输入
	 */
	CSWBasePin(CSWBaseFilter* pFilter, DWORD dwDir);
	virtual ~CSWBasePin();
	/**
	 * @brief 添加此接口的数据格式，只有符合此格式的数据单元才能经过此接口
	 * @param [in] dwFormat 数据协议单元格式
   * @return 成功返回S_OK,其他值为错误代码
	 */
	virtual HRESULT AddObject(DWORD dwObjectID);
	/**
	 * @brief 连接另一个Pin,这里要求只有输出能够连接输入,其他的连接方式都是不允许的
	 * @param [in] pIn 输入
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Connect(CSWBasePin* pIn);
	/**
	 * @brief 传送数据
	 * @param [in] pdu数据协议单元
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Deliver(CSWObject* obj);
	/**
	 * @brief 获得Filter指针
	 * @return 成功返回Filter指针，失败返回NULL
	 */
	virtual CSWBaseFilter* GetFilter();
	/**
	 * @brief 获得连接此Pin的另一个Pin
	 * @return 成功返回Pin指针，失败返回NULL
	 */
	virtual CSWBasePin* GetPin();
	/**
	 * @brief 返回Pin的状态
	 * @return 返回Pin的状态
	 */
	virtual PIN_STATE GetState();
	/**
	 * @brief 暂停Pin
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Pause();
	/**
	 * @brief 接收输入的数据包
	 * @param [in] pdu 接收数据包
	 * @成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Receive(CSWObject* obj);
	/**
	 * @brief 运行Pin
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Run();
	/**
	 * @brief 停止Pin
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Stop();

private:
	PIN_STATE m_State;
	DWORD m_dwDirection;
	CSWList<DWORD>m_lstObjectID;
	CSWList<DWORD>m_lstConnectID;
	CSWBaseFilter* m_pFilter;
	CSWBasePin* m_pIn;	
};
#endif // !defined(EA_D326A1EB_5AD6_445a_90BC_65F4369EF27C__INCLUDED_)

