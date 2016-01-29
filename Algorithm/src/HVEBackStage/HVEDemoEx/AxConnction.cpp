#include "StdAfx.h"
#include "AxLib.h"

using namespace dbAx;

///////////////////////////////////////////////////////////////
//	Construction

//Global connection object collection
dbAx::_AxConnectionsT dbAx::m_AxConnections;

void dbAx::Init()
{
	//Initialize COM
  HRESULT hr = CoInitialize(NULL);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_INIT, _T("AxLibInit - COM Initialization."), hr);

	//Clear Connections array
  if ( !m_AxConnections.empty() )
    m_AxConnections.erase(m_AxConnections.begin(), m_AxConnections.end());
}

void dbAx::Term()
{
	//Ensure all connections are closed / cleanup
  _AxConnectionsT::iterator i;

  for ( i = m_AxConnections.begin(); i != m_AxConnections.end(); i++ )
  {
    if ( (*i)->_IsOpen() )
      (*i)->Close();
  }
  
  if ( !m_AxConnections.empty() )
    m_AxConnections.erase(m_AxConnections.begin(), m_AxConnections.end());

  CoUninitialize();
}

void dbAx::DropConnection(dbAx::CAxConnection *pCn)
{
  _AxConnectionsT::iterator i = m_AxConnections.begin();

  for (; i != m_AxConnections.end(); i++ )
  {
    if ( *i == pCn )
    {
      m_AxConnections.erase(i);
      break;
    }
  }
}


//--------------------------------------------------------------------------
// Utility functions

//Converty MultiByte to WideChar
const wchar_t* dbAx::M2W(LPCSTR lpszCharStr)
{
	wchar_t* pszBuf = NULL;
	int nBufSize = MultiByteToWideChar(CP_ACP, 0, lpszCharStr, -1, (LPWSTR)pszBuf, 0);
	pszBuf = new wchar_t[nBufSize];
	int bytes = MultiByteToWideChar(CP_ACP, 0, lpszCharStr, -1, (LPWSTR)pszBuf, nBufSize);

	if ( bytes == 0 )
		return ( NULL );

	return ( pszBuf );
}

//Converty WideChar to MultiByte
const char* dbAx::W2M(LPCWSTR lpszWideCharStr)
{
	size_t nl = wcslen(lpszWideCharStr);
	int nLenOfBuf = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)lpszWideCharStr, -1, NULL, 0, NULL, NULL);
	char *pszBuf = new char[nLenOfBuf];
	int bytes = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)lpszWideCharStr, -1, (LPSTR)pszBuf, nLenOfBuf, NULL, NULL);

	if ( bytes == 0 )
		return ( NULL );

	return ( pszBuf );
}

//Generate a GUID formatted string
#ifdef GEN_GUID_STR
HRESULT dbAx::GenGUIDStr(CString& szGUID)
{
  GUID m_guid = GUID_NULL;
	HRESULT hr = ::UuidCreate(&m_guid);

  if ( SUCCEEDED(hr) )
    szGUID.Format(_T("{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
	  	m_guid.Data1, m_guid.Data2, m_guid.Data3,
		  m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2], m_guid.Data4[3],
		  m_guid.Data4[4], m_guid.Data4[5], m_guid.Data4[6], m_guid.Data4[7]);

  return ( hr );
}
#endif

///////////////////////////////////////////////////////////////
//	Construction

CAxConnection::CAxConnection()
{
  m_piConnection = NULL;
  m_dwEvents = 0;
}

CAxConnection::CAxConnection(LPCTSTR lpszConnectStr)
{
  m_strConnect = lpszConnectStr;
  m_piConnection = NULL;
}

CAxConnection::~CAxConnection()
{
  if ( _IsOpen() )
    Close();

  DropConnection(this);

  if ( m_piConnection )
  {
    HRESULT hr = m_piConnection->Release();
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Close"), hr);
  }

	m_piConnection = NULL;
}

