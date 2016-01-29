///////////////////////////////////////////////////////////
//  CSWBaseGraph.h
//  Implementation of the Class CSWBaseGraph
//  Created on:      27-二月-2013 16:29:44
//  Original author: huanggc
///////////////////////////////////////////////////////////

#if !defined(EA_4086695D_55CD_4dc6_859F_96CE104EEE33__INCLUDED_)
#define EA_4086695D_55CD_4dc6_859F_96CE104EEE33__INCLUDED_

#include "SWObject.h"
#include "SWList.h"

/**
 * @brief AM管理类,主要管理Filter的连接、运行、停止、查询等等操作
 * @brief 黄国超编写
 * @brief 2013-02-27创建此类
 */
class CSWBaseGraph;
class CSWBaseFilter;
class CSWBasePin;

class CSWBaseGraph : public CSWObject
{
	CLASSINFO(CSWBaseGraph,CSWObject)
	friend class CSWBaseFilter;
	friend class CSWBasePin;
public:
	CSWBaseGraph();
	virtual ~CSWBaseGraph();
	/**
	 *@brief 取得数据流的版本号
	 *@return 成功返回版本号失败返回-1
	 */
	static INT GetVersion(VOID);
	/**
	 * @brief 添加一个Filter，只有添加Filter到AM去才能实现连接
	 * @param [in] pszName 指定Filter的名称
	 * @param [in] pFilter Filter对象
	 * @return 成功返回S_OK,其他值代表错误代码
	 */
	virtual HRESULT AddFilter(LPCSTR pszName, CSWBaseFilter* pFilter);
	/**
	 * @brief 连接2个Pin
	 * @param [in] pOut输出Pin
	 * @param [in] pIn 输入Pin
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Connect(CSWBasePin* pOut, CSWBasePin* pIn);
	/**
   * @brief 响应旗下的Filter的命令事件
   * @param [in] pFilter 触发此事件的Filter
   * @param [in] dwID  dwID
   * @param [in] wParam 命令参数1
   * @param [in] lParam 命令参数2
   * @return 协议自定义返回类型
   */
	virtual HRESULT OnCommand(CSWBaseFilter* pFilter, DWORD dwID, DWORD wParam, DWORD lParam);
	/**
	 * @brief 暂停整个Filter的运行，里面的实现是调用了旗下所有Filter的Pause函数
	 * @return 成功返回S_OK，其他值代表错误代码
	 */
	virtual HRESULT Pause();
	/**
	 * @brief 查询AM下是否有此Filter,通过pszName查询
	 * @param [in] pszName Filter的名称
	 * @param [out] pFilter  保存查询到的Filter
	 * @return 成功返回S_OK,其他值为错误代码
	 */
	virtual CSWBaseFilter* QueryFilter(LPCSTR pszName);
	/**
	 * @brief 删除Filter
	 * @param [in] pszName Filter名称
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT RemoveFilter(LPCSTR lpzName);
	/**
	 * @brief  运行所有的Filter,其内部实现是调用了所有Filter的Run函数
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Run();
	/**
	 * @brief  停止所有的Filter,其内部实现是调用了所有Filter的Stop函数
	 * @return 成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Stop();
	
	/**
	 * @brief 查询各个Filter的状态
	 * @return 返回Filter的状态
	 */
	virtual FILTER_STATE GetState();

protected:
	CSWList<CSWBaseFilter *>m_lstFilter;
};
#endif // !defined(EA_4086695D_55CD_4dc6_859F_96CE104EEE33__INCLUDED_)

