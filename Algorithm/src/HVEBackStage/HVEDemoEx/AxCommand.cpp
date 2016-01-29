#include "StdAfx.h"
#include "AxLib.h"

using namespace dbAx;

///////////////////////////////////////////////////////////////
//	Construction

CAxCommand::CAxCommand()
{
  m_piCommand = NULL;
  m_piParameters = NULL;
}

CAxCommand::~CAxCommand()
{
	if ( m_piCommand )
	{
		if ( m_piParameters )
		{
			m_piParameters->Release();
			m_piParameters = NULL;
		}

		if ( m_piCommand )
      m_piCommand->Release();
		m_piCommand = NULL;
	}
}

///////////////////////////////////////////////////////////////
//	Initialization

// Method: Create
//   Desc: Creates and attacheds the underlying ADO Recordset object.
//         Create must be called before using the CAxRecordset object.
//
//   Args: none
//
// Return: void
//
void CAxCommand::Create()
{
  HRESULT hr = ::CoCreateInstance(CLSID_CADOCommand, NULL, CLSCTX_INPROC_SERVER,
    IID_IADOCommand, (LPVOID *)&m_piCommand);

	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_INIT, _T("CAxCommand::Create"), hr);

	VALID_ADO_OBJECT(m_piCommand);

  hr = m_piCommand->get_Parameters(&m_piParameters);

	if FAILED( hr )
	{
		delete m_piCommand;
		m_piCommand = NULL;
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::Create"), hr);
	}

  _CreateParameters();
}

///////////////////////////////////////////////////////////////
//	AxLib Specific

// Method: _SetParamValue
//   Desc: Sets the underlying ADO Parameter's value. Called by
//         _UpdateParameters which is overridden in the derive
//         class.
//
//   Args:
//      lpszParam: Parameter name.
//         pValue: Parameter value.
//
// Return: void
//
void CAxCommand::_SetParamValue(LPCTSTR lpszParam, _variant_t *pValue)
{
	VALID_ADO_OBJECT(m_piCommand);
	ASSERT( m_piParameters != NULL );
	DataTypeEnum eDataType;
  HRESULT hr = S_OK;
	ADOParameter *pParameter = NULL;

  hr =  m_piParameters->get_Item(_variant_t(lpszParam), &pParameter);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::_SetParamValue"), hr);

  hr = pParameter->get_Type(&eDataType);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::_SetParamValue"), hr);

  hr = pParameter->put_Type(eDataType);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::_SetParamValue"), hr);

  hr = pParameter->put_Value(*pValue);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::_SetParamValue"), hr);

}

///////////////////////////////////////////////////////////////
//	Collections

// Method: Parameters
//   Desc: Returns a reference to the underlying ADO Command's
//         Prameters collection
//
//   Args: none
//
// Return: Pointer to the ADOParameters collection
//
ADOParameters* CAxCommand::Parameters()
{
	VALID_ADO_OBJECT(m_piCommand);
  return (m_piParameters);
}

// Method: Properties
//   Desc: Returns a reference to the underlying ADO Command's
//         Properties collection
//
//   Args: none
//
// Return: Pointer to the ADOProperties collection
//
ADOProperties* CAxCommand::Properties()
{
	VALID_ADO_OBJECT(m_piCommand);
  ADOProperties *pProps;

  HRESULT hr = m_piCommand->get_Properties(&pProps);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::Properties"), hr);

  return (pProps);
}

///////////////////////////////////////////////////////////////
//	Methods

// Method: Cancel
//   Desc: Cancels pending asychronous command that is executing.
//         Not supported in this version of AxLib.
//
//   Args: none
//
// Return: void
//
void CAxCommand::Cancel()
{
}

// Method: CreateParameter
//   Desc: Creates and appends a parameter to the underlying
//         ADO Command's Parameters collection. Note, to avoid
//         memory leaks the client application should assign
//         the returned pointer and call Release when it is no
//         longer needed. Otherwise a memory leak will occur
//
//   Args:
//              lpszParam: The parameter name.
//              eDataType: Data type of the parameter
//    eParameterDirection: Parameter direction
//                  lSize: Max size in chars or bytes
//                 pValue: Initial value
//
// Return: Pointer to an ADOParameter object
//
ADOParameter* 
CAxCommand::CreateParameter(LPCTSTR lpszParam,
                                  DataTypeEnum eDataType,
                                  ParameterDirectionEnum eParameterDirection, 
                                  long lSize, _variant_t *pValue)
{
	VALID_ADO_OBJECT(m_piCommand);
  ASSERT( m_piParameters != NULL );
  ADOParameter *pParam = NULL;

  HRESULT hr = m_piCommand->CreateParameter(bstr_t(lpszParam), eDataType, 
                                           eParameterDirection, lSize,
                                           *pValue, &pParam);

  if  FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CreateParameter"), hr);

  hr = m_piParameters->Append(pParam);

  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CreateParameter"), hr);

  return (pParam);
}

