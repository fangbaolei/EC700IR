#include "StdAfx.h"
#include "AxLib.h"

using namespace dbAx;

///////////////////////////////////////////////////////////////
//	Construction

CAxRecordset::CAxRecordset()
{
  m_pConnection = NULL;
  m_pCommand = NULL;
  m_piConnection = NULL;
  m_piRecordset = NULL;
  m_pFields= NULL;
  m_dwEvents = 0;
  m_eCursorType = (CursorTypeEnum)-2;
  m_eLockType = (LockTypeEnum)-2;
}

CAxRecordset::~CAxRecordset()
{
  if ( _IsOpen() )
    Close();

  if( m_piRecordset )
    m_piRecordset->Release();
  m_piRecordset = NULL;
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
//   v1.1: Moved call to Create internal to the Open method. Client
//         application no longer needs to call Create(). See note
//         in AxLib.h

HRESULT CAxRecordset::Create()
{
  HRESULT hr = CoCreateInstance(CLSID_CADORecordset, NULL, CLSCTX_INPROC_SERVER, 
              IID_IADORecordset15, (LPVOID *)&m_piRecordset);

	if FAILED( hr )
  {
    if ( m_piRecordset != NULL )
    {
      m_piRecordset->Release();
      m_piRecordset = NULL;
    }
  }

  return ( hr );
}

///////////////////////////////////////////////////////////////
//	AxLibSpecific

// Method: _ClearRecordsetEvents
//   Desc: Releases the ADO Events sink interface.
//
//   Args: none
//
// Return: void
//
void CAxRecordset::_ClearRecordsetEvents()
{
  if ( m_piRecordset == NULL || m_dwEvents == 0 )
    return;

  IConnectionPointContainer *pCPC = NULL;
  IConnectionPoint          *pCP = NULL;
  HRESULT hr = m_piRecordset->QueryInterface(__uuidof(IConnectionPointContainer),
    (LPVOID*)&pCPC);
  if FAILED(hr) 
	  ThrowAxException(AXLIB_ERROR_NONE, _T("Failed to create connection object"));

  hr = pCPC->FindConnectionPoint(__uuidof(RecordsetEvents), &pCP);
  pCPC->Release();
  if FAILED(hr)
		ThrowAxException(AXLIB_ERROR_NONE, _T("Failed to create connection object"));

  hr = pCP->Unadvise(m_dwEvents);
  pCP->Release();
  m_dwEvents--;
}

// Method: _IsEmpty
//   Desc: Indicates if there are any records in the recordset.
//
//   Args: none
//
// Return: True if the recordset is empty. Otherwise, false.
//
bool CAxRecordset::_IsEmpty()
{
	VALID_ADO_OBJECT(m_piRecordset);
	VARIANT_BOOL vbBOF = 0;
  VARIANT_BOOL vbEOF = 0;

	m_piRecordset->get_BOF(&vbBOF);
	m_piRecordset->get_EOF(&vbEOF);

  return ( (vbBOF<0) && (vbEOF<0) );
}

// Method: _IsOpen
//   Desc: Indicates if the recordset is currently open.
//
//   Args: none
//
// Return: True if the recordset is open. Otherwise, false.
//
bool CAxRecordset::_IsOpen()
{
  if ( m_piRecordset == NULL )
    return (FALSE);

	VALID_ADO_OBJECT(m_piRecordset);

  long lState;
  if FAILED( m_piRecordset->get_State(&lState) )
		ThrowAxException(AXLIB_ERROR_NONE);

  return (lState == adStateOpen);
}

// Method: _SetRecordsetEvents
//   Desc: Attaches the ADO Events sink interface allowing client
//         applications to handle ADO Recordset events. The host
//         program provides a specific implementation of the
//         CAxRecordsetEvents class to handle and respond to ADO
//         events that may be raised.
//
//   Args: CAxRecordsetEvents*
//          A reference to an AxLib Events object derive from
//          CAxRecordsetEvents.
//
// Return: void
//
void CAxRecordset::_SetRecordsetEvents(CAxRecordsetEvents* pEvents)
{
    IConnectionPointContainer *pCPC = NULL;
    IConnectionPoint          *pCP = NULL;
    HRESULT hr = m_piRecordset->QueryInterface(__uuidof(IConnectionPointContainer),
      (LPVOID*)&pCPC);
    if FAILED(hr) 
			ThrowAxException(AXLIB_ERROR_NONE, _T("Failed to create connection object"));

    hr = pCPC->FindConnectionPoint(__uuidof(RecordsetEvents), &pCP);
    pCPC->Release();
    if FAILED(hr)
			ThrowAxException(AXLIB_ERROR_NONE, _T("Failed to create connection object"));

    hr = pCP->Advise(pEvents, &m_dwEvents);
    pCP->Release();
}


///////////////////////////////////////////////////////////////
//	Collections

// Method: Fields
//   Desc: Returns a reference to the ADO Filelds collection.
//
//   Args: none
//
// Return: ADOFields*
//
ADOFields* CAxRecordset::Fields()
{
  return (m_pFields);
}

// Method: Properties
//   Desc: Returns a reference to the ADO Properties collection.
//
//   Args: none
//
// Return: ADOProperties*
//
ADOProperties* CAxRecordset::Properties()
{
	VALID_ADO_OBJECT(m_piRecordset);
  ADOProperties *pProps;

  HRESULT hr = m_piRecordset->get_Properties(&pProps);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Properties"), hr);

  return (pProps);
}


///////////////////////////////////////////////////////////////
//	Methods

// Method: AddNew
//   Desc: Creates a new blank record in the recordset. After
//         calling AddNew, the client application typically
//         completes the member variables of the CAxRecordset
//         derive class then calls the Update method to post
//         the new recordset information to the database.
//
//   Args: none
//
// Return: void
//
void CAxRecordset::AddNew()
{
	VALID_ADO_OBJECT(m_piRecordset);
  _variant_t vNames(DISP_E_PARAMNOTFOUND, VT_ERROR);
  _variant_t vValues(DISP_E_PARAMNOTFOUND, VT_ERROR);

  if ( Supports(adAddNew) )
  {
    HRESULT hr = m_piRecordset->AddNew(vNames, vValues);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Properties"), hr);
  }
}

// Method: Cancel
//   Desc: Cancels an asynchronous operation for the recordset object.
//
//   Args: none
//
// Return: void
//
void CAxRecordset::Cancel()
{
}

void CAxRecordset::CancelBatch()
{
}

// Method: CancelUpdate
//   Desc: Cancels editing of a record. A call to the CancelUpdate method
//         prior to calling Update will cancel any changes made to the
//         record since editing began. Additionally, if AddNew was called,
//         the creation of a new record is also canceled. With respect
//         to AxLib, a call to CancelUpdate is generally not needed since
//         changes are made to the local member variables. Any need to
//         cancel updating of the record would be handled in the client
//         application before making a call to the Update method.
//
//   Args: none
//
// Return: void
//
void CAxRecordset::CancelUpdate()
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;
	EditModeEnum	lEditMode;

  hr = m_piRecordset->get_EditMode(&lEditMode);
  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CancelUpdate"), hr);

  if ( lEditMode == adEditInProgress || lEditMode == adEditAdd )
  {
    hr = m_piRecordset->CancelUpdate();
    if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CancelUpdate"), hr);
  }
}

