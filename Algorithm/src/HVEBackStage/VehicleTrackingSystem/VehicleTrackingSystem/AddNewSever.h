#pragma once 

#include "resource.h"

typedef struct _NewSeverType
{
	char szLocaName[256];
	char szServerIP[30];
	char szDataBaseName[32];
	char szUserName[32];
	char szPassWord[16];
	_NewSeverType()
	{
		memset(this, 0, sizeof(_NewSeverType));
	}
}NewSeverType;

class CAddNewSever : public CDialog
{
	DECLARE_DYNAMIC(CAddNewSever);
public:
	CAddNewSever(NewSeverType* pNewServerType, BOOL fIsNew, CWnd* pParent = NULL);
	~CAddNewSever();
	enum {IDD = IDD_ADD_SERVER_DLG};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnOK();
	virtual void OnCancel();

public:
	NewSeverType* m_pNewSever;
	BOOL m_fIsNew;
};
