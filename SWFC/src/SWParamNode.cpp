#include "SWParamNode.h"
//#include "assert.h"

//#include "hvutils.h"

#define RTN_IF_FAILED(fun) { HRESULT hr = fun; if(FAILED(hr)) return hr; }

//CParamNode实现
CSWParamNode::CSWParamNode(WORD nFlag)
    :m_nFlag(nFlag)
    ,m_nInfoMask(NI_NONE)
    ,m_pSubNode(NULL)
{
    m_strName = "NULL";
	if ( nFlag & NT_SECTION )
	{
		m_pSubNode = new CSWParamNodeEnum;
		//assert(m_pSubNode);
	}
	// 初始化只读标记
	this->SetRWFlag( 0 );
}

CSWParamNode::CSWParamNode(CSWParamNode& src)
{
	Assign(src);
}

CSWParamNode::~CSWParamNode()
{
	Clear();
}

HRESULT CSWParamNode::Clear()
{
//	SAFE_DELETE(m_pSubNode);
	//m_pParent = NULL;
	m_nFlag = NT_NULL;
	m_nInfoMask = NI_NONE;

	m_strName.Clear();
	m_Val.Clear();
	m_DefaultVal.Clear();
	m_MinVal.Clear();
	m_MaxVal.Clear();
	m_Rank.Clear();
	m_strComment.Clear();
	m_strChName.Clear();

	return S_OK;
}

HRESULT CSWParamNode::ReInit() //重初始化
{
	if(m_nFlag & NT_SECTION)
	{
//		SAFE_DELETE(m_pSubNode);
		m_pSubNode = new CSWParamNodeEnum;
//		assert(m_pSubNode);
	}
	else if(m_nFlag & NT_KEY)
	{
		m_nFlag &= ~NF_INFOMASK;
		m_nInfoMask = NI_NONE;

		m_Val.Clear();
		m_DefaultVal.Clear();
		m_MinVal.Clear();
		m_MaxVal.Clear();
		m_Rank.Clear();
		m_strComment.Clear();
		m_strChName.Clear();
	}
	return S_OK;
}


//子结点操作
HRESULT CSWParamNode::AddSubNode(CSWParamNode* pNode)
{
	if (!pNode) return E_POINTER;

	if ( !IsSectionNode() ) return S_FALSE;

	//pNode->m_pParent = this;

	return m_pSubNode->EnumBase_Add(pNode);
}

HRESULT CSWParamNode::RemoveSubNode( LPCSTR szNodeName, PARAM_NODE_TYPE nType )
{
	if (!szNodeName) return E_POINTER;

	if( !IsSectionNode() ) return S_FALSE;

	CSWParamNode* pNode = GetSubNode( szNodeName, nType );

	return m_pSubNode->EnumBase_Remove(pNode);
}

CSWParamNode* CSWParamNode::GetSubNode( LPCSTR szNodeName, PARAM_NODE_TYPE nType )
{
	if ( !szNodeName ||
		  !IsSectionNode() )
	{
		return NULL;
	}

	CSWParamNode* pNode;
	BOOL fFound = FALSE;
	m_pSubNode->EnumBase_Reset();
	while(S_OK == m_pSubNode->EnumBase_Next(1, &pNode, NULL))
	{
		if( pNode->m_strName == szNodeName &&
			( (pNode->m_nFlag & 0x0003) == nType ) )
		{
			fFound = TRUE;
			break;
		}
	}

	return fFound?pNode:NULL;
}

CSWParamNode* CSWParamNode::GetNextSubNode( CSWParamNode* pNode )
{
	if( !IsSectionNode() || NULL == m_pSubNode) return NULL;

	if(pNode == NULL)
	{
		m_pSubNode->EnumBase_Reset();
	}

	CSWParamNode* pTmp;
	HRESULT  hr = m_pSubNode->EnumBase_Next(1, &pTmp, NULL);

	return (hr == S_OK)?pTmp:NULL;
}

