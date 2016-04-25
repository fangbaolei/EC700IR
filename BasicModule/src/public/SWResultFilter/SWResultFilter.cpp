//该文件编码格式必须为WINDOWS-936格式
#include "swpa.h"
#include "SWFC.h"
#include "SWResultFilter.h"
#include "SWCarLeft.h"
#include "SWMessage.h"

#ifndef VK_SPACE
#define VK_SPACE 0x20
#endif
#ifndef VK_TAB
#define VK_TAB 0x09
#endif

//////////////////////////////////////////////////////////////////////////////////////
//时间格式类
///////////////////////////////////////////////////////////////////////////////////////
CRuleTime::CRuleTime()
            : m_iHour(0)
            , m_iMinute(0)
            , m_iSecond(0)
{
}

CRuleTime::~CRuleTime()
{
}

BOOL CRuleTime::SetTime( const INT iHour, const INT iMinute, const INT iSecond )
{
    m_iHour = (iHour < 0 || iHour > 23) ? 0 : iHour;
    m_iMinute = (iMinute < 0 || iMinute > 59) ? 0 : iMinute;
    m_iSecond = (iSecond < 0 || iSecond > 59) ? 0 : iSecond;

    return (iHour < 0 || iHour > 23 || iMinute < 0 || iMinute > 59 || iSecond < 0 || iSecond > 59) ? false : true;
}

BOOL CRuleTime::SetTime( LPCSTR lpszTime )
{
    BOOL fRet = true;

    m_iHour = m_iMinute = m_iSecond = 0;
    if ( 3 != sscanf(lpszTime, "%d:%d:%d", &m_iHour, &m_iMinute, &m_iSecond) )
    {
        m_iHour = m_iMinute = m_iSecond = 0;
        fRet = false;
    }
    else
    {
        fRet = SetTime( m_iHour, m_iMinute, m_iSecond );
    }

    return fRet;
}

BOOL CRuleTime::operator == (const CRuleTime& crt) const
{
    return (m_iHour == crt.GetHour() && m_iMinute == crt.GetMinute() && m_iSecond == crt.GetSecond()) ? true : false;
}

BOOL CRuleTime::operator > (const CRuleTime& crt) const
{
    if ( m_iHour > crt.GetHour() )
    {
        return true;
    }
    else if ( m_iHour == crt.GetHour() )
    {
        if ( m_iMinute > crt.GetMinute() )
        {
            return true;
        }
        else if ( m_iMinute == crt.GetMinute() )
        {
            if ( m_iSecond > crt.GetSecond() && m_iSecond != crt.GetSecond() )
            {
                return true;
            }
        }
    }

    return false;
}

BOOL CRuleTime::operator < (const CRuleTime& crt) const
{
    return (*this > crt || *this == crt) ? false : true;
}

BOOL CRuleTime::operator >= (const CRuleTime& crt) const
{
    return (*this > crt || *this == crt) ? true : false;
}

BOOL CRuleTime::operator <= (const CRuleTime& crt) const
{
    return (*this < crt || *this == crt) ? true : false;
}

BOOL CRuleTime::IsCourseOfTime( const CRuleTime& crtLeft, const CRuleTime& crtRight, const CRuleTime& crtNow )
{
    BOOL fRet = true;
    if ( crtRight >= crtLeft )
    {
        if ( crtNow < crtLeft || crtNow > crtRight )
        {
            fRet = false;
        }
    }
    else if ( crtRight < crtLeft )
    {
        if ( crtNow > crtRight && crtNow < crtLeft )
        {
            fRet = false;
        }
    }
    return fRet;
}

///////////////////////////////////////////////////////////////////////////////////////
//                        后处理类
///////////////////////////////////////////////////////////////////////////////////////////

const INT g_cbMaxRead = 1024 * 32;

//后处理字段名
const char* g_szLeach = "过滤规则";
const char* g_szReplace = "替换规则";
const char* g_szCompaty = "通配符规则";


//全角转半角
BOOL SBC2DBC( CSWString& str )
{
    if ( str.IsEmpty() )
    {
        return TRUE;
    }

    INT iLen = str.Length();
    PBYTE pszTemp = NULL;
    pszTemp = new BYTE[ iLen + 1 ];
    if ( pszTemp == NULL )
    {
        return FALSE;
    }
    INT iPos = 0;
    BYTE ch = '\0';
    for ( INT i = 0; i < iLen; ++i )
    {
        ch = str[i];
        if ( ch != 163 )
        {
            if ( ch > 127 )
            {
                pszTemp[ iPos++ ] = ch;
                pszTemp[ iPos++ ] = str[++i];
            }
            else
            {
                pszTemp[ iPos++ ] = ch;
            }
        }
        else
        {
            ++i;
            if ( i >= iLen )
            {
                break;
            }
            ch = str[i];
            pszTemp[ iPos++ ]  = ch - 128;
        }
    }
    pszTemp[ iPos ] = '\0';
    str = (char*)pszTemp;
    delete[] pszTemp;
    return TRUE;
}

//得到当前时间
void GetTime( CRuleTime& crtime )
{
    CSWDateTime dtNow;
    SWPA_DATETIME_TM tm;
    dtNow.GetTime(&tm);

    crtime.SetTime(tm.hour, tm.min, tm.sec);
}

CSWResultFilter::CSWResultFilter(void)
:CSWBaseFilter(1,1), CSWMessage(MSG_RESULT_FILTER_BEGIN, MSG_RESULT_FILTER_END)
{
    m_fInitialize = FALSE;
    m_pProcessRule = NULL;
    
    GetIn(0)->AddObject(CLASSID(CSWCarLeft));
    GetOut(0)->AddObject(CLASSID(CSWCarLeft));
}

CSWResultFilter::CSWResultFilter(PROCESS_RULE *pRule)
:CSWBaseFilter(1,1), CSWMessage(MSG_RESULT_FILTER_BEGIN, MSG_RESULT_FILTER_END)
{
    m_fInitialize = FALSE;
    m_pProcessRule = NULL;
    
    InitRule(pRule);
    
    GetIn(0)->AddObject(CLASSID(CSWCarLeft));
    GetOut(0)->AddObject(CLASSID(CSWCarLeft));
}

CSWResultFilter::~CSWResultFilter(void)
{
}