// Method: Clone
//   Desc: Return a duplicate copy of the underlying ADO Recordset object.
//
//   Args: eLockType
//     adLockUnspecified, (default)
//     adLockReadOnly
//
// Return: ADORecordset*
//
ADORecordset* CAxRecordset::Clone(LockTypeEnum eLockType)
{
	VALID_ADO_OBJECT(m_piRecordset);
  ADORecordset* pRec;

  HRESULT hr = m_piRecordset->Clone(eLockType, &pRec);
  if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Clone"), hr);

  return (pRec);
}

// Method: Close
//   Desc: Closes and open recordset. All dependent objects such as 
//         CAxRecordsetEvents, CAxCommand objects and the underlying
//         ADO Recordset object are closed as well.
//
//   Args: none
//
// Return: void
//
void CAxRecordset::Close()
{
	VALID_ADO_OBJECT(m_piRecordset);

  //Cleanup any related Command objects
  if ( m_pCommand )
  {
		if ( m_pCommand->m_piParameters )
		{
			m_pCommand->m_piParameters->Release();
			m_pCommand->m_piParameters = NULL;
		}
  	m_pCommand->m_piCommand->Release();
		m_pCommand->m_piCommand = NULL;
  }

  //Release Recordset events
  _ClearRecordsetEvents();

	if ( m_piRecordset && _IsOpen() )
	{
		m_piRecordset->MoveFirst();
		HRESULT hr = m_piRecordset->Close();
		if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Close"), hr);
	}

	if(m_pFields)
	{
		m_pFields->Release();
        m_pFields = NULL;
	}
	if(m_pConnection)
	{
		m_pConnection->Close();
		delete m_pConnection;
		m_pConnection = NULL;
	}
	m_piRecordset->Release();
	m_piRecordset = NULL;
}

// Method: CompareBookmarks
//   Desc: Determines the position of two bookmarks relative to one
//         another. A CompareEnum value is returned that indicates which
//         bookmark comes first in ordinal position.
//
//   Args: vtBkMark1, vtBkMark2
//          Bookmark variants to be compared.
//
// Return: A CompareEnum enumeration
//          adCompareLessThan	= 0,
//          adCompareEqual	= 1,
//          adCompareGreaterThan	= 2,
//          adCompareNotEqual	= 3,
//          adCompareNotComparable	= 4
//
CompareEnum CAxRecordset::CompareBookmarks(_variant_t vtBkMark1, _variant_t vtBkMark2)
{
 	VALID_ADO_OBJECT(m_piRecordset);
  CompareEnum eComp;

  HRESULT hr = m_piRecordset->CompareBookmarks(vtBkMark1, vtBkMark2, &eComp);
  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CompareBookmarks"), hr);

  return (eComp);
}

// Method: Delete 
//   Desc: Deletes specified records in the recordset. In general the
//         client application will use the default of adAffectCurrent
//         to delete the current record only.
//
//   Args: eAffect
//          An AffectEnum value:
//            adAffectCurrent	= 1,
//            adAffectGroup	= 2,
//            adAffectAll	= 3,
//           dAffectAllChapters	= 4
//
// Return: void
//
void CAxRecordset::Delete(AffectEnum eAffect)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;
	EditModeEnum	lEditMode;

	//Watch for empty recordset
	if ( _IsEmpty() )
		return;

	//Delete method depends on current mode
  hr = m_piRecordset->get_EditMode(&lEditMode);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Delete"), hr);

	switch (lEditMode)
  {
	case adEditNone: // Just delete it
    hr = m_piRecordset->Delete(eAffect);
	  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Delete"), hr);
		break;

	case adEditInProgress: //Forget changes
    hr = m_piRecordset->CancelUpdate();
		if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Delete"), hr);

    hr = m_piRecordset->Delete(eAffect);
	  if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Delete"), hr);
		break;

	case adEditAdd: //If new record, go back to last known
    hr = m_piRecordset->CancelUpdate();
		if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Delete"), hr);
    
    hr = m_piRecordset->put_Bookmark(m_varLastGoodRecord);
		if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Delete"), hr);
    break;
	}
}

// Method: Find 
//   Desc: Performs a search based on a single column search criteria.
//         If successful, the record pointer is positioned on the 
//         first record to match the search criteria. The search
//         criteria matches that of the SQL WHERE clause, such as
//         "AccountID = 'ABC'". The client application should set the
//         record pointer by making a call to Move, MoveFirst, etc.
//
//   Args: strCriteria - search criteria
//         nSkip - number of rows to skip before searching
//         SearchDirectionEnum - determines the direction of the search
//            adSearchForward	= 1,
//            adSearchBackward = -1
//         pvtStart - bookmark used as a starting point for the search
//
// Return: true if match is found, otherwise false
//

bool CAxRecordset::Find(LPCTSTR lpszCriteria, long nSkip,
	         SearchDirectionEnum eSearchDirection, _variant_t* pvtStart)
{
	VALID_ADO_OBJECT(m_piRecordset);
	HRESULT hr;
	_bstr_t bstrCriteria(lpszCriteria);

  if ( _IsEmpty() )
    return ( false );

  if ( pvtStart )
	  hr = m_piRecordset->Find(bstrCriteria, nSkip, eSearchDirection, *pvtStart);
  else
	  hr = m_piRecordset->Find(bstrCriteria, nSkip, eSearchDirection, _variant_t(AbsolutePosition()));

  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Find"), hr);

  if ( IsBOF() || IsEOF() )
    return ( false );

	DoFieldExchange(FALSE);
	return ( true );
}

void CAxRecordset::GetRows()
{
}

void CAxRecordset::GetString()
{
}

// Method: Move 
//   Desc: Moves a specified number of records forward or backward
//         from the current position. If the resultant move is beyond
//         the end of the recordset or past the beginning, the 
//         record point is positioned just after the last record or
//         just before the first record, i.e. BOF/EOF.
//
//   Args: nNumRecs - number of records to move from the current position
//                    or that specified by the pvtStartRec bookmark
//         pvtStartRec - bookmark specifing the starting record position.
//                       The default is the current record, adBookmarkCurrent
//
// Return: void
//
void CAxRecordset::Move(long nNumRecs, _variant_t* pvtStartRec)
{
	VALID_ADO_OBJECT(m_piRecordset);

  HRESULT hr = S_OK;
  
  if ( pvtStartRec )
    hr = m_piRecordset->Move(nNumRecs, *pvtStartRec);
  else
    hr = m_piRecordset->Move(nNumRecs, _variant_t(0L));

	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Move"), hr);
		
  if ( !IsBOF() && !IsEOF() )
	  DoFieldExchange(FALSE);
}

