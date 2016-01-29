#ifndef _HVPARAM_NODE_H
#define _HVPARAM_NODE_H

#include "HvEnum.h"

#include "HvUtils.h"

#include "HvInterface.h"

#include "assert.h"

typedef struct _PARAM_VAR
{
	//0x00~0xff
	enum  _VAR_TYPE
	{
		VT_NULL = 0,
		VT_INT = 10,
		VT_UINT = 11,
		VT_FLOAT = 20,
		VT_DOUBLE = 21,
		VT_BIN = 100,
		VT_STR = 101
	}
	vt;

	union
	{
		int Int;
		UINT uInt;
		float Flt;
		double Dbl;
		struct
		{
			void* pBuf;
			WORD16 nLen;
		} Bin;
	}
	val;

	_PARAM_VAR()
	{
		vt = VT_NULL;
	}
}
PARAM_VAR;

class CParamVar : public PARAM_VAR
{
public:
	CParamVar(){}

	~CParamVar()
	{
		Clear();
	}

	CParamVar(const int& src)
	{
		vt = VT_INT;
		val.Int = src;
	}
	CParamVar(const UINT& src)
	{
		vt = VT_UINT;
		val.uInt = src;
	}
	CParamVar(const float& src)
	{
		vt = VT_FLOAT;
		val.Flt = src;
	}
	CParamVar(const double& src)
	{
		vt = VT_DOUBLE;
		val.Dbl = src;
	}
	CParamVar(const char* szSrc)
	{
		SetString(szSrc);
	}
	CParamVar(const void* pBuf, const WORD16& nLen)
	{
		SetBin(pBuf, nLen);
	}

public:
	CParamVar& operator=(const int& src)
	{
		if (vt == VT_INT) 
		{
			val.Int = src;
		}
		else
		{
			Clear();
			vt = VT_INT;
			val.Int = src;
		}
		return *this;
	}
	CParamVar& operator=(const UINT& src)
	{
		if (vt == VT_UINT)
		{
			val.uInt = src;
		}
		else
		{
			Clear();
			vt = VT_UINT;
			val.uInt = src;
		}
		return *this;
	}
	CParamVar& operator=(const float& src)
	{
		if (vt == VT_FLOAT)
		{
			val.Flt = src;
		}
		else
		{
			Clear();
			vt = VT_FLOAT;
			val.Flt = src;
		}
		return *this;
	}
	CParamVar& operator=(const double& src)
	{
		if (vt == VT_DOUBLE)
		{
			val.Dbl =(double)src;
		}
		else
		{
			Clear();
			vt = VT_DOUBLE;
			val.Dbl = src;
		}
		return *this;
	}
	CParamVar& operator=(const char* szSrc)
	{
		SetString(szSrc);
		return *this;
	}
	CParamVar& operator=(const CParamVar& cSrcVar)
	{
		Clear();
		if ( cSrcVar.vt == VT_INT)
		{
			vt = VT_INT;
			val.Int = cSrcVar.val.Int;
		}
		else if ( cSrcVar.vt == VT_UINT)
		{
			vt = VT_UINT;
			val.uInt = cSrcVar.val.uInt;
		}
		else if ( cSrcVar.vt == VT_FLOAT)
		{
			vt = VT_FLOAT;
			val.Flt = cSrcVar.val.Flt;
		}
		else if ( cSrcVar.vt == VT_DOUBLE)
		{
			vt = VT_DOUBLE;
			val.Dbl = cSrcVar.val.Dbl;
		}
		else if ( cSrcVar.vt == VT_BIN)
		{
			SetBin(cSrcVar.val.Bin.pBuf, cSrcVar.val.Bin.nLen);
		}
		else if (cSrcVar.vt == VT_STR)
		{
			SetString((LPCSTR)cSrcVar);
		}
		
		return *this;
	}

public:
	operator int() const
	{
		if (vt == VT_INT)
		{
			return val.Int;
		}
		if (vt == VT_UINT)
		{
			return (int)val.uInt;
		}
		else if (vt == VT_FLOAT)
		{
			return (int)val.Flt;
		}
		else if (vt == VT_DOUBLE)
		{
			return (int)val.Dbl;
		}
		else
		{
			return 0;
		}
	}
	operator UINT() const
	{
		if(vt == VT_INT)
		{
			return (UINT)val.Int;
		}
		if (vt == VT_UINT)
		{
			return val.uInt;
		}
		else if(vt == VT_FLOAT)
		{
			return (UINT)val.Flt;
		}
		else if(vt == VT_DOUBLE)
		{
			return (UINT)val.Dbl;
		}
		else
		{
			return 0;
		}
	}
	operator float() const
	{
		if (vt == VT_INT)
		{
			return (float)val.Int;
		}
		else if (vt == VT_UINT)
		{
			return (float)val.uInt;
		}
		else if (vt == VT_FLOAT)
		{
			return val.Flt;
		}
		else if(vt == VT_DOUBLE)
		{
			return (float)val.Dbl;
		}
		else
		{
			return 0;
		}
	}
	operator double() const
	{
		if (vt == VT_INT)
		{
			return (double)val.Flt;
		}
		else if (vt == VT_UINT)
		{
			return (double)val.uInt;
		}
		else if (vt == VT_FLOAT)
		{
			return (double)val.Flt;
		}
		else if(vt == VT_DOUBLE)
		{
			return val.Dbl;
		}
		else
		{
			return 0;
		}
	}
	operator LPCSTR() const
	{
		if (vt == VT_STR)
		{
			return (LPCSTR)val.Bin.pBuf;
		}
		else
		{
			return '\0';
		}
	}

