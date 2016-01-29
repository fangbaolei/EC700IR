 ///////////////////////////////////////////////////////////
//  CSWLog.h
//  Implementation of the Class CSWLog
//  Created on:      28-二月-2013 14:09:51
//  Original author: zhouy
///////////////////////////////////////////////////////////

#if !defined(EA_4EA928BD_1800_4402_937B_2B9BF062CA08__INCLUDED_)
#define EA_4EA928BD_1800_4402_937B_2B9BF062CA08__INCLUDED_

#include "SWObject.h"
#include "SWSemaphore.h"

#define SW_LOG_MAX_LEN		(4096 + 64)

//日志等级：错误级 < 普通级 < 操作级 < 调试级
#define SW_LOG_LV_ERROR   0
#define SW_LOG_LV_NORMAL  1
#define SW_LOG_LV_OPERATE 2
#define SW_LOG_LV_DEBUG   3
#define SW_LOG_LV_DSP     0x10 // !! internal use only   

/**
 * @brief 日志输出模块
 */
class CSWLog : public CSWObject
{
CLASSINFO(CSWLog,CSWObject)

public:
	CSWLog();
	virtual ~CSWLog();

	/**
	 * @brief 增加引用计数
	 * 
	 * @note
	 * - 实现为空
	 */
	DWORD AddRef();
	
	/**
	 * @brief 增加日志输出的等级
	 * @param [in] dwLevel 日志输出等级
	 */
	static VOID SetLevel(DWORD dwLevel);
	static DWORD GetLevel(VOID){return m_dwLevel;}
	
    /**
	 * @brief 输出日志
	 * 
	 * @param [in] szSrcFile : 日志来源文件
     * @param [in] szSrcLine : 日志来源文件中的具体行数
	 * @param [in] dwLevel   : 日志等级
	 * @param [in] szFormat  : 日志字串格式
	 * @param [in] ... : 可变参数列表
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	static HRESULT Print(
        const char* szSrcFile,
        int iSrcLine,
        DWORD dwLevel,
        const char* szFormat, ...);
	
    /**
	 * @brief 减少引用计数并在引用计数为0时释放对象
	 * 
	 * @note
	 * - 实现为空
	 */
	HRESULT Release();
private:
	
	static HRESULT SendLogViaSocket(CHAR * szLog, DWORD dwLogLen);

	static DWORD m_dwLevel;
	static CSWSemaphore m_cSemaLock;
};

#define SW_TRACE(level, format, ...) CSWLog::Print(__FILE__, __LINE__, level, format, ## __VA_ARGS__)

#define SW_TRACE_ERROR(format, ...)   SW_TRACE(SW_LOG_LV_ERROR,   format, ## __VA_ARGS__)
#define SW_TRACE_NORMAL(format, ...)  SW_TRACE(SW_LOG_LV_NORMAL,  format, ## __VA_ARGS__)
#define SW_TRACE_OPERATE(format, ...) SW_TRACE(SW_LOG_LV_OPERATE, format, ## __VA_ARGS__)
#define SW_TRACE_DEBUG(format, ...)   SW_TRACE(SW_LOG_LV_DEBUG,   format, ## __VA_ARGS__)
#define SW_TRACE_DSP(format, ...)     SW_TRACE(SW_LOG_LV_DSP,     format, ## __VA_ARGS__)
#endif // !defined(EA_4EA928BD_1800_4402_937B_2B9BF062CA08__INCLUDED_)

