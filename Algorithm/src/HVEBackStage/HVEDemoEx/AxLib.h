/*------------------------------------------------------------------------ 
 dbAx class library - version 1.0.0
 Copyright (c) 2007-2008 Data Management Systems. 

 Description
 -----------
 The dxAx library consists of a set of C++ classes that wrap the
 Microsoft ActiveX Data Objects (ADO) interface allowing C++ 
 application developers to access databases in a native C++ environment.
 The objective of this library is to relieve the developer from the
 details of COM interfaces and having to translate VARIANT data types
 into common C++ data types.

 The dbAx library does not rely on MFC with the exception of the 
 CString and COleDateTime classes. Since the majority Windows
 applications are expected to utilize MFC, this is not seen as a
 real drawback. It is possibile, though, to develop non-MFC
 applications using this library since CString and COleDateTime are
 stand-alone and do not rely on any base class from the MFC library.

 Disclaimer
 ----------
 This software is provided 'AS IS', WITHOUT ANY WARRANTY, express
 or implied, as to MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 In no event shall the author be held liable for any damages arising
 from the use of this software.

 Any conflict with outside or third party entities regarding the naming
 convention used in this software, specifically 'dbAx', 'AxLib' or the
 prefix 'CAx' is unintended and purly coincidental.

 Data Management System reserves the right to modify and update this
 code without notice.

 License
 -------
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications. You may alter this software to
 suite a particular purpose and redistribute it subject to the following
 conditions:

  1. You must not misrepresent the origin of this work claiming that
     you wrote the original software. Using this software in a project
     or product does not require that you acknowledge its use, but
     reference to it is certainly appreciated.

  2. If you alter this version of the software, it must be plainly marked
     as such and not misrepresented as being the original work.

  3. This notice and disclaimer may not be removed or altered from any
     source distribution of the product.

  Revision History
  ----------------
  v1.0.0  - 12/21/07
    Initial release.

  v1.0.1  - 2/10/2008
    Corrected fuction calls mixing 'BOOL' and 'bool' data types. VS2005
    produces errors since 'bool' is now an intrinsic data type. Also
    corrected errors in AxGen producing erroneous code.

  v1.1.0  - 2/25/08
    Numerious updates...
    
    .Rewrote a lot of code to avoid potentially calling invalid or
     NULL pointers.

    .Moved the 'Release' of underlying COM objects into the Destructor of
     class objects.

    .No longer necessary to call Create() prior to opening a CAxRecordset
     object provided no calls are made to methods affecting object settings
     or parameters prior to calling Open(). This is especially useful where
     temporary or ad-hoc recordsets are opened, closed and reopened using a
     new SQL query statement generated on the fly.

    .Revisited the FX_xxx functions to take into account the ADO flags
     adFldUpdatable and adFldUnknownUpdatable (see comment on 
     _isUpdatable()). Replaced the FX_Provider and SetVariant functions 
     with:
      
      HRESULT ::_setGetFieldValue(bool bSave, LPCTSTR lpszColumn,
                                  unsigned short varType, void* pvValue);

     This uses the helper function _getFldValue() or _setFldValue depending
     on the bSave flag. More testing done on all SQL data types to ensure
     correct translation. Client code, however, can still use the FX_xxx
     functions as always with the exception of FX_Money (see following
     comment).
    
    .Changed the data types for FX_Money and FX_SmallMoney to COleCurrency. 
     This MFC class encapsulates the CURRENCY struct and is better suited
     to handling monetary values as opposed to a double. Previous client
     code will need to be updated to handle the new data type.

    .Corrected the situation where an error was thrown if a match was not
     found for an ADOField specified in the DoFieldExchange method of a
     CAxRecordset object. This can occur if the derived class tags most or
     all of the fields in a table, however, a CAxCommand object
     (i.e. stored procedure) is used to return only a subset of the fields.
     The library generates the error code ERROR_INVALID_ACCESS, but does
     not consider it fatal.

    .Updated the AxGen utility to support the changes made in this version.
     When creating a user specified class for a CAxCommand object, AxGen
     matches the variant types in the derived class' _CreateParameters()
     method. AxGen substitutes '\' with '\\' when building a connection
     string so the C++ compiler recognizes the single delimiter.

    .Updated documentation as needed.

  v1.2.0  - 12/25/08
    .Added call to CursorLocation in the Open methods of CAxRecordset. This
     performs bounds checking and sets the CursorLocation for the Recordset
     based on the eCursorLocation parameter (per Dan via CodeProject).

    .Removed assumed values in the CAxRecordset::_getFldValue method. If the
     method failed, an assumed valued was used (i.e. 0). No assumption should
     be made, therefore the client application should initialize a recordset
     field variable before making a call to _getFldValue, directly or through
     one of the CAxRecordset::FX_{xxx} methods.

    .Moved call to CAxCommand::_CreateParameters to CAxCommand::Create(). 
     Deleted call to _CreateParameters in the CAxRecordset::Open method.
     This was necessary in the event that a CAxCommand object is used in a
     manner other than opening a recordset. For example, using a CAxCommand
     object to simply execute a stored procedure that does not involve
     returning table records (i.e. INSERT)

    .Added code to the CAxCommand::Execute method. A CAxCommand object can
     now be used to execute a stored procedure that may not involve a recordset.
     A typical case is accessing a stored procedure that does not involve a 
     SQL SELECT (i.e. INSERT)

    .Changed CAxRecordset::FX_TinyInt to use VT_UI1 (BYTE) rather than VT_I2
     (short). 

  How to use this library
  -----------------------
  1. Add the AxLib source files to your project. This includes:
      AxLib.h
      AxConnection.cpp
      AxCommand.cpp
      AxRecordset.cpp
      AxException.cpp

	2. Add "#include <AxLib.h> in the project's stdafx.h file.

  3. AxLib provides the GenGUIDStr function to support the generation
     of a unique GUID character string. A GUID string can be used with
     SQL Server's UniqueIdentifier field. If you need this functionality,
     un-remark the "#define GEN_GUID_STR" statement below. You will also 
     need to add a reference to Rpcrt4.lib in the project settings. This
     is found under:

         Project Settings/Linker Options/Input/Additional Dependencies

  4. Create an instance of the CAxConnection object.

  5. Create instance(s) of custom derived version of CAxCommand and CAxRecordset.

  6. Initialize the library at program startup using dbAx::Init()

  7. Open connection, command, and recordset objects as needed.

  8. At program terminstion, call the dbAx::Term() function to properly
     shut down the library.

  For a complete discussion on using the AxLib library, see the 
  accompanying documentation in the compiled help file - dbAx.chm
------------------------------------------------------------------------*/