///////////////////////////////////////////////////////////////
//	Initialization
void CAxConnection::Create()
{
	//Create object instance
	try
	{
		HRESULT hr =  CoCreateInstance(CLSID_CADOConnection, NULL, 
			CLSCTX_INPROC_SERVER, IID_IADOConnection, (LPVOID *)&m_piConnection);

		if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_INIT, _T("CAxConnection::Create"), hr);
    
    //Validate connection
		VALID_ADO_OBJECT(m_piConnection);
    m_AxConnections.push_back(this);
	}
	catch ( CAxException *e)
	{
		if ( m_piConnection )
		{
			m_piConnection->Release();
			m_piConnection = NULL;
		}

		//Throw generated exception back to client's catch block
		throw e;
	}
}

///////////////////////////////////////////////////////////////
//	AxLib Specific

void CAxConnection::_ClearConnectionEvents()
{
  if ( m_piConnection == NULL || m_dwEvents == 0 )
    return;

  IConnectionPointContainer *pCPC = NULL;
  IConnectionPoint          *pCP = NULL;
  HRESULT hr = m_piConnection->QueryInterface(__uuidof(IConnectionPointContainer),
    (LPVOID*)&pCPC);

  if ( SUCCEEDED(hr) )
  {
    hr = pCPC->FindConnectionPoint(__uuidof(ConnectionEvents), &pCP);
    pCPC->Release();
    if ( SUCCEEDED(hr) )
    {
      hr = pCP->Unadvise(m_dwEvents);
      pCP->Release();
      m_dwEvents--;
    }
  }

  if FAILED( hr ) 
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::_ClearConnectionEvents"), hr);
}

bool CAxConnection::_IsOpen()
{
  if ( m_piConnection == NULL )
    return (FALSE);

	VALID_ADO_OBJECT(m_piConnection);

  long lState;
  HRESULT hr =  m_piConnection->get_State(&lState);
  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::_IsOpen"), hr);

  return (lState == adStateOpen);
}

void CAxConnection::_SetConnectionEvents(CAxConnectionEvents* pEvents)
{
  IConnectionPointContainer *pCPC = NULL;
  IConnectionPoint          *pCP = NULL;
  HRESULT hr = m_piConnection->QueryInterface(__uuidof(IConnectionPointContainer),
    (LPVOID*)&pCPC);

  if ( SUCCEEDED(hr) )
  {
    hr = pCPC->FindConnectionPoint(__uuidof(ConnectionEvents), &pCP);
    pCPC->Release();
    if ( SUCCEEDED(hr) )
    {
      hr = pCP->Advise(pEvents, &m_dwEvents);
      pCP->Release();
    }
  }

  if FAILED( hr ) 
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::_SetConnectionEvents"), hr);
}

bool CAxConnection::_SupportsTransactions()
{
  ADOProperties *pProps = Properties();
  ADOProperty *pProp = NULL;
  bool bSupports = FALSE;

  if ( pProps )
  {
    if FAILED( pProps->get_Item(_variant_t(_T("Transaction DDL")), &pProp) )
      ThrowAxException(AXLIB_ERROR_NONE);

    _variant_t varValue;
    if FAILED( pProp->get_Value(&varValue) )
      ThrowAxException(AXLIB_ERROR_NONE);

    bSupports = (varValue.lVal == 8);
  }

  return (bSupports);
}

///////////////////////////////////////////////////////////////
//	Collections

ADOErrors* CAxConnection::Errors()
{
	VALID_ADO_OBJECT(m_piConnection);
  ADOErrors* pErrors;

  HRESULT hr = m_piConnection->get_Errors(&pErrors);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Errors"), hr);

  return (pErrors);
}

ADOProperties* CAxConnection::Properties()
{
	VALID_ADO_OBJECT(m_piConnection);
  ADOProperties *pProps;

  HRESULT hr = m_piConnection->get_Properties(&pProps);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Properties"), hr);

  return (pProps);
}