//遍历操作
HRESULT CSWParamNode::OnEnum(IEnumNodeCallBack* pCallBack)
{
	if (pCallBack == NULL) return E_POINTER;

	pCallBack->OnEnumNode(this);

	if (!IsSectionNode()) return S_OK;

	CSWParamNode* pNode = NULL;
	while( pNode = GetNextSubNode(pNode) )
	{
		if(pNode->IsNullNode()) continue;
		pNode->OnEnum(pCallBack);
	}

	//返回结束标志
	CSWParamNode cNullNode(NT_NULL);
	pCallBack->OnEnumNode(&cNullNode);

	return S_OK;
}

//覆盖
CSWParamNode& CSWParamNode::Assign( const CSWParamNode& cSrcNode)
{
	Clear(); //如果源节点为空,则目标节点也会被置空

	CSWParamNode* pSrcNode = const_cast<CSWParamNode*>(&cSrcNode);

	m_strName = pSrcNode->m_strName;

	if(pSrcNode->IsSectionNode() &&
		pSrcNode->m_pSubNode )
	{
		m_nFlag |= NT_SECTION;
		m_pSubNode = new CSWParamNodeEnum;
//		assert(m_pSubNode);

		CSWParamNode* pNode = NULL;
		CSWParamNode* pNewNode = NULL;
		while( pNode = pSrcNode->GetNextSubNode(pNode) )
		{
			if(pNode->IsNullNode()) continue;
			pNewNode = new CSWParamNode;
			pNewNode->Assign(*pNode);
			AddSubNode(pNewNode);
		}
	}
	else if(pSrcNode->IsKeyNode())
	{
		m_nFlag |= NT_KEY;
		m_Val = pSrcNode->m_Val;

		if(pSrcNode->HasExtInfo())
		{
			m_nFlag |= NF_INFOMASK;
			m_nInfoMask = pSrcNode->m_nInfoMask;
			m_DefaultVal = pSrcNode->m_DefaultVal;
			m_MinVal = pSrcNode->m_MinVal;
			m_MaxVal = pSrcNode->m_MaxVal;
			m_Rank = pSrcNode->m_Rank;
			m_strComment = pSrcNode->m_strComment;
			m_strChName = pSrcNode->m_strChName;
		}
	}

	return *this;
}

