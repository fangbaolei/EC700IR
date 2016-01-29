#ifndef _HVPARAM_STORE_H
#define _HVPARAM_STORE_H


#include "SWEnumBase.h"
#include "SWParamNode.h"
#include "SWString.h"
#include "tinyxml.h"

#define MAX_SECTION_LEN		32
#define MAX_STRING_LEN		4096
#define MAX_BIN_LEN			8192
#define MAX_COMMENT_LEN		64
#define MAX_CHNAME_LEN		64

//static LPCSTR HV_PARAM_NAME = "HvParam";

//节点枚举辅助类
template <class T>
class CSWEnumNodeProxy : public IEnumNodeCallBack
{
public:
	typedef HRESULT (T::*EnumFunc)(CSWParamNode*);

	//IEnumNodeCallBack
	HRESULT STDMETHODCALLTYPE OnEnumNode(CSWParamNode* pNode)
	{
		if (m_pOwner && m_pEnumFunc) (m_pOwner->*m_pEnumFunc)(pNode);

		return S_OK;
	}

public:
	HRESULT SetEnumFunc(EnumFunc pfn)
	{
		m_pEnumFunc = pfn;
		return S_OK;
	}

	CSWEnumNodeProxy(T* pOwner, EnumFunc pfn = NULL)
		:m_pOwner(pOwner)
		,m_pEnumFunc(pfn)
	{
	}

protected:
	T* m_pOwner;
	EnumFunc m_pEnumFunc;
};