HRESULT CSWResultFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWCarLeft, obj))
	{
		CSWCarLeft *pCarLeft = (CSWCarLeft *)obj;

		// todo.
		// 此处调用是为了保证后处理之前违章类型已经是正确的。
		// 卡口下有部分违章要调用此接口才生成。
		CSWString strTemp;
		pCarLeft->GetEventDetInfo(strTemp);

		RESULT_INFO resultInfo;
		resultInfo.strPlate = pCarLeft->GetPlateNo();
		// 后处理用的车牌符加信息字符串不是XML格式。
		resultInfo.strOther = pCarLeft->BuildNormalString();
		resultInfo.strApplied = "";
		resultInfo.iPT = pCarLeft->GetPTType();

		if(FilterProcess(&resultInfo))
		{
			SW_TRACE_DEBUG("<Filter>replace plate info:%s",(LPCSTR)resultInfo.strApplied);
			pCarLeft->SetPlateNo(resultInfo.strPlate);			
		}
		else
		{
			SW_TRACE_DEBUG("replace plate error.");
		}

		// 输出后处理信息
		if( NULL != m_pProcessRule && m_pProcessRule->fOutPutFilterInfo )
		{
			pCarLeft->SetFilterInfo(resultInfo.strApplied);
		}

		// 如果过滤的类型是2，即只会过滤违章信息，结果要输出。
		if( resultInfo.iLeachType == 2 )
		{
			INT iPTType = pCarLeft->GetPTType();
			SW_TRACE_DEBUG("<Filter>pt %d->%d.", pCarLeft->GetPTType(), (INT)resultInfo.iPT);
			pCarLeft->SetPTType(resultInfo.iPT);
			// 处理大图。
			// 如果过滤前是违章而且不违章不输出三张轨迹图。
			if( iPTType != PT_NORMAL && resultInfo.iPT == PT_NORMAL && CSWCarLeft::GetOutputCaptureImage() != 1 )
			{
				CSWImage* pImageBest = pCarLeft->GetImage(2);
				CSWImage* pImageLast = pCarLeft->GetImage(3);
				if( CSWCarLeft::GetOutputBestImage() == 1 && pImageBest != NULL)
				{
					pCarLeft->SetImage(0, pImageBest);
				}
				if( CSWCarLeft::GetOutputLastImage() == 1 && pImageLast != NULL )
				{
					pCarLeft->SetImage(1, pImageLast);
				}
				pCarLeft->SetImage(2, NULL);
				pCarLeft->SetImage(3, NULL);
				pCarLeft->SetImage(4, NULL);
			}
		}

		if( resultInfo.iLeachType != 1 )
		{	//压缩前对CarLeft进行锁,变成只读
			pCarLeft->Done();
			CSWImage *pImage = NULL;
			SW_RECT rcRect;
			for(int i = 0; i <  pCarLeft->GetImageCount() - 2; i++)
			{
				if( (pImage = pCarLeft->GetImage(i, &rcRect)) != NULL )
				{
				    BOOL fIsTestImage = FALSE;
                    if( swpa_strcmp("VPIF", pImage->GetFrameName()) != 0 )
                    {
                        fIsTestImage = TRUE;
                    }

					//替换成降噪的图像
					if (FALSE == fIsTestImage 
						&& SENSOR_ICX816 == swpa_get_sensor_type())
					{
						if (S_OK != pImage->ReplaceFrameBuffer(CSWMemoryFactory::GetInstance(SW_SHARED_MEMORY)))
						{
							//SAFE_RELEASE(pImage);
							SW_TRACE_DEBUG("result get vnf frame failed, release the image and SetImage = NULL!\n");
                            pCarLeft->SetImage(i, NULL, NULL);
							continue;
						}
					}

					//TODO测试协议待测试
					//如果是jpeg就跳过jpeg编码
					//if(pImage->GetType() == SW_IMAGE_JPEG)
					//	continue;

                    //fanghy Crop test
                    /*if (CSWCarLeft::GetOutputCropImage() && 1 == i)
                    {
                         CSWImage* pImageCrop = NULL;
                        pImageCrop = pCarLeft->GetImage(i, &rcRect) ;
                        if (pImageCrop != NULL)     // 截取最后大图
                        {
                            pCarLeft->m_fCropLastImage = TRUE;
                            if( S_OK != CSWMessage::SendMessage(MSG_JPEGENCODE, (WPARAM)&pImageCrop, (LPARAM)pCarLeft))
                            {
                                pCarLeft->SetImage(0, NULL);
                                SW_TRACE_DEBUG("CropTest: JPEGENCODE_CROP fail.\n");
                            }
                            else
                            {
                                SW_TRACE_DEBUG("CropTest: JPEGENCODE_CROP OK.\n");
                                pCarLeft->SetImage(0, pImageCrop);
                            }
                            pCarLeft->m_fCropLastImage = FALSE;
                        }
                        SAFE_RELEASE(pImageCrop);
                    }*/

                    if(fIsTestImage == FALSE && pImage->GetType() == SW_IMAGE_YUV420SP)
                    {
                        CSWMessage::SendMessage(MSG_OVERLAY_PROCESS, (WPARAM)pImage,(LPARAM)pCarLeft);
                    }

                    //pCarLeft->m_fCropLastImage = FALSE;
                    //如果压缩失败，则把图片设置为空。
                    if( S_OK != CSWMessage::SendMessage(MSG_JPEGENCODE, (WPARAM)&pImage, (LPARAM)pCarLeft))
                    {
                        SW_TRACE_DEBUG("CSWResultFilter MSG_JPEGENCODE Fail!\n");
                        pCarLeft->SetImage(i, NULL,NULL);
                    }
                    else
					{
                        // 注释，可能影响到截图功能
                        {
//					    // 一体机协议才做防篡改加密
//                            if (fIsTestImage == FALSE)
//                            {
//                                //防伪码
//                                BYTE pbSecurityCode[16];
//                                pImage->GetSecurityCode(pbSecurityCode,sizeof(pbSecurityCode));
//                                SW_COMPONENT_IMAGE sComponentImage;
//                                pImage->GetImage(&sComponentImage);
//                                PBYTE pJPEGDataEnd = pImage->GetImageBuffer() + sComponentImage.iSize;
//                                swpa_memcpy(pJPEGDataEnd,pbSecurityCode,sizeof(pbSecurityCode));
//                                sComponentImage.iSize += sizeof(pbSecurityCode);
//                                pImage->SetImage(sComponentImage);

//                                //防篡改
//                                int iImgLen = pImage->GetSize();
//                                if( S_OK == CSWMessage::SendMessage(MSG_MD5_ENCRYPT_PROCESS, (WPARAM)pImage->GetImageBuffer(), (WPARAM)&iImgLen))
//                                {
//                                    SW_COMPONENT_IMAGE sNewComponentImage;
//                                    pImage->GetImage(&sNewComponentImage);
//                                    sNewComponentImage.iSize = iImgLen;
//                                    pImage->SetImage(sNewComponentImage);

//                                    // 一体机协议并且有字符叠加时需要校正车牌坐标
//                                    //如果全是0的情况就不用矫正
//    #if 0
//                                    if (sNewComponentImage.iHeight > 1080 &&
//                                        (rcRect.left != 0 &&
//                                         rcRect.top != 0 &&
//                                         rcRect.right != 0 &&
//                                         rcRect.bottom != 0))
//                                    {
//                                        if (m_pProcessRule->nOverlayY == 0)
//                                        {
//                                            rcRect.top = ((rcRect.top * 1080) / 100 + (sNewComponentImage.iHeight - 1080) + 5) * 100 / sNewComponentImage.iHeight;
//                                            rcRect.bottom = ((rcRect.bottom * 1080) / 100 + (sNewComponentImage.iHeight - 1080) + 5) * 100 / sNewComponentImage.iHeight;
//                                        }
//                                        else if (m_pProcessRule->nOverlayY == 1080)
//                                        {
//                                            rcRect.top = ((rcRect.top * 1080) / 100) * 100 / sNewComponentImage.iHeight;
//                                            rcRect.bottom = ((rcRect.bottom * 1080) / 100) * 100 / sNewComponentImage.iHeight;
//                                        }
//                                    }
//    #endif
//                                }
//                                else
//                                {
//                                    SW_TRACE_NORMAL("图片加密失败!!!!\n");
//                                }
//                            }
                        }

                        SW_TRACE_DEBUG("CSWResultFilter MSG_JPEGENCODE OK!\n");

						pCarLeft->SetImage(i, pImage, &rcRect);
						SAFE_RELEASE(pImage);
					}
				}	
			}
			GetOut(0)->Deliver(pCarLeft);
		}
		else
		{
			SW_TRACE_DEBUG("<Filter>leach plate %s.", (LPCSTR)pCarLeft->GetPlateNo());
		}
	}
	return S_OK;
}