//合并,如fReplace为FALSE则只进行追加
CSWParamNode& CSWParamNode::AddNew( const CSWParamNode& cSrcNode, BOOL fReplace)
{
	CSWParamNode* pSrcNode = const_cast<CSWParamNode*>(&cSrcNode);

	if ( m_strName != pSrcNode->m_strName )	//必须是同名节点才能合并
	{
		return *this;
	}

	//如果是都是Key节点并且是覆盖形式则赋值
	if ( IsKeyNode() )
	{
		if (fReplace && pSrcNode->IsKeyNode() )
		{
			m_Val = pSrcNode->m_Val;

			//如果源节点没有附加信息,则只进行赋值
			if(pSrcNode->HasExtInfo())
			{
				WORD dwFlag = pSrcNode->m_nInfoMask;

				if(dwFlag & NI_DEFAULT_VAL)
				{
					SetDefaultValue(pSrcNode->m_DefaultVal);
				}

				if(dwFlag & NI_SCALE)
				{
					SetScale(pSrcNode->m_MinVal, pSrcNode->m_MaxVal);
				}

				if(dwFlag & NI_COMMENT)
				{
					SetComment( (LPCSTR)pSrcNode->m_strComment);
				}

				if(dwFlag & NI_CHNAME)
				{
					SetChName((LPCSTR)pSrcNode->m_strChName);
				}

				if(dwFlag & NI_RANK)
				{
					SetRank(pSrcNode->m_Rank);
				}
			}
		}

		return *this;
	}

	//只有都是Section节点才进行合并
	if ( !IsSectionNode() ||
		 !pSrcNode->IsSectionNode() )
	{
		return *this;
	}

	CSWParamNode* pSrcSubNode = NULL;
	CSWParamNode* pSubNode;
	PARAM_NODE_TYPE nType;

	while( pSrcSubNode = pSrcNode->GetNextSubNode(pSrcSubNode) )
	{
		if(pSrcSubNode->IsNullNode()) continue;
		nType = (PARAM_NODE_TYPE)(pSrcSubNode->m_nFlag&0x0003);
		if ( pSubNode = this->GetSubNode((LPCSTR)(pSrcSubNode->m_strName), nType)  )
		{
			pSubNode->AddNew( *pSrcSubNode, fReplace);	//如果找到匹配的子节点, 则进行合并
		}
		else
		{
			pSubNode = new CSWParamNode;
			if(pSubNode)
			{
				*pSubNode = *pSrcSubNode;
				AddSubNode(pSubNode);
			}
		}
	}
	return *this;
}
HRESULT CSWParamNode::SaveToStream(CSWStream* pStream, BOOL fSimpMode)
{
	if ( !pStream) return E_POINTER;

	HRESULT hr(E_FAIL);
	if(IsNullNode())
	{
		//如果是NULL节点只写入标志值(用于存储结束标志)
		hr = pStream->Write(&m_nFlag, sizeof(m_nFlag), NULL);
	}
	else if (IsKeyNode())
	{
		hr = SaveAsKeyNode(pStream, fSimpMode);
	}
	else if (IsSectionNode())
	{
		hr = SaveAsSectionNode(pStream, fSimpMode);
	}

	return hr;
}
//存储操作
//SimpMode: flag name value
//AdvMode: flag infomask name value [defaultvalue] [scale] [comment] [chname] [rank]
HRESULT CSWParamNode::SaveAsKeyNode(CSWStream* pStream, BOOL fSimpMode)
{
    CSWString strName = CSWString( (const CHAR*)m_strName);
	if( !pStream || strName.Compare("") ==0) return E_INVALIDARG;

	WORD nFlag = fSimpMode ? (m_nFlag &~ NF_INFOMASK) : m_nFlag;
    // 保存节点标记
	RTN_IF_FAILED(pStream->Write(&nFlag, sizeof(nFlag), NULL));
	// 保存只读标记
	INT IsOnlyRead = 0 ; this->GetRWFlag( IsOnlyRead );
	RTN_IF_FAILED(pStream->Write(&IsOnlyRead, sizeof(IsOnlyRead), NULL));

	BOOL fWriteInfoMask = !fSimpMode && (nFlag & NF_INFOMASK);

	if( fWriteInfoMask ){
		RTN_IF_FAILED(pStream->Write(&m_nInfoMask, sizeof(m_nInfoMask), NULL));
	}

	// 名字和值必备
	RTN_IF_FAILED(m_strName.WriteToStream(pStream));
	RTN_IF_FAILED(m_Val.WriteToStream(pStream));

	if( !fWriteInfoMask ) return S_OK;


	if( m_nInfoMask & NI_DEFAULT_VAL){
		RTN_IF_FAILED(m_DefaultVal.WriteToStream(pStream));
	}

	if(	 m_nInfoMask & NI_SCALE){
		RTN_IF_FAILED(m_MinVal.WriteToStream(pStream));
		RTN_IF_FAILED(m_MaxVal.WriteToStream(pStream));
	}

	if(m_nInfoMask & NI_COMMENT){
		RTN_IF_FAILED(m_strComment.WriteToStream(pStream));
	}

	if(m_nInfoMask & NI_CHNAME){
		RTN_IF_FAILED(m_strChName.WriteToStream(pStream));
	}

	if(m_nInfoMask & NI_RANK){
		RTN_IF_FAILED(m_Rank.WriteToStream(pStream));
	}

	return S_OK;
}
HRESULT CSWParamNode::SaveAsSectionNode(CSWStream* pStream, BOOL fSimpMode)
{
    CSWString strName = CSWString( (const CHAR*)m_strName);
	if ( !pStream || strName.Compare("") ==0 ) return E_INVALIDARG;


    RTN_IF_FAILED(pStream->Write(&m_nFlag, sizeof(m_nFlag), NULL));

    RTN_IF_FAILED(m_strName.WriteToStream(pStream));

	HRESULT hr(S_OK);
	CSWParamNode* pNode;
	m_pSubNode->EnumBase_Reset();
	while(S_OK == m_pSubNode->EnumBase_Next(1, &pNode, NULL) )
	{
		//通过Assign有可能将某节点置为NULL,存储时跳过这些节点
		if(pNode->IsNullNode()) continue;

		hr = pNode->SaveToStream(pStream, fSimpMode);
		if(FAILED(hr)) break;
	}

	if ( !FAILED(hr) )
	{
		CSWParamNode cNode(NT_NULL);
		hr = cNode.SaveToStream(pStream, fSimpMode);	//写入一个空节点做为结束标志
	}

	return hr;
}
/**
 * @brief 从文件流中恢复数据结点
 * @param [in] pStream : 二进制文件流
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWParamNode::RestoreFromStream(CSWStream *pStream)
{
	if ( !pStream ) return E_INVALIDARG;

    // 清楚原来数据
	// this->Clear();

	RTN_IF_FAILED(pStream->Read(&m_nFlag, sizeof(m_nFlag), NULL));

	HRESULT hr = E_FAIL;

	if( (m_nFlag & 0x03) == NT_NULL) //空结点只做为结束标志,实际参数树中不会有空节点
	{
		hr = S_FALSE;
	}
	else if(m_nFlag & NT_KEY)
	{// 读取节点值
		hr = this->ReadAsKeyNode(pStream, m_nFlag);
	}
	else if(m_nFlag & NT_SECTION)
	{// 读取父节点值
		hr = this->ReadAsSectionNode(pStream, m_nFlag);
	}

	return hr;
}
/**
 * @brief 读取KEY结点数据
 * @param [in] pStream : 二进制文件流
 * @param [in] nFlag : 结点标志
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWParamNode::ReadAsKeyNode(CSWStream* pStream, WORD nFlag)
{
	if(!pStream) return E_INVALIDARG;

	// 读取当前读写标记
	INT IsOnlyRead = 0 ;
	RTN_IF_FAILED(pStream->Read(&IsOnlyRead, sizeof(IsOnlyRead), NULL));
    this->SetRWFlag( IsOnlyRead );

	if(nFlag & NF_INFOMASK)
	{
		RTN_IF_FAILED(pStream->Read(&m_nInfoMask,sizeof(m_nInfoMask),NULL));
	}

	m_strName.ReadFromStream(pStream);
	m_Val.ReadFromStream(pStream);

	if(m_nInfoMask & NI_DEFAULT_VAL)
	{
		m_DefaultVal.ReadFromStream(pStream);
	}

	if(m_nInfoMask & NI_SCALE)
	{
		m_MinVal.ReadFromStream(pStream);
		m_MaxVal.ReadFromStream(pStream);
	}

	if(m_nInfoMask & NI_COMMENT)
	{
		m_strComment.ReadFromStream(pStream);
	}

	if(m_nInfoMask & NI_CHNAME)
	{
		m_strChName.ReadFromStream(pStream);
	}

	if (m_nInfoMask & NI_RANK)
	{
		m_Rank.ReadFromStream(pStream);
	}

	return S_OK;
}
/**
 * @brief 读取Section结点数据
 * @param [in] pStream : 二进制文件流
 * @return - S_OK : 成功 - E_FAIL : 失败
 */