#ifndef _AXLIB_H_
#define _AXLIB_H_

#include <icrsint.h>	  // ADO class extensions
#include <comdef.h>		  // COM support
#include <crtdbg.h>     // Debug _ASSERT support

//Redefine GUID macro to eliminate EXTERN_C
//ADO GUID interface delcaration (contained in adoid.h) are defined here, otherwise
//#else directive defines them as "extern "C" declarations
//NOTE: DEFINE_GUID macro in objbase.h includes the EXTERN_C statement which 
//      causes the compiler to choke on macro expansion during Link 
//      operations. Why??
#ifdef DEFINE_GUID
	#undef DEFINE_GUID
#endif

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        const GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

// ******************************************************************************
//Un-Remark this define if you intended to use the
//global GenGUIDStr function. If so, you will need to
//add a reference to Rpcrt4.lib to the project settings.
//See note 3 in the "How to use this Library" section
//above.
//#define GEN_GUID_STR
// ******************************************************************************

#if _MSC_VER == 1200
  #include <afx.h>        // Support for CString VC6
  #include <afxdisp.h>    // Support for COleDateTime VC6
#else
  #include <atlstr.h>     // Support for CString
  #include <ATLComTime.h> // Support for COleDateTime
#endif

#include <adoid.h>      // ADO GUID's
#include <adoint.h>		  // ADO C++ header