	BOOL operator==(const CParamVar& cDest)
	{
		if (vt != cDest.vt) return FALSE;
		if (vt == VT_INT)
		{
			return (val.Int == cDest.val.Int);
		}
		else if (vt == VT_UINT)
		{
			return (val.uInt == cDest.val.uInt);
		}
		else if (vt == VT_FLOAT || vt == VT_DOUBLE)
		{
			return FALSE;	//浮点数不比较
		}
		else if (vt == VT_STR)
		{
			return IsEqualString((LPCSTR)(*this), (LPCSTR)cDest);
		}
		else if (vt == VT_BIN)
		{
			if (val.Bin.nLen != cDest.val.Bin.nLen) return FALSE;
			return HV_memcmp(val.Bin.pBuf, cDest.val.Bin.pBuf, val.Bin.nLen) == 0;
		}
		else
		{
			return FALSE;
		}
	}

	BOOL operator!=(const CParamVar& cDest)
	{
		return !(*this == cDest);
	}

	BOOL operator==(const char* pszDest)
	{
		if ( vt != VT_STR) return FALSE;
		return IsEqualString((const char*)val.Bin.pBuf, pszDest);
	}

	BOOL operator==(const int& iDest)
	{
		if ( vt != VT_INT) return FALSE;
		return ( val.Int == iDest );
	}

	BOOL operator==(const UINT& nDest)
	{
		if ( vt != VT_UINT) return FALSE;
		return ( val.uInt == nDest );
	}

public:
	BOOL IsNull()
	{
		return (vt == VT_NULL);
	}

	HRESULT SetString(const char* szSrc)
	{
		if( szSrc == NULL ) return E_INVALIDARG;
		Clear();	//重置为VT_NULL
		UINT nLen = (UINT)strlen(szSrc) + 1;
		HRESULT hr = AllocBuf(nLen);
		if (hr == S_OK)
		{
			HV_memcpy((char*)val.Bin.pBuf, szSrc, nLen);
			*((char*)val.Bin.pBuf + nLen - 1) ='\0';
			vt = VT_STR; //只有分配成功才会改变TYPE
		}
		return hr;
	}

	HRESULT SetBin(const void* pBuf, WORD16 nLen)
	{
		if( pBuf == NULL ) return E_INVALIDARG;
		Clear(); //重置为VT_NULL
		HRESULT hr = AllocBuf(nLen);
		if (hr == S_OK)
		{
			HV_memcpy(val.Bin.pBuf, pBuf, nLen);
			vt = VT_BIN; //只有分配成功才会改变TYPE
		}
		return hr;
	}

	HRESULT GetBin(void** ppBuf, WORD16* pLen)
	{
		if (!ppBuf) return E_POINTER;
		if (vt == VT_BIN)
		{
			*ppBuf = val.Bin.pBuf;
			if (pLen) *pLen = val.Bin.nLen;
		}
		else
		{
			*ppBuf = NULL;
			if (pLen) *pLen = 0;
		}
		return (*ppBuf)?S_OK:E_FAIL;
	}