HRESULT CSWParamNode::ReadAsSectionNode( CSWStream* pStream, WORD nFlag )
{
	if(!pStream) return E_INVALIDARG;

	SAFE_DELETE(m_pSubNode);
	m_pSubNode =  new CSWParamNodeEnum;
	if (m_pSubNode == NULL) return E_OUTOFMEMORY;

	RTN_IF_FAILED(m_strName.ReadFromStream(pStream));
//	RTN_IF_FAILED(m_strChName.ReadFromStream(pStream));

	HRESULT hr(E_FAIL);
	while( true )
	{
		CSWParamNode* pNode = new CSWParamNode;
		if ( pNode == NULL)
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		hr = pNode->RestoreFromStream(pStream);
		if (hr == S_OK)
		{
			hr = m_pSubNode->EnumBase_Add(pNode);
			continue;
		}
		else if (hr == S_FALSE)	//读到结束标志
		{
			hr = S_OK;
		}

		//结束或者读取失败直接跳出
		SAFE_DELETE(pNode);
		break;
	}

	return hr;

}
HRESULT CSWParamNode::ToXml( CSWString &strXml )
{
    HRESULT hr = S_OK;
 	if( (m_nFlag & 0x03) == NT_NULL) //空结点只做为结束标志,实际参数树中不会有空节点
	{
		hr = S_FALSE;
	}
	else if(m_nFlag & NT_KEY)
	{// 读取节点值
		hr = this->KeyNodeToXml( strXml );
	}
	else if(m_nFlag & NT_SECTION)
	{// 读取父节点值
		hr = this->SectionNodeToXml( strXml );
	}

	return hr;
}


