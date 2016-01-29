/**
* @file SWResultFtpRenderFilter.cpp
* @brief Implementation of SWResultFtpRenderFilter
* 
* @copyright Signalway All Rights Reserved
* @author zouzy
* @date 2014-07-28
* @version 1.0
*/

#include "SWFC.h"
#include "SWFilterStruct.h"
#include "SWRecord.h"
#include "qftp.h"
#include "SWResultFtpRenderFilter.h"
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "SWDateTime.h"


#define FTPDirLen 32
#define FTPNameLen 32

using namespace nsFTP;
static CQFTP* g_pFTP = NULL;


CSWResultFtpRenderFilter::CSWResultFtpRenderFilter()
	: CSWBaseFilter(1,0)
	, m_fInited(FALSE)
	, m_dwSaveQueueSize(20)
	, m_pListLock(NULL)
	, m_IsLoginFtp(FALSE)
	, m_IsInitFtp(FALSE)
	, m_TimeOut(5)
{
	GetIn(0)->AddObject(CLASSID(CSWRecord));
}

CSWResultFtpRenderFilter::~CSWResultFtpRenderFilter()
{
	if (!m_fInited)
	{
		return ;
	}

	Stop();

	m_dwSaveQueueSize = 0;

	SAFE_RELEASE(m_pListLock);

	m_fInited = FALSE;

	SW_TRACE_DEBUG("Info: ResultFtpRender de-inited!\n");
}

inline INT SetInt32ToByteArray(BYTE *pbDest, int i32Src)
{
	memcpy( pbDest, &i32Src, sizeof( i32Src ) );
	return sizeof( i32Src );
}

inline INT SetInt16ToByteArray(BYTE *pbDest, SHORT i16Src)
{
	memcpy( pbDest, &i16Src, sizeof( i16Src ) );
	return sizeof( i16Src );
}

HRESULT CSWResultFtpRenderFilter::Yuv2Rgb(
					   BYTE *pbDest,
					   BYTE *pbSrc,
					   int iSrcWidth,
					   int iSrcHeight,
					   int iBGRStride
					   )
{
	bool fBottomUp = true;
	if (iSrcHeight < 0)
	{
		iSrcHeight = -iSrcHeight;
		fBottomUp = false;
	}
	int x, y;
	unsigned char *pY = reinterpret_cast< unsigned char* >(pbSrc);
	unsigned char *pCb = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight;
	unsigned char *pCr = reinterpret_cast< unsigned char* >(pbSrc) + iSrcWidth * iSrcHeight + (iSrcWidth >> 1) * iSrcHeight;
	for (y = 0; y < iSrcHeight; y++)
		for (x = 0; x < iSrcWidth; x++)
		{
			int iY = *(pY + y * iSrcWidth + x);
			int iCb = *(pCb + y * (iSrcWidth >> 1) + (x >> 1));
			int iCr = *(pCr + y * (iSrcWidth >> 1) + (x >> 1));
			int iR = static_cast< int >(1.402 * (iCr - 128) + iY);
			int iG = static_cast< int >(-0.34414 * (iCb - 128) - 0.71414 * (iCr - 128) + iY);
			int iB = static_cast< int >(1.772 * (iCb - 128) + iY);
			if (iR > 255)
				iR = 255;
			if (iR < 0)
				iR = 0;
			if (iG > 255)
				iG = 255;
			if (iG < 0)
				iG = 0;
			if (iB > 255)
				iB = 255;
			if (iB < 0)
				iB = 0;
			if (fBottomUp)
			{
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 ] = iB;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ (iSrcHeight - y - 1) * iBGRStride + x * 3 + 2 ] = iR;
			}
			else
			{
				pbDest[ y * iBGRStride + x * 3 ] = iB;
				pbDest[ y * iBGRStride + x * 3 + 1 ] = iG;
				pbDest[ y * iBGRStride + x * 3 + 2 ] = iR;
			}
		}
		return S_OK;
}


