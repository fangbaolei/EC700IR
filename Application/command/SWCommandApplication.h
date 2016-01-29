#ifndef __SW_COMMAND_APPLICATION_H__
#define __SW_COMMAND_APPLICATION_H__
class CSWCommandApplication : public CSWApplication
{
	CLASSINFO(CSWCommandApplication, CSWApplication)
public:
	CSWCommandApplication();
	virtual ~CSWCommandApplication();
	virtual HRESULT Run();
	virtual HRESULT OnException(INT iSignalNo);
};
CREATE_PROCESS(CSWCommandApplication)
#endif
