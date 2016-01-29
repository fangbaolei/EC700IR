#include "HvParamConvert.h"
#include "HvParamStore.h"
#include "AutoPtr.h"
#include "assert.h"
#include "memstorage.h"

//szBuf必须足够大, 为保证BIN数据(最大8K)输出正常可设为32K
static LPCSTR GetVarString( CParamVar& var, LPSTR szBuf )
{
    szBuf[0] = '\0';

    switch ( var.vt )
    {
    case PARAM_VAR::VT_INT:
        sprintf(szBuf, "%d", (INT)var);
        break;
    case PARAM_VAR::VT_UINT:
        sprintf(szBuf, "%d", (UINT)var);
        break;
    case PARAM_VAR::VT_FLOAT:
        sprintf(szBuf, "%0.3f", (float)var);
        break;
    case PARAM_VAR::VT_DOUBLE:
        sprintf(szBuf,"%0.3f",(double)var);
        break;
    case PARAM_VAR::VT_BIN:
    {
        void* pbBinBuf;
        WORD16 nLen = 0;

        var.GetBin(&pbBinBuf, &nLen);

        char* pCurPos = (char*)szBuf;
        sprintf(szBuf, "0x");
        pCurPos+=2;
        for (int i = 0; i < nLen; i++)
        {
            sprintf(pCurPos,"%02x",*((BYTE8*)pbBinBuf+i));
            pCurPos+=2;
        }
    }
    break;
    case PARAM_VAR::VT_STR:
    {
        const char* szStr = (LPCSTR)var;
        strcpy(szBuf, szStr);
    }
    break;
    default:
        szBuf[0] = '\0';
        break;
    }

    return (LPCSTR)szBuf;
}

static LPCSTR GetVarType( CParamVar& var, char* szBuf )
{
    switch ( var.vt )
    {
    case PARAM_VAR::VT_INT:
        strcpy(szBuf, "INT");
        break;
    case PARAM_VAR::VT_UINT:
        strcpy(szBuf, "UINT");
        break;
    case PARAM_VAR::VT_FLOAT:
        strcpy(szBuf, "FLOAT");
        break;
    case PARAM_VAR::VT_DOUBLE:
        strcpy(szBuf, "DOUBLE");
        break;
    case PARAM_VAR::VT_BIN:
        strcpy(szBuf, "BIN");
        break;
    case PARAM_VAR::VT_STR:
        strcpy(szBuf, "STR");
        break;
    default:
        strcpy(szBuf, "NULL");
        break;
    }

    return szBuf;
}