///////////////////////////////////////////////////////////////
//	Methods

void CAxConnection::BeginTrans(long *plTransLevel)
{
	VALID_ADO_OBJECT(m_piConnection);

  HRESULT hr = m_piConnection->BeginTrans(plTransLevel);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::BeginTrans"), hr);
}

void CAxConnection::Cancel()
{
	VALID_ADO_OBJECT(m_piConnection);

  HRESULT hr = m_piConnection->Cancel();
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Cancel"), hr);
}

void CAxConnection::Close()
{
	VALID_ADO_OBJECT(m_piConnection);

  //Release connection events
  _ClearConnectionEvents();

  HRESULT hr = m_piConnection->Close();
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Close"), hr);
}

void CAxConnection::CommitTrans()
{
	VALID_ADO_OBJECT(m_piConnection);

  HRESULT hr = m_piConnection->CommitTrans();
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::CommitTrans"), hr);
}

void CAxConnection::Execute(LPCTSTR lpszCmdText, 
                                  long* lpRecsAffected,
                                  CommandTypeEnum eOption)
{
	VALID_ADO_OBJECT(m_piConnection);
	VARIANT vRecs;
	bstr_t bsCmdText(lpszCmdText);

	_ASSERT (bsCmdText.length() > 0);

	if ( lpRecsAffected != NULL )
		vRecs.vt = VT_I4;
	else
		vRecs.vt = VT_EMPTY;
	
  HRESULT hr = m_piConnection->Execute(bsCmdText, &vRecs, eOption, NULL);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Execute"), hr);

	if ( lpRecsAffected != NULL )
		*lpRecsAffected = vRecs.lVal;
}

void CAxConnection::Open(LPCTSTR lpszConnectStr, LPCTSTR lpszUserID, 
                         LPCTSTR lpszPwd, ConnectOptionEnum eConnect)
{
	VALID_ADO_OBJECT(m_piConnection);

  if (_tcslen(lpszConnectStr) == 0 )
		_ASSERT (m_strConnect.GetLength() > 0);
	else
		m_strConnect = lpszConnectStr;

  HRESULT hr = m_piConnection->Open(bstr_t((LPCTSTR)m_strConnect), 
    _bstr_t(lpszUserID), _bstr_t(lpszPwd), eConnect);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Open"), hr);
}

void CAxConnection::RollbackTrans()
{
	VALID_ADO_OBJECT(m_piConnection);

  HRESULT hr = m_piConnection->RollbackTrans();
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::RollbackTrans"), hr);
}


///////////////////////////////////////////////////////////////
//	Properties

XactAttributeEnum CAxConnection::Attributes(long lAttribute)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( lAttribute > 0 )
  {
    hr = m_piConnection->put_Attributes(lAttribute);
	  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Attributes"), hr);
  }

  long lAttributes;

  hr = m_piConnection->get_Attributes(&lAttributes);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Attributes"), hr);

  return ((XactAttributeEnum)lAttributes);
}

long CAxConnection::CommandTimeout(long lSeconds)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( lSeconds >= 0 )
  {
    hr = m_piConnection->put_CommandTimeout(lSeconds);
	  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::CommandTimeout"), hr);
  }

  long lTimeout = 0;
  hr = m_piConnection->get_CommandTimeout(&lTimeout);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::CommandTimeout"), hr);

	return (lTimeout);
}

LPCTSTR CAxConnection::ConnectionString(LPCTSTR lpszConnectStr)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( lpszConnectStr )
  {
    hr = m_piConnection->put_ConnectionString(bstr_t(lpszConnectStr));
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::ConnectionString"), hr);
  }

  BSTR bstrConnect;
  hr = m_piConnection->get_ConnectionString(&bstrConnect);
	if FAILED( hr )
		 ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::ConnectionString"), hr);

  return (CString(bstrConnect).operator LPCTSTR());
}