HRESULT CSWResultFtpRenderFilter::Yuv2BMP(
				BYTE *pbDest,
				int iDestBufLen,
				int *piDestLen,
				BYTE *pbSrc,
				int iSrcWidth,
				int iSrcHeight
				)
{
	DWORD32 dwOffSet = 54;
	DWORD32 dwRBGStride = ((iSrcWidth * 24 + 31) >> 5) << 2;
	DWORD32 dwRgbSize = dwRBGStride * iSrcHeight;
	*piDestLen = dwOffSet + dwRgbSize;
	if (*piDestLen > iDestBufLen)
	{
		*piDestLen = 0;
		return E_FAIL;
	}

	*(pbDest++) = 'B';										//BMP文件标志.
	*(pbDest++) = 'M';
	pbDest += SetInt32ToByteArray(pbDest, *piDestLen);		//BMP文件大小.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//reserved
	pbDest += SetInt32ToByteArray(pbDest, dwOffSet);		//文件头偏移量.
	pbDest += SetInt32ToByteArray(pbDest, 0x28);			//biSize.
	pbDest += SetInt32ToByteArray(pbDest, iSrcWidth);		//biWidth.
	pbDest += SetInt32ToByteArray(pbDest, iSrcHeight);		//biHeight.
	pbDest += SetInt16ToByteArray(pbDest, 1);				//biPlanes.
	pbDest += SetInt16ToByteArray(pbDest, 24);				//biBitCount.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biCompression.
	pbDest += SetInt32ToByteArray(pbDest, dwRgbSize);		//biSizeImage.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biXPerMeter.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biYPerMeter.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biClrUsed.
	pbDest += SetInt32ToByteArray(pbDest, 0);				//biClrImportant.
	Yuv2Rgb(pbDest, pbSrc, iSrcWidth, iSrcHeight, dwRBGStride);
	return S_OK;
}


HRESULT CSWResultFtpRenderFilter::FtpSender()
{
	char* upload_errStr;	
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}
	SW_TRACE_NORMAL("Info: ResultFtp filter run.");
	char ftpfilename[FTPNameLen+FTPDirLen] = {0};

	while(FILTER_RUNNING == GetState())
	{	
		if(!m_IsInitFtp)
		{
			if(S_OK == InitFtp())
				m_IsInitFtp = TRUE;	
			CSWApplication::Sleep(m_TimeOut * 1000);
		}
		else
		{
			m_pListLock->Lock();
			DWORD ftpFileCount = m_lstFile.GetCount();
			m_pListLock->Unlock();
			while(ftpFileCount--)
			{
				if(ERROR_QFTP_OK == g_pFTP->TestConnectValidity())
			{
				//SW_TRACE_DEBUG("g_pFTP->KeepAlive()\n");
				g_pFTP->KeepAlive();
				
			}
			else
			{
				//SW_TRACE_DEBUG("g_pFTP->LoginFtpServer()\n");
				g_pFTP->LoginFtpServer();
			}

			
			m_pListLock->Lock();
			FtpPackage * ftpPackage = m_lstFile.GetHead();
			m_pListLock->Unlock();
			sprintf(ftpfilename,"/%s/%s",ftpPackage->dir,ftpPackage->name);
			//SW_TRACE_DEBUG("ftpPackage->name %d is %s\n",ftpFileCount,ftpfilename);
			if(ERROR_QFTP_OK != g_pFTP->WriteFileToFtp(ftpfilename,(CHAR*)ftpPackage->buffer,ftpPackage->size))
			{
				if(m_IsLoginFtp)
				{
					SW_TRACE_DEBUG("WriteFileToFtp error!!!\n");
					ReportStatus("无法上传结果,请检查FTP服务器连接");
					m_IsLoginFtp = FALSE;
					break;
				}
			}
			else
			{
				if(!m_IsLoginFtp)
				{
					ReportStatus("服务器已连接");
					m_IsLoginFtp = TRUE;
				}
				swpa_mem_free(ftpPackage->buffer);
				swpa_mem_free(ftpPackage->name);
				swpa_mem_free(ftpPackage->dir);
		       	swpa_mem_free(ftpPackage);
				m_pListLock->Lock();
				m_lstFile.RemoveHead();
				m_pListLock->Unlock();
			}

			CSWApplication::Sleep(100);
			}
			
			CSWApplication::Sleep(50);
		}
		
	}
	return S_OK;
}

PVOID CSWResultFtpRenderFilter::OnFtpSenderProxy(PVOID pvArg)
{
	SW_TRACE_DEBUG("in the CSWResultFtpRenderFilter OnFtpSenderProxy\n");
	if (NULL == pvArg)
	{
		SW_TRACE_NORMAL("Err: NULL == pvArg\n");
		return NULL;
	}

	return (PVOID)((CSWResultFtpRenderFilter*)pvArg)->FtpSender();
}


