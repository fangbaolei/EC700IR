/**
* @file SWResultFtpRenderFilter.h 
* @brief 结果FTP上传
* 
* @copyright Signalway All Rights Reserved
* @author zouzy
* @date 2014-07-28
* @version 1.0
*/

#ifndef __SW_RESULT_FTP_RENDER_FILTER_H__
#define __SW_RESULT_FTP_RENDER_FILTER_H__
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "SWBaseFilter.h"
#include "SWClassFactory.h"
#include "SWFC.h"
#include "SWList.h"
#include "SWMessage.h"
#include "SWDateTime.h"
#include "SWFC.h"
#include "SWFilterStruct.h"
#include "SWRecord.h"
#include "qftp.h"




class FtpPackage
{
public:
	char* name;
	
	char* dir;
	
	unsigned char* buffer;
	
	DWORD32 size;
};

/**
* @brief 		将记录通过FTP发送
*/

class CSWResultFtpRenderFilter : public CSWBaseFilter
{
	CLASSINFO(CSWResultFtpRenderFilter, CSWBaseFilter)
public:
	CSWResultFtpRenderFilter();
	
	virtual ~CSWResultFtpRenderFilter();
	
	HRESULT Initialize(const CHAR * szDstIp, const CHAR * szUserName, const CHAR * szPassWd, INT TimeOut);

	SW_BEGIN_DISP_MAP(CSWResultFtpRenderFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 4)
	SW_END_DISP_MAP();
	
	virtual HRESULT Receive(CSWObject* obj);
	
	virtual HRESULT Run();
	
	virtual HRESULT Stop();
	
	HRESULT Yuv2BMP(BYTE *pbDest, int iDestBufLen, int *piDestLen, BYTE *pbSrc,
				int iSrcWidth,int iSrcHeight);
	
	HRESULT Yuv2Rgb(BYTE *pbDest, BYTE *pbSrc, int iSrcWidth, int iSrcHeight, int iBGRStride);
	
protected:
	
	HRESULT InitFtp();
	
	static PVOID OnFtpSenderProxy(PVOID pvArg);

	/**
	* @brief 	将FtpPacker打包成的FtpPackage通过FTP发送
	* @retval S_OK : 成功
	* @retval E_FAIL : 失败
	*/
	HRESULT FtpSender();
	
	/**
	* @brief 	将记录打包为FtpPackage，一个FtpPackage
	*		将被存为"/FtpPackage.dir/FtpPackage.name"的文件
	* @retval S_OK : 成功
	* @retval E_INVALIDARG : 参数非法
	* @retval E_FAIL : 失败
	*/
	HRESULT FtpPacker(CSWRecord* pRecord);
	
	HRESULT ReportStatus(const CHAR* szStr);

private:
 	BOOL 					m_fInited;
	
	BOOL					m_IsLoginFtp;
	
	BOOL					m_IsInitFtp;
	
	DWORD 					m_dwSaveQueueSize;
	
	CSWThread				m_cThreadFTPSender;
	
	CSWList<FtpPackage*>	m_lstFile;
	
	CSWSemaphore*			m_pSemaSaveQueueSync;
	
	CSWMutex*				m_pListLock;

	CHAR  					m_szDstIp[32];

	CHAR  					m_szUserName[32];

	CHAR  					m_szPassWd[32];

	INT 					m_TimeOut;
	
	CHAR					m_szSrcIP[32];
	
};
REGISTER_CLASS(CSWResultFtpRenderFilter)
#endif //__SW_CENTAURUS_H264_ENCODE_FILTER_H__

