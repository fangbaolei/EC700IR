#ifndef _FTP_PRARAM_H_
#define _FTP_PRARAM_H_

typedef struct _tagFtpParam
{
    BOOL fFtpEnable;             		 /**< FTP使能 */
    CHAR szFtpIP[32];              		 /**< FTP服务器IP */
	CHAR szUserName[32];              		 /**< FTP用户名 */
	CHAR szPassWord[32];              		 /**< FTP密码 */
	INT iTimeOut;						
   	

    _tagFtpParam()
    {
        fFtpEnable = FALSE;
		swpa_strcpy(szFtpIP ,	"172.18.10.100");
        swpa_strcpy(szUserName , "anonymous");
		swpa_strcpy(szPassWord , "yetpasswd");
		iTimeOut = 5;
    }
}FTP_PARAM;



#endif