//CParamStore 定义
class CSWParamStore
{
public:
	// IUnknown
	ULONG QueryInterface()
	{
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release(void)
	{
		return 0;
	}

public: //IHvParam
	STDMETHOD(GetInt)(
		LPCSTR szSection,
		LPCSTR szKey,
		INT* pVal,
		INT iDefault
		);

	STDMETHOD(GetFloat)(
		LPCSTR szSection,
		LPCSTR szKey,
		FLOAT* pVal,
		FLOAT fltDefault
		);

	STDMETHOD(GetString)(
		LPCSTR szSection,
		LPCSTR szKey,
		LPSTR szRet,
		INT nLen
		);

	STDMETHOD(GetBin)(
		LPCSTR szSection,
		LPCSTR szKey,
		VOID* pBuf,
		INT* pLen
		);

	STDMETHOD(SetInt)(
		LPCSTR szSection,
		LPCSTR szKey,
		INT iVal
		);

	STDMETHOD(SetFloat)(
		LPCSTR szSection,
		LPCSTR szKey,
		FLOAT fltVal
		);

	STDMETHOD(SetString)(
		LPCSTR szSection,
		LPCSTR szKey,
		LPCSTR szString
		);

	STDMETHOD(SetBin)(
		LPCSTR szSection,
		LPCSTR szKey,
		const VOID* pBuf,
		INT nBufLen
		);

public: //IHvParam2, 在读取时同时写入附加信息
	STDMETHOD(GetInt)(
		LPCSTR szSection,
		LPCSTR szKey,
		INT* pVal,
		const INT nDefault,
		const INT nMin,
		const INT nMax,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE	nRank,
		const INT IsOnlyRead
		);

	STDMETHOD(GetFloat)(
		LPCSTR szSection,
		LPCSTR szKey,
		FLOAT* pVal,
		const FLOAT fltDefault,
		const FLOAT fltMin,
		const FLOAT fltMax,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE	nRank,
		const INT IsOnlyRead
		);

	STDMETHOD(GetString)(
		LPCSTR szSection,
		LPCSTR szKey,
		LPSTR szRet,
		const CHAR* szDefaultVal,
		const INT nLen,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
		);

	STDMETHOD(GetBin)(
		LPCSTR szSection,
		LPCSTR szKey,
		VOID* pBuf,
		const INT* pLen,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
		);

	STDMETHOD(GetEnum)(
		LPCSTR szSection,
		LPCSTR szKey,
        INT* piValue,
        const INT iDefaultVal,
		const CHAR* szValueRange,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
		);

	STDMETHOD(UpdateEnum)(
		LPCSTR szSection,
		LPCSTR szKey,
        const INT iValue
		);

	STDMETHOD(GetSet)(
		LPCSTR szSection,
		LPCSTR szKey,
        INT* piValue,
        const INT iDefaultVal,
		const CHAR* szValueRange,
		const DWORD dwMaxLen,
		LPCSTR szChName,
		LPCSTR szComment,
		const BYTE nRank,
		const INT IsOnlyRead
		);

	STDMETHOD(UpdateSet)(
		LPCSTR szSection,
		LPCSTR szKey,
        const INT iValue
		);


	STDMETHOD(XmlToParam)( TiXmlDocument *pXmlDoc );


    STDMETHOD(ToXml)( CSWString& strParamXml );
public:
	CSWParamStore();
	CSWParamStore(CSWParamStore& src);
	virtual ~CSWParamStore();


	HRESULT Clear();

	HRESULT EnableAutoAdd(BOOL fAutoAdd);
	HRESULT SetClearMode(BOOL fClear);

	DWORD m_dwCookie;

public: //批量提取及合并
	//合并模式
	typedef enum _APPEND_MODE
	{
		MODE_REPLACE_KEY = 0,				//覆写同名(键)节点
		MODE_ADD_NEW = 1,					//只添加新(键)节点
		MODE_REPLACE_SECTION = 2		    //覆写段节点,可用于批量删除
	}
	APPEND_MODE;

	// 提取指定的节点,将包含从根节点到指定节点的完整路径
	HRESULT Extract(LPCSTR szSection, LPCSTR szKey, CSWParamStore* pDestStore);
	//合并参数树,源参数集应包含从根节点到指定节点的完整路径
	HRESULT Inject(LPCSTR szSection, LPCSTR szKey, CSWParamStore* pSrcStore
                , DWORD dwMode = 0);

	//删除参数段
	HRESULT RemoveSection(LPCSTR szSection);
	//删除单个参数
	HRESULT RemoveKey(LPCSTR szSection, LPCSTR szKey);

public: //合并操作
	//拷贝
	CSWParamStore& Assign(const CSWParamStore& cSrcStore);
	//iSrc = NULL时则清空
	CSWParamStore& Assign(const INT& iSrc);
	//追加
	CSWParamStore& AddNew(const CSWParamStore& cSrcStore, BOOL fReplace);

	//拷贝
	CSWParamStore& operator =(const CSWParamStore& cSrcStore);
	//iSrc = NULL时则清空
	CSWParamStore& operator =(const INT& iSrc);
	//追加,跳过同名节点
	CSWParamStore& operator +=(const CSWParamStore& cSrcStore);
	//追加,覆盖同名节点
	CSWParamStore& operator |=(const CSWParamStore& cSrcStore);

public:
    //存储, SimpMode下只保存值,不保存附加信息
    HRESULT SaveToStream( CSWStream* pStream, BOOL fSimpMode = 0 );
    // 从文件流中恢复节点
    HRESULT RestoreFromStream( CSWStream* pStream );
     // 从文件流中恢复节点
    HRESULT RestoreFromXml( TiXmlDocument *pXmlDoc );


	//创建参数节点
	CSWParamNode* CreateNode(LPCSTR szName, PARAM_NODE_TYPE nType);
	//取得段节点
	CSWParamNode* GetSectionNode(LPCSTR szSection, BOOL fCreate = FALSE);
	CSWParamNode* GetSectionNode(CSWParamNode* pParentNode , LPCSTR szSection , BOOL fCreate);
	//通过段节点取得键节点
	CSWParamNode* GetKeyNode( CSWParamNode* pSectionNode, LPCSTR szKey, BOOL fCreate = FALSE);
	//通过段名取得键节点
	CSWParamNode* GetKeyNode( LPCSTR szSection, LPCSTR szKey, BOOL fCreate = FALSE);
	//遍历
	HRESULT EnumParam(IEnumNodeCallBack* pEnumCallBack);

private:

    CSWParamNode* ReadSectionNode( TiXmlElement* pElement , CSWParamNode *pParentNode);
    CSWParamNode* ReadKeyNode( TiXmlElement* pElement , CSWParamNode *pSectionNode );
    HRESULT ConvertXml2Var(TiXmlElement* pElement, CSWParamVar* pVar);
    // 路径分析
    HRESULT XmlPathAnalysys( CSWString strPath );

    HRESULT FindMaxMin(const CHAR* szValueRange , CHAR cSplit
                                  ,INT &nMinValue , INT &nMaxValue );
protected:
	CSWParamNode* m_pRoot;
	BOOL m_fAutoAdd;

	CSWParamStore* m_pClone;	//清理模式使用的副本
};

#endif

