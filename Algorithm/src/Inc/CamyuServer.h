/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		CamyuServer.h
* @version	1.0
* @brief	CCamyuServer类的定义，实现了以Camyu协议传输图像帧。
* @author	Shaorg
* @date		2010-11-29
*/

#ifndef _CAMYUSERVER_H_
#define _CAMYUSERVER_H_

#include "ImgGatherer.h"

class CCamyuServer : public CHvThreadBase
{
public:
	CCamyuServer();
	virtual ~CCamyuServer();

public:
	// CHvThreadBase Interface
	virtual const char* GetName()
	{
	    static char szName[] = "CCamyuServer";
	    return szName;
    }
	virtual HRESULT Run(void* pvParam);

public:
    HRESULT PutOneFrame(IMG_FRAME imgFrame);

private:
    HRESULT GetOneFrame(IMG_FRAME* pImgFrame);

private:
    HiVideo::CHvList<IMG_FRAME> m_queImg;
	HV_SEM_HANDLE m_hSemQueCount;
	HV_SEM_HANDLE m_hSemQueCtrl;
};

#endif
