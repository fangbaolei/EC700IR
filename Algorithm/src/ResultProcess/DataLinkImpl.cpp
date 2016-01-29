#include "DataLinkImpl.h"
#include "hvutils.h"
#include "HvSockUtils.h"
#include "SocketStream.h"
#include "ResultSenderImpl.h"
#include "hvtarget_ARM.h"
using namespace HiVideo;

#include "misc.h"

G_QUEUE< HVIO_CarInfoBegin, HVIO_INFOBEGIN_COUNT > CDataLinkThread::s_queCarInfoBegin;
G_QUEUE< HVIO_CarInfoEnd, HVIO_INFOEND_COUNT > CDataLinkThread::s_queCarInfoEnd;
G_QUEUE< HVIO_CarInLeft, HVIO_INLEFT_COUNT > CDataLinkThread::s_queCarInLeft;
G_QUEUE< HVIO_CarPlate, HVIO_PLATE_COUNT > CDataLinkThread::s_queCarPlate;
G_QUEUE< HVIO_BigImage, HVIO_BIGIMG_COUNT > CDataLinkThread::s_queBigImage;
G_QUEUE< HVIO_SmallImage, HVIO_SMALLIMG_COUNT > CDataLinkThread::s_queSmallImage;
G_QUEUE< HVIO_Video, HVIO_VIDEO_COUNT > CDataLinkThread::s_queVideo;
G_QUEUE< HVIO_String, HVIO_STR_COUNT > CDataLinkThread::s_queString;

