///////////////////////////////////////////////////////////
//  CSWBaseFilter.h
//  Implementation of the Class CSWBaseFilter
//  Created on:      27-二月-2013 16:29:44
//  Original author: huanggc
///////////////////////////////////////////////////////////

#if !defined(EA_F720ABF8_1CAB_4d3e_A68C_33C28371966D__INCLUDED_)
#define EA_F720ABF8_1CAB_4d3e_A68C_33C28371966D__INCLUDED_

#include "SWBasePin.h"
#include "SWBaseGraph.h"
#include "SWClassFactory.h"
/**
 * @brief 功能模块Filter的基类，实现了模块间的连接、数据发送等等
 */
 
class CSWBaseGraph;
class CSWBaseFilter;
class CSWBasePin;

class CSWBaseFilter : public CSWObject
{
	CLASSINFO(CSWBaseFilter,CSWObject)
	friend class CSWBaseGraph;
	friend class CSWBasePin;
public:
	/**
	 * @brief 构造函数，需要指定此Filter输入输出的个数
	 */
	CSWBaseFilter(DWORD dwInCount, DWORD dwOutCount);
	virtual ~CSWBaseFilter();
	/**
	 *@brief 模块的初始化函数
	 *@param [in] pvParam 模块参数结构体指针
	 *@return 成功返回S_OK，其他值为错误代码
	 */
	 virtual HRESULT Initialize(PVOID pvParam);
	/**
	 * @brief 设置Filter的名称
	 * @param [in] pszName 设置Filter的名称
	 * @return S_OK表示成功,其他值为错误代码
	*/
	virtual HRESULT SetName(LPCSTR pszName);
	/**
	 * @brief 获取Filter的名称
	 * @return 成功返回Filter的名称，失败返回NULL
	*/
	virtual LPCSTR GetName(void);
	/**
	 * @brief 传送数据
	 * @param [in] pdu数据协议单元
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Deliver(CSWObject* obj);
	/**
	 * @brief 获得AM的指针
	 * @return 返回AM指针
	 */
	virtual CSWBaseGraph* GetGraph();
	/**
	 * @brief 获得输入的指针
	 * @param [in] n 第几个输入
	 * @return 成功返回输入指针，错误返回NULL
	 */
	virtual CSWBasePin* GetIn(DWORD n);
	/**
	 * @brief 获得输入的总数
	 * @return 返回输入的总数
	 */
	virtual DWORD GetInCount();	
	/**
	 *@brief 创建输入Pin
	 *@param [in] n第几个输入
	 *@return 返回输入Pin指针
	 */
	virtual CSWBasePin* CreateIn(DWORD n);
	/**
	 * @brief 获得输出的指针
	 * @param [in] n 第几个输出
	 * @return 成功返回输出指针，错误返回NULL
	 */
	virtual CSWBasePin* GetOut(DWORD n);
	/**
	 * @brief 获得输出的总数
	 * @return 返回输出的总数
	 */
	virtual DWORD GetOutCount();
	/**
	 *@brief 创建输出Pin
	 *@param [in] n第几个输出
	 *@return 返回输入Pin指针
	 */
	virtual CSWBasePin* CreateOut(DWORD n);
	/**
	 * @brief 返回Filter的状态
	 */
	virtual FILTER_STATE GetState();
protected:
	/**
	 * @brief 暂停Filter
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
	 * @brief 运行Filter
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Run();
	/**
	 * @brief 调用AM发送命令
	 * @param [in] dwID  dwID
	 * @param [in] wParam 命令参数1
	 * @param [in] lParam 命令参数2
	 * @return 协议自定义返回类型
	 */
	virtual HRESULT SendCommand(DWORD dwID, DWORD wParam = 0, DWORD lParam = 0);
	/**
	 * @brief 停止Filter
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Stop();
	
private:
	FILTER_STATE m_fsState;
	DWORD m_dwInCount;
	DWORD m_dwOutCount;
	CSWString m_strName;
	CSWBaseGraph *m_pGraph;
	CSWBasePin  **m_pIn;
	CSWBasePin  **m_pOut;
};
#endif // !defined(EA_F720ABF8_1CAB_4d3e_A68C_33C28371966D__INCLUDED_)

