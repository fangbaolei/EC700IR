/**
* @file SWNetCommandProcess .h 
* @brief 网络命令处理模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-01
* @version 1.0
*/


#ifndef __SW_NET_COMMAND_PROCESS_H__
#define __SW_NET_COMMAND_PROCESS_H__

#include "SWObject.h"
#include "SWClassFactory.h"
#include "SWMessage.h"
#include "SWThread.h"
#include "SWList.h"
#include "tinyxml.h"
#include "SWSocket.h"
#include "SWUDPSocket.h"
#include "SWNetOpt.h"



/**
* @brief 网络命令处理模块
*
*
*
* 
*/
class CSWNetCommandProcess : public CSWObject, CSWMessage
{
	CLASSINFO(CSWNetCommandProcess, CSWObject);
		

		
public:
	/*
	* @brief 构造函数
	*
	*/
	CSWNetCommandProcess();
	
	/*
	* @brief 析构函数
	*
	*/
	virtual ~CSWNetCommandProcess();

	/*
	* @brief 初始化函数
	* @param [in] wProbePort : 设备侦测监听端口
	* @param [in] wCMDProcessPort : 网络命令监听端口
	* @retval S_OK : 成功
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT Initialize(const WORD wProbePort, const WORD wCMDProcessPort);

	/*
	* @brief 开始运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	HRESULT Run();

	/*
	* @brief 停止运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	HRESULT Stop();


protected:

	SW_BEGIN_DISP_MAP(CSWNetCommandProcess,CSWObject)
		//SW_DISP_METHOD(Initialize, 2)
		SW_DISP_METHOD(Run, 0)
		SW_DISP_METHOD(Stop, 0)
	SW_END_DISP_MAP();
	

	/*
	* @brief 设备侦测线程主函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT OnProbe(VOID);	

	/*
	* @brief 设备侦测线程主函数的静态代理函数
	*/
	static VOID* OnProbeProxy(VOID* pParam);

	/*
	* @brief 网络命令线程链接主函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT OnNetCMDConnect(VOID);	

	/*
	* @brief 网络命令线程链接主函数的静态代理函数
	*/
	static VOID* OnNetCMDConnectProxy(VOID* pParam);

	/*
	* @brief 网络命令线程处理主函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT OnNetCMDProcess(SWPA_SOCKET_T sInSock);	

	/*
	* @brief 网络命令线程处理主函数的静态代理函数
	*/
	static VOID* OnNetCMDProcessProxy(VOID* pParam);
	

private:

	/*
	* @brief 处理 网络命令XML
	* @param [in] pszXMLInBuf : 接收到的XML缓存地址
	* @param [in] dwXMLInSize : 接收到的XML缓存区大小
	* @param [out] ppvXMLOutBuf :响应的XML缓存区地址
	* @param [out] pdwXMLOutSize : 响应的XML缓存区大小
	* @param [out] pfReboot : 是否需要重启设备的标志
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT ProcessXmlCmd(const CHAR* pszXMLInBuf,  const DWORD dwXMLInSize, PVOID* ppvXMLOutBuf,  DWORD* pdwXMLOutSize, BOOL* pfReboot);

	/*
	* @brief 生成响应XML的文件头
	* @param [out] ppXmlOutputDoc : 响应XML的文件指针
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateReplyXMLHeader(TiXmlDocument ** ppXmlOutputDoc);

	/*
	* @brief 生成处理命令失败的XML
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateCMDProcessFailedXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

	/*
	* @brief 生成命令没有带数据类型(Type)属性的XML
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateNoTypeErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

	/*
	* @brief 生成命令没有带操作类型(Class)属性的XML
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateNoClassErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

	/*
	* @brief 生成命令没有带数据值(Value)属性的XML
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateNoValueErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);
	/*
	* @brief 生成命令没有带数据返回长度属性的XML
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateNoRetLenErrorXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

	/*
	* @brief 生成无法正确分析接收到的XML文件的答复文件
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateCMDParseFailedXml(TiXmlDocument * pXmlOutputDoc);

	/*
	* @brief 生成命令的响应XML
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @param [in] pszType : 网络命令的数据类型
	* @param [in] pvData : 网络命令的数据缓存区地址
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateCMDReplyXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName, const CHAR * pszType, const CHAR * pszClass, const PVOID pvData);

	/*
	* @brief 生成没有找到命令的XML答复文件
	* @param [in] pXmlOutputDoc : 响应XML的文件指针
	* @param [in] szCmdName : 网络命令的名字
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	*/
	HRESULT GenerateCMDNotFoundXml(TiXmlDocument * pXmlOutputDoc, const CHAR * szCmdName);