long CAxConnection::ConnectionTimeout(long lSeconds)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( lSeconds >= 0 )
  {
    hr = m_piConnection->put_ConnectionTimeout(lSeconds);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::ConnectionTimeout"), hr);
  }

  long lTimeout = 0;
  hr = m_piConnection->get_ConnectionTimeout(&lTimeout);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::ConnectionTimeout"), hr);

	return (lTimeout);
}

CursorLocationEnum CAxConnection::CursorLocation(long lCursorLocation)
{
	VALID_ADO_OBJECT(m_piConnection);

  if ( (lCursorLocation < adUseNone || lCursorLocation > adUseClientBatch) && 
    lCursorLocation != -1 )
    ThrowAxException(AXLIB_ERROR_ENUM, _T("CAxConnection::CursorLocation"));

  HRESULT hr = S_OK;
  if ( lCursorLocation != -1)
  {
    hr = m_piConnection->put_CursorLocation((CursorLocationEnum)lCursorLocation);
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::CursorLocation"), hr);
  }

  CursorLocationEnum curLoc;

  hr = m_piConnection->get_CursorLocation(&curLoc);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::CursorLocation"), hr);

	return (curLoc);
}

LPCTSTR CAxConnection::DefaultDatabase(LPCTSTR lpszDatabase)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( lpszDatabase )
  {
    hr = m_piConnection->put_DefaultDatabase(bstr_t(lpszDatabase));
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::DefaultDatabase"), hr);
  }

	BSTR bstrDatabase;
  hr = m_piConnection->get_DefaultDatabase(&bstrDatabase);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::DefaultDatabase"), hr);

#ifdef _UNICODE
  return (_bstr_t(bstrDatabase).operator const wchar_t *());
#else
  return (_bstr_t(bstrDatabase).operator const char *());
#endif
}

IsolationLevelEnum CAxConnection::IsolationLevel(IsolationLevelEnum eIsoLevel)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( eIsoLevel != adXactUnspecified )
  {
    hr = m_piConnection->put_IsolationLevel(eIsoLevel);
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::LsolationLevel"), hr);
  }

	IsolationLevelEnum isoLevel;

  hr = m_piConnection->get_IsolationLevel(&isoLevel);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::LsolationLevel"), hr);

	return (isoLevel);
}

ConnectModeEnum CAxConnection::Mode(long lConMode)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;

  if ( lConMode >= 0 )
  {
    hr = m_piConnection->put_Mode((ConnectModeEnum)lConMode);
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Mode"), hr);
  }

	ConnectModeEnum eConMode;
  hr = m_piConnection->get_Mode(&eConMode);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Mode"), hr);

	return (eConMode);
}

LPCTSTR CAxConnection::Provider(LPCTSTR lpszProvider)
{
	VALID_ADO_OBJECT(m_piConnection);
  HRESULT hr = S_OK;
  
  if ( lpszProvider )
  {
    hr = m_piConnection->put_Provider(bstr_t(lpszProvider));
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Provider"), hr);
  }

	BSTR bstrProvider;
  hr = m_piConnection->get_Provider(&bstrProvider);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxConnection::Provider"), hr);

	return (CString(bstrProvider).operator LPCTSTR());
}

ObjectStateEnum CAxConnection::State()
{
	VALID_ADO_OBJECT(m_piConnection);
	long lState;

	if FAILED( m_piConnection->get_State(&lState) )
		ThrowAxException(AXLIB_ERROR_NONE);

	return ((ObjectStateEnum)lState);
}

LPCTSTR CAxConnection::Version()
{
	VALID_ADO_OBJECT(m_piConnection);
	BSTR bstrVersion;

	if FAILED( m_piConnection->get_Version(&bstrVersion) )
		ThrowAxException(AXLIB_ERROR_NONE);

	return (CString(bstrVersion).operator LPCTSTR());
}
