#include "swpa.h"
#include "swpa_private.h"
#include "swpa_command.h"

#ifdef SWPA_COMMAND
#define SWPA_COMMAND_PRINT(fmt, ...) SWPA_PRINT("[%s:%d]"fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#define SWPA_COMMAND_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define SWPA_COMMAND_PRINT(fmt, ...)
#define SWPA_COMMAND_CHECK(arg)
#endif

int exec_command(TiXmlDocument& xmlDoc)
{
	SWPA_SOCKET_ATTR_T stSocketAttr;
  bzero(&stSocketAttr,sizeof(SWPA_SOCKET_ATTR_T));
  stSocketAttr.af = SWPA_AF_INET;
  stSocketAttr.type = SWPA_SOCK_STREAM;
  SWPA_SOCKET_T s;
  if(SWPAR_FAIL == swpa_socket_create(&s, &stSocketAttr))
  {
  	return SWPAR_FAIL;
	}
	
	struct SWPA_TIMEVAL tv = {3, 0};    
	if(SWPAR_FAIL == swpa_socket_opt(s, SWPA_SOL_SOCKET , SWPA_SO_SNDTIMEO, &tv, sizeof(tv))
	|| SWPAR_FAIL == swpa_socket_opt(s, SWPA_SOL_SOCKET , SWPA_SO_RCVTIMEO, &tv, sizeof(tv)))
	{
		swpa_socket_delete(s);
		return SWPAR_FAIL;
	}
	
	SWPA_SOCKADDR addr = {9999, "127.0.0.1"};
	if(SWPAR_FAIL == swpa_socket_connect(s, &addr, sizeof(addr)))
	{
		swpa_socket_delete(s);
		return SWPAR_FAIL;
	}
	    
	TiXmlPrinter cPrt;
	xmlDoc.Accept(&cPrt);
	const char* szXML = cPrt.CStr();
	SWPA_COMMAND_PRINT("%s", szXML);
	int iLen = swpa_strlen(szXML) + 1;
	if(SWPAR_FAIL == swpa_socket_send(s, &iLen, sizeof(int), NULL) || SWPAR_FAIL == swpa_socket_send(s, szXML, iLen, NULL))
	{
		swpa_socket_delete(s);
		return SWPAR_FAIL;
	}
	int iLenSize = sizeof(int);
	if(SWPAR_FAIL == swpa_socket_recv(s, &iLen, (unsigned int *)&iLenSize))
	{
		swpa_socket_delete(s);
		return SWPAR_FAIL;
	}
	char* szRESXML = new char[iLen];
	if(SWPAR_FAIL == swpa_socket_recv(s, szRESXML, (unsigned int *)&iLen))
	{
		delete []szRESXML;
		swpa_socket_delete(s);
		return SWPAR_FAIL;
	}		
	swpa_socket_delete(s);
	xmlDoc.Clear();	
	xmlDoc.Parse(szRESXML);
	delete []szRESXML;
	return xmlDoc.RootElement() ? SWPAR_OK : SWPAR_FAIL;
}