//初始化规则
HRESULT CSWResultFilter::InitRule(PVOID pProcessRule)
{
    m_pProcessRule = (PROCESS_RULE *)pProcessRule;
    if ( InitLeach() && InitReplace() && InitCompaty() )
    {
        m_fInitialize = TRUE;
    }
    else
    {
        m_fInitialize = FALSE;
    }

    return m_fInitialize ? S_OK : E_FAIL;
}

//执行后处理
BOOL CSWResultFilter::FilterProcess(RESULT_INFO* pResultInfo)
{
    if ( pResultInfo == NULL ) return FALSE;

    m_strApplied = "";
    m_strOther = pResultInfo->strOther;

	INT iPT = pResultInfo->iPT;

    if ( m_pProcessRule->fReplace )
    {
		iPT = pResultInfo->iPT;
        if ( !ReplacePlate( pResultInfo->strPlate, m_pProcessRule->fLoopReplace, iPT ) ) return FALSE;
    }
    if ( m_pProcessRule->fLeach )
    {
		iPT = pResultInfo->iPT;
        pResultInfo->iLeachType = IsNeedLeach( pResultInfo->strPlate, iPT );
		pResultInfo->iPT = iPT;
    }

    pResultInfo->strApplied = m_strApplied;

    return TRUE;
}

HRESULT GetFileSection(char *pbBuffer, char *lpszOutStr, INT iLen)
{
    if (!pbBuffer || !lpszOutStr)
    {
        return E_POINTER;
    }

    INT cbRead = swpa_strlen(pbBuffer), iRet;

    if ( cbRead > 1 )
    {
        INT iPos = 0;
        if ( pbBuffer[0] == '$' )
        {
            iPos = 1;
        }
        iRet = iLen > (INT)cbRead ? cbRead : iLen;
        if ( pbBuffer[cbRead - 1] == '$' )
        {
            iRet -= iPos + 1;
        }
        else
        {
            iRet -= iPos;
        }

        swpa_memset(lpszOutStr, 0, iLen);
        swpa_strncpy(lpszOutStr, (const char*)&(pbBuffer[iPos]), iRet);
        iPos = 0;
        while ( lpszOutStr[iPos] != '\0' && iPos < iLen )
        {
            if ( lpszOutStr[iPos] == '$' )
            {
                lpszOutStr[iPos] = '\0';
            }
            ++iPos;
        }
    }
    return S_OK;
}

//规则库的初始化
BOOL CSWResultFilter::InitLeach()
{
    m_lstLeach.RemoveAll();

    char *buf = m_pProcessRule->szLeachRule;
    char *pszOutput = (char *)swpa_mem_alloc(MAX_OUTPUT_BUF_LEN);
	swpa_memset(pszOutput, 0, MAX_OUTPUT_BUF_LEN);

    if (S_OK != GetFileSection(buf, pszOutput, MAX_OUTPUT_BUF_LEN))
    {
        swpa_mem_free(pszOutput);
        return FALSE;
    }

    CSWString strResult = pszOutput;
    LEACH_INFO leachinfo;

    INT iPos1 = 0;
    INT iPos2 = 0;
    INT iLen = strResult.Length() + 1;

    strResult.Remove( VK_TAB );
    strResult.Remove( VK_SPACE );
    while ( !strResult.IsEmpty() )
    {
        iPos1 = strResult.Find( "=" );
        iPos2 = strResult.Find( ";" );
        if ( iPos1 != -1 )
        {
            leachinfo.strPattern = strResult.Substr(0, iPos1 );
            INT iTmp = 0;
            INT iLeach = 0;
            if ( iPos2 != -1 )
            {
                iTmp = sscanf( strResult.Substr( iPos1+1, iPos2-iPos1-1 ),
                               "%d", &iLeach );
                leachinfo.strComment = strResult.Substr( iPos2 );
            }
            else
            {
                iTmp = sscanf( strResult.Substr( iPos1+1 ),
                               "%d", &iLeach );
                leachinfo.strComment = "";
            }

            if ( iTmp == 1 && !leachinfo.strPattern.IsEmpty() )
            {
                leachinfo.fLeach = (iLeach == 1);
				leachinfo.iLeachType = iLeach;
				if( iLeach < 0 || iLeach > 2 )
				{
					leachinfo.iLeachType = 0;
				}
				
                SBC2DBC( leachinfo.strPattern );
                SBC2DBC( leachinfo.strComment );

                //避免规则的重复
                SW_POSITION hvpos = m_lstLeach.GetHeadPosition();
                LEACH_INFO infoTmp;
                BOOL fHas = FALSE;
                while (m_lstLeach.IsValid(hvpos))
                {
                    infoTmp = m_lstLeach.GetNext(hvpos);
                    if ( infoTmp.strPattern == leachinfo.strPattern )
                    {
                        fHas = TRUE;
                        break;
                    }
                }
                if ( !fHas )
                {
                    m_lstLeach.AddTail(leachinfo);
                }
            }
        }
        strResult = ( pszOutput + iLen );
        iLen += strResult.Length() + 1;
        strResult.Remove( VK_TAB );
        strResult.Remove( VK_SPACE );
    }

    swpa_mem_free(pszOutput);

    return TRUE;
}