	/*
	* @brief 把XML打包转成字符串
	* @param [in] pXmlOutputDoc : XML的文件指针
	* @param [out] ppvXMLOutBuf : 字符串地址
	* @param [out] pdwXMLOutSize : 字符串的长度
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	* @retval E_INVALIDARG: 参数非法
	*/
	HRESULT PackXml(TiXmlDocument* pXmlOutputDoc,  PVOID* ppvXMLOutBuf,  DWORD *pdwXMLOutSize);

	/*
	* @brief 把网络命令转成消息ID发送给其它功能模块
	* @param [in] dwCMDID : 命令ID
	* @param [in] szValue : 命令数据值
	* @param [in] szType : 命令数据类型
	* @param [out] pvRetBuf : 命令返回值的缓存地址
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	* @retval E_INVALIDARG: 参数非法
	*/
	HRESULT SendCMD(const DWORD dwCMDID, const CHAR * szValue, const CHAR * szType, const CHAR* szClass, PVOID* ppvRetBuf);

	
	HRESULT SendDomeCameraCustomCMD(const DWORD dwCMDID, const CHAR * szValue, PVOID* ppvRetBuf);

	
	HRESULT SendCustomCMD(const DWORD dwCMDID, const CHAR * szValue, PVOID* ppvRetBuf);

//#if USE_XML_IN_PROBE

	/*
	* @brief 生成设备侦测命令的响应XML
	* @param [out] ppszXmlBuf : XML的缓存地址
	* @param [out] pdwXmlLen : XML的长度
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	* @retval E_OUTOFMEMORY: 内存不足
	* @retval E_INVALIDARG: 参数非法
	*/
	HRESULT GenerateProbeReplyXml(CHAR ** ppszXmlBuf, DWORD *pdwXmlLen);

	/*
	* @brief 分析设备侦测的XML
	* @param [in] pszMsg : XML的缓存地址
	* @retval S_OK : 分析匹配成功
	* @retval E_FAIL: 失败
	*/
	HRESULT ParseProbeXmlMsg(CHAR * pszMsg);

//#else	
	HRESULT GenerateGetIPCMDReplay(BYTE** pbReplyPack, DWORD* pdwPackLen);

	HRESULT GenerateSetIPCMDReplay(const BYTE* pbMsg, const DWORD dwMsgLen, BYTE** pbReplyPack, DWORD* pdwPackLen);

	
	HRESULT ProcessProtocolCMD(PVOID* ppvOutBuf,	DWORD *pdwOutSize);


	HRESULT ProcessSetParamCMD(const VOID* pvInputBuf,  PVOID* ppvOutBuf,  DWORD *pdwOutSize);

	HRESULT ProcessGetDomeParamCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize);

	HRESULT ProcessSetDomeParamCMD(const VOID* pvInputBuf,  PVOID* ppvOutBuf,  DWORD *pdwOutSize);

	HRESULT ProcessGetParamCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize);

    HRESULT ProcessGetHDD_ReportCMD(PVOID* ppvOutBuf,  DWORD *pdwOutSize);
    HRESULT GetConnectedIPInfo(CHAR * RecordLinkIP,CHAR * ImageLinkIP,CHAR *VideoLinkIP);

//#endif
	
	const CHAR* SearchCmdNameByID(const DWORD dwCMDID);

	HRESULT OnGetProtocol(WPARAM wParam, LPARAM lParam);
   // HRESULT OnNULL(WPARAM wParam, LPARAM lParam);//前期开发测试接口


	SW_BEGIN_MESSAGE_MAP(CSWNetCommandProcess, CSWMessage)
		SW_MESSAGE_HANDLER(MSG_NETCOMMAND_GET_PROTOCOL, OnGetProtocol)
   //     SW_MESSAGE_HANDLER(MSG_NULL,OnNULL);//前期开发测试接口
	SW_END_MESSAGE_MAP();


private:

	enum {
		STATE_READY = 0,
		STATE_RUNNING,
		STATE_STOPPED,
	
		STATE_COUNT
	};
	
	const DWORD 		m_dwGETIP_COMMAND;
	const DWORD 		m_dwSETIP_COMMAND;

	BOOL				m_fInited;
	INT					m_dwState;
	
	WORD				m_wProbePort;
	CSWThread *			m_pProbeThread;

	WORD				m_wCMDProcessPort;
	CSWThread *			m_pCMDServerThread;
	CSWList<CSWThread*>	m_lstCMDProcessThread;
	
};

REGISTER_CLASS(CSWNetCommandProcess)
	
#endif //__SW_NET_COMMAND_PROCESS_H__