HRESULT STDMETHODCALLTYPE CDataLinkThread::SendInfoBegin(
    const HVIO_CarInfoBegin* pCarInfoBegin,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queCarInfoBegin.Release( index );
        return S_OK;
    }

    bool fCheck = (S_OK == CheckStreamID( ( WORD16 )pCarInfoBegin->m_wVideoID, _TYPE_CARINFO_BEGIN ));
    if ( pCarInfoBegin != NULL )
    {
        m_dwCurCarID = pCarInfoBegin->m_dwCarID;
    }
    if ( pCarInfoBegin == NULL || IsFree() || !fCheck )
    {
        s_queCarInfoBegin.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pCarInfoBegin->m_wType;

    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queCarInfoBegin.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//用户接口：发送车辆信息结束接收标志
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendInfoEnd(
    const HVIO_CarInfoEnd* pCarInfoEnd,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queCarInfoEnd.Release( index );
        return S_OK;
    }

    if ( pCarInfoEnd == NULL
            || IsFree()
            || pCarInfoEnd->m_dwCarID != m_dwCurCarID
            || (S_OK != CheckStreamID( ( WORD16 )pCarInfoEnd->m_wVideoID, _TYPE_CARINFO_END )) )
    {
        s_queCarInfoEnd.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pCarInfoEnd->m_wType;

    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queCarInfoEnd.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//用户接口：发送车辆到达离开标志
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendInLeftFlag(
    const HVIO_CarInLeft* pCarInLeft,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queCarInLeft.Release( index );
        return S_OK;
    }

    if ( pCarInLeft == NULL
            || IsFree()
            || (S_OK != CheckStreamID( ( WORD16 )pCarInLeft->m_wVideoID, _TYPE_CAR_IN_AND_LEFT ) ) )
    {
        s_queCarInLeft.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pCarInLeft->m_wType;
    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queCarInLeft.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//用户接口：发送车牌
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendCarPlate(
    const HVIO_CarPlate* pCarPlate,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queCarPlate.Release( index );
        return S_OK;
    }

    if ( pCarPlate == NULL
            || IsFree()
            || (S_OK != CheckStreamID( ( WORD16 )pCarPlate->m_wVideoID, _TYPE_PLATE_STR ))
            || pCarPlate->m_dwCarID != m_dwCurCarID )
    {
        s_queCarPlate.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pCarPlate->m_wType;
    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queCarPlate.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//用户接口：发送车牌附加信息
HRESULT CDataLinkThread::SendAppendInfo(
    const HVIO_AppendInfo* pAppendInfo,
    const int& index,
    DWORD32 dwFlag
)
{
    return E_NOTIMPL;
}

//用户接口：发送车辆YUV大图
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendBigImage(
    const HVIO_BigImage* pBigImage,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queBigImage.Release( index );
        return S_OK;
    }

    if (	IsFree()
            || (S_OK != CheckStreamID( ( WORD16 )pBigImage->m_wVideoID, _TYPE_BIG_IMAGE ))
            ||  pBigImage->m_dwCarID != m_dwCurCarID )
    {
        s_queBigImage.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pBigImage->m_wType;
    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queBigImage.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//发送YUV小图
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendSmallImageYUV(
    const HVIO_SmallImage* pSmallImage,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queSmallImage.Release( index );
        return S_OK;
    }

    if ( pSmallImage == NULL
            || IsFree()
            || (S_OK != CheckStreamID( ( WORD16 )pSmallImage->m_wVideoID, _TYPE_SMALL_IMAGE ))
            || pSmallImage->m_dwCarID != m_dwCurCarID )
    {
        s_queSmallImage.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pSmallImage->m_wType;
    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queSmallImage.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//发送二值化小图
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendBinaryImage(
    const HVIO_SmallImage* pSmallImage,
    const int& index,
    DWORD32 dwFlag
)
{
    if ( m_fSafeEnable )
    {
        s_queSmallImage.Release( index );
        return S_OK;
    }

    if ( pSmallImage == NULL
            || IsFree()
            || (S_OK != CheckStreamID( ( WORD16 )pSmallImage->m_wVideoID, _TYPE_BINARY_IMAGE ))
            || pSmallImage->m_dwCarID != m_dwCurCarID )
    {
        s_queSmallImage.Release( index );
        return E_FAIL;
    }

    SENDSTYLE styleTemp;
    styleTemp.m_index = index;
    styleTemp.m_wSendType = pSmallImage->m_wType;
    if ( !m_queStype.PushBack( styleTemp ) )
    {
        s_queSmallImage.Release( index );
        return E_FAIL;
    }
    SemPost(&m_hSemResult);

    return S_OK;
}

//发送视频
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendVideoYUV(
    const HVIO_Video* pVideo,
    const int& index,
    DWORD32* pdwSendCount,
    DWORD32 dwFlag
)
{
    if( pVideo == NULL
		|| IsFree()
		|| (S_OK != CheckStreamID( ( WORD16 )pVideo->m_wVideoID, _TYPE_FULL_CMP_IMAGE )))
	{
		if( pVideo == NULL && (S_OK == CheckStreamID( 0, _TYPE_FULL_CMP_IMAGE )))
		{
			return S_OK;
		}
		s_queVideo.Release( index );
		return E_FAIL;
	}

	SENDSTYLE styleTemp;
	styleTemp.m_index = index;
	styleTemp.m_wSendType = pVideo->m_wType;
	if( !m_queStype.PushBack( styleTemp ) )
	{
		s_queVideo.Release( index );
		return S_FALSE;
	}

	SemPost(&m_hSemResult);

	return S_OK;
}

//用户接口：发送字符流
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendString(
    const HVIO_String* pString,
    const int& index,
    DWORD32 dwFlag
)
{
    if( pString == NULL
		|| IsFree()
		|| (S_OK != CheckStreamID( ( WORD16 )pString->m_wVideoID, ( WORD16 )pString->m_wType )))
	{
		s_queString.Release( index );
		return E_FAIL;
	}

	SENDSTYLE styleTemp;
	styleTemp.m_index = index;
	styleTemp.m_wSendType = pString->m_wType;
	if( !m_queStype.PushBack( styleTemp ) )
	{
		s_queString.Release( index );
		HV_Trace(3, "Style String FULL!\n");
		return E_FAIL;
	}

	SemPost(&m_hSemResult);
	return S_OK;
}

//用户接口：发送二进制流
HRESULT STDMETHODCALLTYPE CDataLinkThread::SendBin(
    const HVIO_Binary* pBinary,
    const int& index,
    DWORD32 dwFlag
)
{
    return E_NOTIMPL;
}

//设置过滤条件
HRESULT STDMETHODCALLTYPE CDataLinkThread::SetStreamID(
    DWORD32 dwIP,
    WORD16 wPort,
    WORD16 wVideoID,
    WORD16 wStreamID
)
{
    if (S_OK != CheckLink(dwIP, wPort))
    {
        return S_FALSE;
    }

    SemPend(&m_hSemStreamID);

    HRESULT hr = E_FAIL;
    int i;
    for ( i = 0; i < m_wStreamCount; i++)
    {
        if ( wVideoID == m_pAllowVideoID[ i ] && wStreamID == m_pAllowStreamID[i])
        {
            hr = S_OK;
            break;
        }
    }
    if (i == m_wStreamCount && m_wStreamCount < MAX_STREAMID_COUNT)
    {
        m_pAllowVideoID[ i ] = wVideoID;
        m_pAllowStreamID[ i ] = wStreamID;
        m_wStreamCount++;
        hr = S_OK;
    }

    SemPost(&m_hSemStreamID);

    return hr;
}

//删除过滤条件
HRESULT STDMETHODCALLTYPE CDataLinkThread::DeleteStreamID(
    DWORD32 dwIP,
    WORD16 wPort,
    WORD16 wVideoID,
    WORD16 wStreamID
)
{
    if (S_OK != CheckLink(dwIP, wPort)) return S_FALSE;

    SemPend(&m_hSemStreamID);

    int i, iDist(0);
    for (i = 0; i < m_wStreamCount; i++)
    {
        if (wVideoID == m_pAllowVideoID[i] && wStreamID == m_pAllowStreamID[i])
        {
            iDist++;
        }
        else
        {
            if (iDist > 0)
            {
                m_pAllowVideoID[i - iDist] = m_pAllowVideoID[i];
                m_pAllowStreamID[i - iDist] = m_pAllowStreamID[i];
            }
        }
    }
    m_wStreamCount -= iDist;

    SemPost(&m_hSemStreamID);

    return S_OK;
}

//清除所有过滤条件
HRESULT STDMETHODCALLTYPE CDataLinkThread::ClearAllStreamID(
    DWORD32 dwIP,
    WORD16 wPort
)
{
    HRESULT hr = S_FALSE;

    SemPend(&m_hSemStreamID);

    if ( S_OK == CheckLink(dwIP, wPort) )
    {
        m_wStreamCount = 0;
        hr = S_OK;
    }
    SemPost(&m_hSemStreamID);

    return hr;
}

HRESULT STDMETHODCALLTYPE CDataLinkThread::SetSafeMode(
    DWORD32 dwIP,
    WORD16 wPort,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    int index,
    int iPageIndex /* = -1*/,
    int iPageSize /* = 0*/
)
{
    HRESULT hr = CheckLink(dwIP, wPort);
    if ( S_OK == hr )
    {
        m_dwTimeLow = dwTimeLow;
        m_dwTimeHigh = dwTimeHigh;
        MultiplicationDWORD64(m_dwTimeLow, m_dwTimeHigh, 1000);

        m_SS_RecodeIndex = index;
        m_fSafeEnable = true;
    }
    else
    {
        HV_Trace(1, "SetSafeMode is failed\n");
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CDataLinkThread::GetAviByTime(
    DWORD32 dwIP,
    WORD16 wPort,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    DWORD32 *pdwAviTimeLow,
    DWORD32 *pdwAviTimeHigh,
    DWORD32 *pdwAviLen)
{
    return E_NOTIMPL;
}

//查询硬盘录像
HRESULT CDataLinkThread::GetHistoryVideo(
    DWORD32 dwIP,
    WORD16 wPort,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    DWORD32 dwTimeSliceS
)
{
    return E_NOTIMPL;
}

//检查是否匹配
HRESULT CDataLinkThread::CheckLink(
    DWORD32 dwIP,
    WORD16 wPort
)
{
    if (m_hSocket == NULL) return E_OBJ_NO_INIT;

    DWORD32 dwPeerIP;
    WORD16 wPeerPort;
    HvGetPeerName(m_hSocket, &dwPeerIP, &wPeerPort);

    //debug info
    HRESULT hr = ( (dwIP == dwPeerIP) && (wPort == wPeerPort) ) ? S_OK : E_FAIL;
    if ( hr != S_OK )
    {
        //printf("CheckLink: [%u,%u] : [%u,%u]\n", dwPeerIP,wPeerPort,dwIP,wPort);
    }

    return ( (dwIP == dwPeerIP) && (wPort == wPeerPort) ) ? S_OK : E_FAIL;
}

//判断是否符合过滤条件
HRESULT CDataLinkThread::CheckStreamID(
    WORD16 wVideoID,
    WORD16 wStreamID
)
{
    int i;
    for (i = 0; i < m_wStreamCount; i++)
    {
        if (wVideoID == m_pAllowVideoID[i] && wStreamID == m_pAllowStreamID[i])
        {
            break;
        }
    }
    if (i == m_wStreamCount)
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

//处理发送队列
HRESULT CDataLinkThread::ProcessQueue()
{
    if ( m_hSocket == INVALID_SOCKET )
    {
        return E_FAIL;
    }

    HvCore::IHvStream* pStream = NULL;
    CSocketStream cSockStream(m_hSocket);
    pStream = (HvCore::IHvStream*)&cSockStream;

    int iRst = 0;
    int iError = 0;
    bool fCanSend = false;
    int nPackLen = m_hvioThrob.m_dwLen + sizeof(m_hvioThrob.m_dwLen);

    WORD16 wVideoID = 0;
    WORD16 wType = _TYPE_PLATE_STR;

//    //根据开关来初始化安全保存，因为该模块占用大概14M内存，太耗资源
//    //modify by huanggr 2011-03-23
//    //TODO: 如果Arm端内存<=64M，那么在启动安全保存的情况下同时有两个客户端连接，
//    //识别程序将会由于内存不足而崩溃，解决办法是修改安全保存内存申请机制，或者增大Arm端可用内存
//    CSafeSaverData *pcSafeData = NULL;
//    if (m_fSafeEnable)
//    {
//        // Comment by Shaorg: 可靠性存储器应该是全局唯一一个，这里又new一个出来是不对的。需要修正。
//        m_pcSafeSaver_Dm6467 = new CSafeSaverDm6467Impl;
//        if (m_pcSafeSaver_Dm6467)
//        {
//            m_pcSafeSaver_Dm6467->Init("");
//        }
//        m_pcSafeSaver = m_pcSafeSaver_Dm6467;
//        pcSafeData = new CSafeSaverData;
//    }

    HVIO_CarInfoBegin infoBegin;
    infoBegin.m_dwLen = 8;
    infoBegin.m_wType = _TYPE_CARINFO_BEGIN;

    HVIO_CarInfoEnd infoEnd;
    infoEnd.m_dwLen = 8;
    infoEnd.m_wType = _TYPE_CARINFO_END;
    int nPendTimes = 0;

    HV_Trace(3, "Data link run...\n");
    while ( !m_fExit )
    {
        m_dwLastThreadIsOkTime = GetSystemTick();

//        //处理可靠性保存结果的发送
//        if ( m_fSafeEnable && m_pcSafeSaver )
//        {
//            DWORD32 dwNowTimeLow, dwNowTimeHigh;
//            GetSystemTime(&dwNowTimeLow, &dwNowTimeHigh);
//
//            //检查车牌回调是否已设置成功
//            HRESULT hr = E_FAIL;
//            if ( !fCanSend && S_OK == CheckStreamID(wVideoID, wType) )
//            {
//                fCanSend = true;
//            }
//            if ( fCanSend )
//            {
//                InitSafeData(pcSafeData);
//                hr = m_pcSafeSaver->GetPlateRecord(
//                    m_dwTimeLow,
//                    m_dwTimeHigh,
//                    m_SS_RecodeIndex,
//                    0,
//                    pcSafeData
//                );
//                if ( S_OK != hr )
//                {
//                    printf("GetPlateRecord is failed. index = %d\n", m_SS_RecodeIndex);
//                }
//
//                //对车牌号码类型做判断
//                if ( hr == S_OK && pcSafeData->pPlateNoInfo->m_wType != _TYPE_PLATE_STR )
//                {
//                    hr = E_FAIL;
//                }
//            }
//            if ( fCanSend && S_OK == hr )
//            {
//                Lan2LedLight();
//                iError = 0;
//
//                infoBegin.m_dwCarID = pcSafeData->pPlateNoInfo->m_dwCarID;
//                infoBegin.m_wVideoID = pcSafeData->pPlateNoInfo->m_wVideoID;
//
//                infoEnd.m_dwCarID = pcSafeData->pPlateNoInfo->m_dwCarID;
//                infoEnd.m_wVideoID = pcSafeData->pPlateNoInfo->m_wVideoID;
//
//                if (FAILED(pStream->Write(&infoBegin, infoBegin.m_dwLen + 4, NULL)))
//                {
//                    iRst = -1;
//                    break;
//                }
//                if ( pcSafeData->pSmallImg->m_dwImageSize > 0
//                        && FAILED(pStream->Write(pcSafeData->pSmallImg, pcSafeData->pSmallImg->m_dwLen + 4, NULL)) )
//                {
//                    iRst = -1;
//                    break;
//                }
//                if ( pcSafeData->pBinaryImg->m_dwImageSize > 0
//                        && FAILED(pStream->Write(pcSafeData->pBinaryImg, pcSafeData->pBinaryImg->m_dwLen + 4, NULL)) )
//                {
//                    iRst = -1;
//                    break;
//                }
//
//                for (int i = 0; i < MAX_BIG_IMG_COUNT; ++i)
//                {
//                    if ( pcSafeData->rgpBigimg[i]->m_dwImageSize > 0 )
//                    {
//                        if ( FAILED(pStream->Write(pcSafeData->rgpBigimg[i], pcSafeData->rgpBigimg[i]->m_dwLen + 4, NULL)) )
//                        {
//                            iRst = -1;
//                            break;
//                        }
//                    }
//                    m_dwLastThreadIsOkTime = GetSystemTick();
//                }
//                if ( iRst == -1)
//                {
//                    break;
//                }
//
//                if ( FAILED(pStream->Write(pcSafeData->pPlateNoInfo, pcSafeData->pPlateNoInfo->m_dwLen + 4, NULL)) )
//                {
//                    iRst = -1;
//                    break;
//                }
//                if ( FAILED(pStream->Write(&infoEnd, infoEnd.m_dwLen + 4, NULL)))
//                {
//                    iRst = -1;
//                    break;
//                }
//
//                ++m_SS_RecodeIndex;
//            }
//            else if ( fCanSend && (E_RECORD_NONE == hr) && ComputeHour(m_dwTimeLow, m_dwTimeHigh, dwNowTimeLow, dwNowTimeHigh) )
//            {
//                //如果找不到而且当前时间大于记录时间一个小时,则记录时间加一小时
//                DWORD32 dwMs = 0xFFFFFFFF - (60 * 60 * 1000);
//                if ( dwMs < m_dwTimeLow )
//                {
//                    m_dwTimeHigh++;
//                    m_dwTimeLow = (m_dwTimeLow - dwMs + 1);
//                }
//                else
//                {
//                    m_dwTimeLow += (60 * 60 * 1000);
//                }
//
//                m_SS_RecodeIndex = 0;
//            }
//            else if ( fCanSend && E_RECORD_NONE == hr )
//            {
//                REAL_TIME_STRUCT rtBegin, rtEnd;
//                ConvertMsToTime(m_dwTimeLow, m_dwTimeHigh, &rtBegin);
//                ConvertMsToTime(dwNowTimeLow, dwNowTimeHigh, &rtEnd);
//                if (rtBegin.wYear == rtEnd.wYear && rtBegin.wMonth == rtEnd.wMonth && rtBegin.wDay == rtEnd.wDay && rtBegin.wHour == rtEnd.wHour)
//                {
//                    //读取完毕
//                    HV_Trace(5, "SafeSave Data is Read Over!\n");
//                    m_fSafeEnable = false;
//                }
//            }
//            else if ( fCanSend && E_FAIL == hr )
//            {
//                ++m_SS_RecodeIndex;
//            }
//        }

        //处理实时结果的发送
        if ( SemPend(&m_hSemResult, 500) == S_OK )
        {
            Lan2LedLight();

            SENDSTYLE styleTemp;
            styleTemp.m_wSendType = 0xFFFF;
            if ( !m_queStype.GetFront( styleTemp ) )
            {
                HV_Trace(1, "queStype IsEmpty!\n");
            }

            switch ( styleTemp.m_wSendType )
            {
            case _TYPE_CARINFO_BEGIN:
                iRst = s_queCarInfoBegin.Send( pStream, styleTemp.m_index );
                break;

            case _TYPE_CARINFO_END:
                iRst = s_queCarInfoEnd.Send( pStream, styleTemp.m_index );
                break;

            case _TYPE_CAR_IN_AND_LEFT:
                iRst = s_queCarInLeft.Send( pStream, styleTemp.m_index );
                break;

            case _TYPE_PLATE_STR:
                iRst = s_queCarPlate.Send( pStream, styleTemp.m_index );
                break;

            case _TYPE_BIG_IMAGE:
                iRst = s_queBigImage.Send( pStream, styleTemp.m_index );
                break;

            case _TYPE_SMALL_IMAGE:
                iRst = s_queSmallImage.Send( pStream, styleTemp.m_index );
                break;

            case _TYPE_FULL_CMP_IMAGE:
				iRst = s_queVideo.Send( pStream, styleTemp.m_index );
				break;

            default:
                iRst = 0;
                break;
            }

            if ( iRst == -1 ) //如果出错
            {
                break;	//退出循环
            }
        }
        else
        {
            if (nPendTimes++ > 0)
            {
                Lan2LedLight();
                nPendTimes = 0;

                //发送心跳包
                if (FAILED(pStream->Write((const void*)&m_hvioThrob, nPackLen, NULL)))
                {
                    iRst = 2;
                    break;  //发送失败则退出线程
                }
            }
            else
            {
                Lan2LedLight();
            }
        }
    }

    SetLan2LedOff();

    // 释放队列
    SENDSTYLE styleTemp;
    styleTemp.m_wSendType = 0xFFFF;
    while ( m_queStype.GetFront( styleTemp ) )
    {
        switch ( styleTemp.m_wSendType )
        {
        case _TYPE_CARINFO_BEGIN:
            s_queCarInfoBegin.Release( styleTemp.m_index );
            break;

        case _TYPE_CARINFO_END:
            s_queCarInfoEnd.Release( styleTemp.m_index );
            break;

        case _TYPE_CAR_IN_AND_LEFT:
            s_queCarInLeft.Release( styleTemp.m_index );
            break;

        case _TYPE_PLATE_STR:
            s_queCarPlate.Release( styleTemp.m_index );
            break;

        case _TYPE_BIG_IMAGE:
            s_queBigImage.Release( styleTemp.m_index );
            break;

        case _TYPE_SMALL_IMAGE:
            s_queSmallImage.Release( styleTemp.m_index );
            break;
        case _TYPE_FULL_CMP_IMAGE:
			s_queVideo.Release( styleTemp.m_index );
			break;

        default:
            break;
        }
    }

    if ( INVALID_SOCKET != m_hSocket )
    {
        CloseSocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    //SAFE_DELETE(pcSafeData);
    SAFE_DELETE(m_pcSafeSaver_Dm6467);

    HV_Trace(3, "Data link exit...\n");
    m_fExit = true;
    return (iRst < 0) ? S_FALSE : S_OK;
}

HRESULT CDataLinkThread::Run(void* pvParam)
{
    HRESULT hr = E_FAIL;
    if ( m_hSocket != INVALID_SOCKET )
    {
        hr = ProcessQueue();
    }

    m_fExit = TRUE;
    return hr;
}

bool CDataLinkThread::ThreadIsOk()
{
    if (GetLastStatus() == S_OK)
    {
        return GetSystemTick() - m_dwLastThreadIsOkTime < 60000;
    }
    else
    {
        return true;
    }
}

//--------------------------------------------------------------------------

HRESULT CDataLinkCtrlThread::Run(void* pvParam)
{
    if (m_fInitialized)
    {
        HV_SOCKET_HANDLE m_hSocket = HvCreateSocket();
        if ( INVALID_SOCKET == m_hSocket )
        {
            return E_FAIL;
        }
        else
        {
            if ( S_OK != HvListen(m_hSocket, HV_DATA_PORT, MAX_CONNECT_TEAM_COUNT) )
            {
                return E_FAIL;
            }
        }

        HV_SOCKET_HANDLE hNewSocket;
        while ( !m_fExit )
        {
            m_dwLastThreadIsOkTime = GetSystemTick();

            //移除已断开的“数据连接”
            EnumLock();
            while (TRUE)
            {
                CDataLinkThread* pThread = NULL;
                BOOL fFound = FALSE;
                m_cDataLinkEnum.EnumBase_Reset();
                bool fExsit = false;
                while (S_OK == m_cDataLinkEnum.EnumBase_Next(1,&pThread,NULL))
                {
                    fExsit = true;
                    if (pThread->GetLastStatus() != S_OK)
                    {
                        m_cDataLinkEnum.EnumBase_Remove(pThread);
                        fFound = TRUE;
                        break;
                    }
                    else
                    {
                        m_iClientStatus = 1;
                    }
                }
                if (!fExsit) m_iClientStatus = 0;

                if (!fFound) break;
            }

            int nConCount = 0;
            CDataLinkThread* pThread = NULL;
            m_cDataLinkEnum.EnumBase_Reset();
            while (S_OK == m_cDataLinkEnum.EnumBase_Next(1, &pThread, NULL))
            {
                nConCount++;
            }

            EnumUnLock();

            //如果当前连接数>=指定值，则不再Accept其它连接
            //modify by huanggr 2011-03-23
            if (nConCount >= MAX_CONNECT_TEAM_COUNT)
            {
                HV_Sleep(4000);
                continue;
            }

            hNewSocket = INVALID_SOCKET;
            HRESULT hr = HvAccept(m_hSocket, hNewSocket, 4000);
            if (S_OK == hr)
            {
                HvSetRecvTimeOut(hNewSocket, 4*1000);
                CDataLinkThread* pNewThread = new CDataLinkThread( hNewSocket );
                if (pNewThread == NULL)
                {
                    CloseSocket(hNewSocket);
                }
                else
                {
                    if (FAILED(pNewThread->Start(NULL)))
                    {
                        SAFE_DELETE( pNewThread );
                    }
                    else
                    {
                        EnumLock();
                        m_cDataLinkEnum.EnumBase_Add(pNewThread);
                        EnumUnLock();
                    }
                }
            }
        }

        if ( INVALID_SOCKET != m_hSocket )
        {
            CloseSocket(m_hSocket);
            m_hSocket = INVALID_SOCKET;
        }
    }

    //移除已断开的“数据连接”
    EnumLock();
    while (TRUE)
    {
        CDataLinkThread* pThread = NULL;
        BOOL fFound = FALSE;
        m_cDataLinkEnum.EnumBase_Reset();
        while (S_OK == m_cDataLinkEnum.EnumBase_Next(1,&pThread,NULL))
        {
            if (pThread->GetLastStatus() != S_OK)
            {
                m_cDataLinkEnum.EnumBase_Remove(pThread);

                fFound = TRUE;
                break;
            }
        }

        if (!fFound) break;
    }
    EnumUnLock();

    //停止数据连接
    EnumLock();
    CDataLinkThread* pThread = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next(1,&pThread,NULL) )
    {
        pThread->Stop(-1);
        m_cDataLinkEnum.EnumBase_Remove(pThread);
    }
    EnumUnLock();

    m_fExit = TRUE;
    return S_OK;
}

HRESULT CDataLinkCtrlThread::EnumLock()
{
    SemPend(&m_hSemDataLink);
    return S_OK;
}

HRESULT CDataLinkCtrlThread::EnumUnLock()
{
    SemPost(&m_hSemDataLink);
    return S_OK;
}

HRESULT CDataLinkCtrlThread::SendInfoBegin(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_CarInfoBegin* pCarInfoBegin = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pCarInfoBegin = CDataLinkThread::s_queCarInfoBegin.GetReference(index);
        pThread->SendInfoBegin( pCarInfoBegin, index, dwFlag );
    }

    EnumUnLock();

    CDataLinkThread::s_queCarInfoBegin.Release(index);
    return S_OK;
}

//用户接口：发送车辆信息结束接收标志
HRESULT CDataLinkCtrlThread::SendInfoEnd(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_CarInfoEnd* pCarInfoEnd = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pCarInfoEnd = CDataLinkThread::s_queCarInfoEnd.GetReference(index);
        pThread->SendInfoEnd( pCarInfoEnd, index, dwFlag );
    }

    EnumUnLock();

    CDataLinkThread::s_queCarInfoEnd.Release(index);
    return S_OK;
}

//用户接口：发送车辆到达离开标志
HRESULT CDataLinkCtrlThread::SendInLeftFlag(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_CarInLeft* pCarInLeft = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pCarInLeft = CDataLinkThread::s_queCarInLeft.GetReference(index);
        pThread->SendInLeftFlag( pCarInLeft, index, dwFlag );
    }

    EnumUnLock();

    CDataLinkThread::s_queCarInLeft.Release(index);
    return S_OK;
}

//用户接口：发送车牌
HRESULT CDataLinkCtrlThread::SendCarPlate(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_CarPlate* pCarPlate = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pCarPlate = CDataLinkThread::s_queCarPlate.GetReference(index);
        pThread->SendCarPlate( pCarPlate, index, dwFlag );
    }

    EnumUnLock();

    CDataLinkThread::s_queCarPlate.Release(index);
    return S_OK;
}

//用户接口：发送车辆YUV大图
HRESULT CDataLinkCtrlThread::SendBigImage(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_BigImage* pBigImge = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pBigImge = CDataLinkThread::s_queBigImage.GetReference(index);
        pThread->SendBigImage(pBigImge, index, dwFlag);
    }

    EnumUnLock();

    CDataLinkThread::s_queBigImage.Release(index);
    return S_OK;
}

//发送YUV小图
HRESULT CDataLinkCtrlThread::SendSmallImageYUV(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_SmallImage* pSmallImage = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pSmallImage = CDataLinkThread::s_queSmallImage.GetReference(index);
        pThread->SendSmallImageYUV( pSmallImage, index, dwFlag );
    }

    EnumUnLock();

    CDataLinkThread::s_queSmallImage.Release(index);
    return S_OK;
}

//发送二值化小图
HRESULT CDataLinkCtrlThread::SendBinaryImage(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

    CDataLinkThread* pThread = NULL;
    HVIO_SmallImage* pSmallImage = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        pSmallImage = CDataLinkThread::s_queSmallImage.GetReference(index);
        pThread->SendBinaryImage( pSmallImage, index, dwFlag );
    }

    EnumUnLock();

    CDataLinkThread::s_queSmallImage.Release(index);
    return S_OK;
}

//设置过滤条件
HRESULT CDataLinkCtrlThread::SetStreamID(
    DWORD32 dwIP,
    WORD16 wPort,
    WORD16 wVideoID,
    WORD16 wStreamID
)
{
    HRESULT hr = S_FALSE;

    EnumLock();

    CDataLinkThread* pThread = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        hr = pThread->SetStreamID(
                 dwIP,
                 wPort,
                 wVideoID,
                 wStreamID
             );

        if (hr == S_OK) break;
    }

    EnumUnLock();

    return hr;
}

//删除过滤条件
HRESULT CDataLinkCtrlThread::DeleteStreamID(
    DWORD32 dwIP,
    WORD16 wPort,
    WORD16 wVideoID,
    WORD16 wStreamID
)
{
    HRESULT hr = S_FALSE;

    EnumLock();

    CDataLinkThread* pThread = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        hr = pThread->DeleteStreamID(
                 dwIP,
                 wPort,
                 wVideoID,
                 wStreamID
             );

        if (hr == S_OK) break;
    }

    EnumUnLock();

    return hr;
}