//写入KeyNode
HRESULT WriteKeyNode(
    CParamNode* pNode,
    TiXmlElement* pParentElement,
    INT nRank,
    BOOL fShowExtInfo,
    BOOL fShowRank
)
{
    if ( !pNode || !pParentElement) return E_INVALIDARG;

    BOOL fRankExist = pNode->HasExtInfo() && (pNode->m_nInfoMask & NI_RANK);

    if (fRankExist)
    {
        //级别信息有效时,如果此参数级别小于指定级别则不输出
        if ((int)pNode->m_Rank < nRank) return S_OK;
    }
    else
    {
        //级别信息无效时,如果指定的是用户级别(>3)则不输出
        if (nRank > 3) return S_OK;
    }

    AutoPtr<TiXmlElement> pKey = new TiXmlElement("KEY");
    if (!pKey.get()) return E_OUTOFMEMORY;

    pKey->SetAttribute("name", (LPCSTR)pNode->m_strName);

    CFastMemAlloc cStack;
    char* szValue = (char*)cStack.StackAlloc(32* 1024, FALSE);
    if (NULL == szValue) return E_OUTOFMEMORY;

    char szType[8] = {0};	//类型名

    AutoPtr<TiXmlElement> pValue = new TiXmlElement("VALUE");
    AutoPtr<TiXmlText> pValueText = new TiXmlText(GetVarString(pNode->m_Val, szValue));
    if ( !pValue.get() || !pValueText.get() ) return E_OUTOFMEMORY;

    pValue->SetAttribute("type", GetVarType(pNode->m_Val, szType));
    pValue->LinkEndChild(pValueText.release());
    pKey->LinkEndChild(pValue.release());

    if ( pNode->HasExtInfo() && fShowExtInfo )
    {
        if (pNode->m_nInfoMask & NI_DEFAULT_VAL)
        {
            AutoPtr<TiXmlElement> pDefault = new TiXmlElement("DEFAULT");
            AutoPtr<TiXmlText> pDefaultText = new TiXmlText(GetVarString(pNode->m_DefaultVal, szValue));
            if ( !pDefault.get() || !pDefaultText.get() ) return E_OUTOFMEMORY;
            pDefault->SetAttribute("type", GetVarType(pNode->m_Val, szType));
            pDefault->LinkEndChild(pDefaultText.release());
            pKey->LinkEndChild(pDefault.release());
        }

        if (pNode->m_nInfoMask & NI_SCALE)
        {
            AutoPtr<TiXmlElement> pMin = new TiXmlElement("MIN");
            AutoPtr<TiXmlText> pMinText = new TiXmlText(GetVarString(pNode->m_MinVal, szValue));
            if ( !pMin.get() || !pMinText.get() ) return E_OUTOFMEMORY;
            pMin->SetAttribute("type", GetVarType(pNode->m_MinVal, szType));
            pMin->LinkEndChild(pMinText.release());
            pKey->LinkEndChild(pMin.release());

            AutoPtr<TiXmlElement> pMax = new TiXmlElement("MAX");
            AutoPtr<TiXmlText> pMaxText = new TiXmlText(GetVarString(pNode->m_MaxVal, szValue));
            if ( !pMax.get() || !pMaxText.get() ) return E_OUTOFMEMORY;
            pMax->SetAttribute("type", GetVarType(pNode->m_MaxVal, szType));
            pMax->LinkEndChild(pMaxText.release());
            pKey->LinkEndChild(pMax.release());
        }

        if (pNode->m_nInfoMask & NI_CHNAME)
        {
            AutoPtr<TiXmlElement> pChName = new TiXmlElement("CHNAME");
            AutoPtr<TiXmlText> pChNameText = new TiXmlText(GetVarString(pNode->m_strChName, szValue));
            if ( !pChName.get() || !pChNameText.get() ) return E_OUTOFMEMORY;
            pChName->SetAttribute("type", "STR");
            pChName->LinkEndChild(pChNameText.release());
            pKey->LinkEndChild(pChName.release());
        }

        if (pNode->m_nInfoMask & NI_COMMENT)
        {
            AutoPtr<TiXmlElement> pComment = new TiXmlElement("COMMENT");
            AutoPtr<TiXmlText> pCommentText = new TiXmlText(GetVarString(pNode->m_strComment, szValue));
            if ( !pComment.get() || !pCommentText.get() ) return E_OUTOFMEMORY;
            pComment->SetAttribute("type", "STR");
            pComment->LinkEndChild(pCommentText.release());
            pKey->LinkEndChild(pComment.release());
        }
    }

    if ( (pNode->m_nInfoMask & NI_RANK) && fShowRank )
    {
        sprintf(szValue, "%d", (int)pNode->m_Rank);
        AutoPtr<TiXmlElement> pRank = new TiXmlElement("RANK");
        AutoPtr<TiXmlText> pRankText = new TiXmlText(szValue);
        if ( !pRank.get() || !pRankText.get() ) return E_OUTOFMEMORY;
        pRank->SetAttribute("type", "INT");
        pRank->LinkEndChild(pRankText.release());
        pKey->LinkEndChild(pRank.release());
    }

    pParentElement->LinkEndChild(pKey.release());

    return S_OK;
}

//写入SectionNode
HRESULT WriteSectionNode(
    CParamNode* pNode,
    TiXmlElement* pParentElement,
    INT nRank,
    BOOL fShowExtInfo,
    BOOL fShowRank,
    BOOL fIsRoot = FALSE
)
{
    if ( !pNode || !pParentElement ) return E_INVALIDARG;

    AutoPtr<TiXmlElement> pSection = new TiXmlElement(fIsRoot?"HvParam":"SECTION");
    if (!pSection.get()) return E_OUTOFMEMORY;

    if (!fIsRoot)
    {
        pSection->SetAttribute("name",(LPCSTR)pNode->m_strName);
    }

    HRESULT hr = S_OK;
    CParamNode* pSubNode = NULL;
    while ( pSubNode = pNode->GetNextSubNode(pSubNode) )
    {
        SystemDog();
        if (pSubNode->IsNullNode())
        {
            continue; //如果是NULL节点则跳过
        }
        else if (pSubNode->IsSectionNode())
        {
            hr = WriteSectionNode(pSubNode, pSection.get(), nRank, fShowExtInfo, fShowRank);
        }
        else if (pSubNode->IsKeyNode())
        {
            hr = WriteKeyNode(pSubNode, pSection.get(), nRank, fShowExtInfo, fShowRank);
        }

        if (FAILED(hr)) break;
    }

    if (SUCCEEDED(hr)) pParentElement->LinkEndChild(pSection.release());

    return hr;
}