BOOL CSWResultFilter::InitReplace()
{
    m_lstReplace.RemoveAll();

    char *buf = m_pProcessRule->szReplaceRule;
    char *pszOutput = (char *)swpa_mem_alloc(MAX_OUTPUT_BUF_LEN);
	swpa_memset(pszOutput, 0, MAX_OUTPUT_BUF_LEN);

    if (S_OK != GetFileSection(buf, pszOutput, MAX_OUTPUT_BUF_LEN))
    {
        swpa_mem_free(pszOutput);
        return FALSE;
    }

    CSWString strResult = pszOutput;
    REPLACE_INFO replaceinfo;
    INT iPos1 = 0;
    INT iPos2 = 0;
    INT iLen = strResult.Length() + 1;

    strResult.Remove( VK_TAB );
    strResult.Remove( VK_SPACE );

    while ( !strResult.IsEmpty() )
    {
        iPos1 = strResult.Find( "=" );
        iPos2 = strResult.Find( ";" );
        if ( iPos1 != -1 )
        {
            replaceinfo.strPattern = strResult.Substr(0, iPos1 );
            if ( iPos2 != -1 )
            {
                replaceinfo.strResult = strResult.Substr( iPos1+1, iPos2-iPos1-1 );
                replaceinfo.strComment = strResult.Substr( iPos2 );
            }
            else
            {
                replaceinfo.strResult  = strResult.Substr( iPos1+1 );
                replaceinfo.strComment = "";
            }
            if ( !replaceinfo.strPattern.IsEmpty()
                    && !replaceinfo.strResult.IsEmpty() )
            {
                SBC2DBC( replaceinfo.strPattern );
                SBC2DBC( replaceinfo.strResult );
                SBC2DBC( replaceinfo.strComment );

                //避免规则的重复
                SW_POSITION hvpos = m_lstReplace.GetHeadPosition();
                REPLACE_INFO infoTmp;
                BOOL fHas = FALSE;
                while (m_lstReplace.IsValid(hvpos))
                {
                    infoTmp = m_lstReplace.GetNext(hvpos);
                    if ( infoTmp.strPattern == replaceinfo.strPattern )
                    {
                        fHas = TRUE;
                        break;
                    }
                }
                if ( !fHas )
                {
                    m_lstReplace.AddTail(replaceinfo);
                }
            }
        }
        strResult = ( pszOutput + iLen );
        iLen += strResult.Length() + 1;

        strResult.Remove( VK_TAB );
        strResult.Remove( VK_SPACE );
    }

    swpa_mem_free(pszOutput);

    return TRUE;
}

BOOL CSWResultFilter::InitCompaty()
{
    m_lstCompaty.RemoveAll();

    char *buf = m_pProcessRule->szCompatyRule;
    char *pszOutput = (char *)swpa_mem_alloc(MAX_OUTPUT_BUF_LEN);
	swpa_memset(pszOutput, 0, MAX_OUTPUT_BUF_LEN);

    if (S_OK != GetFileSection(buf, pszOutput, MAX_OUTPUT_BUF_LEN))
    {
        swpa_mem_free(pszOutput);
        return FALSE;
    }

    CSWString strResult = pszOutput;
    COMPATY_WORD compatyword;
    INT iPos1 = 0;
    INT iPos2 = 0;
    INT iLen = strResult.Length() + 1;

    strResult.Remove( VK_TAB );
    strResult.Remove( VK_SPACE );
    while ( !strResult.IsEmpty() )
    {
        iPos1 = strResult.Find( "=" );
        iPos2 = strResult.Find( ";" );
        if ( iPos1 != -1 )
        {
            compatyword.strPattern = strResult.Substr(0, iPos1 );
            if ( iPos2 != -1 )
            {
                compatyword.strResult  = strResult.Substr( iPos1+1, iPos2-iPos1-1 );
            }
            else
            {
                compatyword.strResult  = strResult.Substr( iPos1+1 );
            }
            if ( !compatyword.strPattern.IsEmpty()
                    && !compatyword.strResult.IsEmpty() )
            {
                SBC2DBC( compatyword.strPattern );
                SBC2DBC( compatyword.strResult );

                //避免规则的重复
                SW_POSITION hvpos = m_lstCompaty.GetHeadPosition();
                COMPATY_WORD infoTmp;
                BOOL fHas = FALSE;
                while (m_lstCompaty.IsValid(hvpos))
                {
                    infoTmp = m_lstCompaty.GetNext(hvpos);
                    if ( infoTmp.strPattern == compatyword.strPattern )
                    {
                        fHas = TRUE;
                        break;
                    }
                }
                if ( !fHas )
                {
                    m_lstCompaty.AddTail(compatyword);
                }
            }
        }
        strResult = ( pszOutput + iLen );
        iLen += strResult.Length() + 1;

        strResult.Remove( VK_TAB );
        strResult.Remove( VK_SPACE );
    }

    swpa_mem_free(pszOutput);

    return TRUE;
}