HRESULT CSWParamNode::SectionNodeToXml( CSWString &strXml )
{
    CSWString strSectionXml;

    CSWString strName = CSWString( (const CHAR*)m_strName);

    if( 0 == strName.Compare( "HvParam" )){
        strSectionXml.Append( "<HvParam>\r\n" );
    }
    else if( strName.Compare("NULL") == 0) return E_FAIL;
    else{
        strSectionXml.Append("<SECTION name=\"");
        strSectionXml.Append(m_strName);
        strSectionXml.Append("\" cname=\"");
        strSectionXml.Append( m_strChName );
        strSectionXml.Append("\">\r\n" );
    }

    CSWParamNode* pKeyNode = NULL; HRESULT hr = S_OK;
    while( (pKeyNode = this->GetNextSubNode( pKeyNode )) != NULL )
     {
        hr = pKeyNode->ToXml( strSectionXml );
     }

    strXml.Append(strSectionXml);

    if( 0 == strName.Compare( "HvParam" )){
        strXml.Append( "</HvParam>\r\n" );
    }
    else
        strXml.Append("</SECTION>\r\n");

    return hr;
}

HRESULT CSWParamNode::KeyNodeToXml( CSWString &strXml )
{
    CSWString strNodeName = CSWString( (const CHAR*)m_strName);
    if( strNodeName.Compare("NULL") == 0) return E_FAIL;

    // 增加key结点
    strXml.Append("<KEY name=\"");
    strXml.Append( m_strName );
    strXml.Append("\">\r\n" );

    CHAR *buffer = NULL;
    if( m_Val.GetValLen() < 512 )
        buffer = new CHAR[512];
    else
        buffer = new CHAR[m_Val.GetValLen() + 1];


    CSWString strValue ,strDefaultValue, strMin , strMan
    , strChName , strRank , strComment , strType;

    switch( m_Val.vt )
    {
        case _PARAM_VAR::VT_INT:
            swpa_sprintf( buffer , "%d", (INT)m_Val);
            strType = "INT" ;
			break;
        case _PARAM_VAR::VT_FLOAT:
             swpa_sprintf( buffer , "%f", (FLOAT)m_Val);
             strType = "FLOAT" ;
			 break;
        case _PARAM_VAR::VT_STR:
             swpa_sprintf( buffer , "%s", (LPCSTR)m_Val);
             strType = "STR" ;
			 break;
		case _PARAM_VAR::VT_ENUM:
			swpa_sprintf( buffer, "%d", (INT)m_Val);
			strType = "ENUM" ;
			break;
		case _PARAM_VAR::VT_DATASET:
			swpa_sprintf( buffer, "%d", (INT)m_Val);
			strType = "SET" ;
			break;
        default:
            break;
    }

    strValue = CSWString( buffer);

    // 添加值
    strXml.Append("<VALUE type=\"" );
    strXml.Append(strType  + "\">");
    strXml.Append( strValue + "</VALUE>");

    strXml.Append("<CHNAME type=\"STR\">" );
    strXml.Append( m_strChName );
    strXml.Append( "</CHNAME>");

	const CHAR* pszTemp = (const CHAR*)m_strComment;
	if( pszTemp != NULL && swpa_strlen(pszTemp) > 0 )
	{
		strXml.Append("<COMMENT type=\"STR\">" );
		strXml.Append( m_strComment );
		strXml.Append( "</COMMENT>");
	}

	if( (m_nInfoMask & NI_DEFAULT_VAL) == NI_DEFAULT_VAL )
	{
		switch( m_DefaultVal.vt )
		{
		case _PARAM_VAR::VT_INT:
			swpa_sprintf( buffer , "%d", (INT)m_DefaultVal);
			strDefaultValue=CSWString(buffer);
			break;
		case _PARAM_VAR::VT_FLOAT:
			swpa_sprintf( buffer , "%f", (FLOAT)m_DefaultVal);
			strDefaultValue=CSWString(buffer);
			break;
		case _PARAM_VAR::VT_STR:
			swpa_sprintf( buffer , "%s", (LPCSTR)m_DefaultVal);
			strDefaultValue=CSWString(buffer);
			break;
		case _PARAM_VAR::VT_ENUM:
			swpa_sprintf( buffer, "%d", (INT)m_DefaultVal);
			strDefaultValue = CSWString(buffer);
			break;
		case _PARAM_VAR::VT_DATASET:
			swpa_sprintf( buffer, "%d", (INT)m_DefaultVal);
			strDefaultValue = CSWString(buffer);
			break;
		default:
			break;
		}
		// 添加默认值
		strXml.Append("<DEFAULT type=\"" );

		if( m_Val.vt == _PARAM_VAR::VT_ENUM ||  _PARAM_VAR::VT_DATASET ==  m_Val.vt )
		{
			strXml.Append("INT\">");
		}
		else
		{
			strXml.Append(strType + "\">");
		}

		strXml.Append( strDefaultValue + "</DEFAULT>");
	}

	// zhaopy
	// 只有INT UINT FLOAT 类型有最小最大值。

	if( (m_nInfoMask & NI_SCALE) == NI_SCALE )
	{
		// 获取最小值
		swpa_sprintf( buffer , "%d", (INT)m_MinVal);
		strMin = CSWString( buffer);

		strXml.Append("<MIN type=\"" );
		strXml.Append("INT\">");
		strXml.Append( CSWString(buffer) + "</MIN>");

		// 获取最大值
		swpa_sprintf( buffer , "%d", (INT)m_MaxVal);
		strMin = CSWString( buffer);

		strXml.Append("<MAX type=\"" );
		strXml.Append("INT\">");
		strXml.Append( CSWString(buffer) + "</MAX>");
	}

    // 获取等级
    swpa_sprintf( buffer , "%d", (INT)m_Rank);
    strRank = CSWString( buffer);

    strXml.Append("<RANK type=\"" );
    strXml.Append("INT\">");
    strXml.Append( CSWString(buffer) + "</RANK>");

    const CHAR *szEnum = m_strEnum;
    if( szEnum != NULL )
    {
         strXml.Append("<ENUM type=\"" );
         strXml.Append("STR\">");
         strXml.Append( CSWString(szEnum) + "</ENUM>");
    }

    const CHAR *strSet = m_strSet;
    if( strSet != NULL )
    {
         strXml.Append("<SET type=\"" );
         strXml.Append("STR\">");
         strXml.Append( CSWString(strSet) + "</SET>");
    }

    INT IsOnlyRead = 0 ; this->GetRWFlag(IsOnlyRead);
    strXml.Append("<ONLYREAD type=\"" );
    strXml.Append("INT\">");
    swpa_sprintf( buffer , "%d", IsOnlyRead );
    strXml.Append( CSWString(buffer) + "</ONLYREAD>");

    // 该节点结束
    strXml.Append("</KEY>\r\n");

    delete []buffer;
    return S_OK;
}