BYTE8* ConvertStr2Bin(const char* szValue, BYTE8* pbBuf, UINT* pLen)
{
    char* pCurPos = (char*)szValue;

    UINT nLen = (UINT)strlen(szValue);

    if ( nLen > 2 &&
            pCurPos[0] == '0' &&
            (pCurPos[1] == 'x' || pCurPos[1] == 'X') )
    {
        pCurPos += 2;
        nLen -= 2;
    }

    UINT nConvLen = MIN_INT(*pLen, nLen / 2);

    char szTmp[3] = {0};
    int iTmp = 0;

    for (UINT i = 0; i < nConvLen; i++)
    {
        iTmp = 0;
        HV_memcpy(szTmp, pCurPos, 2);
        sscanf(szTmp, "%02x", &iTmp);
        pbBuf[i] = (BYTE8)iTmp;
        pCurPos += 2;
    }

    *pLen = nConvLen;

    return pbBuf;
}

HRESULT ConvertXml2Var(TiXmlElement* pElement, CParamVar* pVar)
{
    if ( !pElement || !pVar ) return E_INVALIDARG;

    LPCSTR szType = pElement->Attribute("type");
    if ( !szType ) return E_FAIL;

    LPCSTR szValue = pElement->GetText();
    if (!szValue) return E_FAIL;

    HRESULT  hr = S_OK;
    if (strcmp(szType,"INT") == 0)
    {
        *pVar = (INT)atoi(szValue);
    }
    else if (strcmp(szType, "UINT") == 0)
    {
        *pVar = (UINT)atoi(szValue);
    }
    else if (strcmp(szType, "FLOAT") == 0)
    {
        *pVar = (float)atof(szValue);
    }
    else if (strcmp(szType, "STR") == 0)
    {
        hr = pVar->SetString(szValue);
    }
    else if (strcmp(szType, "BIN") == 0)
    {
        CFastMemAlloc cStack;
        UINT nBufLen = 8 * 1024;
        BYTE8* pbBuf = (BYTE8*)cStack.StackAlloc(nBufLen, FALSE);
        if (NULL == pbBuf)
        {
            hr = E_OUTOFMEMORY;
        }
        else
        {
            ConvertStr2Bin(szValue, pbBuf, &nBufLen);
            hr = pVar->SetBin(pbBuf, nBufLen);
        }
    }
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}

CParamNode* ReadKeyNode( TiXmlElement* pElement )
{
    if (!pElement) return NULL;

    AutoPtr<CParamNode> pKeyNode = new CParamNode(NT_KEY);
    if (!pKeyNode.get()) return NULL;

    LPCSTR szName = pElement->Attribute("name");
    if (szName == NULL) return NULL;
    pKeyNode->m_strName = szName;

    CParamVar cValueVar;
    TiXmlElement* pValue = pElement->FirstChildElement("VALUE");
    if (!pValue) return NULL;
    if (FAILED(ConvertXml2Var(pValue, &cValueVar))) return NULL;
    pKeyNode->SetValue(cValueVar);

    TiXmlElement* pDefault = pElement->FirstChildElement("DEFAULT");
    if ( pDefault && SUCCEEDED(ConvertXml2Var(pValue, &cValueVar)))
    {
        pKeyNode->SetDefaultValue(cValueVar);
    }

    CParamVar cMinVar, cMaxVar;
    TiXmlElement* pMin = pElement->FirstChildElement("MIN");
    TiXmlElement* pMax = pElement->FirstChildElement("MAX");
    if ( pMin && pMax &&
            SUCCEEDED(ConvertXml2Var(pMin, &cMinVar)) &&
            SUCCEEDED(ConvertXml2Var(pMax, &cMaxVar)) )
    {
        pKeyNode->SetScale(cMinVar, cMaxVar);
    }

    TiXmlElement* pChName = pElement->FirstChildElement("CHNAME");
    if ( pChName )
    {
        LPCSTR szChName = pChName->GetText();
        if (szChName)
        {
            pKeyNode->SetChName(szChName);
        }
    }

    TiXmlElement* pComment = pElement->FirstChildElement("COMMENT");
    if ( pComment )
    {
        LPCSTR szComment = pComment->GetText();
        if (szComment)
        {
            pKeyNode->SetComment(szComment);
        }
    }

    TiXmlElement* pRank = pElement->FirstChildElement("RANK");
    if ( pRank )
    {
        LPCSTR szRank = pRank->GetText();
        if (szRank)
        {
            pKeyNode->SetRank(atoi(szRank));
        }
    }

    return pKeyNode.release();
}