// Method: MoveFirst
//   Desc: Sets the record pointer to the first record in the recordset.
//
//   Args: none 
//
// Return: void
//
void CAxRecordset::MoveFirst()
{
	VALID_ADO_OBJECT(m_piRecordset);

	if ( _IsEmpty() )
    return;

  HRESULT hr = m_piRecordset->MoveFirst();
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MoveFirst"), hr);

	DoFieldExchange(FALSE);
}

// Method: MoveLast
//   Desc: Sets the record pointer to the last record in the recordset.
//
//   Args: none 
//
// Return: void
//
void CAxRecordset::MoveLast()
{
	VALID_ADO_OBJECT(m_piRecordset);

	if ( _IsEmpty() )
    return;

	HRESULT hr = m_piRecordset->MoveLast();
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MoveLast"), hr);

	DoFieldExchange(FALSE);
}

// Method: MoveNext
//   Desc: Sets the record pointer to the next record in the recordset.
//         If the record pointer is on the last record before a call to
//         MoveNext, the record pointer is moved beyond the last record
//         and the EOF property is set to true.
//
//   Args: none 
//
// Return: void
//
void CAxRecordset::MoveNext()
{
	VALID_ADO_OBJECT(m_piRecordset);

//  if ( _IsEmpty() || IsEOF() )
//    if ( IsEOF() )
//    return;

  HRESULT hr = m_piRecordset->MoveNext();
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MoveNext"), hr);

	if ( !IsEOF() )
		DoFieldExchange(FALSE);
}

// Method: MovePrevious
//   Desc: Sets the record pointer to the previous record in the recordset.
//         If the record pointer is on the first record before a call to
//         MovePrevious, the record pointer is moved to a position before
//         the first record and the BOF property is set to true.
//
//   Args: none 
//
// Return: void
//
void CAxRecordset::MovePrevious()
{
	VALID_ADO_OBJECT(m_piRecordset);

//  if ( _IsEmpty() || IsBOF() )
//    return;

  HRESULT hr = m_piRecordset->MovePrevious();
  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MovePrevious"), hr);

	if ( !IsBOF() )
		DoFieldExchange(FALSE);
}

// Method: NextRecordset
//   Desc: Not supported in this version of AxLib
//
void CAxRecordset::NextRecordset()
{
}

// Method: Open
//   Desc: The first version of the Open method accepts a CAxCommand object
//         whereby a stored procedure is used to return recordset results.
//         A derived CAxRecordset object must exist and be initialized before
//         calling the Open method. If the method fails, a CAxException is
//         thrown.
//
//   Args: 
//        pCmd: Reference to a CAxCommand object.
//       cType: Cursor type to use, default is adOpenForwardOnly
//      cLocat: Cursor location, default is adUseClient
//       lType: Lock type to use, default is adLockOptimistic
//    nOptions: Command options, default is adCmdUnspecified
//
// Return: void
//
void CAxRecordset::Open(CAxCommand* pCmd, 
				                CursorTypeEnum eCursorType,
				                CursorLocationEnum eCursorLocation,
				                LockTypeEnum eLockType,
				                long nOptions)
{
  if ( m_piRecordset == NULL )
  {
    HRESULT hr = Create();
    if  FAILED ( hr )
      ThrowAxException(AXLIB_ERROR_INIT, _T("CAxRecordset::Open"), hr);
  }

  VALID_ADO_OBJECT(m_piRecordset);
  VARIANT vNull, vCmd;
  HRESULT hr = S_OK;

	vNull.vt = VT_ERROR;
	vNull.scode = DISP_E_PARAMNOTFOUND;

	vCmd.vt = VT_DISPATCH;
	vCmd.pdispVal = pCmd->m_piCommand;

  //Capture references to controlling objects
  m_pConnection = pCmd->ActiveConnection();
  m_pCommand = pCmd;
  m_piConnection = m_pConnection->_GetActiveConnection();

  //Create command object's parameters (one time only) and Update
  //Note: v1.2.0 now calls the _CreateParameters method from within the
  //CAxCommand::Create method
  m_pCommand->_UpdateParameters();

  CursorTypeEnum ct = (m_eCursorType == -2) ? eCursorType : m_eCursorType;
  LockTypeEnum lt = (m_eLockType == -2) ? eLockType : m_eLockType;
  CursorLocation(eCursorLocation);

  hr = m_piRecordset->Open(vCmd, vNull, ct, lt, nOptions);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Open"), hr);

  hr = m_piRecordset->get_Fields(&m_pFields);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Open"), hr);

	DoFieldExchange(FALSE);
}

// Method: Open
//   Desc: The second version of the Open method uses a source argument in
//         place of a CAxCommand object. A typical source statement would 
//         be "SELECT * From Accounts ORDER By AccountID"
//
// v1.1.1: Added call to CursorLocation
//
//   Args: 
//        szSource: Valid SQL Source statement
//     pConnection: Reference to an existing CAxConnection object
//           cType: Cursor type to use, default is adOpenForwardOnly
//          cLocat: Cursor location, default is adUseClient
//           lType: Lock type to use, default is adLockOptimistic
//        nOptions: Command options, default is adCmdUnspecified
//
// Return: void
//
void CAxRecordset::Open(LPCTSTR szSource, CAxConnection *pConnection,
				                CursorTypeEnum eCursorType,
				                CursorLocationEnum eCursorLocation,
				                LockTypeEnum eLockType,
				                long nOptions)
{
  if ( m_piRecordset == NULL )
  {
    HRESULT hr = Create();
    if  FAILED ( hr )
      ThrowAxException(AXLIB_ERROR_INIT, _T("CAxRecordset::Open"), hr);
  }

  VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

	if ( szSource != NULL )
    m_strSourceString = szSource;

	if ( pConnection != NULL )
		m_piConnection = pConnection->_GetActiveConnection();

	ASSERT( !m_strSourceString.IsEmpty() );
	ASSERT( m_piConnection != NULL );

  hr =  m_piRecordset->put_Source(m_strSourceString.AllocSysString());
	if FAILED( hr )
	{
		m_piRecordset->Release();
		m_piRecordset = NULL;
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Open"), hr);
	}

  hr = m_piRecordset->putref_ActiveConnection((ADOConnection*)m_piConnection);
	if FAILED( hr )
	{
		m_piRecordset->Release();
		m_piRecordset = NULL;
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Open"), hr);
	}

  CursorTypeEnum ct = (m_eCursorType == -2) ? eCursorType : m_eCursorType;
  LockTypeEnum lt = (m_eLockType == -2) ? eLockType : m_eLockType;
  CursorLocation(eCursorLocation);

  hr = m_piRecordset->Open(_variant_t(szSource), _variant_t(m_piConnection),
    ct, lt, nOptions);

	if FAILED( hr )
	{
		m_piRecordset->Release();
		m_piRecordset = NULL;
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Open"), hr);
	}

  hr = m_piRecordset->get_Fields(&m_pFields);

	if FAILED( hr )
	{
		m_piRecordset->Release();
		m_piRecordset = NULL;
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Open"), hr);
	}

  DoFieldExchange(FALSE);
}

