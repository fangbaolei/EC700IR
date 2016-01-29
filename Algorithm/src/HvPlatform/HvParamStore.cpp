#include "HvParamStore.h"
#include "assert.h"

static const char* HV_PARAM_NAME = "HvParam";

//CParamStore实现
//IHvParam

//////////////////////////////////////////////////////////////////////////
//取值
HRESULT STDMETHODCALLTYPE CParamStore::GetInt(
    const char* szSection,
    const char* szKey,
    int* pVal,
    int iDefault
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = iDefault; //先赋默认值

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_INT );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetValue(iDefault);
        hr = S_FALSE;
    }

    *pVal = (int)pNode->m_Val;

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CParamStore::GetFloat(
    const char* szSection,
    const char* szKey,
    float* pVal,
    float fltDefault
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = fltDefault; //先赋默认值

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_FLOAT );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetValue(fltDefault);
        hr = S_FALSE;
    }

    *pVal = (float)pNode->m_Val;

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//读取字符串,读取失败将不会改变szRet内容,
//如果允许创建则将szRet做为默认值写入
HRESULT STDMETHODCALLTYPE CParamStore::GetString(
    const char* szSection,
    const char* szKey,
    char* szRet,
    int nLen
)
{
    if (!szSection || !szKey ||!szRet || nLen <= 0) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_STR );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetValue(szRet);
        hr = S_FALSE;
    }

    LPCSTR szValue = (LPCSTR)pNode->m_Val;
    if (szValue == NULL) return E_INVALIDARG;

    UINT nCopyLen = MIN_INT((UINT)(nLen - 1), (UINT)strlen(szValue));
    HV_memcpy(szRet, szValue, nCopyLen);
    szRet[nCopyLen] = '\0';

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//读取二进制数据,如果*pLen为0则不写入默认值
//当pBuf == NULL时只取得BIN数据长度
//读取时缓存空间不能小于此长度
HRESULT STDMETHODCALLTYPE CParamStore::GetBin(
    const char* szSection,
    const char* szKey,
    void* pBuf,
    int* pLen
)
{
    if ( !szSection || !szKey || !pLen ) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    BOOL fDefaultValueValid = (pBuf != NULL) && (*pLen > 0);

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_BIN );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空, 重新设值
    {
        pNode->ReInit();
        if (fDefaultValueValid)
        {
            pNode->SetValue(pBuf, *pLen);
        }
        else
        {
            char szNull[2] = {0};
            pNode->SetValue(szNull,2); //如果未指定值,则置为0.使用时可能需要进行判断
        }

        hr = S_FALSE;
    }

    void* pValueBuf;
    WORD16 nValueLen;

    if (FAILED(pNode->m_Val.GetBin((void**)&pValueBuf, &nValueLen))) return E_FAIL;

    if (pBuf) HV_memcpy(pBuf, pValueBuf, nValueLen);
    *pLen = nValueLen;

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//设值
HRESULT STDMETHODCALLTYPE CParamStore::SetInt(
    const char* szSection,
    const char* szKey,
    int iVal
)
{
    if ( !szSection || !szKey ) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode(szSection, szKey, TRUE);
    if (!pNode) return E_FAIL;

    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_INT );

    if ( !fValid )
    {
        pNode->ReInit();
    }

    pNode->SetValue(iVal);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CParamStore::SetFloat(
    const char* szSection,
    const char* szKey,
    float fltVal
)
{
    if ( !szSection || !szKey ) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode(szSection, szKey, TRUE);
    if (!pNode) return E_FAIL;

    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_FLOAT );

    if ( !fValid )
    {
        pNode->ReInit();
    }

    pNode->SetValue(fltVal);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CParamStore::SetString(
    const char* szSection,
    const char* szKey,
    const char* szSrc
)
{
    if ( !szSection || !szKey || !szSrc ) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode( szSection, szKey, TRUE );
    if (!pNode) return E_FAIL;

    BOOL fValid =( pNode->m_Val.vt == PARAM_VAR::VT_STR );

    if (!fValid)
    {
        pNode->ReInit();
    }

    pNode->SetValue(szSrc);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CParamStore::SetBin(
    const char* szSection,
    const char* szKey,
    const void* pBuf,
    int nBufLen
)
{
    if ( !szSection || !szKey || !pBuf || (nBufLen <= 0) ) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode( szSection, szKey, TRUE );
    if (!pNode) return E_FAIL;

    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_BIN );

    if (!fValid)
    {
        pNode->ReInit();
    }

    pNode->m_Val.SetBin(pBuf, nBufLen);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

//S_OK: 保存成功
//其他: 保存失败
HRESULT STDMETHODCALLTYPE CParamStore::Save()
{
    if ( !m_pStorage ) return S_FALSE;

    if ( FAILED(m_pStorage->Seek(0, HvCore::STREAM_SEEK_SET, NULL))) return E_FAIL;

    if (m_pClone)
    {
        return m_pClone->SaveTo(m_pStorage, FALSE);	//用副本进行存储
    }

    HRESULT hr = m_pRoot->SaveToStream(m_pStorage, TRUE);
    if (S_OK == hr)
    {
        hr = m_pStorage->Commit();
    }

    if (hr != S_OK)
    {
        HV_Trace(5, "Warnning: Save param failed!!!!!");
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//IHvParam2
HRESULT STDMETHODCALLTYPE CParamStore::GetInt(
    const char* szSection,
    const char* szKey,
    int* pVal,
    int nDefault,
    int nMin,
    int nMax,
    const char* szChName,
    const char* szComment,
    BYTE8	nRank
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = nDefault; //先赋默认值

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_INT );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetValue(nDefault);
        hr = S_FALSE;
    }

    *pVal = (int)pNode->m_Val;

    //参数有效性判断
    if ( (*pVal < nMin) ||
            (*pVal > nMax) )
    {
        *pVal = nDefault;
        pNode->SetValue(nDefault);
        hr = S_FALSE;
    }

    //写入参数附加信息
    pNode->SetDefaultValue(nDefault);
    pNode->SetScale(nMin, nMax);
    pNode->SetComment(szComment);
    pNode->SetChName(szChName);
    pNode->SetRank(nRank);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CParamStore::GetFloat(
    const char* szSection,
    const char* szKey,
    float* pVal,
    float fltDefault,
    float fltMin,
    float fltMax,
    const char* szChName,
    const char* szComment,
    BYTE8	nRank
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = fltDefault; //先赋默认值

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_FLOAT );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetValue(fltDefault);
        hr = S_FALSE;
    }

    *pVal = (float)pNode->m_Val;

    //参数有效性判断
    if ( ((*pVal - fltMin) < -0.00000001) ||
            ((*pVal - fltMax) > 0.00000001) )
    {
        *pVal = fltDefault;
        pNode->SetValue(fltDefault);
        hr = S_FALSE;
    }

    //写入参数附加信息
    pNode->SetDefaultValue(fltDefault);
    pNode->SetScale(fltMin, fltMax);
    pNode->SetComment(szComment);
    pNode->SetChName(szChName);
    pNode->SetRank(nRank);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CParamStore::GetString(
    const char* szSection,
    const char* szKey,
    char* szRet,
    int nLen,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank
)
{
    if (!szSection || !szKey ||!szRet || nLen <= 0) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_STR );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetValue(szRet);
        hr = S_FALSE;
    }

    LPCSTR szValue = (LPCSTR)pNode->m_Val;
    if (szValue == NULL) return E_INVALIDARG;

    UINT nCopyLen = MIN_INT((UINT)(nLen - 1), (UINT)strlen(szValue));
    HV_memcpy(szRet, szValue, nCopyLen);
    szRet[nCopyLen] = '\0';

    //写入附加信息
    pNode->SetComment(szComment);
    pNode->SetChName(szChName);
    pNode->SetRank(nRank);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CParamStore::GetBin(
    const char* szSection,
    const char* szKey,
    void* pBuf,
    int* pLen,
    const char* szChName,
    const char* szComment,
    BYTE8 nRank
)
{
    if ( !szSection || !szKey || !pLen ) return E_INVALIDARG;

    CParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    BOOL fDefaultValueValid = (pBuf != NULL) && (*pLen > 0);

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_BIN );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空, 重新设值
    {
        pNode->ReInit();
        if (fDefaultValueValid)
        {
            pNode->SetValue(pBuf, *pLen);
        }
        else
        {
            char szNull[2] = {0};
            pNode->SetValue(szNull,2); //如果未指定值,则置为0.使用时可能需要进行判断
        }

        hr = S_FALSE;
    }

    void* pValueBuf;
    WORD16 nValueLen;

    if (FAILED(pNode->m_Val.GetBin((void**)&pValueBuf, &nValueLen))) return E_FAIL;

    if (pBuf) HV_memcpy(pBuf, pValueBuf, nValueLen);
    *pLen = nValueLen;

    //写入附加信息
    pNode->SetComment(szComment);
    pNode->SetChName(szChName);
    pNode->SetRank(nRank);

    if (m_pClone)
    {
        CParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//S_OK: 保存成功
//其他: 保存失败
HRESULT STDMETHODCALLTYPE CParamStore::Save( DWORD32 dwFlag )
{
    if ( !m_pStorage ) return S_FALSE;

    if ( FAILED(m_pStorage->Seek(0, HvCore::STREAM_SEEK_SET, NULL))) return E_FAIL;

    BOOL fSimpMode = (dwFlag == HvCore::PSM_SIMPLE);

    if (m_pClone)
    {
        return m_pClone->SaveTo(m_pStorage, fSimpMode);	//用副本进行存储
    }

    HRESULT hr = m_pRoot->SaveToStream(m_pStorage, fSimpMode);

    if (S_OK == hr)
    {
        hr = m_pStorage->Commit();
    }

    if (hr != S_OK)
    {
        HV_Trace(5, "Warnning: Save param failed!!!!!");
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//S_OK : 读取成功
//其他 : 读取失败
HRESULT CParamStore::Load()
{
    if ( !m_pStorage )
    {
        return S_FALSE;
    }

    if ( FAILED(m_pStorage->Seek(0, HvCore::STREAM_SEEK_SET, NULL)))
    {
        return E_FAIL;
    }

    HRESULT hr = m_pRoot->RestoreFromStream(m_pStorage);

    if ( hr != S_OK )
    {
        Clear(); //读取失败重置根节点
        hr = S_FALSE;
    }

    return hr;
}

HRESULT CParamStore::Clear()
{
    SAFE_DELETE(m_pRoot);
    SAFE_DELETE(m_pClone); //已经不是CLEAR模式

    m_pRoot = CreateNode(HV_PARAM_NAME, NT_SECTION);
    assert(m_pRoot);

    return S_OK;
}

HRESULT CParamStore::SaveTo(HvCore::IHvStream* pStream, BOOL fSimpMode)
{
    if (pStream == NULL) return E_INVALIDARG;

    if (m_pClone)
    {
        return m_pClone->SaveTo(pStream, fSimpMode);
    }

    HRESULT hr = E_FAIL;

    if ( S_OK == m_pRoot->SaveToStream(pStream, fSimpMode) &&
            S_OK == pStream->Commit() )
    {
        hr = S_OK;
    }
    return hr;
}

HRESULT CParamStore::LoadFrom(HvCore::IHvStream* pStream)
{
    if (pStream == NULL) return E_INVALIDARG;

    HRESULT hr = m_pRoot->RestoreFromStream(pStream);

    if ( hr != S_OK )
    {
        Clear(); //读取失败重置根节点
        hr = S_FALSE;
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//构造
CParamStore::CParamStore()
        :m_dwCookie(0)
        ,m_pRoot(NULL)
        ,m_fAutoAdd(TRUE)
        ,m_pStorage(NULL)
        ,m_pClone(NULL)
{
    m_pRoot = CreateNode( HV_PARAM_NAME, NT_SECTION);
    assert(m_pRoot);
}

CParamStore::CParamStore(CParamStore& src)
{
    Assign(src);
}

CParamStore::~CParamStore()
{
    SAFE_DELETE(m_pRoot);
    SAFE_DELETE(m_pClone);
}

//method
HRESULT CParamStore::Initialize(
    HvCore::IHvStream* pStream,
    BOOL fLoad,
    BOOL fAutoAdd,
    BOOL fClearMode
)
{
    m_pStorage = pStream;
    m_fAutoAdd = fAutoAdd;
    SetClearMode(fClearMode);

    HRESULT hr = S_OK;
    if (fLoad && m_pStorage)
    {
        hr = Load();
    }

    return hr;
}

HRESULT CParamStore::SetStorage(HvCore::IHvStream* pStream)
{
    m_pStorage = pStream;
    return S_OK;
}

HRESULT CParamStore::EnableAutoAdd(BOOL fAutoAdd)
{
    m_fAutoAdd = fAutoAdd;
    return S_OK;
}

HRESULT CParamStore::SetClearMode(BOOL fClear)
{
    SAFE_DELETE(m_pClone);

    if (fClear)
    {
        m_pClone = new CParamStore;
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CParamNode* CParamStore::CreateNode(const char* szName, PARAM_NODE_TYPE nType)
{
    CParamNode* pNewNode = new CParamNode(nType);
    if (pNewNode == NULL) return NULL;

    if ( S_OK != pNewNode->m_strName.SetString(szName) )
    {
        delete pNewNode;
        pNewNode = NULL;
    }

    return pNewNode;
}

//解析SECTION
HRESULT ParseSection(char** pszRemain, char* szSection)
{
    char* pChar = *pszRemain;
    while ( *pChar != '\\' && *pChar != '\0') pChar++;

    int nCpyLen = (int)(pChar - *pszRemain);
    HV_memcpy(szSection, *pszRemain, nCpyLen);
    szSection[nCpyLen] = '\0';

    *pszRemain = (*pChar == '\0')?pChar:(pChar+1);

    return (nCpyLen == 0)?S_FALSE:S_OK;
}

CParamNode* CParamStore::GetSectionNode(const char* szSection, BOOL fCreate)
{
    if ( !szSection ) return NULL;

    if (strlen(szSection) == 0) return m_pRoot;

    char* szRemain = (char*)szSection;
    char szRetSection[64] = {0};

    CParamNode* pNode(m_pRoot);
    if (pNode == NULL) return NULL;

    CParamNode* pFound = NULL;

    //如果剩余的SECTION字段不为空则继续
    while ( strlen(szRemain) > 0)
    {
        //截取首字段
        if (S_OK != ParseSection( &szRemain, szRetSection)) continue;

        //查找相符的节点
        if ( pFound = pNode->GetSubNode( szRetSection, NT_SECTION) )
        {
            //如果找到则取得子节点做为当前节点
            pNode = pFound;
        }
        else	if (fCreate)
        {
            //增加新节点
            pFound = CreateNode(szRetSection, NT_SECTION);
            if (pFound != NULL)
            {
                //加入子节点并做为当前指针
                pNode->AddSubNode(pFound);
                pNode = pFound;
            }
            else
            {
                pFound = NULL;
                break;
            }
        }
    }

    return pFound;
}

CParamNode* CParamStore::GetKeyNode(
    CParamNode* pSectionNode,
    const char* szKey,
    BOOL fCreate
)
{
    if ( !pSectionNode || !szKey ) return NULL;

    if (strlen(szKey) == 0) return NULL;

    CParamNode* pNode  = pSectionNode->GetSubNode(szKey, NT_KEY);

    if ( !pNode &&  fCreate)
    {
        pNode = CreateNode(szKey, NT_KEY);
        if (pNode != NULL)
        {
            pSectionNode->AddSubNode(pNode);
        }
    }

    return pNode;
}

CParamNode* CParamStore::GetKeyNode(
    const char* szSection,
    const char* szKey,
    BOOL fCreate
)
{
    if ( !szSection || !szKey ) return NULL;

    CParamNode* pSection = GetSectionNode(szSection, fCreate);
    if (!pSection) return NULL;

    return GetKeyNode(pSection, szKey, fCreate);
}

HRESULT CParamStore::RemoveSection(const char* szSection)
{
    if (strlen(szSection) == 0)	//如果段名为空则清除
    {
        Clear();
        return S_OK;
    }

    if (m_pClone)
    {
        m_pClone->RemoveSection(szSection);
    }

    char* szRemain = (char*)szSection;
    char szRetSection[64]= {0};

    CParamNode* pNode(m_pRoot);

    if (pNode == NULL) return E_UNEXPECTED;

    CParamNode* pTmp = NULL;
    BOOL fRemoved = FALSE;

    //如果剩余的SECTION字段不为空则继续
    while ( strlen(szRemain) > 0)
    {
        //截取首字段
        if (S_OK != ParseSection( &szRemain, szRetSection)) continue;

        //查找相符的节点
        if ( pTmp =  pNode->GetSubNode(szRetSection, NT_SECTION ) )
        {
            if (strlen(szRemain) == 0)	//此时szRetSection为要删除的字段
            {
                fRemoved = ( S_OK == pNode->RemoveSubNode(szRetSection, NT_SECTION) );
                break;
            }
            else
            {
                pNode = pTmp;
            }
        }
        else
        {
            //查找失败
            break;
        }
    }

    return fRemoved?S_OK:S_FALSE;
}

HRESULT CParamStore::RemoveKey(const char* szSection, const char* szKey)
{
    if (m_pClone)
    {
        m_pClone->RemoveKey(szSection, szKey);
    }

    if (strlen(szKey) == 0)
    {
        return RemoveSection(szSection);
    }

    CParamNode *pSection = GetSectionNode(szSection, FALSE);

    if ( !pSection ) return E_FAIL;

    return pSection->RemoveSubNode( szKey, NT_KEY);
}

//////////////////////////////////////////////////////////////////////////
//拷贝
CParamStore& CParamStore::Assign(const CParamStore& cSrcStore)
{
    if (m_pClone) m_pClone->Assign(cSrcStore);
    m_pRoot->Assign(*cSrcStore.m_pRoot);
    return *this;
}
//iSrc = NULL时则清空
CParamStore& CParamStore::Assign(const int& iSrc)
{
    if (0 == iSrc) Clear();
    return *this;
}
//追加
CParamStore& CParamStore::AddNew(const CParamStore& cSrcStore, BOOL fReplace)
{
    if (m_pClone) m_pClone->AddNew(cSrcStore, fReplace);
    m_pRoot->AddNew(*cSrcStore.m_pRoot, fReplace);
    return *this;
}

CParamStore& CParamStore::operator =(const CParamStore& cSrcStore)
{
    return Assign(cSrcStore);
}

CParamStore& CParamStore::operator = (const int& iSrc)
{
    return Assign(iSrc);
}

CParamStore& CParamStore::operator |=(const CParamStore& cSrcStore)
{
    return AddNew(cSrcStore, TRUE);
}

CParamStore& CParamStore::operator +=(const CParamStore& cSrcStore)
{
    return AddNew(cSrcStore, FALSE);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CParamStore::EnumParam(IEnumNodeCallBack* pEnumCallBack)
{
    if (m_pClone)
    {
        return m_pClone->EnumParam(pEnumCallBack);
    }
    return m_pRoot->OnEnum(pEnumCallBack);
}

HRESULT CParamStore::Extract(const char* szSection, const char* szKey, CParamStore* pDestStore)
{
    if ( !pDestStore ) return E_POINTER;

    if (m_pClone)
    {
        return m_pClone->Extract(szSection, szKey, pDestStore);	//此时只能提取已读取过的数据
    }

    HRESULT hr = S_FALSE;

    pDestStore->Clear();

    CParamNode *pSrcNode, *pDestNode;
    if (pSrcNode = GetSectionNode(szSection, FALSE) )
    {
        pDestNode = pDestStore->GetSectionNode(szSection, TRUE);
        assert(pDestNode);

        if ( strlen(szKey) == 0 ) //未指定KEY取得整个SECTION
        {
            *pDestNode = *pSrcNode;
            hr = S_OK;
        }
        else
        {
            CParamNode *pDestKeyNode , *pSrcKeyNode;
            if ( pSrcKeyNode = GetKeyNode( pSrcNode, szKey, FALSE) )
            {
                pDestKeyNode = pDestStore->GetKeyNode( pDestNode, szKey, TRUE);
                *pDestKeyNode = *pSrcKeyNode;
                hr = S_OK;
            }
        }
    }

    return hr;
}

HRESULT CParamStore::Inject(const char* szSection, const char* szKey, CParamStore* pSrcStore, DWORD32 dwMode)
{
    HRESULT hr = S_FALSE;

    if ( !pSrcStore ) return E_INVALIDARG;

    if (m_pClone)
    {
        m_pClone->Inject( szSection, szKey, pSrcStore, dwMode );
    }

    CParamNode *pSrcNode, *pDestNode;
    if (pSrcNode = pSrcStore->GetSectionNode(szSection, FALSE) )
    {
        pDestNode = GetSectionNode(szSection, TRUE);

        if ( strlen(szKey) == 0 ) //未指定KEY设置整个SECTION
        {
            hr = S_OK;
            if (dwMode == MODE_REPLACE_KEY) //以覆盖形式追加
            {
                pDestNode->AddNew(*pSrcNode, TRUE);
            }
            else if (dwMode == MODE_ADD_NEW) //只追加新数据
            {
                pDestNode->AddNew(*pSrcNode, FALSE);
            }
            else if (dwMode == MODE_REPLACE_SECTION) //直接替换
            {
                pDestNode->Assign(*pSrcNode);
            }
            else
            {
                hr = S_FALSE;
            }
        }
        else
        {
            CParamNode *pDestKeyNode , *pSrcKeyNode;
            if ( pSrcKeyNode = pSrcStore->GetKeyNode( pSrcNode, szKey, FALSE) )
            {
                pDestKeyNode = GetKeyNode( pDestNode, szKey, TRUE);
                *pDestKeyNode = *pSrcKeyNode;
                hr = S_OK;
            }
            else if ( dwMode == MODE_REPLACE_SECTION )
            {
                RemoveKey(szSection, szKey);
            }
        }
    }
    else if ( dwMode == MODE_REPLACE_SECTION )
    {
        RemoveKey(szSection,szKey);
    }

    return hr;
}