CParamNode* ReadSectionNode( TiXmlElement* pElement )
{
    if (!pElement) return NULL;

    AutoPtr<CParamNode> pSectionNode = new CParamNode(NT_SECTION);
    if (!pSectionNode.get()) return NULL;

    LPCSTR szName = pElement->Attribute("name");
    if (szName == NULL)
    {
        szName = pElement->Value();
        if (strcmp(szName, "HvParam") != 0) return NULL;
    }
    pSectionNode->m_strName = szName;

    CParamNode* pChildNode = NULL;
    TiXmlElement* pChild = NULL;
    for ( pChild = pElement->FirstChildElement("SECTION"); pChild; pChild = pChild->NextSiblingElement("SECTION"))
    {
        SystemDog();
        pChildNode = ReadSectionNode(pChild->ToElement());

        if (pChildNode) pSectionNode->AddSubNode(pChildNode);
    }

    for ( pChild = pElement->FirstChildElement("KEY"); pChild; pChild = pChild->NextSiblingElement("KEY"))
    {
        SystemDog();
        pChildNode = ReadKeyNode(pChild->ToElement());

        if (pChildNode) pSectionNode->AddSubNode(pChildNode);
    }

    return pSectionNode.release();
}

//参数转为XML
HRESULT ConvertParam2Xml(
    BYTE8* pbParamBuf,
    UINT nBufLen,
    TiXmlDocument* pDoc,
    INT nRank,
    BOOL fShowExtInfo,
    BOOL fShowRank
)
{
    if ( !pbParamBuf || !nBufLen ) return E_INVALIDARG;
    CMemStorage cMemStg;
    cMemStg.Initialize(pbParamBuf, nBufLen);

    //从流中加载参数
    CParamStore cParam;
    cParam.LoadFrom(&cMemStg);

    //取得根节点
    CParamNode* pRootNode = cParam.GetSectionNode("");
    if (!pRootNode) return E_FAIL;

    pDoc->Clear();
    TiXmlDeclaration* pDecl = new TiXmlDeclaration("1.0", "GB2312", "yes");
    pDoc->LinkEndChild(pDecl);

    TiXmlElement* pRoot = new TiXmlElement("HvParamDoc");
    pRoot->SetAttribute("version", "1.0");
    pDoc->LinkEndChild(pRoot);

    return WriteSectionNode(pRootNode, pRoot, nRank, fShowExtInfo, fShowRank, TRUE);
}

//XML转为参数
HRESULT ConvertXml2Param(
    TiXmlDocument* pDoc,
    BYTE8* pbParamBuf,
    UINT& nBufLen,
    BOOL fSimpMode
)
{
    if ( NULL == pbParamBuf ) return E_INVALIDARG;

    CMemStorage cMemStg;
    cMemStg.Initialize(pbParamBuf, nBufLen);

    TiXmlElement* pParamElement = pDoc->RootElement()->FirstChildElement("HvParam");
    if (pParamElement == NULL) return E_FAIL;

    CParamNode* pRootNode = ReadSectionNode(pParamElement);
    if (!pRootNode) return E_FAIL;

    HRESULT hr = pRootNode->SaveToStream(&cMemStg, fSimpMode);
    delete pRootNode;

    nBufLen = cMemStg.GetCurPos();

    return hr;
}
