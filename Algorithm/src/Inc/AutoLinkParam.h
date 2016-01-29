#ifndef _AUTO_LINK_PARAM_H_
#define _AUTO_LINK_PARAM_H_

typedef struct _tagAutoLinkParam
{
    BOOL fAutoLinkEnable;                /**< 主动连接使能 */
    char szAutoLinkIP[32];               /**< 主动连接服务器IP */
    int iAutoLinkPort;                   /**< 主动连接服务器端口 */

    _tagAutoLinkParam()
    {
        fAutoLinkEnable = FALSE;
        iAutoLinkPort = 0;
        strcpy(szAutoLinkIP , "172.18.10.100");
    }
}AUTO_LINK_PARAM;



#endif