// Method: Requery
//   Desc: The Requery method re-executes the command that was used to
//         originally open the recordset. If the recordset was opened
//         using a command object (i.e. CAxCommand), Requery is used to
//         update the recordset after changing one or more parameter
//         values of the command.
//
//   Args: 
//      eExecuteOption: Execute option, default is adOptionUnspecified
//        adOptionUnspecified	= -1,
//        adAsyncExecute	= 0x10,
//        adAsyncFetch	= 0x20,
//        adAsyncFetchNonBlocking	= 0x40,
//        adExecuteNoRecords	= 0x80
//
// Return: void
//
void CAxRecordset::Requery(ExecuteOptionEnum eExecuteOption)
{
	VALID_ADO_OBJECT(m_piRecordset);

  if ( m_pCommand )
    m_pCommand->_UpdateParameters();

  HRESULT hr = m_piRecordset->Requery(eExecuteOption);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Requery"), hr);
  
	DoFieldExchange(FALSE);
}

// Method: Resync
//   Desc: The Resync method is used to resychronize the the records
//         with the data provider. Resync is typically used when the
//         recordset was opened using a Static or Forward only cursor
//         and the client application needs to check is any other users
//         have made changes.
//
//   Args: 
//      eAffectRecords: Affect records options, default is adAffectAll
//        adAffectCurrent	= 1,
//        adAffectGroup	= 2,
//        adAffectAll	= 3,
//        adAffectAllChapters	= 4
//
//      eResyncValues: Resync option, default is adResyncAllValues
//        adResyncUnderlyingValues	= 1,
//        adResyncAllValues	= 2
//
// Return: void
//
void CAxRecordset::Resync(AffectEnum eAffectRecords,
                                ResyncEnum eResyncValues) 
{
	VALID_ADO_OBJECT(m_piRecordset);

  HRESULT hr = m_piRecordset->Resync(eAffectRecords, eResyncValues);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Resync"), hr);
}

// Method: Save
//   Desc: Saves the recordset to a file. The AxLib version differs
//         from ADO in that a file name is used as the destination
//         argument rather than a variant. Therefore, only saving
//         to a file is supported.
//
//   Args: 
//      lpszFileName: File name to save recordset data to.
//    ePersistFormat: Persist format option, default is adPersistADTG
//      adPersistADTG	= 0,
//      adPersistXML	= 1
//
// Return: void
//
void CAxRecordset::Save(LPCTSTR lpszFileName,
                              PersistFormatEnum ePersistFormat) 
{
#if _MSC_VER == 1200
  _bstr_t bstrFileName(lpszFileName);
  HRESULT hr = m_piRecordset->Save(bstrFileName, ePersistFormat);
#else
  #ifdef _UNICODE
    _variant_t varFile(lpszFileName);
  #else
    _variant_t varFile(M2W(lpszFileName));
  #endif
  HRESULT hr = m_piRecordset->Save(varFile, ePersistFormat);
#endif

  if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Save"), hr);
}


// Method: Supports
//   Desc: Determines if the data provider supports a certain
//         functionality.
//
//   Args: 
//      eCursorOption: Cursor option to validate
//        adHoldRecords	= 0x100,
//        adMovePrevious	= 0x200,
//        adAddNew	= 0x1000400,
//        adDelete	= 0x1000800,
//        adUpdate	= 0x1008000,
//        adBookmark	= 0x2000,
//        adApproxPosition	= 0x4000,
//        adUpdateBatch	= 0x10000,
//        adResync	= 0x20000,
//        adNotify	= 0x40000,
//        adFind	= 0x80000
//
// Return: If the data provider supports the indicated option true,
//         otherwise false.
//
bool CAxRecordset::Supports(CursorOptionEnum eCursorOption)
{
	VALID_ADO_OBJECT(m_piRecordset);
	VARIANT_BOOL vSucceeded;
	
  HRESULT hr = m_piRecordset->Supports(eCursorOption, &vSucceeded);
	if FAILED( hr )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Supports"), hr);

	return ( vSucceeded != 0 );
}

// Method: Update
//   Desc: Posts changes made to the recordset back to the server. The
//         Update method calls the virtual function DoFieldExchange in
//         the derived CAxRecordset object to write individual field
//         data back to the data provider.
//
//   Args: none
//
// Return: void
//
void CAxRecordset::Update()
{
	VALID_ADO_OBJECT(m_piRecordset);
	_variant_t vNames(DISP_E_PARAMNOTFOUND, VT_ERROR);
	_variant_t vValues(DISP_E_PARAMNOTFOUND, VT_ERROR);

	DoFieldExchange(TRUE);

	//Use 'adResyncUnderlyingValues' on Resync to avoid errors on compound
	//Recordsets (i.e. INNER/OUTER Joins of tables)
  HRESULT hr = S_OK;
	if ( Supports(adResync) )
	{
		if FAILED( m_piRecordset->Resync(adAffectCurrent, adResyncUnderlyingValues) )
		{
      hr = m_piRecordset->Resync(adAffectCurrent, adResyncAllValues);
			if FAILED( hr )
        ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Update"), hr);
		}
	}

	if ( Supports(adUpdate) )
	{
    hr = m_piRecordset->Update(vNames, vValues);
		if FAILED( hr )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Update"), hr);
	}
}


///////////////////////////////////////////////////////////////
//	Properties

// Method: AbsolutePage
//   Desc: Sets/Returns the current page in the record set.
//         Call method with no argument to return current setting.
//
//   Args: nPage
//          Page to set active. Page value must be between
//          1 and the total page count. Page count is 
//          returned by the PageCount() method.
//
// Return: Current page or PositionEnum value.
//            adPosUnknown	= -1,
//            adPosBOF	= -2,
//            adPosEOF	= -3
//
long CAxRecordset::AbsolutePage(long nPageNum)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( nPageNum < 0 || nPageNum > PageCount() )
		  ThrowAxException(AXLIB_ERROR_INVALID_POS, _T("CAxRecordset::AbsolutePage"));

  if ( nPageNum > 0 )
  {
    hr = m_piRecordset->put_AbsolutePage((PositionEnum)nPageNum);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::AbsolutePage"), hr);
  }

	PositionEnum pageNum;
  hr = m_piRecordset->get_AbsolutePage(&pageNum);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::AbsolutePage"), hr);

	return ((PositionEnum)pageNum);
}

// Method: AbsolutePage
//   Desc: Sets/Returns the current record position.
//         Call method with no argument to return current setting.
//
//   Args: nPosition
//          Position to set the current record. Position is 1' based.
//          Position is between 1 and the record count. Record count
//          is returned by the RecordCount() method.
//
// Return: Current record position (ordinal value) or PositionEnum value.
//            adPosUnknown	= -1,
//            adPosBOF	= -2,
//            adPosEOF	= -3
//
long CAxRecordset::AbsolutePosition(long nPosition)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;
  _variant_t* pvtBookmark = NULL;

  if ( _IsEmpty() )
    return ( 0L );

  if ( nPosition < 0 || nPosition > RecordCount() )
		  ThrowAxException(AXLIB_ERROR_INVALID_POS, _T("CAxRecordset::AbsolutePosition"));

  if ( nPosition > 0 )
  {
		pvtBookmark = Bookmark();
    hr = m_piRecordset->put_AbsolutePosition(PositionEnum(nPosition));
		if FAILED( hr )
		{
			if ( pvtBookmark->vt != VT_EMPTY )
				m_piRecordset->put_Bookmark(*pvtBookmark);
		 
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::AbsolutePosition"), hr);
		}

		delete pvtBookmark;
    DoFieldExchange(FALSE);
  }

	PositionEnum recPos;

  hr = m_piRecordset->get_AbsolutePosition(&recPos);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::AbsolutePosition"), hr);

	return ((long)recPos);
}