	DWORD32 GetValLen()
	{
		DWORD32 dwLen = 0;

		switch(vt)
		{
		case VT_NULL:
			dwLen = 0;
			break;
		case VT_INT:
			dwLen = sizeof(int);
			break;
		case VT_UINT:
			dwLen = sizeof(UINT);
			break;
		case VT_FLOAT:
			dwLen = sizeof(float);
			break;
		case VT_DOUBLE:
			dwLen = sizeof(double);
			break;
		case VT_STR:
			dwLen = val.Bin.nLen - 1; //不包括结尾'\0'
			break;
		case	VT_BIN:
			dwLen = val.Bin.nLen;
			break;
		default:
			dwLen = 0;
			break;
		}
		return dwLen;
	}

	//S_OK : 写入成功
	//其他: 写入失败
	HRESULT WriteToStream(HvCore::IHvStream* pStream)
	{
		if(vt == VT_NULL) return E_FAIL; //不允许写入无类型值

		HRESULT hr;

		//写入相关信息
		BYTE8 nType = (BYTE8)vt;
		WORD16 nValLen = GetValLen();

		if ( S_OK != pStream->Write(&nType, sizeof(BYTE8), NULL) )
		{
			return E_FAIL;
		}

		//写入数据
		if (vt == VT_INT)
		{
			hr = pStream->Write(&val.Int, nValLen, NULL);
		}
		else if (vt == VT_UINT)
		{
			hr = pStream->Write(&val.uInt, nValLen, NULL);
		}
		else if (vt == VT_FLOAT)
		{
			hr = pStream->Write(&val.Flt, nValLen, NULL);
		}
		else if(vt == VT_DOUBLE)
		{
			hr = pStream->Write(&val.Dbl, nValLen, NULL);
		}
		else if (vt == VT_STR || vt == VT_BIN) //写入字串时nValLen = nBufLen -1
		{
			if ( S_OK == pStream->Write(&nValLen, sizeof(WORD16), NULL)  &&
			     S_OK == pStream->Write( val.Bin.pBuf, nValLen, NULL) )
			{
				hr = S_OK;
			}
			else
			{
				hr = E_FAIL;
			}
		}
		else
		{
			hr = E_FAIL; //未执行写入操作
		}

		return hr;
	}

	// S_OK : 读取成功
	// 其他 : 读取失败(值无效)
	HRESULT ReadFromStream(HvCore::IHvStream* pStream)
	{
		HRESULT hr(S_OK);

		Clear();

		BYTE8 nType;

		if (S_OK != pStream->Read( &nType, sizeof(BYTE8), NULL) )
		{
			return E_FAIL;
		}

		if (nType == VT_INT)
		{
			hr = pStream->Read(&val.Int, sizeof(int), NULL);
			vt = VT_INT;
		}
		else if (nType == VT_UINT)
		{
			hr = pStream->Read(&val.uInt, sizeof(UINT), NULL);
			vt = VT_UINT;
		}
		else if (nType == VT_FLOAT)
		{
			hr = pStream->Read(&val.Flt, sizeof(float), NULL);
			vt = VT_FLOAT;
		}
		else if (nType == VT_DOUBLE)
		{
			hr = pStream->Read(&val.Dbl, sizeof(double), NULL);
			vt = VT_DOUBLE;
		}
		else if (nType == VT_STR)
		{
			//读字符串长度
			WORD16 nStrLen;
			hr = pStream->Read(&nStrLen, sizeof(WORD16), NULL);

			if ( hr == S_OK)
			{
				//分配缓存
				CFastMemAlloc cFastMem;
				BYTE8* pBuf = (BYTE8*)cFastMem.StackAlloc(nStrLen + 1, FALSE);
				if (NULL == pBuf)
				{
					hr = E_OUTOFMEMORY;
				}
				else
				{
					HV_memset(pBuf, 0, nStrLen + 1);

					//读入数据
					if( nStrLen == 0)
					{
						pBuf[0] = '\0';
						hr = S_OK;
					}
					else
					{
						hr = pStream->Read(pBuf, nStrLen, NULL);
					}
				}
				//assert(SUCCEEDED(hr));

				if (hr == S_OK)
				{
					//设置字符串
					hr = SetString((LPCSTR)pBuf);
				}
			}
		}
		else if (nType == VT_BIN)
		{
			//读BIN数据长度
			WORD16 nBufLen;
			hr = pStream->Read(&nBufLen, sizeof(WORD16), NULL);

			if ( hr == S_OK)
			{
				//分配缓存
				CFastMemAlloc cFastMem;
				BYTE8* pBuf = (BYTE8*)cFastMem.StackAlloc(nBufLen, FALSE);
				if (NULL == pBuf)
				{
					hr = E_OUTOFMEMORY;
				}
				else
				{
					HV_memset(pBuf, 0, nBufLen);

					//读入数据
					hr = pStream->Read(pBuf, nBufLen, NULL);
				}
				
				if (hr == S_OK)
				{
					//设置二进制数据
					hr = SetBin(pBuf, nBufLen);
				}
			}
		}
		else
		{
			hr = E_FAIL; //无效类型不允许读入
		}

		if ( hr != S_OK)
		{
			Clear();
		}

		return hr;
	}

public:
	void Clear()
	{
		if ( (vt == VT_BIN || vt == VT_STR) && 
			val.Bin.pBuf != NULL )
		{
			HV_FreeMem(val.Bin.pBuf, val.Bin.nLen);
			val.Bin.pBuf = NULL;
			val.Bin.nLen = 0;
		}
		vt = VT_NULL;
	}

protected:
	HRESULT AllocBuf(WORD16 nLen)
	{
		val.Bin.pBuf = HV_AllocMem(nLen);
		if (val.Bin.pBuf != NULL) 
		{
			val.Bin.nLen = nLen;
			HV_memset(val.Bin.pBuf, 0, nLen);
		}
		return (val.Bin.pBuf == NULL)?E_OUTOFMEMORY:S_OK;
	}