#include <math.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <tchar.h>			// Unicode
#include <vector>       // STL vector

using namespace std;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
namespace dbAx {
  //AxLib errors.
  //Note: most error conditions use the AXLIB_ERROR_NONE enum and pull
  //      error information from the ADO COM interface.
  enum AxLibErrorEnums
  {
    AXLIB_ERROR_NONE = 0,
    AXLIB_ERROR_INIT = 5001,
    AXLIB_ERROR_DXBIND = 5002,
    AXLIB_ERROR_OBJECT_NOTOPEN = 5003,
    AXLIB_ERROR_BUF_SIZE = 5004,
    AXLIB_ERROR_NULL_PTR = 5005,
    AXLIB_ERROR_ENUM = 5006,
    AXLIB_ERROR_INVALID_POS = 5007,
  };

   //Header information prepended to binay data
   //field (i.e. image)
   typedef struct tagBINDATAINFOHEADER
    {
	    long		imgSize;	//uncompressed size
	    long		cmpSize;	//compressed size
	    long		lReserved1;	//Future
	    long		lReserved2; //Future
    } BINDATAINFOHEADER;


  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Classes defined in this header file

    class CAxConnection;
    class CAxCommand;
    class CAxRecordset;
    class CAxException;
    class CAxConnectionEvents;
    class CAxRecordsetEvents;
 
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // Global namespace definitions and functions
  // AxLib Exception handling
  void ThrowAxException(int nAxError = AXLIB_ERROR_NONE, 
                         LPCTSTR lpszMsg = NULL,
                         HRESULT hr = S_OK);

  //Connection objects collection
  typedef std::vector<CAxConnection*> _AxConnectionsT;
  extern _AxConnectionsT m_AxConnections;

  //Initialization and Termination
  void Init();                                    //Initialize the library
  void Term();                                    //Terminate and cleanup
  void DropConnection(CAxConnection *pCn);

  //Utility functions
  const wchar_t*   M2W(LPCSTR lpszCharStr);       //char -> wchar
  const char*      W2M(LPCWSTR lpszWideCharStr);  //wchar -> char

#ifdef GEN_GUID_STR
  //The GenGUIDStr function will generate a string
  //version of a GUID that can be used with the SQL
  //Server UniqueIdentifier field.
  HRESULT          GenGUIDStr(CString& szGUID);
#endif


//Debug assert on ADO objects pointers
#ifdef _DEBUG
#define VALID_ADO_OBJECT(a) if(a == NULL) ThrowAxException(AXLIB_ERROR_NULL_PTR)
#else
#define VALID_ADO_OBJECT(a) 
#endif

  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  // CAxConnection class
  // Desc: Open and maintain a connection to an OLEDB
  //       Data Provider. AxCommand and AxRecordset
  //       objects use an existing AxConnection object.
  class CAxConnection
  {
    friend class CAxException;
    friend class CAxRecordset;

    //Construction
    public:
    CAxConnection();
    CAxConnection(LPCTSTR lpszConnectStr);
    virtual ~CAxConnection();

    //Attributes
    public:

    protected:
    //CAxRecordset objects collection associated with
    //this connection object
    ADOConnection* m_piConnection;

    DWORD          m_dwEvents;               //Connection events cookie
    CString 			 m_strConnect;             //OLEDB Provider connection string

    //Implementation
    public:
    void           Create();
    void           _SetConnectionEvents(CAxConnectionEvents* pEvents);
    void           _ClearConnectionEvents();
	  bool           _IsOpen();
    bool           _SupportsTransactions();
	  ADOConnection* _GetActiveConnection() { return m_piConnection; } 

    //Collections
    ADOErrors*     Errors();
    ADOProperties* Properties();