HRESULT CSWResultFtpRenderFilter::InitFtp()
{
	INT iErrNum = 0;
	if (NULL == g_pFTP)
	{
		if(NULL != m_szDstIp && NULL != m_szUserName && NULL != m_szPassWd)
		{
			SW_TRACE_DEBUG("CSWResultFtpRenderFilter::InitFtp %s, %s, %s, %d\n",m_szDstIp,m_szUserName,m_szPassWd,m_TimeOut);
			g_pFTP = new CQFTP(m_szDstIp, m_szUserName, m_szPassWd, (UINT)m_TimeOut);
			if (NULL == g_pFTP)
			{
				SW_TRACE_DEBUG("Err: no enough memory for g_pFTP\n");
				return E_OUTOFMEMORY;
			}
		}
		
	}
	
	iErrNum = g_pFTP->LoginFtpServer();
	if(iErrNum == ERROR_QFTP_OK)
	{
		SW_TRACE_DEBUG("LoginFtpServer OK!!\n");
		m_IsLoginFtp = TRUE;
		ReportStatus("连接到FTP服务器");
		return S_OK;
	}
	else
	{
		SW_TRACE_DEBUG("Can not LoginFtpServer!!err no is %d\n",iErrNum);
		ReportStatus("连接FTP服务器失败");
		m_IsLoginFtp = FALSE;
		return S_FALSE;
	}
	
}

HRESULT CSWResultFtpRenderFilter::Initialize(const CHAR * szDstIp, const CHAR * szUserName, const CHAR * szPassWd, INT TimeOut)
{
	SW_TRACE_DEBUG("CSWResultFtpRenderFilter::Initialize %s, %s, %s, %d\n",szDstIp,szUserName,szPassWd,TimeOut);
	HRESULT hr = S_OK;

	if (m_fInited)
	{
		SW_TRACE_DEBUG("CSWResultFtpRenderFilter already init\n");
		return S_OK;
	}
	if(NULL != szDstIp && NULL != szUserName && NULL != szPassWd)
	{
		strcpy(m_szDstIp, szDstIp);
		strcpy(m_szUserName, szUserName);
		strcpy(m_szPassWd, szPassWd);
		m_TimeOut = TimeOut;
		
		m_lstFile.RemoveAll();
		m_lstFile.SetMaxCount(m_dwSaveQueueSize);
		if (NULL == m_pListLock)
		{
			m_pListLock = new CSWMutex();
			if (NULL == m_pListLock)
			{
				SW_TRACE_DEBUG("Err: no enough memory for m_pListLock\n");
				return E_OUTOFMEMORY;
			}
		}
		m_szSrcIP = {0};
		CHAR szMask[32] = {0};
		CHAR szGateway[32] = {0};
		CHAR szMac[32] = {0};
		
		if (SWPAR_OK != swpa_tcpip_getinfo("eth0",
		m_szSrcIP, sizeof(m_szSrcIP),
		szMask, sizeof(szMask),
		szGateway, sizeof(szGateway),
		szMac, sizeof(szMac)))
		{
			SW_TRACE_DEBUG("Err: failed to get net info!\n");
			return E_FAIL;
		}
		else
		{
			SW_TRACE_DEBUG("FTP get IP is %s\n",m_szSrcIP);
		}
	}
	else
	{
		m_lstFile.SetMaxCount(0);
	}
	SW_TRACE_DEBUG("CSWResultFtpRenderFilter::Initialize OK!!!\n");
	m_fInited = TRUE;
	return hr;
}