//清除所有过滤条件
HRESULT CDataLinkCtrlThread::ClearAllStreamID(
    DWORD32 dwIP,
    WORD16 wPort
)
{
    HRESULT hr = S_FALSE;

    EnumLock();

    CDataLinkThread* pThread = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        hr = pThread->ClearAllStreamID(
                 dwIP,
                 wPort
             );

        if (hr == S_OK) break;
    }

    EnumUnLock();

    return hr;
}

//用户接口：发送车牌附加信息
HRESULT CDataLinkCtrlThread::SendAppendInfo(
    const int& index,
    DWORD32 dwFlag
)
{
    return E_NOTIMPL;
}

//发送视频
HRESULT CDataLinkCtrlThread::SendVideoYUV(
    const int& index,
    DWORD32* pdwSendCount,
    DWORD32 dwFlag
)
{
    EnumLock();

	DWORD32 dwSend = 0;
	HVIO_Video* pVideo = NULL;

	CDataLinkThread* pThread = NULL;
	m_cDataLinkEnum.EnumBase_Reset();
	while(S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
	{
		pVideo = CDataLinkThread::s_queVideo.GetReference(index);
		if (SUCCEEDED(pThread->SendVideoYUV( pVideo, index, pdwSendCount, dwFlag )))
		{
			dwSend++;
		}
	}

	EnumUnLock();

	*pdwSendCount = dwSend;
	CDataLinkThread::s_queVideo.Release(index);
	return S_OK;
}

//用户接口：发送字符流
HRESULT CDataLinkCtrlThread::SendString(
    const int& index,
    DWORD32 dwFlag
)
{
    EnumLock();

	CDataLinkThread* pThread = NULL;
	HVIO_String* pString = NULL;
	m_cDataLinkEnum.EnumBase_Reset();
	while(S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
	{
		pString = CDataLinkThread::s_queString.GetReference(index);
		pThread->SendString( pString, index, dwFlag );
	}

	EnumUnLock();

	CDataLinkThread::s_queString.Release(index);
	return S_OK;
}

//用户接口：发送二进制流
HRESULT CDataLinkCtrlThread::SendBin(
    const int& index,
    DWORD32 dwFlag
)
{
    return E_NOTIMPL;
}

HRESULT CDataLinkCtrlThread::SetSafeMode(
    DWORD32 dwIP,
    WORD16 wPort,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    int index,
    int iPageIndex /*= -1*/,
    int iPageSize /*= 0*/
)
{
    HRESULT hr = E_FAIL;

    EnumLock();

    CDataLinkThread* pThread = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while ( S_OK == m_cDataLinkEnum.EnumBase_Next(1, &pThread, NULL) )
    {
        hr = pThread->SetSafeMode(dwIP, wPort,
                                  dwTimeLow, dwTimeHigh, index,
                                  iPageIndex, iPageSize);

        if (hr == S_OK) break;
    }

    EnumUnLock();

    return hr;
}

//查询AVI文件
HRESULT CDataLinkCtrlThread::GetAviByTime(
    DWORD32 dwIP,
    WORD16 wPort,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    DWORD32 *pdwAviTimeLow,
    DWORD32 *pdwAviTimeHigh,
    DWORD32 *pdwAviLen
)
{
    return E_NOTIMPL;
}

//查询硬盘录像
HRESULT CDataLinkCtrlThread::GetHistoryVideo(
    DWORD32 dwIP,
    WORD16 wPort,
    DWORD32 dwTimeLow,
    DWORD32 dwTimeHigh,
    DWORD32 dwTimeSliceS
)
{
    return E_NOTIMPL;
}

bool CDataLinkCtrlThread::ThreadIsOk()
{
    if (m_fExit)
    {
        return false;
    }
    bool fOk = true;
    EnumLock();
    CDataLinkThread* pThread = NULL;
    m_cDataLinkEnum.EnumBase_Reset();
    while (S_OK == m_cDataLinkEnum.EnumBase_Next( 1, &pThread, NULL ))
    {
        fOk = pThread->ThreadIsOk();
        if (fOk)
        {
            break;
        }
    }
    EnumUnLock();
    return fOk;
}

int CDataLinkCtrlThread::GetClientStatus()
{
    return m_iClientStatus;
}
