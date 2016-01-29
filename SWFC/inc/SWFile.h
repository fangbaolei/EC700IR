#ifndef CSWFILE_H
#define CSWFILE_H

#include "SWObject.h"

/**
 * @brief 文件管理基类
 */
class CSWFile : public CSWObject
{
CLASSINFO(CSWFile,CSWObject)
public:
	/**
	 * @brief 构造函数
	 */
	CSWFile();
	/**
	 * @brief 构造函数
	 * @param [in] szFileName : 被打开的文件名，包含设备文件和一般的磁盘文件
	 * @param [in] szMode : 文件打开模式
	 */
	CSWFile(PCSTR szFileName, PCSTR szMode);
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWFile();
	
	
	/**
	 * @brief 打开一个文件
	 *
	 * @param [in] szFileName : 文件名
	 * @param [in] szMode : 打开模式
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Open(PCSTR szFileName, PCSTR szMode);
	/**
	 * @brief 关闭文件
	 *
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Close(void);
	
	/**
	 * @判断文件是否打开
	 * @return 有效则返回S_OK，否则返回E_FAIL;
	 */
	virtual HRESULT IsValid(VOID){return 0 != m_iFileHandle ? S_OK : E_FAIL;}
		
	/**
	 * @brief 获得文件的大小
	 *
	 * @param [out] pdwSize : 文件的大小
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT GetSize(DWORD * pdwSize);
	/**
	 * @brief 用于向设备发控制和配置命令
	 *
	 * @param [in] dwCmd : 控制命令ID
	 * @param [in] pvArg : 控制命令参数指针
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT IOCtrl(DWORD dwCmd, VOID * pvArg);
	/**
	 * @brief 文件读取函数
	 *
	 * @param [in] pvBuf : 存放读取结果的缓冲区
	 * @param [in] dwCB : 存放读取结果的缓冲区的大小
	 * @param [out] pdwRead : 实际读取到的字节数
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT Read(PVOID pvBuf, DWORD dwCB, PDWORD pdwRead=NULL);
	/**
	 * @brief 重定位文件内部位置指针
	 *
	 * @param [in] sdwOffset : 偏移量
	 * @param [in] dwFromWhere : 相对起始位置，0:文件头(SEEK_SET)，1:当前位置(SEEK_CUR)，2:
	 * 文件尾(SEEK_END)
	 * @param [out] pdwNewPos : 新位置的内部文件指针值
	 * @return
	 * - S_OK : 成功
	 * - E_FAIL : 失败
	 */
	virtual HRESULT Seek(INT iOffset, DWORD dwFromWhere, DWORD * pdwNewPos=NULL);
	/**
	 * @brief 文件写入函数
	 *
	 * @param [in] pvBuf : 存放要写入数据的的缓冲区
	 * @param [in] dwCB : 写入数据的的大小
	 * @param [out] pdwWritten : 实际写入的字节数
	 * @return
	 * - S_OK : 成功
	 * _ E_FAIL : 失败
	 */
	virtual HRESULT Write(PVOID pvBuf, DWORD dwCB, PDWORD pdwWritten=NULL);

private:
	// 是否是有效的文件句柄。
	BOOL IsValid(INT iFileHandle);
private:
	INT	m_iFileHandle;

};

#endif // SWFILE_H