HRESULT CSWResultFtpRenderFilter::FtpPacker(CSWRecord* pRecord)
{
	//SW_TRACE_DEBUG("in the CSWResultFtpRenderFilter FtpPacker\n");
	CSWDateTime cTime(pRecord->GetCarArriveTime());
	INT iYear = cTime.GetYear();
	INT iMon = cTime.GetMonth();
	INT iDay = cTime.GetDay();
	INT iHour = cTime.GetHour();
	INT iMinute = cTime.GetMinute();
	INT iSecond = cTime.GetSecond();
	INT iMSSecond = cTime.GetMSSecond();
	BOOL fHaveRecordImage = FALSE;
	//获得图片
	INT img_num = 1;
	for (DWORD i=0; i<pRecord->GetImageTypeCount(); i++)
	{
		if (NULL != pRecord->GetImage(i))
		{
			fHaveRecordImage = TRUE;
			FtpPackage* ftpPackage = (FtpPackage*)swpa_mem_alloc(sizeof(FtpPackage));
			ftpPackage->name = (char*)swpa_mem_alloc(FTPNameLen);
			ftpPackage->dir = (char*)swpa_mem_alloc(FTPDirLen);
				
			sprintf(ftpPackage->name, "%02d%02d%03d", 
				iMinute, iSecond, iMSSecond);
			sprintf(ftpPackage->dir, "%s/%04d%02d%02d%02d", 
				m_szSrcIP, iYear, iMon, iDay, iHour);
			
			SW_COMPONENT_IMAGE sComponentImage;
			if (FAILED(pRecord->GetImage(i)->GetImage(&sComponentImage)))
			{
				SW_TRACE_DEBUG("Err: failed to get image\n");
				continue;
			}
			switch(sComponentImage.cImageType)
			{
				case SW_IMAGE_BIN:{		
										ftpPackage->buffer = (unsigned char*)swpa_mem_alloc(sComponentImage.iSize);
										unsigned char* pbOffset = ftpPackage->buffer;
										BYTE *pSrc = sComponentImage.rgpbData[0];
										INT iDestLine = sComponentImage.iWidth >> 3;
										//get bin image
										for (INT iHeight = 0; iHeight < sComponentImage.iHeight; iHeight++, pSrc += iDestLine)
										{
											swpa_memcpy(pbOffset, pSrc, iDestLine);
											pbOffset += iDestLine;
										}
										ftpPackage->size= pbOffset - ftpPackage->buffer;
										sprintf(ftpPackage->name,"%s.bin",ftpPackage->name);
										m_pListLock->Lock();
										m_lstFile.AddTail(ftpPackage);
										m_pListLock->Unlock();
										break;
									}
				case SW_IMAGE_JPEG:	{	//get JPEG image
										ftpPackage->buffer = (unsigned char*)swpa_mem_alloc(sComponentImage.iSize);
										swpa_memcpy(ftpPackage->buffer, sComponentImage.rgpbData[0], sComponentImage.iSize);
										ftpPackage->size = sComponentImage.iSize;
										sprintf(ftpPackage->name,"%s_image%d.jpg",ftpPackage->name,img_num);
										img_num++;
										m_pListLock->Lock();
										m_lstFile.AddTail(ftpPackage);
										m_pListLock->Unlock();
										break;
									}
				case SW_IMAGE_YUV_422:{	//get bmp image
										INT bmp_size = (((sComponentImage.iWidth * 24 + 31) >> 5) << 2) * sComponentImage.iHeight + 128;
										INT iDestLen = 0;
										ftpPackage->buffer = (unsigned char*)swpa_mem_alloc(bmp_size);
										if(S_OK ==Yuv2BMP(ftpPackage->buffer, bmp_size, &iDestLen, sComponentImage.rgpbData[0], 
											sComponentImage.iWidth, sComponentImage.iHeight))
										{
											SW_TRACE_DEBUG("YUV2BMP OK!!!\n");
											sprintf(ftpPackage->name,"%s.bmp",ftpPackage->name);
											ftpPackage->size = iDestLen;
											m_pListLock->Lock();
											m_lstFile.AddTail(ftpPackage);
											m_pListLock->Unlock();
										}
										else
										{
											SW_TRACE_DEBUG("YUV2BMP err!!!\n");
											SW_TRACE_DEBUG("bmp ftpPackage err,dir is %s,name is %s\n",ftpPackage->dir,ftpPackage->name);
										}
										
										break;
									}	
										
					
				default:
										SW_TRACE_DEBUG("sComponentImage.cImageType is %d",sComponentImage.cImageType);
										break;

			}
			
		}
	}
	
	//获得xml
	//if(fHaveRecordImage == TRUE)
	{
		FtpPackage* xml_ftpPackage = (FtpPackage*)swpa_mem_alloc(sizeof(FtpPackage));
		xml_ftpPackage->name = (char*)swpa_mem_alloc(FTPNameLen);
		xml_ftpPackage->dir = (char*)swpa_mem_alloc(FTPDirLen);
		xml_ftpPackage->size = pRecord->GetXmlSize();
		xml_ftpPackage->buffer = (unsigned char*)swpa_mem_alloc(xml_ftpPackage->size);
			
		sprintf(xml_ftpPackage->name, "%02d%02d%03d.xml", 
				iMinute, iSecond, iMSSecond);
		sprintf(xml_ftpPackage->dir, "%s/%04d%02d%02d%02d", 
				m_szSrcIP, iYear, iMon, iDay, iHour);
		
		swpa_memcpy(xml_ftpPackage->buffer, pRecord->GetXmlString(), xml_ftpPackage->size);
		m_pListLock->Lock();
		m_lstFile.AddTail(xml_ftpPackage);
		m_pListLock->Unlock();
	}
	
	return S_OK;	
}