//车牌替换
BOOL CSWResultFilter::ReplacePlate( CSWString& strPlate, const BOOL& fLoopReplace, INT& iPT )
{
    if ( !m_fInitialize ) return FALSE;
    if ( strPlate.IsEmpty() ) return FALSE;
    if ( m_lstReplace.IsEmpty() ) return TRUE;

    REPLACE_INFO replaceinfo;
    REPLACE_INFO bestinfo;
    CSWString strSrc;
    CSWString strDest(strPlate);
    CSWString strTmp;

    CRuleTime crtime;
    GetTime(crtime);

    CSWString strCur("*");
    INT iPos = -1;

    SW_POSITION position = m_lstReplace.GetHeadPosition();
    while (m_lstReplace.IsValid(position))
    {
        replaceinfo = m_lstReplace.GetNext(position);
        strSrc = replaceinfo.strPattern;
        if ( Match( strSrc, strDest ) )
        {
            if ( !replaceinfo.strComment.IsEmpty() )
            {
                if ( !(CheckRuleTimer( replaceinfo.strComment, crtime )
                        && CheckRuleRely( replaceinfo.strComment, m_strOther )
                        && CheckPlateType( replaceinfo.strComment, m_strOther )
                        && CheckPlateSpeed( replaceinfo.strComment, m_strOther )
                        && CheckRoadNumber(replaceinfo.strComment, m_strOther ) 
						&& CheckPT(replaceinfo.strComment, iPT) ) )
                {
                    continue;
                }
            }
            //是否是循环替换方式
            if ( fLoopReplace )
            {
                DoReplace(strDest, replaceinfo);
            }
            else
            {
                INT iTmp = RuleCompare( strSrc, strCur );
                if ( iTmp == 2 || iTmp == 0 )
                {
                    strCur = strSrc;
                    bestinfo = replaceinfo;
                    iPos = 0;
                }
            }
        }
    }

    if ( !fLoopReplace && iPos != -1)
    {
        DoReplace(strDest, bestinfo);
    }

    strPlate = strDest;

    return TRUE;
}

void CSWResultFilter::DoReplace(CSWString& strPlate, REPLACE_INFO& replaceinfo)
{
    BYTE bTemp = 0;
    CSWString strResult(replaceinfo.strResult);
    CSWString strConv;
    CSWString strDest(strPlate);
    for (INT j=0, i=0; i<strResult.Length() && j<strDest.Length(); j++, ++i)
    {
        if (strResult[i]=='?')
        {
            strConv += strDest[j];
            bTemp = strDest[j];
            if ( (bTemp & 0x80) == 0x80 )
            {
                strConv += strDest[++j];
            }
            continue;
        }
        if (strResult[i]=='*')
        {
            strConv += strDest.Substr(j);
            break;
        }

        strConv += strResult[i];
        bTemp = strResult[i];
        if ( (bTemp & 0x80) == 0x80 )
        {
            strConv += strResult[++i];
        }
        bTemp = strDest[j];
        if ( (bTemp & 0x80) == 0x80 )
        {
            ++j;
        }
    }
    strDest = strConv;

    //通配符替换
    CSWString strPlateTmp;
    CSWString strWord;
    char szTmp[8] ={0};
    for ( INT iStr = 0; iStr < strDest.Length(); ++iStr)
    {
        szTmp[0] = strDest[iStr];
        strWord = szTmp;
        SW_POSITION position = m_lstCompaty.GetHeadPosition();
        while (m_lstCompaty.IsValid(position))
        {
            COMPATY_WORD compatyword = m_lstCompaty.GetNext(position);
            if ( strDest[iStr] == compatyword.strPattern[0] )
            {
                strWord = compatyword.strResult;
                break;
            }
        }
        strPlateTmp += strWord;
    }
    strDest = strPlateTmp;

    CSWString strTmp;
    strTmp.Format("(%s->%s)[%s=%s]", (LPCSTR)strPlate, (LPCSTR)strDest, (LPCSTR)replaceinfo.strPattern, (LPCSTR)replaceinfo.strResult);
    strPlate = strDest;
    if(m_strApplied.IsEmpty())
    {
    	m_strApplied = "车牌替换:";
    }
    m_strApplied.Append( strTmp );
}

//是否要过滤
INT CSWResultFilter::IsNeedLeach(CSWString& strPlate, INT& iPT)
{
    if ( !m_fInitialize ) return FALSE;
    if ( strPlate.IsEmpty() ) return FALSE;
    if ( m_lstLeach.IsEmpty() ) return FALSE;

	INT iRet = 0;
    CSWString strSrc;
    CSWString strDest(strPlate);

    LEACH_INFO leachinfo;
    CSWString strCur("*");

    CRuleTime crtime;
    GetTime(crtime);

    SW_POSITION position = m_lstLeach.GetHeadPosition();
    while (m_lstLeach.IsValid(position))
    {
        leachinfo = m_lstLeach.GetNext(position);
        strSrc = leachinfo.strPattern;

        if ( Match( strSrc, strDest ) )
        {
            if ( !leachinfo.strComment.IsEmpty() )
            {
                if ( !(CheckRuleTimer( leachinfo.strComment, crtime )
                        && CheckRuleRely( leachinfo.strComment, m_strOther )
                        && CheckPlateType( leachinfo.strComment, m_strOther )
                        && CheckPlateSpeed(leachinfo.strComment, m_strOther )
                        && CheckReverseRun(leachinfo.strComment, m_strOther )
                        && CheckRoadNumber(leachinfo.strComment, m_strOther )
						&& CheckPT(leachinfo.strComment, iPT) ) )
                {
                    continue;
                }
            }

            INT iTmp = RuleCompare( strSrc, strCur );
            if ( iTmp == 2 || iTmp == 0 )
            {
                strCur = strSrc;
                iRet = leachinfo.iLeachType;
            }
        }
    }

    return iRet;
}