// Method: ActiveCommand
//   Desc: Returns a reference to the associated CAxCommand object provided
//         the record set was initially opened by means of a CAxCommand.
//
//   Args: none
//
// Return: Reference to ADOCommand object. NULL if record set was
//         opened directly.
//
CAxCommand* CAxRecordset::ActiveCommand()
{
  return (m_pCommand);
}

// Method: ActiveConnection
//   Desc: Returns a reference to the associated CAxConnection object. 
//
//   Args: none
//
// Return: Reference to ADOConnection object.
//
CAxConnection* CAxRecordset::ActiveConnection()
{
  return (m_pConnection);
}

// Method: IsBOF - Proxy for ADO's BOF method
//   Desc: Determine if the current record pointer is before
//         the first record.
//
//   Args: none
//
// Return: Returns true is the record pointer is located before the
//         first record or if the recordset contains no records.
//         Otherwise false.
//
bool CAxRecordset::IsBOF()
{
	VALID_ADO_OBJECT(m_piRecordset);
	VARIANT_BOOL vbBOF = 0;

  HRESULT hr = m_piRecordset->get_BOF(&vbBOF);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::IsBOF"), hr);

	return ( vbBOF < 0 );
}

// Method: Bookmark
//   Desc: Set/Return a unique record identifier in the form of a 
//         _variant_t class. If the Bookmark method is called with
//         no arguments, a new variant is created and returned to
//         the caller. The host application needs to save this
//         refernece and delete it when no longer needed. Otherwise,
//         a memory leak will occur. If a variant argument is supplied,
//         Bookmark will attempt to restore the record position as 
//         specified in the variant argument.
//
//   Args: _variant_t*
//           Reference to an existing variant type. This is generally
//           the variant reference that was returned from a previous
//           call to Bookmark
//
// Return: Reference to a newly created instance of a _variant_t class
//         if the method was called with no argument. Otherwise, NULL
//
_variant_t* CAxRecordset::Bookmark(_variant_t* pvtBookmark)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;
  _variant_t *pvtBkMrk = NULL;

  if ( pvtBookmark && pvtBookmark->vt == VT_R8 )
  {
    hr = m_piRecordset->put_Bookmark(*pvtBookmark);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Bookmark"), hr);

  	DoFieldExchange(FALSE);
  }
  else
  {
    if ( AbsolutePosition() > 0 )
    {
      pvtBkMrk  = new _variant_t;
      hr = m_piRecordset->get_Bookmark(pvtBkMrk );
      if FAILED( hr )
      {
        delete pvtBkMrk;
		    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Bookmark"), hr);
      }
    }
  }

  return (pvtBkMrk );
}

// Method: CacheSize
//   Desc: Set/Return the number of records that are kept in
//         local memory. The default value is 1. Can be changed
//         throughout the life of the record set object.
//
//   Args: nCacheSize
//           Specified the number of records to keep in local memory.
//           The value must be 1 or greater.
//
// Return: The current CacheSize setting.
//
long CAxRecordset::CacheSize(long nCacheSize)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( nCacheSize > 0 )
  {
    hr = m_piRecordset->put_CacheSize(nCacheSize);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CacheSize"), hr);
  }

  long cacheSize;
  hr = m_piRecordset->get_CacheSize(&cacheSize);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CacheSize"), hr);

  return (cacheSize);
}

// Method: CursorLocation
//   Desc: Set/Return the location of the cursor service.
//         Read-Write while the record set is closed. Read-only
//         if the record set is open. Default is adUseServer.
//         Host applications will generally want to set this
//         to adUseClient.
//
//   Args: pCursorLocation
//           Reference to a CursorLocationEnum variable.
//            adUseNone	= 1,
//            adUseServer	= 2,
//            adUseClient	= 3,
//            adUseClientBatch	= 3
//
// Return: The current CursorLocationEnum setting.
//
CursorLocationEnum CAxRecordset::CursorLocation(long eCursorLocation)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( eCursorLocation >= adUseNone && eCursorLocation <= adUseClient )
  {
    hr = m_piRecordset->put_CursorLocation((CursorLocationEnum)eCursorLocation);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CursorLocation"), hr);
  }

  CursorLocationEnum curLoc;
  hr = m_piRecordset->get_CursorLocation(&curLoc);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CursorLocation"), hr);

  return (curLoc);
}

// Method: CursorType
//   Desc: Set/Return the type of cursor used for the recordset.
//         Read-Write while the recordset is closed. Read-only
//         if the recordset is open. Default is adOpenForwardOnly.
//
//   Args: pCursorLocation
//           Reference to a CursorTypeEnum variable.
//            adOpenUnspecified	= -1,
//            adOpenForwardOnly	= 0,
//            adOpenKeyset	= 1,
//            adOpenDynamic	= 2,
//            adOpenStatic	= 3
//
// Return: The current CursorTypeEnum setting.
//
CursorTypeEnum CAxRecordset::CursorType(CursorTypeEnum eCursorType)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( eCursorType >= adOpenForwardOnly && eCursorType <= adOpenStatic )
  {
    hr = m_piRecordset->put_CursorType(eCursorType);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CursorType"), hr);
  }

  hr = m_piRecordset->get_CursorType(&m_eCursorType);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::CursorType"), hr);

   
	return (m_eCursorType);
}

// Method: EditMode
//   Desc: Return the editing status for the current record.
//
//   Args: none
//
// Return: The current EditModeEnum setting.
//          adEditNone	= 0,
//          adEditInProgress	= 0x1,
//          adEditAdd	= 0x2,
//          adEditDelete	= 0x4
//
EditModeEnum CAxRecordset::EditMode()
{
	VALID_ADO_OBJECT(m_piRecordset);
	EditModeEnum editMode;

  HRESULT hr = m_piRecordset->get_EditMode(&editMode);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::EditMode"), hr);

	return (editMode);
}

// Method: IsEOF - Proxy for ADO's EOF method
//   Desc: Determine if the current record pointer is immediately
//         after the last record.
//
//   Args: none
//
// Return: Returns true is the record pointer is located after the
//         last record or if the recordset contains no records.
//         Otherwise false.
//
bool CAxRecordset::IsEOF()
{
	VALID_ADO_OBJECT(m_piRecordset);
	VARIANT_BOOL vbEOF = 0;

	if FAILED( m_piRecordset->get_EOF(&vbEOF) )
		return false;

	return ( vbEOF < 0);
}
 