// Method: Execute
//   Desc: Executes a stored procedure, an SQL statement
//         or query against a data provider.
//         
//
//   Args: plAffected - pointer to long indicating the
//         number of records affected.
//
// Return: Pointer to an newly allocated ADORecordset object
//
ADORecordset* CAxCommand::Execute(long *plAffected)
{
	VALID_ADO_OBJECT(m_piCommand);
  _variant_t varAffected(0L);
  _variant_t varParams;
  _variant_t HUGEP *pvtItems = NULL;
  ADORecordset *piRecSet = NULL;
  HRESULT hr = S_OK;

  //Construct parameter arguments from existing parameters
  long lParamCount = 0;

  hr = m_piParameters->get_Count(&lParamCount);
  if  FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::Execute"), hr);
	
  //Create a SafeArray to contain the parameters
  SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = lParamCount;

  varParams.vt = VT_ARRAY | VT_VARIANT;
  varParams.parray = SafeArrayCreate(VT_VARIANT, 1, rgsabound);

  if ( varParams.parray == NULL )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::Execute"), hr);

  hr = SafeArrayAccessData(varParams.parray, (void HUGEP* FAR*)&pvtItems);
  if  FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::Execute"), hr);

  //Compile parameter values into SafeArray
  ADOParameter *pParam = NULL;
  for ( long i = 0; i < lParamCount; i++ )
  {
    m_piParameters->get_Item(_variant_t(i), &pParam);
    pParam->get_Value(&pvtItems[i]);
  }

  //Execute the command
  hr = m_piCommand->Execute(&varAffected, pvtItems, CommandType(), &piRecSet);
  SafeArrayUnaccessData(varParams.parray);

  if  FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::Execute"), hr);

  if ( plAffected )
    *plAffected = varAffected.lVal;

  return (piRecSet);
}

///////////////////////////////////////////////////////////////
//	Properties

// Method: ActiveConnection
//   Desc: Sets/Returns the object's CAxConnection reference.
//         Call with no argument to return the current value.
//
//   Args:
//      pCn: Reference to an existing CAxConnection object.
//
// Return: The current CAxConnection reference.
//
CAxConnection* CAxCommand::ActiveConnection(CAxConnection *pCn)
{
	VALID_ADO_OBJECT(m_piCommand);

  if ( pCn )
  {
    m_pConnection = pCn;

    if ( !pCn->_IsOpen() )
      ThrowAxException(AXLIB_ERROR_INIT, 
        _T("CAxCommand::SetActiveConnection"));

    HRESULT hr = m_piCommand->putref_ActiveConnection(m_pConnection->_GetActiveConnection());

    if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::ActiveConnection"), hr);
  }

  return (m_pConnection);
}

// Method: CommandText
//   Desc: Sets/Returns the object's commandtext string.
//         Call with no argument to return the current value.
//
//   Args:
//      lpszCmdText: New command text string
//
// Return: The current command text.
//
LPCTSTR CAxCommand::CommandText(LPCTSTR lpszCmdText) 
{
	VALID_ADO_OBJECT(m_piCommand);

#ifdef _UNICODE
  bstr_t bstrCmdText(lpszCmdText);
#else
  bstr_t bstrCmdText(M2W(lpszCmdText));
#endif

  HRESULT hr = S_OK;

  if ( lpszCmdText )
  {
    hr = m_piCommand->put_CommandText(bstrCmdText);
	  if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CommandText"), hr);
  }

  BSTR bstrCommandText;
  hr = m_piCommand->get_CommandText(&bstrCommandText);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CommandText"), hr);
    
  return (CString(bstrCommandText).operator LPCTSTR());
}

// Method: CommandTimeout
//   Desc: Sets/Returns the object's timeout setting.
//         Call with no argument to return the current value.
//
//   Args:
//      lSeconds: Number of seconds to wait before terminating
//
// Return: The current timeout setting.
//
long CAxCommand::CommandTimeout(long lSeconds)
{
	VALID_ADO_OBJECT(m_piCommand);
  HRESULT hr = S_OK;

  if ( lSeconds >= 0 )
  {
    hr = m_piCommand->put_CommandTimeout(lSeconds);
	  if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CommandTimeout"), hr);
  }

  long lCmdTimeout;
  hr = m_piCommand->get_CommandTimeout(&lCmdTimeout);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CommandTimeout"), hr);

  return (lCmdTimeout);
}

// Method: CommandType
//   Desc: Sets/Returns the object's command type setting.
//         Call with no argument to return the current value.
//
//   Args:
//      eCmdType: The command type to use
//
// Return: The current command type setting.
//
CommandTypeEnum CAxCommand::CommandType(CommandTypeEnum eCmdType)
{
	VALID_ADO_OBJECT(m_piCommand);
  HRESULT hr = S_OK;

  if ( eCmdType > adCmdUnspecified )
  {
    hr = m_piCommand->put_CommandType(eCmdType);
	  if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CommandType"), hr);
  }

  CommandTypeEnum eCommandType;
  hr = m_piCommand->get_CommandType(&eCommandType);
  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::CommandType"), hr);

  return (eCommandType);
}

/*
void CAxCommand::Prepared(bool iPrepared)
{
	VALID_ADO_OBJECT(m_piCommand);

	if FAILED( m_piCommand->put_Prepared(iPrepared) )
    ThrowAxException(AXLIB_ERROR_NONE);
}

bool CAxCommand::GetPrepared()
{
	VALID_ADO_OBJECT(m_piCommand);
	short bPrepared;

	if FAILED( m_piCommand->get_Prepared(&bPrepared) )
    ThrowAxException(AXLIB_ERROR_NONE);

	return ( bPrepared > 0 );
}
*/

// Method: State
//   Desc: Returns the object's immediate state.
//
//   Args: none
//
// Return: The current state of the object.
//
ObjectStateEnum CAxCommand::State()
{
	VALID_ADO_OBJECT(m_piCommand);
	long lState = 0;

  HRESULT hr = m_piCommand->get_State(&lState);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxCommand::State"), hr);

	return ((ObjectStateEnum)lState);
}