    //Methods
	  void           BeginTrans(long *plTransLevel);
	  void	         Cancel();
    void           Close();
	  void	         CommitTrans();
	  void           Execute(LPCTSTR lpszCmdText, long* lpRecsAffected = NULL, 
                        CommandTypeEnum lOptions = adCmdUnknown);
	  void           Open(LPCTSTR lpszConnectStr = NULL,
                        LPCTSTR lpszUserID = NULL,
                        LPCTSTR lpszPwd = NULL,
                        ConnectOptionEnum eConnnect = adConnectUnspecified);
	  void	         RollbackTrans();

    //Properties
	  XactAttributeEnum  
                   Attributes(long lAttribute = -1);
	  long           CommandTimeout(long lSeconds = -1);
	  LPCTSTR        ConnectionString(LPCTSTR lpszConnectStr = NULL);
	  long           ConnectionTimeout(long lSeconds = -1);
	  CursorLocationEnum 
                   CursorLocation(long lCursorLocation = -1);
	  LPCTSTR        DefaultDatabase(LPCTSTR lpszDatabase = NULL);
	  IsolationLevelEnum           
                   IsolationLevel(IsolationLevelEnum eIsoLevel = adXactUnspecified);
	  ConnectModeEnum
                   Mode(long lConMode = -1);
	  LPCTSTR        Provider(LPCTSTR lpszProvider = NULL);
	  ObjectStateEnum
                   State();
	  LPCTSTR	       Version();

  };  // CAxConnection




  /////////////////////////////////////////////////////////////////////////////
  // CAxCommand class
  // Desc: Provide and interface to the stored procedures of a data provider.
  //       A derrived CAxCommand object is initialized with parameters and
  //       may be used as an argument to open a CAxRecordset object.
  class CAxCommand
  {
    friend class CAxRecordset;

  public:
    CAxCommand();
    virtual ~CAxCommand();

    //Attributes
    ADOCommand*     m_piCommand;
    ADOParameters*  m_piParameters;
    CAxConnection*  m_pConnection;

    //Implementation
    void            Create();

    //Collections
    ADOParameters*  Parameters();
    ADOProperties*  Properties();

    //Methods
    void            Cancel();
    ADOParameter*   CreateParameter(LPCTSTR lpszParam, DataTypeEnum eDataType,
                      ParameterDirectionEnum eParameterDirection, long lSize, 
                      _variant_t *pValue);
    ADORecordset*   Execute(long* plAffected = NULL);

    //Properties
    CAxConnection*  ActiveConnection(CAxConnection* pCn = NULL);
	  LPCTSTR         CommandText(LPCTSTR lpszCmdText = NULL);
	  long            CommandTimeout(long lSeconds = -1);
	  CommandTypeEnum CommandType(CommandTypeEnum eCmdType = adCmdUnspecified);
//	  void            Prepared(BOOL bPrepare = FALSE);
	  ObjectStateEnum State();

  protected:
	  virtual void    _CreateParameters() { /* Override in derrived class */  }
	  virtual void    _UpdateParameters() { /* Override in derrived class */  }
    void            _SetParamValue(LPCTSTR lpszParam, _variant_t* pValue);

  };

  /////////////////////////////////////////////////////////////////////////////
  // CAxRecordset class
  // Desc:
  //
  class CAxRecordset
  {
  //Construction
  public:
	  CAxRecordset();
	  virtual ~CAxRecordset();

  //Attributes
  public:

  protected:
    //dbAx objects
    CAxConnection*   m_pConnection;
    CAxCommand*      m_pCommand;
    CursorTypeEnum   m_eCursorType;
    LockTypeEnum     m_eLockType;

    //ADO COM objects
	  ADORecordset*		 m_piRecordset;
	  ADOConnection*	 m_piConnection;
    DWORD            m_dwEvents;                    //Recordset events cookie