	BOOL IsEqualString(const char* str1, const char* str2)
	{
		BOOL fSame = TRUE;
		const char* p1 = str1;
		const char* p2 = str2;

		while( *p1 !='\0' && *p2 != '\0' )
		{
			if ( *p1 != *p2)
			{
				if ( ( (*p1 > 'a' && *p1 < 'z') && (*p1 - *p2) == 32)  ||
					( (*p1 > 'A' && *p1 < 'Z') && (*p2 - *p1) == 32)  )
				{
					//如果只是大小写差异不做处理
				}
				else
				{
					fSame = FALSE;
					break;
				}
			}
			p1++;
			p2++;
		}

		if ( (*p1 == '\0') || (*p2 == '\0') )
		{
			fSame = (*p1 == *p2);
		}

		return fSame;
	}
};

//节点类型
//低4位
typedef enum _PARAM_NODE_TYPE
{
	NT_NULL = 0x0000,
	NT_SECTION = 0x0001,
	NT_KEY = 0x0002
}
PARAM_NODE_TYPE;

//节点标志位
//低5~8位
typedef enum _PARAM_NODE_FLAG
{
	NF_NORMAL= 0x0000,
	NF_INFOMASK = 0x0010		//有信息标志位
}
PARAM_NODE_FLAG;

//参数信息,后12位表示,前4位保留
typedef enum _PARAM_NODE_INFO
{
	NI_NONE = 0x0000,
	NI_DEFAULT_VAL = 0x0001,	//默认值
	NI_SCALE = 0x0002,				//范围
	NI_COMMENT = 0x0004,			//注释
	NI_CHNAME = 0x008,			    //中文名
	NI_RANK	= 0x0010,					//级别

	NI_ALL = 0x0FFF					//全部有效
}
_PARAM_NODE_INFO;

class CParamNodeEnum;
interface IEnumNodeCallBack;

//CParamNode 定义
class CParamNode
{
public:
	CParamNode(WORD16 nFlag = NT_NULL);
	CParamNode(CParamNode& src);
	~CParamNode();

public: //标志位判断
	BOOL IsSectionNode()
	{
		return (m_nFlag & NT_SECTION);
	}

	BOOL IsKeyNode()
	{
		return (m_nFlag & NT_KEY);
	}

	BOOL IsNullNode()
	{
		return (m_nFlag & 0x0003) == NT_NULL;
	}

	BOOL HasExtInfo()
	{
		return (m_nFlag & NF_INFOMASK);
	}

public: //值操作
	template<typename T>
	HRESULT SetValue(const T& value)
	{
		m_Val = value;
		return S_OK;
	}