// Method: Filter a set of records from a current recordset.
//   Desc: Sets/Returns the current filter. A variant value
//         is used which may contain a filter string (VT_BSTR)
//         or a predefined FilterGroupEnum (VT_I2). The client
//         application is responsible for determining, setting
//         and extracting variant values. If a filter string
//         is supplied, use a statement similar to that found
//         in an SQL WHERE clause, i.e. _T("AccountID Like 'BA%'")
//         Call method with no argument to get the current setting.
//
//   Args: *pvtFilter a reference to a _variant_t type.
//          If using FilterGroupEnum, values may be:
//            adFilterNone	= 0,
//            adFilterPendingRecords	= 1,
//            adFilterAffectedRecords	= 2,
//            adFilterFetchedRecords	= 3,
//            adFilterPredicate	= 4,
//            adFilterConflictingRecords	= 5
//
// Return:  Current filter string.
//
_variant_t CAxRecordset::Filter(_variant_t *pvtFilter)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( pvtFilter )
  {
    hr = m_piRecordset->put_Filter(*pvtFilter);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Filter"), hr);
  }

  VARIANT vtFilter;

  hr = m_piRecordset->get_Filter(&vtFilter);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Filter"), hr);

  return (vtFilter);
}

//Not supported on VS6
#if _MSC_VER > 1200
LPCTSTR CAxRecordset::Index(LPCTSTR lpszIndex)
{
  VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( Supports(adIndex) )
  {
    hr = m_piRecordset->put_Index(_bstr_t(lpszIndex));
    if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Index"), hr);
  }

  BSTR bstrIndex;
  hr = m_piRecordset->get_Index(&bstrIndex);
  if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Index"), hr);

#ifdef _UNICODE
  return (_bstr_t(bstrIndex).operator const wchar_t *());
#else
  return (_bstr_t(bstrIndex).operator const char*());
#endif
}
#endif

// Method: LockType
//   Desc: Sets/Returns the type of lock on records during edits.
//         Call method with no argument to return current setting.
//         Read-Write when recordset is closed. Read-only when
//         recordset is open.
//   Args: pLockType - Reference to a LockTypeEnum variable.
//          adLockUnspecified	= -1,
//          adLockReadOnly	= 1,
//          adLockPessimistic	= 2,
//          adLockOptimistic	= 3,
//          adLockBatchOptimistic	= 4
//
// Return:  Current LockTypeEnum setting.
//
LockTypeEnum CAxRecordset::LockType(LockTypeEnum eLockType)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( eLockType != adLockUnspecified )
  {
    hr = m_piRecordset->put_LockType(eLockType);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Filter"), hr);
  }

	LockTypeEnum eLT;
  hr = m_piRecordset->get_LockType(&eLT);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Filter"), hr);

  return (eLT);
}

// Method: MarshalOptions
//   Desc: Sets/Returns the option as to which records should be
//         marshaled back to the server.
//
//   Args: *pOption - Reference to a MarshalOptionsEnum variable.
//          adMarshalAll	= 0,
//          adMarshalModifiedOnly	= 1
//
// Return: Current MarshalOptionsEnum value.
//
MarshalOptionsEnum CAxRecordset::MarshalOptions(long eOptions)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( eOptions >= adMarshalAll && eOptions <= adMarshalModifiedOnly )
  {
    hr = m_piRecordset->put_MarshalOptions((MarshalOptionsEnum)eOptions);
	  if FAILED( hr )
		  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MarshalOptions"), hr);
  }

  MarshalOptionsEnum opt;
  hr = m_piRecordset->get_MarshalOptions(&opt);
	if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MarshalOptions"), hr);

  return (opt);
}

// Method: MaxRecords
//   Desc: Sets/Returns the maximum number of records that are
//         returned to a recordset when executing a query.
//         Read-Write when a recordset is closed. Read-only when
//         open. A value of 0 results in all valid records
//         being returned.
//
//   Args: nMaxRecords - Maximum number of records to return.
//
// Return: Current maximum records setting.
//
long CAxRecordset::MaxRecords(long nMaxRecords)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( nMaxRecords >= 0 )
  {
    //Can only set max records if recordset is closed
    if ( !_IsOpen() )
    {
      hr = m_piRecordset->put_MaxRecords(nMaxRecords);
      if FAILED( hr )
		    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MaxRecords"), hr);
    }
  }

  long max;
  hr = m_piRecordset->get_MaxRecords(&max);
  if FAILED( hr )
		ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::MaxRecords"), hr);

  return (max);
}

// Method: PageCount
//   Desc: Returns the number of logical pages in the recordset.
//
//   Args: none
//
// Return: Number of logical pages.
//
long CAxRecordset::PageCount()
{
	VALID_ADO_OBJECT(m_piRecordset);
  long count;

  HRESULT hr = m_piRecordset->get_PageCount(&count);
  if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::PageCount"), hr);
  
  return (count);
}

// Method: PageSize
//   Desc: Sets/Returns the number of records per logical page.
//
//   Args: nPageSize
//          Number of records in a logical page. Must be greater
//          than 0.
//
// Return: Number of records in a page.
//
long CAxRecordset::PageSize(long nPageSize)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( nPageSize > 0 )
  {
    hr = m_piRecordset->put_PageSize(nPageSize);
    if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::PageSize"), hr);
  }

  long pageSize;
  hr = m_piRecordset->get_PageSize(&pageSize);
  if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::PageSize"), hr);

  return (pageSize);
}

// Method: RecordCount
//   Desc: Returns the number of records in the recordset.
//
//   Args: none
//
// Return: Number of records.
//
long CAxRecordset::RecordCount()
{
	VALID_ADO_OBJECT(m_piRecordset);
	long nRecordCount;

  HRESULT hr = m_piRecordset->get_RecordCount(&nRecordCount);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::RecordCount"), hr);

	return (nRecordCount);
}

// Method: Sort
//   Desc: Set/Return a sort on one or more fields in a recordset.
//
//   Args: lpszSort
//          If non-NULL, identifies the name(s) of the fields
//          on which to sort. The syntax is the same as for
//          the SORT clause in an SQL statement. An empty
//          string value removes any existing sort. The
//          CursorLocation property must be set to adUseClient.
//
// Return: The current sort string.
//
LPCTSTR CAxRecordset::Sort(LPCTSTR lpszSort)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( lpszSort )
  {
    hr = m_piRecordset->put_Sort(_bstr_t(lpszSort));
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Sort"), hr);
  }

  BSTR bstrSort;
  hr = m_piRecordset->get_Sort(&bstrSort);
  if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Sort"), hr);

#ifdef _UNICODE
  return (_bstr_t(bstrSort).operator const wchar_t *());
#else
  return (_bstr_t(bstrSort).operator const char*());
#endif
}

// Method: Source
//   Desc: Set/Return the Source property for the recordset.
//
//   Args: lpszSource
//          Sets/Returns the source for the recordset data.
//          Typically the source is specified in the Open
//          method.
//
// Return: The current data source.
//
LPCTSTR CAxRecordset::Source(LPCTSTR lpszSource)
{
	VALID_ADO_OBJECT(m_piRecordset);
  HRESULT hr = S_OK;

  if ( lpszSource  && !_IsOpen() )
  {
    hr = m_piRecordset->put_Source(_bstr_t(lpszSource));
	  if FAILED( hr )
	    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Source"), hr);
  }

  _variant_t vtSource;
  hr =  m_piRecordset->get_Source(&vtSource);
  if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Source"), hr);

#ifdef _UNICODE
  return (_bstr_t(vtSource.bstrVal).operator const wchar_t *());
#else
  return (_bstr_t(vtSource.bstrVal).operator const char *());
#endif
}