	  CString				   m_strSourceString;
	  ADOFields*			 m_pFields;
	  CursorTypeEnum   m_CursorType;
	  LockTypeEnum		 m_LockType;
	  long				     m_lOptions;
	  _variant_t			 m_varLastGoodRecord;
	  _variant_t			 m_vtData;
//    ADOField*        m_pField;

    HRESULT          m_hr;

    //Implementation
    public:
    HRESULT          Create();

    //AxLib specific
    void             _ClearRecordsetEvents();
 	  bool	           _IsEmpty();
	  bool             _IsOpen();
    virtual void     _SetDefaultValues() { /*Virtual function*/ }
    void             _SetRecordsetEvents(CAxRecordsetEvents* pEvents);
    _variant_t*      _GetFieldValue(LPCTSTR lpszColumn);
    _variant_t*      _GetFieldValue(long nColumn);

    HRESULT          _GetFieldValue(LPCTSTR lpszColumn, _variant_t* pvtData);

    //Collections
    ADOFields*       Fields();
    ADOProperties*   Properties();

    //Methods
    void             AddNew();
    void             Cancel();
    void             CancelBatch();
    void             CancelUpdate();
    ADORecordset*    Clone(LockTypeEnum eLockType = adLockUnspecified);
    void             Close();
    CompareEnum      CompareBookmarks(_variant_t vtBkMark1, _variant_t vtBkMark2);
    void             Delete(AffectEnum eAffect = adAffectCurrent);
	  bool	           Find(LPCTSTR lpszCriteria, long nSkip = 0,
		                    SearchDirectionEnum SearchDirection = adSearchForward,
				                _variant_t* pvtStart = NULL);
    void             GetRows();
    void             GetString();
	  void	           Move(long nNumRecs, _variant_t* pvtStartRec = NULL);
	  void	           MoveFirst() ;
	  void	           MoveLast();
	  void	           MoveNext();
	  void	           MovePrevious();
    void             NextRecordset();

	  void	           Open(CAxCommand* pCmd,
				                CursorTypeEnum eCursorType = adOpenForwardOnly,
				                CursorLocationEnum eCursorLocation = adUseClient,
				                LockTypeEnum eLockType = adLockOptimistic, 
				                long nOptions = adCmdUnspecified);

	  void 	           Open(LPCTSTR szSource, CAxConnection *pConnection,
				                CursorTypeEnum eCursorType = adOpenForwardOnly,
				                CursorLocationEnum eCursorLocation = adUseClient,
				                LockTypeEnum eLockType = adLockOptimistic, 
				                long nOptions = adCmdUnspecified);

    void           Requery(ExecuteOptionEnum eExecuteOption = adOptionUnspecified);
    void           Resync(AffectEnum eAffectRecords = adAffectAll,
                          ResyncEnum eResyncValues = adResyncAllValues);

  void             Save(LPCTSTR lpszFileName, 
                        PersistFormatEnum ePersistFormat = adPersistADTG);
  bool             Supports(CursorOptionEnum eCursorOption);
  void             Update();

    //Properties
  long             AbsolutePage(long nPageNum = 0);
	long	           AbsolutePosition(long nPosition = 0);
  CAxCommand*      ActiveCommand();
  CAxConnection*   ActiveConnection();
	bool             IsBOF();
  _variant_t*      Bookmark(_variant_t* pvtBookmark = NULL);
	long             CacheSize(long nCacheSize = 0);
	CursorLocationEnum
                   CursorLocation(long eCursorLocation = -1);
	CursorTypeEnum   CursorType(CursorTypeEnum eCursorType = adOpenUnspecified);
	EditModeEnum     EditMode();
  bool             IsEOF();
  _variant_t       Filter(_variant_t* pvtFilter = NULL);
  LPCTSTR          Index(LPCTSTR lpszIndex = NULL);
	LockTypeEnum     LockType(LockTypeEnum eLockType = adLockUnspecified);
  MarshalOptionsEnum
                   MarshalOptions(long eOptions = -1);
  long             MaxRecords(long nMaxRecords = -1);
  long             PageCount();
  long             PageSize(long nPageSize = 0);
	long             RecordCount();
	LPCTSTR          Sort(LPCTSTR lpszSort = NULL);
	LPCTSTR          Source(LPCTSTR lpszSource = NULL);
	ObjectStateEnum  State();
	RecordStatusEnum Status();