//进行对比
BOOL CSWResultFilter::Match( LPCSTR lpszPattern, LPCSTR lpszTarget )
{
    if ( lpszPattern == NULL || lpszTarget == NULL ) return FALSE;
    INT iPlusPattern = ( (BYTE)lpszPattern[0] & 0x80 ) == 0x80 ? 2 : 1;
    INT iPlusTarger  = ( (BYTE)lpszTarget[0] & 0x80 ) == 0x80 ? 2 : 1;

    switch (lpszPattern[0])
    {
    case 0:
        return (lpszTarget[0]==0);
        //break;
    case '*':
        do
        {
            if (Match(lpszPattern + iPlusPattern, lpszTarget))
            {
                return TRUE;
            }
            if ( *(lpszTarget) == 0 )
            {
                break;
            }
            lpszTarget += iPlusTarger;
        }
        while (true);
        break;
    case '?':
        if (lpszTarget[0]==0)
        {
            return FALSE;
        }
        else
        {
            return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger);
        }
        //break;
    case '%'://%代表数字
        if (lpszTarget[0]<'0' || lpszTarget[0]>'9')
        {
            return FALSE;
        }
        else
        {
            return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger);
        }
    case '@'://@代表字母
        if (lpszTarget[0]<'A' || lpszTarget[0]>'Z')
        {
            return FALSE;
        }
        else
        {
            return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger);
        }
    default:
        if (lpszPattern[0]==lpszTarget[0] && lpszPattern[0 + iPlusPattern - 1] == lpszTarget[0 + iPlusTarger -1] )
        {
            return Match(lpszPattern + iPlusPattern, lpszTarget + iPlusTarger );
        }
        break;
    }

    return FALSE;
}

INT CSWResultFilter::RuleCompare( LPCSTR lpszLeft, LPCSTR lpszRight )
{
    if ( lpszLeft == NULL || lpszRight == NULL ) return -1;
    if ( !Match( lpszLeft, lpszRight ) && !Match( lpszRight, lpszLeft ) )
    {
        if ( IsCross( lpszLeft, lpszRight ) ) return -2;
        return -1;
    }
    if ( Match( lpszLeft, lpszRight ) && Match( lpszRight, lpszLeft ) )
    {
        return 0;
    }
    if ( Match( lpszLeft, lpszRight ) && !Match( lpszRight, lpszLeft ) )
    {
        return 1;
    }
    if ( !Match( lpszLeft, lpszRight ) && Match( lpszRight, lpszLeft ) )
    {
        return 2;
    }
    return -1;
}

BOOL CSWResultFilter::IsCross( LPCSTR lpszLeft, LPCSTR lpszRight )
{
    if ( lpszLeft == NULL || lpszRight == NULL ) return FALSE;

    INT iPos = 0;
    while ( lpszLeft[iPos] != 0 && lpszRight[iPos] != 0 )
    {
        if ( lpszLeft[iPos] != '?' && lpszLeft[iPos] != '*'&& lpszRight[iPos] != '?'&& lpszRight[iPos] != '*'
                &&lpszLeft[iPos] != '%' && lpszLeft[iPos] != '@'&& lpszRight[iPos] != '%'&& lpszRight[iPos] != '@'
           )
        {
            if ( lpszLeft[iPos] != lpszRight[iPos] ) return FALSE;
        }
        ++iPos;
    }

    return TRUE;
}

//检查时间
BOOL CSWResultFilter::CheckRuleTimer( const CSWString& strComment, const CRuleTime& crtime )
{
    BOOL fRet = TRUE;
    CSWString strCmt = strComment;

    if ( strCmt.IsEmpty() ) return fRet;

    strCmt.Remove( VK_TAB );
    strCmt.Remove( VK_SPACE );

    INT iPos = strCmt.Find("[TIME:");
    if ( iPos == -1 ) return fRet;
    INT iMutriPos = strCmt.Find("_");

    iPos  += 6;
    BOOL fLastTimes = TRUE;
    do
    {
        INT lH,lM,lS,rH,rM,rS;
        lH = lM = lS = rH = rM = rS = 0;
        INT iFiled = 0;
        if (iMutriPos == -1 && iPos > -1)
        {
            iFiled = sscanf( strCmt.ToUpper().Substr(iPos), "%d:%d:%d,%d:%d:%d]",
                             &lH, &lM, &lS, &rH, &rM, &rS );
            fLastTimes = FALSE;
        }
        else if ( iMutriPos > -1 && iPos > -1)
        {
            iFiled = sscanf( strCmt.ToUpper().Substr(iPos), "%d:%d:%d,%d:%d:%d_",
                             &lH, &lM, &lS, &rH, &rM, &rS );
        }
        if ( iFiled != 6 ) return fRet;

        CRuleTime crtLeft;
        crtLeft.SetTime(lH, lM, lS);
        CRuleTime crtRight;
        crtRight.SetTime(rH, rM, rS);

        fRet = CRuleTime::IsCourseOfTime( crtLeft, crtRight, crtime);
        if (fRet)
            break;
        iPos = iMutriPos;
        if (iMutriPos > 0)
        {
            iPos += 1;
            iMutriPos = strCmt.Find("_", iMutriPos + 1);
            fLastTimes = TRUE;
        }
    }
    while (fLastTimes);

    return fRet;
}

//检查违章类型条件
BOOL CSWResultFilter::CheckPT( const CSWString& strComment, INT& iPT )
{
	
	BOOL fRet = TRUE;
	CSWString strCmt = strComment;

	SW_TRACE_DEBUG("<Filter>CheckPT:%s, pt:%d",(LPCSTR)strCmt, iPT);

	if ( strCmt.IsEmpty() )
	{
		return fRet;
	}

	//是否有设置违章类型
	strCmt.Remove( VK_TAB );
	strCmt.Remove( VK_SPACE );
	INT iPos = strCmt.Find("[PETYPE:");
	INT iSetType = 0;
	if ( iPos == -1 || 1 != sscanf(strCmt.Substr(iPos), "[PETYPE:%d]", &iSetType) )
	{
		return fRet;
	}

	//识别结果的车牌类型
	INT iNowType = iPT;
	//iPos = strInfo.Find("违章类型:");
	//if ( iPos == -1 )
	//{
	//	iNowType = 0;
	//}
	//else if( 1 != sscanf( strInfo.ToUpper().Substr( iPos ), "违章类型:%d", &iNowType ) )
	//{
	//	iNowType = 0;
	//}

	int iRetType = (iNowType & (~iSetType));

	SW_TRACE_DEBUG("<Filter>CheckPT return<ret:%d,src:%d>.", iRetType, iNowType);

	if ( iRetType == iNowType )
	{
		fRet = FALSE;
	}

	iPT = iRetType;
	return fRet;
}

