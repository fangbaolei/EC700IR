#ifndef  _HV_STORAGE_H_
#define _HV_STORAGE_H_

#include "atlbase.h"
#include "atlpath.h"
#include "atlconv.h"

#include <string>
#include <stack>
#include <vector>
#include <map>

namespace HvStorage
{
	const DWORD32 HVSTG_CREATE = 0x0100;
	const DWORD32 HVSTG_READONLY = 0x2000;

	const DWORD32 HVSTG_NAME_SIZE = 300;
	const DWORD32 HVSTG_PATH_MAXLEN = 1200; 

	typedef CComQIPtr<IStream> IStreamPtr;
	typedef CComQIPtr<IStorage> IStoragePtr;
	typedef CComQIPtr<IEnumSTATSTG> IEnumSTATSTG_Ptr;

	//节点定义
	class CHvStgNode;

	typedef std::map<std::string, CHvStgNode> CStgMap; 

	class CHvStgNode
	{
	public:
		CHvStgNode() 
		{
			Close();
		};

		CHvStgNode(const CHvStgNode& cSrcNode)
		{
			Close();
			m_spStg = cSrcNode.m_spStg;
			m_cSubStgMap = cSrcNode.m_cSubStgMap;
		}

		~CHvStgNode()
		{
			Close();
		}

		HRESULT Close()
		{
			bool fEmpty = m_cSubStgMap.empty();
			if(!fEmpty) m_cSubStgMap.clear();
			if(m_spStg != NULL) 
			{
				m_spStg.Release();
			}
			return S_OK;
		}

	public:
		IStoragePtr m_spStg;
		CStgMap m_cSubStgMap;
	};

	//CHvStorage定义
	class CHvStorage
	{
	public:
		HRESULT OpenStorage( 
			LPCSTR szPath, 
			IStorage** ppStorage, 
			DWORD32 dwFlag = HVSTG_CREATE 
			);

		HRESULT CloseStorage(
			LPCSTR szPath
			);

		HRESULT OpenStream( 
			LPCSTR szPath, 
			IStream** ppStream, 
			DWORD32 dwFlag = HVSTG_CREATE 
			);
	
	public:
		CHvStorage();
		~CHvStorage();

	public:
		HRESULT Initialize( LPCSTR szFilePath, DWORD32  dwFlag = HVSTG_CREATE );
		HRESULT Close();

	public:
		BOOL IsInited()
		{
			return (m_cRootNode.m_spStg != NULL);
		}

	protected:
		CHvStgNode* FindNode( LPCSTR szPath, bool fCreate );

	protected:
		CHvStgNode m_cRootNode;
		BOOL m_fReadOnly;
	};

	//用于IStream和IHvStream转换
/*	
	class CHvStreamTrans : public HvCore::IHvStream
	{
	public:
		STDMETHOD(QueryInterface)( 
			const IID& iid, 
			void** ppv
			)
		{
			if(iid == HvCore::IID_IHvStream)
			{
				*ppv = static_cast<HvCore::IHvStream*>(this);
				AddRef();
			}
			else
			{
				*ppv = NULL;
			}

			return (*ppv)?S_OK:E_NOINTERFACE;
		}

		virtual ULONG STDMETHODCALLTYPE AddRef(void)
		{
			return 1;
		}

		virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			return 0;
		}

	public:
		STDMETHOD(Read)( 
			PVOID pv, 
			UINT cb, 
			PUINT pcbRead
			)
		{
			if (m_spStream == NULL) return E_OBJ_NO_INIT;

			if(!pv) return E_INVALIDARG;

			return m_spStream->Read(pv, (ULONG)cb, (ULONG*)pcbRead);
		}

		STDMETHOD(Write)( 
			const void* pv, 
			UINT cb, 
			PUINT pcbWritten
			)
		{
			if (m_spStream == NULL) return E_OBJ_NO_INIT;

			if(!pv) return E_INVALIDARG;

			return m_spStream->Write(pv, (ULONG)cb, (ULONG*)pcbWritten);
		}

		STDMETHOD(Seek)( 
			INT iOffset,
			HvCore::STREAM_SEEK ssOrigin,
			PUINT pnNewPosition
			)
		{
			if (m_spStream == NULL) return E_OBJ_NO_INIT;

			LARGE_INTEGER liTmp = {iOffset};
			ULARGE_INTEGER uliTmp = {0};

			HRESULT hr = m_spStream->Seek( liTmp, (STREAM_SEEK)ssOrigin, &uliTmp);

			if(pnNewPosition) *pnNewPosition = (UINT)uliTmp.QuadPart;

			return hr;
		}

		STDMETHOD(Commit)(
			DWORD32 grfCommitFlags = 0
			)
		{
			if (m_spStream == NULL) return E_OBJ_NO_INIT;

			return m_spStream->Commit(grfCommitFlags);
		}

	public:
		CHvStreamTrans() {};

		CHvStreamTrans( IStream* pStream )
		{
			m_spStream.Attach(pStream);
		}

		~CHvStreamTrans()
		{
			Close();
		}

	public:
		//绑定后pStream的引用计数只能由转换类管理
		HRESULT Open( IStream* pStream )
		{
			if( !pStream ) return E_INVALIDARG;

			Close();
			m_spStream.Attach(pStream);

			return S_OK;
		}

		HRESULT Close()
		{	
			if(m_spStream != NULL)
			{
				m_spStream.Release();
			}

			return S_OK;
		}

	protected:
		IStreamPtr m_spStream;
	};
*/
}

#endif