ObjectStateEnum CAxRecordset::State()
{
	VALID_ADO_OBJECT(m_piRecordset);
	long lState;

  HRESULT hr = m_piRecordset->get_State(&lState);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::State"), hr);

	return ((ObjectStateEnum)lState);
}

RecordStatusEnum CAxRecordset::Status()
{
	VALID_ADO_OBJECT(m_piRecordset);
	long lStatus;

  HRESULT hr = m_piRecordset->get_Status(&lStatus);
	if FAILED( hr )
	  ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::Status"), hr);

	return ((RecordStatusEnum)lStatus);
}


long CAxRecordset::FieldCount()
{
	VALID_ADO_OBJECT(m_piRecordset);
	long lFieldCount;

	if FAILED( m_pFields->get_Count(&lFieldCount) )
		ThrowAxException(AXLIB_ERROR_NONE);

	return lFieldCount;
}




//----------------------------------------------------------
// Bookmark


//----------------------------------------------------------
// Utility


//----------------------------------------------------------
// Data Exchange
HRESULT CAxRecordset::_getADOField(LPCTSTR lpszColumn, ADOField** ppFld)
{
	VALID_ADO_OBJECT(m_piRecordset);
	ASSERT( m_pFields );

  return ( m_pFields->get_Item(_variant_t(lpszColumn), ppFld) );
}

HRESULT CAxRecordset::_isUpdatable(ADOField* pFld)
{
  long lAttrib = 0;
  HRESULT hr = pFld->get_Attributes(&lAttrib);

  if ( FAILED(hr) )
    return ( hr );

	//If both the adFldUpdatable (4) and adFldUnknownUpdatable (8)
	//flags are false, as is the case with an Identity field, then
	//we need to avoid an attempt to write data back to the field.
  //Returning the E_ACCESSDENIED value which, in this instance,
  //should not be considered an actual error needing to be handled.
	if ( !(lAttrib & adFldUpdatable) && !(lAttrib & adFldUnknownUpdatable) )
    hr = ERROR_ACCESS_DENIED;

  return ( hr );
}


_variant_t* CAxRecordset::_GetFieldValue(LPCTSTR lpszColumn)
{
  m_hr = _setGetFieldValue(false, lpszColumn, VT_VARIANT, NULL);

  if FAILED(m_hr)
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::_GetFieldValue"), m_hr);

  return (&m_vtData);
}

_variant_t* CAxRecordset::_GetFieldValue(long nColumn)
{
	VALID_ADO_OBJECT(m_piRecordset);
	ASSERT( m_pFields );
  ADOField* pFld = NULL;

  m_hr = m_pFields->get_Item(_variant_t(nColumn), &pFld);
  if ( FAILED(m_hr) )
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::_GetFieldValue"), m_hr);

  m_vtData.Clear();
	m_vtData.vt = VT_ERROR;
	m_vtData.scode = DISP_E_PARAMNOTFOUND;

	m_hr = pFld->get_Value(&m_vtData);
  if ( FAILED(m_hr) )
  {
    m_vtData.Clear();
    ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::_GetFieldValue"), m_hr);
  }

  return (&m_vtData);
}

//------------------------------------------------------------------------
// Removed steps to set assumed values if the attempt to get a field
// value should fail. See revision notes under  v1.1.1 in AxLib.h.
HRESULT CAxRecordset::_getFldValue(ADOField* pFld, unsigned short varType, void** ppValue)
{
  HRESULT hr = S_OK;

  //If the table is empty or the field is empty
  //the variant's type is set to VT_NULL
  if ( _IsEmpty() )
  {
    m_vtData.vt = VT_NULL;
  }
  else
  {
    m_vtData.vt = VT_ERROR;
    m_vtData.scode = DISP_E_PARAMNOTFOUND;
    hr = pFld->get_Value(&m_vtData);
  }

  if ( *ppValue == NULL || (m_vtData.vt == VT_NULL) )
    return ( hr );

  if ( FAILED(hr) )
    return ( hr );

  switch ( varType )
  {
  case VT_I2 :
    (*(short*)*ppValue) = (short)m_vtData.iVal;
 //   (*(short*)*ppValue) = (m_vtData.vt == VT_NULL) ? 0 :
 //     (short)m_vtData.iVal;
    break;

  case VT_I4 :
      (*(int*)*ppValue) = (int)m_vtData.lVal;
 //   (*(int*)*ppValue) = (m_vtData.vt == VT_NULL) ? 0 :
 //     (int)m_vtData.lVal;
    break;

  case VT_I8 :
    (*(LONGLONG*)*ppValue) = (LONGLONG)(m_vtData.operator DECIMAL()).Lo64;
//    (*(LONGLONG*)*ppValue) = (m_vtData.vt == VT_NULL) ? 0 :
//      (LONGLONG)(m_vtData.operator DECIMAL()).Lo64;
    break;

  case VT_R4 : 
    (*(float*)*ppValue) = (float)m_vtData.fltVal;
//    (*(float*)*ppValue) = (m_vtData.vt == VT_NULL) ? 0L : 
//      (float)m_vtData.fltVal;
    break;

  case VT_R8 :
    (*(double*)*ppValue) = m_vtData.dblVal;
//    (*(double*)*ppValue) = (m_vtData.vt == VT_NULL) ? 0L :
//      m_vtData.dblVal;
    break;

  case VT_BOOL :
    (*(bool*)*ppValue) = (bool)(m_vtData.boolVal == -1);
 //   (*(bool*)*ppValue) = (m_vtData.vt == VT_NULL) ? false :
 //     (bool)(m_vtData.boolVal == -1);
    break;

  case VT_VARIANT :
    (*(_variant_t*)*ppValue) = m_vtData;
    break;

  case VT_BSTR :
//    if ( m_vtData.vt == VT_NULL )
//      ((CString*)*ppValue)->Empty();
//    else
      ((CString*)*ppValue)->operator =((LPCWSTR)m_vtData.bstrVal);
    break;

  case VT_CY :
//    if ( m_vtData.vt == VT_NULL )
//      (*(COleCurrency*)*ppValue).m_cur.int64 = 0L;
//    else
      (*(COleCurrency*)*ppValue) = m_vtData.cyVal;
    break;

  case VT_DATE :
//    if ( m_vtData.vt == VT_NULL )
//      (*(COleDateTime*)*ppValue) = COleDateTime();
//    else
      ((COleDateTime*)*ppValue)->operator =(m_vtData);
    break;

  case VT_DECIMAL :
//    if ( m_vtData.vt == VT_NULL )
//      (*(double*)*ppValue) = 0L;
 //   else
      hr = VarR8FromDec(&m_vtData.decVal, (double*)*ppValue);
    break;

  case VT_UI1 :
    (*(BYTE*)*ppValue) = m_vtData.bVal;
    break;

  case VT_ARRAY | VT_UI1 :
    //Place holder for Image data type to avoid default action
    break;

  default :
    m_vtData.Clear();
    break;
  }

  return ( hr );
}