  ADOFields*       GetFields() { return (m_pFields); }
	long	           FieldCount();

  
protected:
  HRESULT          _getADOField(LPCTSTR lpszColumn, ADOField** ppFld);
  HRESULT          _isUpdatable(ADOField* pFld);
  HRESULT          _getFldValue(ADOField* pFld, unsigned short varType, void** ppValue);
  HRESULT          _setFldValue(ADOField* pFld, unsigned short varType, void** ppValue);
  HRESULT          _setGetFieldValue(bool bSave, LPCTSTR lpszColumn, 
                                       unsigned short varType, void* pvValue);
public:
  void             FX_BigInt(bool bSave, LPCTSTR lpszColumn, LONGLONG& llValue )
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_I8, (void*)&llValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_BigInt"), m_hr); }

	void	           FX_Binary(bool bSave, LPCTSTR lpszColumn, BYTE** ppValue);

	void             FX_Bool(bool bSave, LPCTSTR lpszColumn, bool& bValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BOOL, (void*)&bValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Bool"), m_hr); }

  void             FX_Char(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Char"), m_hr); }

	void	           FX_DateTime(bool bSave, LPCTSTR lpszColumn, COleDateTime& dtValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_DATE, (void*)&dtValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_DateTime"), m_hr); }

  void             FX_Decimal(bool bSave, LPCTSTR lpszColumn, double& dblValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_DECIMAL, (void*)&dblValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Decimal"), m_hr); }

	void             FX_Float(bool bSave, LPCTSTR lpszColumn, double& dblValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_R8, (void*)&dblValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Float"), m_hr); }

	void             FX_Image(bool bSave, LPCTSTR lpszColumn, BYTE** ppValue);

	void             FX_Integer(bool bSave, LPCTSTR lpszColumn, int& iValue)
                   {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_I4, (void*)&iValue))
                     ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Integer"), m_hr); }

  void             FX_Money(bool bSave, LPCTSTR lpszColumn, COleCurrency& cyValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_CY, (void*)&cyValue))
                      ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Money"), m_hr); }

  void             FX_NChar(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_NChar"), m_hr); }

  void             FX_NText(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                   {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                     ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_NText"), m_hr); }

  void             FX_Numeric(bool bSave, LPCTSTR lpszColumn, double& dblValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_DECIMAL, (void*)&dblValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Numeric"), m_hr); }

  void             FX_NVarChar(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_NVarChar"), m_hr); }

  void             FX_Real(bool bSave, LPCTSTR lpszColumn, float& fltValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_R4, (void*)&fltValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Real"), m_hr); }

	void	           FX_SmallDateTime(bool bSave, LPCTSTR lpszColumn, COleDateTime& dtValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_DATE, (void*)&dtValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_SmallDateTime"), m_hr); }

  void             FX_SmallInt(bool bSave, LPCTSTR lpszColumn, short& sValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_I2, (void*)&sValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_SmallInt"), m_hr); }

  void             FX_SmallMoney(bool bSave, LPCTSTR lpszColumn, COleCurrency& cyValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_CY, (void*)&cyValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_SmallMoney"), m_hr); }

  void             FX_SQLVariant(bool bSave, LPCTSTR lpszColumn, _variant_t& vtValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_VARIANT, (void*)&vtValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_SQLVariant"), m_hr); }

  void             FX_Text(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_Text"), m_hr); }

  void             FX_TinyInt(bool bSave, LPCTSTR lpszColumn, BYTE& chValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_UI1, (void*)&chValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_TinyInt"), m_hr); }

	void             FX_UniqueIdentifier(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_UniqueIdentifier"), m_hr); }

  void	           FX_VarBinary(bool bSave, LPCTSTR lpszColumn, BYTE** ppValue);

  void             FX_VarChar(bool bSave, LPCTSTR lpszColumn, CString& szValue)
                     {if FAILED(_setGetFieldValue(bSave, lpszColumn, VT_BSTR, (void*)&szValue))
                       ThrowAxException(AXLIB_ERROR_NONE, _T("CAxRecordset::FX_VarChar"), m_hr); }
  
protected:
  virtual void     DoFieldExchange(bool bSave) { }

  }; // class CAxRecordset


/////////////////////////////////////////////////////////////////////////////
// CAxException class
// Desc:
//
  class CAxException //: public CException
  {
    //Construction
    public:
	  CAxException();
	  ~CAxException();

    //Attributes
    public:
	  long		       m_nErrorNo;
	  CString 	     m_szErrorDesc;

    protected:
	  ADOErrors	    *m_pErrors;
	  ADOError	    *m_pError;
	  long		       m_nErrorCount;

	  int		         m_nAxLibError;
	  SCODE	         m_scode;
	  static         LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
    static         unexpected_handler  m_prevTerm;

    //Implementation
    public:
	  void           GetErrorsCollection();
	  void           GetErrorInfo();
	  void           SetError(int nError) { m_nAxLibError = nError; }
	  void           SetAScode(SCODE sc) { m_scode = sc; }
    SCODE          GetAScode() { return (m_scode); }
    static void    UnhandledException();

    protected:
	  static LONG WINAPI UnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);
	  static LPTSTR  GetExceptionString(DWORD dwCode);
	  static bool    GetLogicalAddress(PVOID addr, PTSTR szModule, 
                      DWORD len, DWORD& section, DWORD& offset);
  }; // class CAxException


/////////////////////////////////////////////////////////////////////////////
// CAxConnectionEvents class
// Desc:
//
  class CAxConnectionEvents : public ConnectionEventsVt
  {
   public :

      ULONG m_ulRefCount;
      CAxConnectionEvents():m_ulRefCount(1){}

      STDMETHOD(QueryInterface)(REFIID iid, LPVOID * ppvObject)
      {
        if (IsEqualIID(__uuidof(IUnknown), iid) ||
            IsEqualIID(__uuidof(ConnectionEventsVt), iid))
        {
          *ppvObject = this;
          return S_OK;
        }
        else
          return E_NOINTERFACE;
      }


      STDMETHOD_(ULONG, AddRef)()
      {
        return m_ulRefCount++;
      }


      STDMETHOD_(ULONG, Release)()
      {
        if (--m_ulRefCount == 0)
        {
          delete this;
          return 0;
        }
        else
          return m_ulRefCount;
      }


      STDMETHOD(InfoMessage)(
               ADOError * pError,
               EventStatusEnum * adStatus,
               _ADOConnection * pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }


      STDMETHOD(BeginTransComplete)(
               LONG TransactionLevel,
               ADOError * pError,
               EventStatusEnum * adStatus,
               _ADOConnection * pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }


      STDMETHOD(CommitTransComplete)(
               ADOError * pError,
               EventStatusEnum * adStatus,
               _ADOConnection * pConnection)
      {
         *adStatus = adStatusUnwantedEvent;
         return S_OK;
      }


      STDMETHOD(RollbackTransComplete)(
               ADOError * pError,
               EventStatusEnum * adStatus,
               _ADOConnection * pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }


      STDMETHOD(WillExecute)(
               BSTR * Source,
               CursorTypeEnum * CursorType,
               LockTypeEnum * LockType,
               long * Options,
               EventStatusEnum * adStatus,
               _ADOCommand * pCommand,
               _ADORecordset * pRecordset,
               _ADOConnection * pConnection)
      {
         *adStatus = adStatusUnwantedEvent;
         return S_OK;
      }


      STDMETHOD(ExecuteComplete)(
               LONG RecordsAffected,
               ADOError * pError,
               EventStatusEnum * adStatus,
               _ADOCommand * pCommand,
               _ADORecordset * pRecordset,
               _ADOConnection * pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }


      STDMETHOD(WillConnect)(
               BSTR * ConnectionString,
               BSTR * UserID,
               BSTR * Password,
               long * Options,
               EventStatusEnum * adStatus,
               _ADOConnection  *pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }


      STDMETHOD(ConnectComplete)(
               ADOError * pError,
               EventStatusEnum * adStatus,
               _ADOConnection * pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }


      STDMETHOD(Disconnect)(
               EventStatusEnum * adStatus,
               _ADOConnection * pConnection)
      {
        *adStatus = adStatusUnwantedEvent;
        return S_OK;
      }

  };


/////////////////////////////////////////////////////////////////////////////
// CAxRecordsetEvents class
// Desc:
//
  class CAxRecordsetEvents : public RecordsetEventsVt 
  {
  public:
    ULONG m_ulRefCount;
    CAxRecordsetEvents():m_ulRefCount(1){}

    STDMETHOD(QueryInterface)(REFIID iid, LPVOID * ppvObject)
    {
      if (IsEqualIID(__uuidof(IUnknown), iid) || 
        IsEqualIID(__uuidof(RecordsetEventsVt), iid))
      {
         *ppvObject = this;
         return S_OK;
      }
      else 
        return E_NOINTERFACE;
    }

   STDMETHOD_(ULONG, AddRef)()
   {
      return m_ulRefCount++;
   }

   STDMETHOD_(ULONG, Release)()
   {
      if (--m_ulRefCount == 0) {
         delete this;
         return 0;
      }
      else 
         return m_ulRefCount;
   }


   STDMETHOD(WillChangeField)( LONG cFields, 
                               VARIANT Fields, 
                               EventStatusEnum *adStatus,
                               _ADORecordset *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(FieldChangeComplete)( LONG cFields,
                                   VARIANT Fields,
                                   ADOError *pError,
                                   EventStatusEnum *adStatus,
                                   _ADORecordset *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(WillChangeRecord)( EventReasonEnum adReason,
                                LONG cRecords,
                                EventStatusEnum *adStatus,
                                _ADORecordset *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(RecordChangeComplete)( EventReasonEnum adReason,
                                    LONG cRecords,
                                    ADOError  *pError,
                                    EventStatusEnum  *adStatus,
                                    _ADORecordset  *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(WillChangeRecordset)( EventReasonEnum adReason,
                                   EventStatusEnum *adStatus,
                                   _ADORecordset  *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(RecordsetChangeComplete)( EventReasonEnum adReason,
                                       ADOError *pError,
                                       EventStatusEnum  *adStatus,
                                       _ADORecordset  *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(WillMove)( EventReasonEnum adReason,
                        EventStatusEnum  *adStatus,
                        _ADORecordset  *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(MoveComplete)( EventReasonEnum adReason,
                            ADOError *pError,
                            EventStatusEnum *adStatus,
                            _ADORecordset  *pRecordset)
   {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(EndOfRecordset)( VARIANT_BOOL *fMoreData,
                              EventStatusEnum *adStatus,
                              _ADORecordset *pRecordset) {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(FetchProgress)( long Progress,
                             long MaxProgress,
                             EventStatusEnum *adStatus,
                             _ADORecordset *pRecordset) {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }

   STDMETHOD(FetchComplete)( ADOError *pError,
                             EventStatusEnum *adStatus,
                             _ADORecordset *pRecordset) {
      *adStatus = adStatusUnwantedEvent; 
      return S_OK;
   }
  };
}

#endif	// !_AXLIB_H_