//检查规则的可信度条件是否附合
BOOL  CSWResultFilter::CheckRuleRely( const CSWString& strComment, const CSWString& strPlateInfo )
{
    BOOL fRet = TRUE;
    CSWString strCmt = strComment;
    CSWString strInfo = strPlateInfo;

    if ( strCmt.IsEmpty() || strInfo.IsEmpty() )
    {
        return fRet;
    }

    INT iAve = -1;
    INT iWord = -1;
    char szTmp[256] = {0};
    memset(szTmp, 0, 256);
    INT iPos = strInfo.Find( "平均可信度:" );
    INT iPosPoint = -1;
    if ( iPos != -1 )
    {
        iPos += (INT)swpa_strlen("平均可信度:");
        iPosPoint = strInfo.Find(".", iPos);
        if ( iPosPoint != -1 )
        {
            if ( 1 != sscanf( strInfo.Substr(iPosPoint+1, 2), "%d", &iAve ) )
            {
                iAve = -1;
            }
            else
            {
                if ( strInfo[iPosPoint -1] == '1' )
                {
                    iAve += 100;
                }
            }
        }
    }
    iPos = -1;
    iPosPoint = -1;
    iPos = strInfo.Find( "首字可信度:" );
    if ( iPos != -1 )
    {
        iPos += (INT)swpa_strlen("首字可信度:");
        iPosPoint = strInfo.Find(".", iPos);
        if ( iPosPoint != -1 )
        {
            if ( 1 != sscanf( strInfo.Substr(iPosPoint+1, 2), "%d", &iWord ) )
            {
                iWord = -1;
            }
            else
            {
                if ( strInfo[iPosPoint -1] == '1' )
                {
                    iWord += 100;
                }
            }
        }
    }

    if ( iAve == -1 && iWord == -1 )
    {
        return fRet;
    }

    strCmt.Remove( VK_TAB );
    strCmt.Remove( VK_SPACE );

    iPos = strCmt.ToUpper().Find("[RELY:");
    if ( -1 == iPos )
    {
        return fRet;
    }

    INT iAveSet = -1;
    INT iWordSet = -1;

    INT iPosNext = strCmt.ToUpper().Find( "P:", iPos );
    if ( -1 != iPosNext )
    {
        sscanf( strCmt.ToUpper().Substr( iPosNext ), "P:%d", &iAveSet );
    }
    iPosNext = strCmt.ToUpper().Find("W:", iPos );
    if ( -1 != iPosNext )
    {
        sscanf( strCmt.ToUpper().Substr(iPosNext), "W:%d", &iWordSet );
    }

    if ( iAveSet != -1 && iAve != -1 && iAveSet > iAve
            &&( iWordSet == -1 || iWord == -1 ) )
    {
        return fRet;
    }

    if ( iWord != -1 && iWordSet != -1 && iWordSet > iWord
            && (iAve == -1 || iAveSet ==-1) )
    {
        return fRet;
    }

    if ( iWord != -1 && iWordSet != -1 && iAve != -1 && iAveSet != -1
            && iWordSet > iWord && iAveSet > iAve )
    {
        return fRet;
    }

    return FALSE;
}

//检查车牌类型是否符合
//车牌类型
// 0:普通单层牌, 1:双层牌, 2:摩托车牌
BOOL  CSWResultFilter::CheckPlateType( const CSWString& strComment, const CSWString& strPlateInfo )
{
    BOOL fRet = TRUE;
    CSWString strCmt = strComment;
    CSWString strInfo = strPlateInfo;

    if ( strCmt.IsEmpty() || strInfo.IsEmpty() )
    {
        return fRet;
    }

    //是否有设置车牌类型
    strCmt.Remove( VK_TAB );
    strCmt.Remove( VK_SPACE );
    INT iPos = strCmt.Find("[PLATETYPE:");
    INT iSetType = 0;
    if ( iPos == -1 || 1 != sscanf(strCmt.Substr(iPos), "[PLATETYPE:%d]", &iSetType) )
    {
        return fRet;
    }

    //识别结果的车牌类型
    INT iNowType = 0;
    iPos = strInfo.Find("车牌类型:");
    if ( iPos == -1 )
    {
        iNowType = 0;
    }
    else if ( strncmp(strInfo.Substr(iPos + 9), "双", 2) == 0 )
    {
        iNowType = 1;
    }
    else if ( strncmp(strInfo.Substr(iPos + 9), "摩", 2) == 0 )
    {
        iNowType = 2;
    }

    if ( iSetType != iNowType )
    {
        fRet = FALSE;
    }

    return fRet;
}

