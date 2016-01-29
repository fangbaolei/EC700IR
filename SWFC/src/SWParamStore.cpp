#include "SWParamStore.h"

#include "swpa_string.h"
#include "assert.h"

static LPCSTR HV_PARAM_NAME = "HvParam";

//CParamStore实现
//IHvParam

//////////////////////////////////////////////////////////////////////////
//取值
HRESULT STDMETHODCALLTYPE CSWParamStore::GetInt(
    LPCSTR szSection,
    LPCSTR szKey,
    INT* pVal,
    INT iDefault
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = iDefault; //先赋默认值

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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

    *pVal = (INT)pNode->m_Val;

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CSWParamStore::GetFloat(
    LPCSTR szSection,
    LPCSTR szKey,
    FLOAT* pVal,
    FLOAT fltDefault
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = fltDefault; //先赋默认值

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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

    *pVal = (FLOAT)pNode->m_Val;

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//读取字符串,读取失败将不会改变szRet内容,
//如果允许创建则将szRet做为默认值写入
HRESULT STDMETHODCALLTYPE CSWParamStore::GetString(
    LPCSTR szSection,
    LPCSTR szKey,
    LPSTR szRet,
    INT nLen
)
{
    if (!szSection || !szKey ||!szRet || nLen <= 0) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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
    swpa_memcpy(szRet, szValue, nCopyLen);
    szRet[nCopyLen] = '\0';

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//读取二进制数据,如果*pLen为0则不写入默认值
//当pBuf == NULL时只取得BIN数据长度
//读取时缓存空间不能小于此长度
HRESULT STDMETHODCALLTYPE CSWParamStore::GetBin(
    LPCSTR szSection,
    LPCSTR szKey,
    VOID* pBuf,
    INT* pLen
)
{
    if ( !szSection || !szKey || !pLen ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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

    VOID* pValueBuf;
    WORD nValueLen;

    if (FAILED(pNode->m_Val.GetBin((VOID**)&pValueBuf, &nValueLen))) return E_FAIL;

    if (pBuf) swpa_memcpy(pBuf, pValueBuf, nValueLen);
    *pLen = nValueLen;

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

//////////////////////////////////////////////////////////////////////////
//设值
HRESULT STDMETHODCALLTYPE CSWParamStore::SetInt(
    LPCSTR szSection,
    LPCSTR szKey,
    INT iVal)
{
    if ( !szSection || !szKey ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, TRUE);
    if (!pNode) return E_FAIL;

    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_INT );

    if ( !fValid )
    {
        pNode->ReInit();
    }

    pNode->SetValue(iVal);

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSWParamStore::SetFloat(
    LPCSTR szSection,
    LPCSTR szKey,
    FLOAT fltVal
)
{
    if ( !szSection || !szKey ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, TRUE);
    if (!pNode) return E_FAIL;

    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_FLOAT );

    if ( !fValid )
    {
        pNode->ReInit();
    }

    pNode->SetValue(fltVal);

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSWParamStore::SetString(
    LPCSTR szSection,
    LPCSTR szKey,
    LPCSTR szSrc
)
{
    if ( !szSection || !szKey || !szSrc ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode( szSection, szKey, TRUE );
    if (!pNode) return E_FAIL;

    BOOL fValid =( pNode->m_Val.vt == PARAM_VAR::VT_STR );

    if (!fValid)
    {
        pNode->ReInit();
    }

    pNode->SetValue(szSrc);

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT STDMETHODCALLTYPE CSWParamStore::SetBin(
    LPCSTR szSection,
    LPCSTR szKey,
    const VOID* pBuf,
    INT nBufLen
)
{
    if ( !szSection || !szKey || !pBuf || (nBufLen <= 0) ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode( szSection, szKey, TRUE );
    if (!pNode) return E_FAIL;

    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_BIN );

    if (!fValid)
    {
        pNode->ReInit();
    }

    pNode->m_Val.SetBin(pBuf, nBufLen);

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;
}

HRESULT CSWParamStore::XmlPathAnalysys( CSWString strPath )
{
    return S_OK;
}
//////////////////////////////////////////////////////////////////////////
//IHvParam2
HRESULT STDMETHODCALLTYPE CSWParamStore::GetInt(
		LPCSTR szSection,
		LPCSTR szKey,
		INT* pVal,
		INT nDefault,
		INT nMin,
		INT nMax,
		LPCSTR szChName,
		LPCSTR szComment,
		BYTE	nRank,
		INT IsOnlyRead
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = nDefault; //先赋默认值

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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
    if(IsOnlyRead)
    {
    	pNode->SetValue(nDefault);
    }

    *pVal = (INT)pNode->m_Val;

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
    pNode->SetRWFlag( IsOnlyRead );

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}


HRESULT STDMETHODCALLTYPE CSWParamStore::GetFloat(
		LPCSTR szSection,
		LPCSTR szKey,
		FLOAT* pVal,
		FLOAT fltDefault,
		FLOAT fltMin,
		FLOAT fltMax,
		LPCSTR szChName,
		LPCSTR szComment,
		BYTE	nRank,
		INT IsOnlyRead
)
{
    if (!szSection || !szKey || !pVal) return E_INVALIDARG;

    *pVal = fltDefault; //先赋默认值

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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
    
    if(IsOnlyRead)
    {
    	pNode->SetValue(fltDefault);
    }

    *pVal = (FLOAT)pNode->m_Val;

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
    pNode->SetRWFlag( IsOnlyRead );

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE CSWParamStore::GetString(
		LPCSTR szSection,
		LPCSTR szKey,
		LPSTR szRet,
		const CHAR* szDefaultVal,
		const INT nLen,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
)
{
    if (!szSection || !szKey ||!szRet || nLen <= 0) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_STR );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();

        if(swpa_strlen(szRet) > 0)
        {
        	pNode->SetValue(szRet);
        }
        else
        {
        	pNode->SetValue("NULL");
        }
        hr = S_FALSE;
    }
    if(IsOnlyRead)
    {
    	if(swpa_strlen(szRet) > 0)
      {
      	pNode->SetValue(szRet);
      }
      else
      {
      	pNode->SetValue("NULL");
      }
    }

    LPCSTR szValue = (LPCSTR)pNode->m_Val;
    if (szValue == NULL) return E_INVALIDARG;

    UINT nCopyLen = MIN_INT((UINT)(nLen - 1), (UINT)strlen(szValue));
    swpa_memcpy(szRet, szValue, nCopyLen);
    szRet[nCopyLen] = '\0';

    //写入附加信息
    pNode->SetComment(szComment);
    pNode->SetChName(szChName);
    pNode->SetRank(nRank);
    pNode->SetRWFlag( IsOnlyRead );

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}


HRESULT STDMETHODCALLTYPE CSWParamStore::GetBin(
		LPCSTR szSection,
		LPCSTR szKey,
		VOID* pBuf,
		const INT* pLen,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
)
{
    if ( !szSection || !szKey || !pLen ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
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
    if(IsOnlyRead)
    {
    	if (fDefaultValueValid)
      {
          pNode->SetValue(pBuf, *pLen);
      }
      else
      {
          char szNull[2] = {0};
          pNode->SetValue(szNull,2); //如果未指定值,则置为0.使用时可能需要进行判断
      }
    }

    VOID* pValueBuf;
    WORD nValueLen;

    if (FAILED(pNode->m_Val.GetBin((VOID**)&pValueBuf, &nValueLen))) return E_FAIL;

    if (pBuf) swpa_memcpy(pBuf, pValueBuf, nValueLen);

    //写入附加信息
    pNode->SetComment(szComment);
    pNode->SetChName(szChName);
    pNode->SetRank(nRank);
    pNode->SetRWFlag( IsOnlyRead );

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT CSWParamStore::GetEnum(
		LPCSTR szSection,
		LPCSTR szKey,
        INT* piValue,
        const INT iDefaultVal,
		const CHAR* szValueRange,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead)
{

   if (!szSection || !szKey ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, true);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_ENUM );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetEnumValue(iDefaultVal);
        hr = S_FALSE;
    }
    if(IsOnlyRead)
    {
    	pNode->SetValue(iDefaultVal);
    }

    *piValue = pNode->m_Val;
    //写入附加信息
    pNode->SetDefaultValue( iDefaultVal );

	// 如果注释为空即用valueRange来代替.
	if( (szComment == NULL || swpa_strlen(szComment) == 0)
		&& ( szValueRange != NULL && swpa_strlen(szValueRange) > 0 ) )
	{
		pNode->SetComment(szValueRange);
	}
	else
	{
		pNode->SetComment(szComment);
	}

    pNode->SetRank(nRank);
    pNode->SetChName( szChName );
    pNode->SetRWFlag( IsOnlyRead );
    pNode->SetEnum(szValueRange);

    // 查找最大最小值
    INT nMinValue = 0x7FFFFFFF; INT nMaxValue = 0 ;
    this->FindMaxMin( szValueRange , ';' , nMinValue , nMaxValue);
    pNode->SetScale( nMinValue , nMaxValue);


    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;

}
HRESULT CSWParamStore::UpdateEnum(
		LPCSTR szSection,
		LPCSTR szKey,
        INT iValue)
{

   if (!szSection || !szKey ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, false );
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_ENUM );
     // 如果节点本身存在，并为只读，则直接返回
    INT nIsOnlyRead = 0; pNode->GetRWFlag( nIsOnlyRead );
    if( fValid && nIsOnlyRead ) return E_INVALIDARG;

    if( (INT)pNode->m_MinVal > iValue || iValue > (INT)pNode->m_MaxVal) return S_FALSE;


    pNode->SetEnumValue(iValue);

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return S_OK;

}

HRESULT CSWParamStore::GetSet(
		LPCSTR szSection,
		LPCSTR szKey,
        INT* piValue,
        INT iDefaultVal,
		const CHAR* szValueRange,
		const DWORD dwMaxLen,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
    )
{
   if (!szSection || !szKey ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, m_fAutoAdd);
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_INT );
    HRESULT hr = S_OK;

    if (!fValid) //类型不对则清空,重新设值
    {
        pNode->ReInit();
        pNode->SetSetValue(iDefaultVal);
        hr = S_FALSE;
    }
    if(IsOnlyRead)
    {
    	pNode->SetValue(iDefaultVal);
    }

    *piValue = pNode->m_Val;

    pNode->SetDefaultValue( iDefaultVal );
    //写入附加信息
    pNode->SetComment(szComment);
    pNode->SetRank(nRank);
    pNode->SetChName( szChName );
    pNode->SetMaxLen( dwMaxLen );
    pNode->SetRWFlag( IsOnlyRead );
    pNode->SetSet( szValueRange );

    // 查找最大最小值
    INT nMinValue = 0x7FFFFFFF; INT nMaxValue = 0 ;
    this->FindMaxMin( szValueRange , ';' , nMinValue , nMaxValue);
    pNode->SetScale( nMinValue , nMaxValue);


    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}
/**
* 从字符串中查找最大最小值
*/
HRESULT CSWParamStore::FindMaxMin(const CHAR* szValueRange , CHAR cSplit
                                  ,INT &nMinValue , INT &nMaxValue )
{
    if( szValueRange == NULL ) return E_FAIL;

		CHAR szData[255];
		CHAR *szTmp = szData;
		INT	iValue = 0;

		for(LPCSTR ch = szValueRange; *ch != '\0'; ch++)
		{
			if(NULL != szTmp)
			{
				if(*ch != ':')
				{
					*szTmp++ = *ch;
				}
				else
				{
					*szTmp = '\0';
					if(szData[0] == '0' && szData[1] == 'x')
					{
						sscanf(szData, "0x%x", &iValue);
						if(0 == nMaxValue)
						{
							nMaxValue = iValue;
						}
						else
						{
							nMaxValue |= iValue;
						}
					}
					else
					{
						iValue = swpa_atoi(szData);
						if(nMaxValue < iValue)
						{
							nMaxValue = iValue;
						}
					}
					if(nMinValue > iValue)
					{
						nMinValue = iValue;
					}
					szTmp = NULL;
				}
			}
			else if(*ch == ';')
			{
				szTmp = szData;
			}
		}
		return S_OK;
}
HRESULT CSWParamStore::UpdateSet(
		LPCSTR szSection,
		LPCSTR szKey,
        INT iValue
    )
{
   if (!szSection || !szKey ) return E_INVALIDARG;

    CSWParamNode* pNode = GetKeyNode(szSection, szKey, false );
    if ( !pNode ) return S_FALSE; //如果节点不存在,则直接返回默认值

    //如果存在进行类型判断
    BOOL fValid = ( pNode->m_Val.vt == PARAM_VAR::VT_DATASET );
    HRESULT hr = S_OK;

    // 如果节点本身存在，并为只读，则直接返回
    INT nIsOnlyRead = 0; pNode->GetRWFlag( nIsOnlyRead );
    if( fValid && nIsOnlyRead ) return E_INVALIDARG;

    if( (INT)pNode->m_MinVal > iValue || iValue > (INT)pNode->m_MaxVal) return S_FALSE;

   // pNode->ReInit();
    pNode->SetSetValue( iValue );

    if (m_pClone)
    {
        CSWParamNode* pCloneNode = m_pClone->GetKeyNode(szSection, szKey, TRUE);
        if (pCloneNode) pCloneNode->Assign(*pNode);
    }

    return hr;
}

HRESULT CSWParamStore::Clear()
{
    SAFE_DELETE(m_pRoot);
    SAFE_DELETE(m_pClone); //已经不是CLEAR模式

    m_pRoot = CreateNode(HV_PARAM_NAME, NT_SECTION);
    assert(m_pRoot);

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//构造
CSWParamStore::CSWParamStore()
        :m_dwCookie(0)
        ,m_pRoot(NULL)
        ,m_fAutoAdd(TRUE)
        ,m_pClone(NULL)
{
    m_pRoot = CreateNode( HV_PARAM_NAME, NT_SECTION);
    assert(m_pRoot);
}

CSWParamStore::CSWParamStore(CSWParamStore& src)
{
    Assign(src);
}

CSWParamStore::~CSWParamStore()
{
    SAFE_DELETE(m_pRoot);
    SAFE_DELETE(m_pClone);
}


HRESULT CSWParamStore::EnableAutoAdd(BOOL fAutoAdd)
{
    m_fAutoAdd = fAutoAdd;
    return S_OK;
}

HRESULT CSWParamStore::SetClearMode(BOOL fClear)
{
    SAFE_DELETE(m_pClone);

    if (fClear)
    {
        m_pClone = new CSWParamStore;
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CSWParamNode* CSWParamStore::CreateNode(LPCSTR szName, PARAM_NODE_TYPE nType)
{
    CSWParamNode* pNewNode = new CSWParamNode(nType);
    if (pNewNode == NULL) return NULL;

    if ( S_OK != pNewNode->m_strName.SetString(szName) )
    {
        delete pNewNode;
        pNewNode = NULL;
    }

    return pNewNode;
}

//解析SECTION
HRESULT ParseSection(char** pszRemain, LPSTR szSection , LPSTR strCHName )
{
    LPSTR pChar = *pszRemain;
    while ( *pChar != '\\' && *pChar != '\0') pChar++;

    INT nCpyLen = (INT)(pChar - *pszRemain);
    swpa_memcpy(szSection, *pszRemain, nCpyLen);
    szSection[nCpyLen] = '\0';

    CSWString strSecton( szSection );
    int nCBegPos = strSecton.Find( "[" );
    if( nCBegPos > 0 ){
        swpa_memcpy( strCHName , szSection + nCBegPos + 1 , nCpyLen - nCBegPos - 2);
        swpa_memset( szSection + nCBegPos , 0 , strlen( szSection ) - nCBegPos);
    }


    *pszRemain = (*pChar == '\0')?pChar:(pChar+1);

    return (nCpyLen == 0)?S_FALSE:S_OK;
}

CSWParamNode* CSWParamStore::GetSectionNode(LPCSTR szSection, BOOL fCreate)
{
    if ( !szSection ) return NULL;

    if (strlen(szSection) == 0) return m_pRoot;

    LPSTR szRemain = (char*)szSection;
    char szRetSection[64] = {0};
    char strCHName[64] = {0};

    CSWParamNode* pNode(m_pRoot);
    if (pNode == NULL) return NULL;

    CSWParamNode* pFound = NULL;

    //如果剩余的SECTION字段不为空则继续
    while ( strlen(szRemain) > 0)
    {
        //截取首字段
        if (S_OK != ParseSection( &szRemain, szRetSection , strCHName )) continue;

        //查找相符的节点
        if ( pFound = pNode->GetSubNode( szRetSection, NT_SECTION) )
        {
            pFound->m_strChName = swpa_strcmp( strCHName,"" ) == 0 ? " " : strCHName;
            //如果找到则取得子节点做为当前节点
            pNode = pFound;
        }
        else if (fCreate)
        {
            //增加新节点
            pFound = CreateNode(szRetSection, NT_SECTION);
            if (pFound != NULL)
            {
                pFound->m_strChName = swpa_strcmp( strCHName,"" ) == 0 ? " " : strCHName;
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

CSWParamNode* CSWParamStore::GetSectionNode(CSWParamNode* pParentNode , LPCSTR szSection , BOOL fCreate)
{
    if ( NULL == pParentNode ) return NULL;

    CSWParamNode* pNode(pParentNode);
    if (pNode == NULL) return NULL;

    CSWParamNode* pFound = NULL;

    //如果剩余的SECTION字段不为空则继续
    while ( (pFound = pNode->GetNextSubNode( pFound )) != NULL)
    {
        //查找相符的节点
        if ( swpa_strcmp( szSection, pFound->m_strName ) == 0 )
        {
            //如果找到则取得子节点做为当前节点
            break;
        }
    }

    if ( NULL == pFound && fCreate)
    {
        //增加新节点
        pFound = CreateNode(szSection, NT_SECTION);
        if (pFound != NULL)
        {
            pFound->m_strChName = " ";
            //加入子节点并做为当前指针
            pNode->AddSubNode(pFound);
            pNode = pFound;
        }
    }

    return pFound;
}

CSWParamNode* CSWParamStore::GetKeyNode(
    CSWParamNode* pSectionNode,
    LPCSTR szKey,
    BOOL fCreate
)
{
    if ( !pSectionNode || !szKey ) return NULL;

    if (strlen(szKey) == 0) return NULL;

    CSWParamNode* pNode  = pSectionNode->GetSubNode(szKey, NT_KEY);

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

CSWParamNode* CSWParamStore::GetKeyNode(
    LPCSTR szSection,
    LPCSTR szKey,
    BOOL fCreate
)
{
    if ( !szSection || !szKey ) return NULL;

    CSWParamNode* pSection = GetSectionNode(szSection, fCreate);
    if (!pSection) return NULL;

    return GetKeyNode(pSection, szKey, fCreate);
}

HRESULT CSWParamStore::RemoveSection(LPCSTR szSection)
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

    LPSTR szRemain = (char*)szSection;
    char szRetSection[64]= {0};

    CSWParamNode* pNode(m_pRoot);
    char strCHName[64] = {0};

    if (pNode == NULL) return E_UNEXPECTED;

    CSWParamNode* pTmp = NULL;
    BOOL fRemoved = FALSE;

    //如果剩余的SECTION字段不为空则继续
    while ( strlen(szRemain) > 0)
    {
        //截取首字段
        if (S_OK != ParseSection( &szRemain, szRetSection,strCHName)) continue;

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

HRESULT CSWParamStore::RemoveKey(LPCSTR szSection, LPCSTR szKey)
{
    if (m_pClone)
    {
        m_pClone->RemoveKey(szSection, szKey);
    }

    if (strlen(szKey) == 0)
    {
        return RemoveSection(szSection);
    }

    CSWParamNode *pSection = GetSectionNode(szSection, FALSE);

    if ( !pSection ) return E_FAIL;

    return pSection->RemoveSubNode( szKey, NT_KEY);
}

//////////////////////////////////////////////////////////////////////////
//拷贝
CSWParamStore& CSWParamStore::Assign(const CSWParamStore& cSrcStore)
{
    if (m_pClone) m_pClone->Assign(cSrcStore);
    m_pRoot->Assign(*cSrcStore.m_pRoot);
    return *this;
}
//iSrc = NULL时则清空
CSWParamStore& CSWParamStore::Assign(const INT& iSrc)
{
    if (0 == iSrc) Clear();
    return *this;
}
//追加
CSWParamStore& CSWParamStore::AddNew(const CSWParamStore& cSrcStore, BOOL fReplace)
{
    if (m_pClone) m_pClone->AddNew(cSrcStore, fReplace);
    m_pRoot->AddNew(*cSrcStore.m_pRoot, fReplace);
    return *this;
}

CSWParamStore& CSWParamStore::operator =(const CSWParamStore& cSrcStore)
{
    return Assign(cSrcStore);
}

CSWParamStore& CSWParamStore::operator = (const INT& iSrc)
{
    return Assign(iSrc);
}

CSWParamStore& CSWParamStore::operator |=(const CSWParamStore& cSrcStore)
{
    return AddNew(cSrcStore, TRUE);
}

CSWParamStore& CSWParamStore::operator +=(const CSWParamStore& cSrcStore)
{
    return AddNew(cSrcStore, FALSE);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CSWParamStore::EnumParam(IEnumNodeCallBack* pEnumCallBack)
{
    if (m_pClone)
    {
        return m_pClone->EnumParam(pEnumCallBack);
    }
    return m_pRoot->OnEnum(pEnumCallBack);
}

HRESULT CSWParamStore::Extract(LPCSTR szSection, LPCSTR szKey
                               , CSWParamStore* pDestStore)
{
    if ( !pDestStore ) return E_POINTER;

    if (m_pClone)
    {
        return m_pClone->Extract(szSection, szKey, pDestStore);	//此时只能提取已读取过的数据
    }

    HRESULT hr = S_FALSE;

    pDestStore->Clear();

    CSWParamNode *pSrcNode, *pDestNode;
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
            CSWParamNode *pDestKeyNode , *pSrcKeyNode;
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

HRESULT CSWParamStore::Inject(LPCSTR szSection, LPCSTR szKey
                              , CSWParamStore* pSrcStore, DWORD dwMode)
{
    HRESULT hr = S_FALSE;

    if ( !pSrcStore ) return E_INVALIDARG;

    if (m_pClone)
    {
        m_pClone->Inject( szSection, szKey, pSrcStore, dwMode );
    }

    CSWParamNode *pSrcNode, *pDestNode;
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
            CSWParamNode *pDestKeyNode , *pSrcKeyNode;
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
//存储, SimpMode下只保存值,不保存附加信息
HRESULT CSWParamStore::SaveToStream(CSWStream* pStream, BOOL fSimpMode)
{
    if( m_pRoot != NULL ){
        CSWParamNode *pSudNode = NULL; CSWParamNode *pTemNode = NULL;

        while( (pSudNode = m_pRoot->GetNextSubNode( pSudNode )) != NULL){
            pSudNode->SaveToStream( pStream, fSimpMode );
        }
    }

    return S_OK;
}
// 从文件流中恢复节点
HRESULT CSWParamStore::RestoreFromStream(CSWStream* pStream)
{
    if( m_pRoot == NULL ) return E_FAIL;
       // 清理原来旧的数据信息
    this->Clear();

    CSWFileStream *pSWFileStream = (CSWFileStream*)pStream;
    INT nCurrentPos = pSWFileStream->GetCurrentPos( );
	INT nSize = pSWFileStream->GetSize( );

    while( nCurrentPos < nSize){

        CSWParamNode *pSudNode =  new CSWParamNode( NT_SECTION );
        pSudNode->RestoreFromStream( pStream );

        // 如果当前节点没有重覆，则添加为子节点
        m_pRoot->AddSubNode( pSudNode );

        nCurrentPos = pSWFileStream->GetCurrentPos( );
    }
}

// 从文件流中恢复节点
HRESULT CSWParamStore::RestoreFromXml( TiXmlDocument *pXmlDoc )
{
    return this->XmlToParam( pXmlDoc );
}

HRESULT CSWParamStore::ToXml( CSWString& strParamXml )
{
    if( m_pRoot == NULL ) return S_FALSE;
    strParamXml = "";

    CSWParamNode *pSudNode = NULL;

    while( (pSudNode = m_pRoot->GetNextSubNode( pSudNode )) != NULL){
            pSudNode->ToXml( strParamXml  );
    }


    // 如果没有根节点则添加该节点
    CSWString strTemp ;
    strTemp.Append(
    	"<?xml version=\"1.0\" encoding=\"gb2312\" standalone=\"yes\" ?>\r\n"
    	"<HvParamDoc version=\"1.0\">\r\n"
    	"<HvParam>\r\n"
   	);
    strTemp.Append( strParamXml );
    strTemp.Append(
    	 "</HvParam>\r\n"
    	 "</HvParamDoc>\r\n"
   );

    strParamXml = strTemp;

   // const CHAR* pXml = strParamXml;

    //TiXmlText textXml( (CHAR*)pXml);

    //textXml.Parse( const char* p, TiXmlParsingData* data, TiXmlEncoding encoding );

    return S_OK;
}

HRESULT CSWParamStore::XmlToParam( TiXmlDocument *pXmlDoc )
{
    if( pXmlDoc == NULL && m_pRoot == NULL) return S_FALSE;

   // 清理原来旧的数据信息,需要合并，有则修改，没有则增加。
	 // todo.
	 //this->Clear();

    TiXmlElement* pParamElement = pXmlDoc->RootElement()->FirstChildElement("HvParam");
    if (pParamElement == NULL) return E_FAIL;

    TiXmlElement* pChild = NULL;
    for ( pChild = pParamElement->FirstChildElement("SECTION"); pChild ; pChild = pChild->NextSiblingElement("SECTION"))
    {
        CSWParamNode *pSectionNode = ReadSectionNode( pChild->ToElement() , m_pRoot);
    }

    return  S_OK;
}
CSWParamNode* CSWParamStore::ReadSectionNode( TiXmlElement* pElement , CSWParamNode *pParentNode )
{
    if (!pElement) return NULL;

    LPCSTR szName = pElement->Attribute("name");
    LPCSTR szCHName = pElement->Attribute("cname");

    CSWParamNode *pSectionNode = this->GetSectionNode( pParentNode , szName , true );

    pSectionNode->m_strName = szName;
    pSectionNode->m_strChName = szCHName == NULL ? "" : szCHName;

    CSWParamNode* pChildNode = NULL;
    TiXmlElement* pChild = NULL;
    for ( pChild = pElement->FirstChildElement("SECTION"); pChild ; pChild = pChild->NextSiblingElement("SECTION"))
    {
        pChildNode = ReadSectionNode(pChild->ToElement() , pSectionNode);

    }

    for ( pChild = pElement->FirstChildElement("KEY"); pChild; pChild = pChild->NextSiblingElement("KEY"))
    {
        pChildNode = ReadKeyNode(pChild->ToElement() , pSectionNode );
    }

    return pSectionNode;
}
CSWParamNode* CSWParamStore::ReadKeyNode( TiXmlElement* pElement , CSWParamNode *pSectionNode )
{
    if (!pElement) return NULL;


    LPCSTR szName = pElement->Attribute("name");
    if (szName == NULL){ return NULL;}

    CSWParamNode *pKeyNode = GetKeyNode( pSectionNode , szName ,true);

    if ( pKeyNode == NULL ) return NULL;

    pKeyNode->m_strName = szName;

    CSWParamVar cValueVar;
    TiXmlElement* pValue = pElement->FirstChildElement("VALUE");
    if (!pValue) { delete pKeyNode; return NULL;}

    if (FAILED(ConvertXml2Var(pValue, &cValueVar))) { delete pKeyNode; return NULL;}
	
		//进行有效性判断,TODO
		INT IsOnlyRead = 0;
		pKeyNode->GetRWFlag(IsOnlyRead);
		if(!IsOnlyRead)
    {
    	pKeyNode->SetValue(cValueVar);
  	}

/*
    TiXmlElement* pDefault = pElement->FirstChildElement("DEFAULT");
    if ( pDefault && SUCCEEDED(ConvertXml2Var(pValue, &cValueVar)))
    {
        pKeyNode->SetDefaultValue(cValueVar);
    }

    CSWParamVar cMinVar, cMaxVar;
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
            pKeyNode->SetRank(swpa_atoi(szRank));
        }
    }

   TiXmlElement* pOnlyRead = pElement->FirstChildElement("ONLYREAD");
   if( pOnlyRead )
    {
        LPCSTR szOnlyRead = pOnlyRead->GetText();
        if (pOnlyRead)
        {
            pKeyNode->SetRWFlag(swpa_atoi(szOnlyRead));
        }
    }

	TiXmlElement* pEnum = pElement->FirstChildElement("ENUM");
    if( pEnum != NULL){
        LPCSTR szValue = pEnum->GetText();

        pKeyNode->SetEnum( szValue );
    }

	TiXmlElement* pSet = pElement->FirstChildElement("SET");
    if( pSet != NULL){
        LPCSTR szValue = pSet->GetText();

        pKeyNode->SetSet( szValue );
    }
*/
    return pKeyNode;
}

HRESULT CSWParamStore::ConvertXml2Var(TiXmlElement* pElement, CSWParamVar* pVar)
{
    if ( !pElement || !pVar ) return E_INVALIDARG;

    LPCSTR szType = pElement->Attribute("type");
    if ( !szType ) return E_FAIL;

    LPCSTR szValue = pElement->GetText();
    if (!szValue && swpa_strcmp(szType, "STR"))
    {
    	return E_FAIL;
    }

	HRESULT  hr = S_OK;
	if (swpa_strcmp(szType,"INT") == 0)
	{
		*pVar = (INT)atoi(szValue);
	}
	else if (swpa_strcmp(szType, "UINT") == 0)
    {
        *pVar = (UINT)atoi(szValue);
    }
    else if (swpa_strcmp(szType, "FLOAT") == 0)
    {
        *pVar = (float)atof(szValue);
    }
    else if (swpa_strcmp(szType, "STR") == 0)
    {
		// zhaopy
		// 限制字符串的最大长度，不能超过1K。
		if( swpa_strlen(szValue) > 1024 )
		{
			hr = pVar->SetString("NULL");
		}
		else
		{
			hr = pVar->SetString(szValue);
		}
    }
	else if (swpa_strcmp(szType, "ENUM") == 0)
	{
		pVar->vt = _PARAM_VAR::VT_ENUM;
		pVar->val.Int = (INT)atoi(szValue);
	}
	else if (swpa_strcmp(szType, "SET") == 0)
	{
		pVar->vt = _PARAM_VAR::VT_DATASET;
		pVar->val.Int = (INT)atoi(szValue);
	}
    else
    {
        hr = E_INVALIDARG;
    }

    return hr;
}