	HRESULT SetValue(const char* szValue)
	{
		return m_Val.SetString(szValue);
	}

	HRESULT SetValue(void* pBuf, WORD16 nLen)
	{
		return m_Val.SetBin(pBuf, nLen);
	}

	template<typename T>
	HRESULT SetDefaultValue(const T& value)
	{
		m_DefaultVal = value;
		m_nInfoMask |= NI_DEFAULT_VAL;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

	HRESULT SetDefaultValue(const char* szDefault)
	{
		m_DefaultVal.SetString(szDefault);
		m_nInfoMask |= NI_DEFAULT_VAL;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

	HRESULT SetDefaultValue(void* pBuf, WORD16 nLen)
	{
		m_DefaultVal.SetBin(pBuf, nLen);
		m_nInfoMask |= NI_DEFAULT_VAL;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

	template<typename T>
	HRESULT SetScale(const T& Min, const T& Max)
	{
		m_MinVal = Min;
		m_MaxVal = Max;
		m_nInfoMask |= NI_SCALE;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

	HRESULT SetComment( const char* szComment )
	{
		m_strComment = szComment;
		m_nInfoMask |= NI_COMMENT;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

	HRESULT SetChName( const char* szChName)
	{
		m_strChName = szChName;
		m_nInfoMask |= NI_CHNAME;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

	HRESULT SetRank( BYTE8 nRank )
	{
		m_nRank = nRank;
		m_nInfoMask |= NI_RANK;
		m_nFlag |= NF_INFOMASK;
		return S_OK;
	}

public:	//存储, SimpMode下只保存值,不保存附加信息
	HRESULT SaveToStream(HvCore::IHvStream* pStream, BOOL fSimpMode);

	HRESULT RestoreFromStream(HvCore::IHvStream* pStream);

public:	//子节点操作
	//添加
	HRESULT AddSubNode(CParamNode* pNode);	
	//删除
	HRESULT RemoveSubNode( const char* szNodeName, PARAM_NODE_TYPE nType );
	//取得指定名称和类型的子节点
	CParamNode* GetSubNode( const char* szNodeName, PARAM_NODE_TYPE nType );
	//取得下一个子节点
	CParamNode* GetNextSubNode( CParamNode* pNode );
	//遍历
	HRESULT OnEnum(IEnumNodeCallBack* pCallBack);

public:	//合并操作
	CParamNode& Assign( const CParamNode& cSrcNode);	//覆盖
	CParamNode& AddNew( const CParamNode& cSrcNode, BOOL fReplace); //合并

	CParamNode& operator =(const CParamNode& cSrcNode) //覆盖
	{
		return Assign(cSrcNode);
	}

public:
	HRESULT ReInit(); //重初始化

protected:
	HRESULT Clear();	//清理,可用于类型转换时

	HRESULT SaveAsKeyNode(HvCore::IHvStream* pStream, BOOL fSimpMode);
	HRESULT SaveAsSectionNode(HvCore::IHvStream* pStream, BOOL fSimpMode);

	HRESULT ReadAsKeyNode(HvCore::IHvStream* pStream, WORD16 nFlag);
	HRESULT ReadAsSectionNode(HvCore::IHvStream* pStream, WORD16 nFlag);

public:	//参数信息
	WORD16 m_nFlag;
	WORD16 m_nInfoMask;

	CParamVar m_strName;
	CParamVar m_Val;
	CParamVar m_DefaultVal;
	CParamVar m_MinVal;
	CParamVar m_MaxVal;
	CParamVar m_strComment;
	CParamVar m_strChName;
	BYTE8 m_nRank;

	//CParamNode* m_pParent;

protected:	 //子节点
	CParamNodeEnum* m_pSubNode;
};

//CParamNodeEnum 定义
class CParamNodeEnum : public CHvEnumBase<CParamNode*>
{
public:
	~CParamNodeEnum() 
	{
		EnumBase_Reset();
		while(S_OK == EnumBase_Remove())
		{
			SystemDog();
		}
	}
protected:
	virtual HRESULT OnDelNode(NODE* pNode)
	{
		SAFE_DELETE(pNode->Dat);
		return S_OK;
	}
};

interface IEnumNodeCallBack
{
	STDMETHOD(OnEnumNode)(CParamNode* pNode) = 0;
};

#endif
