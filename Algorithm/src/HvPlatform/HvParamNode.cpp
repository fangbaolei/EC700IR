#include "HvParamNode.h"
#include "assert.h"

#include "hvutils.h"

#define RTN_IF_FAILED(fun) { HRESULT hr = fun; if(FAILED(hr)) return hr; }

//CParamNode实现
CParamNode::CParamNode(WORD16 nFlag)
    :m_nFlag(nFlag)
    ,m_nInfoMask(NI_NONE)
    ,m_pSubNode(NULL)
{
	if ( nFlag & NT_SECTION )
	{
		m_pSubNode = new CParamNodeEnum;
		assert(m_pSubNode);
	}
}

CParamNode::CParamNode(CParamNode& src)
{
	Assign(src);
}

CParamNode::~CParamNode()
{
	Clear();
}

HRESULT CParamNode::Clear()
{
	SAFE_DELETE(m_pSubNode);
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

HRESULT CParamNode::ReInit() //重初始化
{
	if(m_nFlag & NT_SECTION)
	{
		SAFE_DELETE(m_pSubNode);
		m_pSubNode = new CParamNodeEnum;
		assert(m_pSubNode);
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

//存储操作
//SimpMode: flag name value
//AdvMode: flag infomask name value [defaultvalue] [scale] [comment] [chname] [rank]
HRESULT CParamNode::SaveAsKeyNode(HvCore::IHvStream* pStream, BOOL fSimpMode)
{
	if( !pStream ) return E_INVALIDARG;

	WORD16 nFlag = fSimpMode?(m_nFlag&~NF_INFOMASK):m_nFlag;

	RTN_IF_FAILED(pStream->Write(&nFlag, sizeof(nFlag), NULL));

	BOOL fWriteInfoMask = !fSimpMode && (nFlag & NF_INFOMASK);

	if( fWriteInfoMask )
	{
		RTN_IF_FAILED(pStream->Write(&m_nInfoMask, sizeof(m_nInfoMask), NULL));
	}

	//名字和值必备
	RTN_IF_FAILED(m_strName.WriteToStream(pStream));
	RTN_IF_FAILED(m_Val.WriteToStream(pStream));

	if( !fWriteInfoMask ) return S_OK;

	if( m_nInfoMask & NI_DEFAULT_VAL)
	{
		RTN_IF_FAILED(m_DefaultVal.WriteToStream(pStream));
	}

	if(	 m_nInfoMask & NI_SCALE)
	{
		RTN_IF_FAILED(m_MinVal.WriteToStream(pStream));
		RTN_IF_FAILED(m_MaxVal.WriteToStream(pStream));
	}

	if(m_nInfoMask & NI_COMMENT)
	{
		RTN_IF_FAILED(m_strComment.WriteToStream(pStream));
	}

	if(m_nInfoMask & NI_CHNAME)
	{
		RTN_IF_FAILED(m_strChName.WriteToStream(pStream));
	}

	if(m_nInfoMask & NI_RANK)
	{
		RTN_IF_FAILED(m_Rank.WriteToStream(pStream));
	}

	return S_OK;
}

HRESULT CParamNode::SaveAsSectionNode(HvCore::IHvStream* pStream, BOOL fSimpMode)
{
	if ( !pStream) return E_INVALIDARG;

	RTN_IF_FAILED(pStream->Write(&m_nFlag, sizeof(m_nFlag), NULL));

	RTN_IF_FAILED(m_strName.WriteToStream(pStream));

	HRESULT hr(S_OK);
	CParamNode* pNode;
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
		CParamNode cNode(NT_NULL);
		hr = cNode.SaveToStream(pStream, fSimpMode);	//写入一个空节点做为结束标志
	}

	return hr;
}

HRESULT CParamNode::SaveToStream(HvCore::IHvStream* pStream, BOOL fSimpMode)
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

HRESULT CParamNode::ReadAsKeyNode(HvCore::IHvStream* pStream, WORD16 nFlag)
{
	if(!pStream) return E_INVALIDARG;

	m_nFlag = nFlag;

	if(nFlag & NF_INFOMASK)
	{
		RTN_IF_FAILED(pStream->Read(&m_nInfoMask,sizeof(m_nInfoMask),NULL));
	}

	RTN_IF_FAILED(m_strName.ReadFromStream(pStream));
	RTN_IF_FAILED(m_Val.ReadFromStream(pStream));

	if(m_nInfoMask & NI_DEFAULT_VAL)
	{
		RTN_IF_FAILED(m_DefaultVal.ReadFromStream(pStream));
	}

	if(m_nInfoMask & NI_SCALE)
	{
		RTN_IF_FAILED(m_MinVal.ReadFromStream(pStream));
		RTN_IF_FAILED(m_MaxVal.ReadFromStream(pStream));
	}

	if(m_nInfoMask & NI_COMMENT)
	{
		RTN_IF_FAILED(m_strComment.ReadFromStream(pStream));
	}

	if(m_nInfoMask & NI_CHNAME)
	{
		RTN_IF_FAILED(m_strChName.ReadFromStream(pStream));
	}

	if (m_nInfoMask & NI_RANK)
	{
		RTN_IF_FAILED(m_Rank.ReadFromStream(pStream));
	}

	return S_OK;
}

HRESULT CParamNode::ReadAsSectionNode(HvCore::IHvStream* pStream, WORD16 nFlag)
{
	if(!pStream) return E_INVALIDARG;

	m_nFlag = nFlag;

	SAFE_DELETE(m_pSubNode);
	m_pSubNode = new CParamNodeEnum;
	if (m_pSubNode == NULL) return E_OUTOFMEMORY;

	RTN_IF_FAILED(m_strName.ReadFromStream(pStream));

	HRESULT hr(E_FAIL);
	while(1)
	{
		CParamNode* pNode = new CParamNode;
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

//S_OK : 读取成功
//E_FAIL : 读取失败
//E_OUTOFMEMORY : 分配内存失败
HRESULT CParamNode::RestoreFromStream(HvCore::IHvStream *pStream)
{
	if ( !pStream ) return E_INVALIDARG;

	Clear();

	WORD16 nFlag;
	RTN_IF_FAILED(pStream->Read(&nFlag, sizeof(nFlag), NULL));

	HRESULT hr = E_FAIL;

	if( (nFlag & 0x03) == NT_NULL) //空结点只做为结束标志,实际参数树中不会有空节点
	{
		hr = S_FALSE;
	}
	else if(nFlag & NT_KEY)
	{
		hr = ReadAsKeyNode(pStream, nFlag);
	}
	else if(nFlag & NT_SECTION)
	{
		hr = ReadAsSectionNode(pStream, nFlag);
	}

	return hr;
}

//子结点操作
HRESULT CParamNode::AddSubNode(CParamNode* pNode)
{
	if (!pNode) return E_POINTER;

	if ( !IsSectionNode() ) return S_FALSE;

	//pNode->m_pParent = this;

	return m_pSubNode->EnumBase_Add(pNode);
}

HRESULT CParamNode::RemoveSubNode( const char* szNodeName, PARAM_NODE_TYPE nType )
{
	if (!szNodeName) return E_POINTER;

	if( !IsSectionNode() ) return S_FALSE;

	CParamNode* pNode = GetSubNode( szNodeName, nType );

	return m_pSubNode->EnumBase_Remove(pNode);
}

CParamNode* CParamNode::GetSubNode( const char* szNodeName, PARAM_NODE_TYPE nType )
{
	if ( !szNodeName ||
		  !IsSectionNode() )
	{
		return NULL;
	}

	CParamNode* pNode;
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

CParamNode* CParamNode::GetNextSubNode( CParamNode* pNode )
{
	if( !IsSectionNode() ) return NULL;

	if(pNode == NULL)
	{
		m_pSubNode->EnumBase_Reset();
	}

	CParamNode* pTmp;
	HRESULT  hr = m_pSubNode->EnumBase_Next(1, &pTmp, NULL);

	return (hr == S_OK)?pTmp:NULL;
}

//遍历操作
HRESULT CParamNode::OnEnum(IEnumNodeCallBack* pCallBack)
{
	if (pCallBack == NULL) return E_POINTER;

	pCallBack->OnEnumNode(this);

	if (!IsSectionNode()) return S_OK;

	CParamNode* pNode = NULL;
	while( pNode = GetNextSubNode(pNode) )
	{
		if(pNode->IsNullNode()) continue;
		pNode->OnEnum(pCallBack);
	}

	//返回结束标志
	CParamNode cNullNode(NT_NULL);
	pCallBack->OnEnumNode(&cNullNode);

	return S_OK;
}

//覆盖
CParamNode& CParamNode::Assign( const CParamNode& cSrcNode)
{
	Clear(); //如果源节点为空,则目标节点也会被置空

	CParamNode* pSrcNode = const_cast<CParamNode*>(&cSrcNode);

	m_strName = pSrcNode->m_strName;

	if(pSrcNode->IsSectionNode() &&
		pSrcNode->m_pSubNode )
	{
		m_nFlag |= NT_SECTION;
		m_pSubNode = new CParamNodeEnum;
		assert(m_pSubNode);

		CParamNode* pNode = NULL;
		CParamNode* pNewNode = NULL;
		while( pNode = pSrcNode->GetNextSubNode(pNode) )
		{
			if(pNode->IsNullNode()) continue;
			pNewNode = new CParamNode;
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
CParamNode& CParamNode::AddNew( const CParamNode& cSrcNode, BOOL fReplace)
{
	CParamNode* pSrcNode = const_cast<CParamNode*>(&cSrcNode);

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
				WORD16 dwFlag = pSrcNode->m_nInfoMask;

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

	CParamNode* pSrcSubNode = NULL;
	CParamNode* pSubNode;
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
			pSubNode = new CParamNode;
			if(pSubNode)
			{
				*pSubNode = *pSrcSubNode;
				AddSubNode(pSubNode);
			}
		}
	}
	return *this;
}