HRESULT CAxRecordset::_setFldValue(ADOField* pFld, unsigned short varType, void** ppValue)
{
  HRESULT hr = S_OK;

  m_vtData.vt = varType;

  switch ( varType )
  {
  case VT_I2 :
    m_vtData.iVal = *(short*)*ppValue;
    break;

  case VT_I4 :
    m_vtData.lVal = *(long*)*ppValue;
    break;

//VT_I8 not supported under VS6
  case VT_I8 :
#if _MSC_VER > 1200
    m_vtData.llVal = *(LONGLONG*)*ppValue;
#else
    m_vtData.cyVal.int64 = (*(LONGLONG*)*ppValue);
#endif
    break;

  case VT_R4 :
    m_vtData.fltVal = *(float*)*ppValue;
    break;

  case VT_R8 :
    m_vtData.dblVal = *(double*)*ppValue;
    break;

  case VT_BOOL :
    m_vtData.boolVal = (*(bool*)*ppValue) ? VARIANT_TRUE : VARIANT_FALSE;
    break;

  case VT_VARIANT :
    m_vtData = *(VARIANT*)*ppValue;
    break;

  case VT_BSTR :
    m_vtData.bstrVal = ((CString*)*ppValue)->AllocSysString();
    break;

  case VT_CY :
    m_vtData.cyVal = *(CURRENCY*)*ppValue;
    break;

  case VT_DATE :
    hr = VarDateFromR8((*(DATE*)*ppValue), &m_vtData.date);
    break;

  case VT_DECIMAL :
    hr = VarDecFromR8((*(double*)*ppValue), &m_vtData.decVal);
    break;

  case VT_UI1 :
    m_vtData.bVal = *(BYTE*)*ppValue;
    break;

  case VT_ARRAY | VT_UI1 :
    //Place holder for Image data type to avoid default action
    break;

  default :
    m_vtData.Clear();
    break;
  }

  if ( SUCCEEDED(hr) )
      hr = pFld->put_Value(m_vtData);

  return ( hr );
}

HRESULT CAxRecordset::_setGetFieldValue(bool bSave, LPCTSTR lpszColumn, 
                                        unsigned short varType, void* pvValue)
{
  ADOField* pADOFld = NULL;
  m_hr = S_OK;

  //If we get an error here, assume the query
  //that resulted in the returned recordset
  //simply did not specify the field in question
  //rather than a bad field definition. The
  //ERROR_INVALID_ACCESS result is returned which
  //is not considered a fatal error.
  m_hr = _getADOField(lpszColumn, &pADOFld);
  if FAILED(m_hr)
    return ( (HRESULT)ERROR_INVALID_ACCESS );

  m_hr = _isUpdatable(pADOFld);
  if FAILED(m_hr)
    return ( m_hr );

  if ( bSave && (m_hr != ERROR_ACCESS_DENIED) )
    m_hr = _setFldValue(pADOFld, varType, &pvValue);
  else
    m_hr = _getFldValue(pADOFld, varType, &pvValue);

  return ( m_hr );
}


//Max size is 8k
void CAxRecordset::FX_Binary(bool bSave, LPCTSTR lpszColumn, BYTE** ppValue)
{
  if ( bSave && *ppValue != NULL )
  { 
    BINDATAINFOHEADER binHeader;
    memcpy((void*)&binHeader, (const void*)*ppValue, sizeof(BINDATAINFOHEADER));
    if ( binHeader.imgSize > 8192 )
		  ThrowAxException(AXLIB_ERROR_BUF_SIZE);
  }
  
  FX_Image(bSave, lpszColumn, ppValue);
}


//Max size is 2^31 - 1 (2,147,483,647) bytes
void CAxRecordset::FX_Image(bool bSave, LPCTSTR lpszColumn, BYTE** ppValue)
{
	HRESULT hr = S_OK;
	BYTE HUGEP *pData = NULL;
  m_vtData.Clear();

  if ( bSave )
  {
    if ( *ppValue == NULL )
      return;

    //Read the header information
 	  BINDATAINFOHEADER binHeader;
    BYTE* pBuf = *ppValue;

	  memcpy((void*)&binHeader, (const void*)pBuf, sizeof(BINDATAINFOHEADER));
    long lBufSize = sizeof(BINDATAINFOHEADER) + binHeader.imgSize;
   
		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].lLbound = 0;
		rgsabound[0].cElements = lBufSize;

    m_vtData.vt = VT_ARRAY | VT_UI1;
    m_vtData.parray = SafeArrayCreate(VT_UI1, 1, rgsabound);

		if ( m_vtData.parray != NULL )
		{
			hr = SafeArrayAccessData(m_vtData.parray, (void HUGEP* FAR*)&pData);
			if ( SUCCEEDED(hr) )
			{
				memcpy((void*)pData, (const void*)pBuf, lBufSize);
				hr = SafeArrayUnaccessData(m_vtData.parray);
  
        hr = _setGetFieldValue(bSave, lpszColumn, VT_ARRAY | VT_UI1, NULL);

        if ( FAILED(m_hr) )
          ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Image"), hr);

        //Clear the variant to free memory used by the parray member
        m_vtData.Clear();
			}
		}
  }
  else
  {
   if ( *ppValue != NULL )
    {
      delete [] *ppValue;
      *ppValue = NULL;
    }
    hr = _setGetFieldValue(bSave, lpszColumn, VT_ARRAY | VT_UI1, NULL);

    if ( FAILED(hr) )
      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Image"), hr);

		if ( m_vtData.vt == VT_NULL )
			return;

    long lUBound;
		hr = SafeArrayGetUBound(m_vtData.parray, 1, &lUBound);
		if ( SUCCEEDED(hr) )
		{
			hr = SafeArrayAccessData(m_vtData.parray, (void HUGEP* FAR*)&pData);
			if ( SUCCEEDED(hr) )
			{
				long lByteCount = lUBound + 1;	//UBound is 0's based
				*ppValue = new BYTE[lByteCount];
				ZeroMemory((void*)*ppValue, lByteCount);

				memcpy((void*)*ppValue, (const void*)pData, lByteCount);

				SafeArrayUnaccessData(m_vtData.parray);
			}
		}
  }

}


//Max size is 8K
void CAxRecordset::FX_VarBinary(bool bSave, LPCTSTR lpszColumn, BYTE** ppValue)
{
  if ( bSave && *ppValue != NULL )
  { 
    BINDATAINFOHEADER binHeader;
    memcpy((void*)&binHeader, (const void*)*ppValue, sizeof(BINDATAINFOHEADER));
    if ( binHeader.imgSize > 8192 )
		  ThrowAxException(AXLIB_ERROR_BUF_SIZE);
  }
  
  FX_Image(bSave, lpszColumn, ppValue);
}

