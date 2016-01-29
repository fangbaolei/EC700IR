#include "StdAfx.h"
#include "AxLib.h"

using namespace dbAx;

CAxException::CAxException()
{
	m_pErrors = NULL;
	m_pError = NULL;
	m_nErrorCount = 0;
  m_scode = S_OK;
}

CAxException::~CAxException()
{
}

void CAxException::GetErrorsCollection()
{
  ADOConnection *pCn = NULL;
  _AxConnectionsT::iterator i = m_AxConnections.begin();

  for (; i != m_AxConnections.end(); i++ )
  {
    pCn = (*i)->_GetActiveConnection();
    pCn->get_Errors(&m_pErrors);
    m_pErrors->get_Count(&m_nErrorCount);
    if ( m_nErrorCount > 0 )
      break;
  }
}

void CAxException::GetErrorInfo()
{
	BSTR bstrDesc;

	GetErrorsCollection();

	if ( m_pErrors )
	{
		for ( long i = 0L; i < m_nErrorCount; i++ )
		{
			m_pErrors->get_Item(_variant_t(i), &m_pError);
			m_pError->get_Number(&m_nErrorNo);
			m_pError->get_Description(&bstrDesc);
      m_szErrorDesc += _T("\n");
      m_szErrorDesc += bstrDesc;

      if ( i < m_nErrorCount - 1 )
				m_szErrorDesc +=_T("\n");
		}
	}

  //Is there a COM error involved
  if ( GetAScode() != S_OK )
  {
    _com_error comErr(GetAScode());
    m_szErrorDesc += _T("\nCOM Error: ");
    m_szErrorDesc += comErr.ErrorMessage();
  }

  if ( m_pErrors != NULL )
		m_pErrors->Clear();
}

void dbAx::ThrowAxException(int nAxError, LPCTSTR lpszMsg, HRESULT hr)
{
  CAxException *pAxException = new CAxException;
  pAxException->SetAScode(hr);

	if ( nAxError == AXLIB_ERROR_NONE )
  {
    pAxException->m_szErrorDesc = lpszMsg;
    pAxException->GetErrorInfo();
  }
  else
  {
    pAxException->m_nErrorNo = nAxError;
    switch ( nAxError )
    {
		case AXLIB_ERROR_INIT :
      pAxException->m_szErrorDesc = _T("The dbAx Library failed to initialize");
			break;

		case AXLIB_ERROR_DXBIND :
      pAxException->m_szErrorDesc = _T("The specified field in the database was not found");
			break;

    case AXLIB_ERROR_OBJECT_NOTOPEN :
      pAxException->m_szErrorDesc = _T("The dbAx object is not open");
			break;

    case AXLIB_ERROR_BUF_SIZE :
      pAxException->m_szErrorDesc = _T("The specified buffer size is too small");
      break;

    case AXLIB_ERROR_NULL_PTR :
      pAxException->m_szErrorDesc = _T("Bad or NULL pointer");
      break;

    case AXLIB_ERROR_ENUM :
      pAxException->m_szErrorDesc = _T("Invalid enumeration value");
      break;

    case AXLIB_ERROR_INVALID_POS :
      pAxException->m_szErrorDesc = _T("Invalid page or record position");
      break;

    default :
       pAxException->m_szErrorDesc = _T("Unknown error");
      break;
   }
    
   if ( pAxException->GetAScode() != S_OK )
   {
     _com_error comErr(pAxException->GetAScode());
     pAxException->m_szErrorDesc += _T("\nCOM Error: ");
     pAxException->m_szErrorDesc += comErr.ErrorMessage();
   }

    if ( lpszMsg )
    {
      pAxException->m_szErrorDesc += _T("\nSource: ");
      pAxException->m_szErrorDesc += lpszMsg;
    }
  }
  throw pAxException;
}
