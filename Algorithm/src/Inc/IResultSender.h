/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		IResultSender.h
* @version	1.0
* @brief	结果发送模块接口定义
* @author	Shaorg
*/

#ifndef _IRESULTSENDER_H_
#define _IRESULTSENDER_H_

#include "hvutils.h"
#include "swobjbase.h"

interface IResultSender
{
	/**
	* @brief        放入要发送的结果数据
	* @param[in]    szResultInfo    结果文本数据
	* @param[in]    lpcImageData    结果图片数据
	* @return       成功：S_OK，失败：E_FAIL
	*/
	virtual HRESULT PutResult(
		LPCSTR szResultInfo,
		LPVOID lpcImageData
		) = 0;

    /**
	* @brief        放入要发送的一帧视频流数据
	* @param[out]   pdwSendCount    已发送帧总数
	* @param[in]    lpFrameData     指向IMG_FRAME结构体指针
	* @param[in]    nRectCount      红框个数
	* @param[in]    pRect           指向存放红框坐标数组指针
	* @return       成功：S_OK，失败：E_FAIL
	*/
    virtual HRESULT PutVideo(
        DWORD32* pdwSendCount,
        LPVOID lpFrameData,
        int nRectCount = 0,
        HV_RECT *pRect = NULL
        ) = 0;

    /**
	* @brief        放入要发送的字符串
	* @param[in]    dwVideoID       视频ID
	* @param[in]    wStreamID       数据流ID
	* @param[in]    dwTimeLow       低32位时间
	* @param[in]    dwTimeHigh      高32位时间
	* @param[in]    pString         指向要发送的字符串
	* @return       成功：S_OK，失败：E_FAIL
	*/
    virtual HRESULT PutString(
        WORD16 wVideoID,
        WORD16 wStreamID,
        DWORD32 dwTimeLow,
        DWORD32 dwTimeHigh,
        const char *pString
        ) = 0;

    virtual HRESULT GetCurStatus(char* pszStatus, int nStatusSizes) = 0;

    virtual HRESULT SendDevStateString(
        BOOL fIsCamConnected,
        float fltFrameRate,
        const char* pszHddStatus
        ) = 0;

    virtual ~IResultSender() {};
};

#endif