//检查车速条件
BOOL CSWResultFilter::CheckPlateSpeed( const CSWString& strComment, const CSWString& strPlateInfo )
{
    BOOL fRet = TRUE;
    CSWString strCmt = strComment;
    CSWString strInfo = strPlateInfo;

    if ( strCmt.IsEmpty() || strInfo.IsEmpty() )
    {
        return fRet;
    }

    strCmt.Remove( VK_TAB );
    strCmt.Remove( VK_SPACE );

    INT iPos = strCmt.ToUpper().Find("[SPEED:");
    INT iLow = -1;
    INT iHigh = -1;
    if ( iPos != -1 )
    {
        INT iPosNext = strCmt.ToUpper().Find("LS:", iPos);
        if ( iPosNext != -1 )
        {
            sscanf(strCmt.ToUpper().Substr(iPosNext), "LS:%d", &iLow);
        }
        iPosNext = strCmt.ToUpper().Find("HS:", iPos);
        if ( iPosNext != -1 )
        {
            sscanf(strCmt.ToUpper().Substr(iPosNext), "HS:%d", &iHigh);
        }
    }
    else
    {
        return fRet;
    }

    if ( iLow == -1 && iHigh == -1 )
    {
        return fRet;
    }

    //查找车速
    iPos = strInfo.Find( "车速" );
    INT iSpeed = -1;
    if ( iPos != -1 )
    {
        sscanf(strInfo.Substr(iPos), "车速:%d", &iSpeed);
    }
    else
    {
        iPos = strInfo.Find("视频测速");
        if ( iPos != -1 )
        {
            sscanf(strInfo.Substr(iPos), "视频测速:%d", &iSpeed);
        }
    }

    if ( iSpeed == -1 )
    {
        return fRet;
    }

    if ( iLow != -1 && iHigh == -1 && iLow > iSpeed )
    {
        fRet = FALSE;
    }
    if ( iLow == -1 && iHigh != -1 && iHigh < iSpeed )
    {
        fRet = FALSE;
    }
    if ( iLow != -1 && iHigh != -1 && (iLow > iSpeed || iHigh < iSpeed) )
    {
        fRet = FALSE;
    }

    return fRet;
}
//检查逆行条件
BOOL CSWResultFilter::CheckReverseRun( const CSWString& strComment, const CSWString& strPlateInfo )
{
    BOOL fRet = TRUE;
    CSWString strCmt = strComment;
    CSWString strInfo = strPlateInfo;

    if ( strCmt.IsEmpty() || strInfo.IsEmpty() )
    {
        return fRet;
    }

    strCmt.Remove( VK_TAB );
    strCmt.Remove( VK_SPACE );

    INT iPos = strCmt.ToUpper().Find("[DIRECT:");
    INT iValue = -1;
    if ( iPos != -1 )
    {
        sscanf(strCmt.Substr(iPos), "[DIRECT:%d", &iValue);
    }
    else
    {
        return fRet;
    }

    if (iValue != 1)
    {
        fRet = FALSE;
        return fRet;
    }
    INT iCarPos = strInfo.Find("车辆逆向行驶:是");
    if (iCarPos == -1)
    {
        fRet = FALSE;
    }

    return fRet;
}
BOOL CSWResultFilter::CheckRoadNumber( const CSWString& strComment, const CSWString& strPlateInfo )
{
        BOOL fRet = FALSE;
        CSWString strCmt = strComment;
        CSWString strInfo = strPlateInfo;

        if ( strCmt.IsEmpty() || strInfo.IsEmpty() )
        {
            return fRet;
        }

        strCmt.Remove( VK_TAB );
        strCmt.Remove( VK_SPACE );

        INT iPos = strCmt.ToUpper().Find("[ROAD:");
        if ( iPos == -1 )
        {
            SW_TRACE_DEBUG("CheckRoadNumber<No Find [ROAD:> return FALSE\n");
            return fRet;
        }

        INT iRoadCount = strCmt.FindCharCount('-');//FindCharCount("[ROAD:1-2-3]", '-');
        CSWString strFormat = "[ROAD:%d";
        for (int i = 0; i < iRoadCount; ++ i)
        {
            strFormat += "-%d";
        }
        strFormat += "]";

        SW_TRACE_DEBUG("CheckRoadNumber<strFormat = %s>\n", (LPCSTR)strFormat);

        const INT MAX_LEACH_ROAD = 4;
        INT iArrayRoad[MAX_LEACH_ROAD] = {-1, -1, -1, -1};
        if (iRoadCount >= MAX_LEACH_ROAD)
        {
            SW_TRACE_DEBUG("RoadCount(%d) >= %d, do not Result Filter.\n", iRoadCount, MAX_LEACH_ROAD);
            return fRet;
        }

        if (iRoadCount <= 0)
        {
            swpa_sscanf(strCmt.Substr(iPos), strFormat, &iArrayRoad[0]);
        }
        else if (iRoadCount == 1)
        {
            swpa_sscanf(strCmt.Substr(iPos), strFormat, &iArrayRoad[0], &iArrayRoad[1]);
        }
        else if (iRoadCount == 2)
        {
            swpa_sscanf(strCmt.Substr(iPos), strFormat, &iArrayRoad[0], &iArrayRoad[1], &iArrayRoad[2]);
        }
        else if (iRoadCount == 3)
        {
            swpa_sscanf(strCmt.Substr(iPos), strFormat, &iArrayRoad[0], &iArrayRoad[1], &iArrayRoad[2], &iArrayRoad[3]);
        }
        else
        {
            SW_TRACE_DEBUG("RoadCount(%d), Invalidate Num, do not Result Filter.\n", iRoadCount);
            return fRet;
        }

        INT iCarPos = strInfo.Find("车道:");
        INT iRoad = -1;
        if ( iCarPos != -1 )
        {
            sscanf(strInfo.Substr(iCarPos), "车道:%d", &iRoad);
        }

        for (int i = 0; i < iRoadCount + 1; ++i)
        {
            if (iRoad == iArrayRoad[i] && iRoad != -1 && iArrayRoad[i] != -1 )
            {
                SW_TRACE_DEBUG("CheckRoadNumber<Find a Road : %d, do Result Filter>\n", iRoad);
                return TRUE;
            }
        }

    return fRet;
}
//检测规则
CSWString CSWResultFilter::CheckRule( CSWString& strLeft, CSWString& strRight,
                                      CSWString& strStyle)
{
    CSWString str, strLine;
    CSWString strRet;
    switch ( RuleCompare( strLeft, strRight) )
    {
    case -1:
        break;

    case 0:
        str.Format( "\"%s\"等于\"%s\"", (LPCSTR)strLeft, (LPCSTR)strRight);
        strLine.Format( "%-20s%-32s%-35s\n", (LPCSTR)strStyle, (LPCSTR)str, "未处理" );
        strRet.Append( strLine );
        break;

    case 1:
        str.Format( "\"%s\"包含\"%s\"", (LPCSTR)strLeft, (LPCSTR)strRight);
        strLine.Format( "%-20s%-32s%-35s\n", (LPCSTR)strStyle, (LPCSTR)str, "未处理" );
        strRet.Append( strLine );
        break;

    case 2:
        str.Format( "\"%s\"包含\"%s\"", (LPCSTR)strRight, (LPCSTR)strLeft);
        strLine.Format( "%-20s%-32s%-35s\n", (LPCSTR)strStyle, (LPCSTR)str, "未处理" );
        strRet.Append( strLine );
        break;

    case -2:
        str.Format( "\"%s\"与\"%s\"相冲突", (LPCSTR)strLeft, (LPCSTR)strRight);
        strLine.Format( "%-20s%-32s%-35s\n", (LPCSTR)strStyle, (LPCSTR)str, "未处理" );
        strRet.Append( strLine );
        break;

    default:
        break;
    }
    return strRet;
}

//生成检查规则报告
void CSWResultFilter::MakeReport( LPSTR lpszReport, INT iLen )
{
    //检查规则的合法性
    return;
}

HRESULT CSWResultFilter::OnOverlayPosChanged(WPARAM wParam, LPARAM lParam)
{
    INT* iPos = (INT *)wParam;
    /*m_pProcessRule->nOverlayX = iPos[0];
    m_pProcessRule->nOverlayY = iPos[1];*/
    return S_OK;
}


