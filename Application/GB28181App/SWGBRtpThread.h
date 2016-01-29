
#ifndef __SWGBRTPTHREAD_H_
#define __SWGBRTPTHREAD_H_

#include "SWThread.h"
#include "SWString.h"
#include "SWFile.h"
#include "RtpParser.h"
#include "SWUDPSocket.h"
#include "SWGB28181Parameter.h"



class SWGBRtpThread : public CSWThread
{
public:
    SWGBRtpThread();
    virtual ~SWGBRtpThread();

    virtual HRESULT Start();
    virtual HRESULT Stop();
    INT set_ssrc(LPCSTR ssrc)
    {
        return m_parser.SetSSrc(swpa_atoi(ssrc));
    }
    int get_ssrc()
    {
        return m_parser.GetSSrc();
    }
    int set_FPS( int nfps )
    {
        return m_parser.SetFPS(  nfps );
    }
    int set_PackMode( int flag )
    {
        return m_parser.SetPackMode( flag );
    }
    int set_Size( int w,int h )
    {
        return m_parser.SetSize(  w, h );
    }

    int set_Pause(BOOL pPause)
    {
        m_bPause = pPause; 
        return 0;
    }

    BOOL get_Historystopflag();
    

    

    


    void get_Timerecord(SWPA_DATETIME_TM* ptemp);


    int set_GBAPPlist(PVOID pvArg);
    BOOL remove_listelment(int did);


   
    


protected:
    /**
     * @brief 线程主运行程序，各线程需重载该函数实现其功能。
     * @return - S_OK : 成功 - E_FAIL : 失败
     */
    virtual HRESULT Run();
    
//    CSWFile m_H264File;
    BYTE *m_pbBuf;
    int add_Frame( const char* pFrame, const int len, const PackTypeFlag ucFrameType )
    {
        return m_parser.AddFrame(pFrame, len, ucFrameType);
    }
    int get_RtpPacket( char* pOutBuf, int size )
    {
        return m_parser.GetRtpPacket(pOutBuf, size);
    }

   



protected:
    CRtpParser                      m_parser;
    CHAR *							m_lpVideoBuffer;
    CHAR *							m_lpAudioBuffer;
    DWORD       					m_nNeedVideoFrameNo;
    BOOL							m_bNeedKey;
    DWORD                           m_nNeedAudioFrameNo;
    BOOL							m_bNewAudio;

    BOOL							m_bSubFlowOpen;
    CSWString                       m_szSendIP;
    SHORT                           m_sendPort;
    BOOL                            m_bNeedVideoSend;
    CSWUDPSocket                    m_Rtpsock;
    BOOL                            m_sendFirst;
    CSWList<client_info *>          m_GBthread_list;
    CSWMutex			            m_cMutexLock;
    CSWMutex			            m_cMutexLock_time;
    CSWMutex			            m_cMutexLock_historystop;


    SWPA_DATETIME_TM timerecord;    //记录帧时间
    BOOL                            m_bPause;
    BOOL                            m_historystop;

    
};

#endif
