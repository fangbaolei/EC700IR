#include "SWFC.h"
#include "SWCommandApplication.h"

CSWCommandApplication::CSWCommandApplication()
{
	
}

CSWCommandApplication::~CSWCommandApplication()
{
}

HRESULT CSWCommandApplication::Run()
{
	CSWTCPSocket cTCPServer;
	cTCPServer.Create();
	while(!IsExited() && FAILED(cTCPServer.Bind(NULL, 9999)))
	{
		SW_TRACE_NORMAL("bind port[9999] error.");
		Sleep(1000);
	}
	
	cTCPServer.SetSendTimeout( 3000 );
	cTCPServer.SetRecvTimeout( 3000 );
	cTCPServer.Listen();
	
	while(!IsExited())
	{
		HeartBeat();
		SWPA_SOCKET_T sInSock;
		if(SUCCEEDED(cTCPServer.Accept(sInSock)))
		{
			//if(!fork())
			{
				CSWTCPSocket client;
				client.Attach(sInSock);
				client.SetSendTimeout( 3000 );
				client.SetRecvTimeout( 3000 );
			
				DWORD dwSize = 0;
				if(SUCCEEDED(client.Read(&dwSize, sizeof(DWORD), NULL)))
				{
					HeartBeat();
					CHAR *szXML = new CHAR[dwSize];
					if(SUCCEEDED(client.Read(szXML, dwSize, NULL)))
					{
						HeartBeat();
						TiXmlDocument xmlDoc, xmlRet;						
						xmlRet.LinkEndChild(new TiXmlDeclaration("1.0", "GB2312", "yes"));
						TiXmlElement* pRoot = new TiXmlElement("LprApp");
						TiXmlElement* pTmp = NULL;
						
						xmlDoc.Parse(szXML);
						TiXmlElement *el = GetElement(xmlDoc.RootElement(), "LprApp");
						
						if(NULL != el)
						{
							HRESULT hr = E_FAIL;
							CSWString strCommand;
							for(el = el->FirstChildElement("Command"); NULL != el; el = el->NextSiblingElement("Command"))
							{
								strCommand.Format("%s %s %s", el->Attribute("shell") ? el->Attribute("shell") : "", el->Attribute("id") ? el->Attribute("id") : "", el->Attribute("param") ? el->Attribute("param") : "");
								SW_TRACE_NORMAL("%s\n", (LPCSTR)strCommand);
								HeartBeat();
								hr = System((LPCSTR)strCommand);						
								HeartBeat();
								pTmp = new TiXmlElement("Command");
								if(el->Attribute("id"))
								{
									pTmp->SetAttribute("id", el->Attribute("id"));
								}
								pTmp->SetAttribute("return", hr == S_OK ? "S_OK" : "E_FAIL");		
								pRoot->LinkEndChild(pTmp);
							}
						}
						else
						{
							pTmp = new TiXmlElement("Command");
							pTmp->SetAttribute("id", "Unkown");
							pTmp->SetAttribute("return", "E_NOTIMPL");
							pRoot->LinkEndChild(pTmp);
						}
						HeartBeat();
						xmlRet.LinkEndChild(pRoot);							
						TiXmlPrinter cPrt;
						xmlRet.Accept(&cPrt);
						const char* szRetXML = cPrt.CStr();
						INT iLen = swpa_strlen(szRetXML) + 1;
						client.Send(&iLen, sizeof(INT), NULL);
						client.Send(szRetXML, iLen, NULL);
						HeartBeat();
					}
					delete []szXML;
				}
			}
		}
	}
}

HRESULT CSWCommandApplication::OnException(INT iSignalNo)
{
	return CSWApplication::OnException(iSignalNo);
}

