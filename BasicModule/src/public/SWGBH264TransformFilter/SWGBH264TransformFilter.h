/**
* @file SWGBH264 .h
* @brief GBH264模块
* 
* @copyright Signalway All Rights Reserved
* @author zhouy
* @date 2013-04-01
* @version 1.0
*/


#ifndef __SW_GBH264_TRANSFORM_FILTER_H__
#define __SW_GBH264_TRANSFORM_FILTER_H__

#include "SWBaseFilter.h"
#include "SWClassFactory.h"
#include "SWFC.h"
#include "SWGB28181Parameter.h"
#include "SWMessage.h"

class CSWH264TransformClient;

class CSWGBH264TransformFilter : public CSWBaseFilter,CSWMessage
{
    CLASSINFO(CSWGBH264TransformFilter, CSWBaseFilter);

public:
	/*
	* @brief 构造函数
    *
	*/
    CSWGBH264TransformFilter();
	
	/*
	* @brief 析构函数
	*
	*/
    virtual ~CSWGBH264TransformFilter();

	/*
	* @brief 初始化函数
	* @retval S_OK : 成功
	* @retval E_OUTOFMEMORY : 内存不足
	*/
	HRESULT Initialize();

	
	/**
	 * @brief 接收输入的数据包
	 * @param [in] pdu 接收数据包
	 * @成功返回S_OK，其他值为错误代码
	 */
	virtual HRESULT Receive(CSWObject* obj);

	/*
	* @brief 开始运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	virtual HRESULT Run();

	/*
	* @brief 停止运行函数
	* @retval S_OK : 成功
	* @retval E_FAIL: 失败
	*/
	virtual HRESULT Stop();

    CSWImage* GetFirstImage();
    BOOL  GetCanBeginPlay();

    float GetSpeed(){return m_fCurrentSpeed;}
    BOOL  GetIsHistoryVideo(){return m_fIsHistoryVideo;}
    BOOL  GetIsPause(){return m_fIsPause;}

    DWORD GetH264FramRate(){return m_nH264FramRate;}
    HRESULT ReadNextSecondVideoData(INT seconds = 1);

protected:

    SW_BEGIN_DISP_MAP(CSWGBH264TransformFilter,CSWBaseFilter)
		SW_DISP_METHOD(Initialize, 0)
	SW_END_DISP_MAP();

    static PVOID AcceptLinkProxy(PVOID pvArg);
    HRESULT AcceptLink();


    HRESULT OnGB28181Command(PVOID pvBuffer, INT iSize);

    //消息映射宏
    SW_BEGIN_REMOTE_MESSAGE_MAP(CSWGBH264TransformFilter, CSWMessage)
        SW_REMOTE_MESSAGE_HANDLER(MSG_GB2818_TRANSFORM_COMMAND,OnGB28181Command)
    SW_END_REMOTE_MESSAGE_MAP()

private:	
    BOOL				m_fInited;                  //初始化标记
    CSWThread			m_cThreadAccept;            //Filter扩展线程,接收网络连接
    DWORD				m_dwQueueSize;              //队列缓冲帧长度
    DWORD               m_nH264FramRate;            //正常最大帧率
    CSWList<CSWImage*>	m_lstFrames;                //缓冲队列

    CSWSemaphore		m_cSemaSync;                //缓冲队列读写锁
    CSWMutex			m_cMutexLock;               //缓冲队列访问锁
    CSWMutex			m_cMutexThread;             //发送线程锁
    CSWH264TransformClient* m_pTransformClient;     //发送线程
    CSWTCPSocket        m_cLocalTcpSock;            //发送用Socket
    BOOL                m_fNeedReadNextSecond;      //读取下一秒的标记
    float               m_fCurrentSpeed;            //当前播放速度
    BOOL                m_fIsHistoryVideo;          //历史视频标记
    BOOL                m_fIsPause;                 //暂停播放标记
    BOOL                m_fBackward;                //播放方向
    CSWDateTime         m_dtHistoryFrom;            //等待获取历史视频的开始时间
    CSWDateTime         m_dtHistoryEnd;             //等待获取历史视频的结束时间
    BOOL                m_FirstPlay;                //首次播放标记
};

REGISTER_CLASS(CSWGBH264TransformFilter);
	
class CSWH264TransformClient : public CSWThread
{
    CLASSINFO(CSWH264TransformClient, CSWThread)
public:
    CSWH264TransformClient(const SWPA_SOCKET_T & sock,const void* pFilter)
    {
        m_tcpClient.Attach(sock);
        m_tcpClient.SetSendTimeout(300000);
        m_tcpClient.SetSendBufferSize(32768);

        m_pFilter = (CSWGBH264TransformFilter*)pFilter;
        m_dtLastSendTime = 0;
        
        SW_TRACE_DEBUG("Construct New CSWH264TransformClient Object\n");
    }

    virtual ~CSWH264TransformClient()
    {
        if(m_tcpClient.IsValid())
        {
            m_tcpClient.Close();
        }

        SW_TRACE_DEBUG("Destroy CSWH264TransformClient Object\n");
    }

    CSWTCPSocket & GetTCP(VOID)
    {
        return m_tcpClient;
    }

    virtual HRESULT Run();
private:
    CSWGBH264TransformFilter*   m_pFilter;
    CSWTCPSocket                m_tcpClient;
    SWPA_DATETIME_TM            m_dtTempVarTime;            //临时缓冲用时间变量
    DWORD                       m_dtLastSendTime;
};

#endif //__SW_GBH264_TRANSFORM_FILTER_H__