HRESULT CSWResultFtpRenderFilter::Receive(CSWObject* obj)
{    
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		return E_NOTIMPL;
	}
	if(m_IsLoginFtp)
	{
		//SW_TRACE_DEBUG("Info: ResultFtp got a CSWRecord obj\n");
		CSWRecord* pRecord = (CSWRecord*)obj;
		if (NULL == pRecord)
		{
			SW_TRACE_DEBUG("Err: NULL == pRecord\n");
			return E_INVALIDARG;
		}
		//SAFE_ADDREF(pRecord)
		FtpPacker(pRecord);
		//SAFE_RELEASE(pRecord);
	}
	return S_OK;
}

HRESULT CSWResultFtpRenderFilter::Run()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (FAILED(CSWBaseFilter::Run()))
	{
		SW_TRACE_NORMAL("Err: Failed to run CSWResultFtpRenderFilter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThreadFTPSender.Start(OnFtpSenderProxy, (PVOID)this)))
	{
		SW_TRACE_NORMAL("Err: failed to start CSWResultFtpRenderFilter sender thread\n");
		return E_FAIL;
	}
	
	SW_TRACE_NORMAL("Info: CSWResultFtpRenderFilter running...\n");	
	
	return S_OK;
}

HRESULT CSWResultFtpRenderFilter::Stop()
{
	if (!m_fInited)
	{
		SW_TRACE_NORMAL("Err: not inited yet\n");
		return E_OBJ_NO_INIT;
	}

	if (m_IsLoginFtp)
	{
		for(;;)
		{
			m_pListLock->Lock();
			BOOL fEmpty = m_lstFile.IsEmpty();
			m_pListLock->Unlock();
			
			if(!fEmpty)
			{
				SW_TRACE_DEBUG("Info: wait till all record to be sent...\n");
				CSWApplication::Sleep(1000);
			}
			else
			{
				break;
			}
		}

		m_pListLock->Lock();
		while (!m_lstFile.IsEmpty())
		{
			CSWObject * pObj = (CSWObject *)m_lstFile.RemoveHead();
			SAFE_RELEASE(pObj);
		}
		m_pListLock->Unlock();

		if (NULL != g_pFTP)
		{
			g_pFTP->LogoutFtpServer();
		}
	}

	if (FAILED(CSWBaseFilter::Stop()))
	{
		SW_TRACE_NORMAL("Err: Failed to stop CSWResultFtpRenderFilter\n");
		return E_FAIL;
	}

	if (FAILED(m_cThreadFTPSender.Stop()))
	{
		SW_TRACE_NORMAL("Err: failed to stop CSWResultFtpRenderFilter sender thread\n");
		return E_FAIL;
	}

	
	SW_TRACE_NORMAL("Info: CSWResultFtpRenderFilter stopped\n");
	
	return S_OK;
}


HRESULT CSWResultFtpRenderFilter::ReportStatus(const CHAR* szStr)
{
	if (!m_fInited)
	{
		SW_TRACE_DEBUG("Err: not initialized yet\n");
		return E_NOTIMPL;
	}

	CSWString strInfo = "";
	strInfo.Append("FTP状态:");	
	strInfo.Append(szStr);

	return CSWMessage::SendMessage(MSG_APP_UPDATE_STATUS, (WPARAM)(const CHAR*)strInfo, 0);
}

