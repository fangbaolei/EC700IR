#ifndef _RTPPARSER_H_ZHANGYANJIE
#define _RTPPARSER_H_ZHANGYANJIE


#include <list>



/* 帧类型定义*/
enum PackTypeFlag
{
	PACK_TYPE_FRAME_P=0,
	PACK_TYPE_FRAME_I,
	PACK_TYPE_FRAME_B,
	PACK_TYPE_FRAME_NULL=0xff,/*结束，无数据*/
};

#define UDP_PACKET_SIZE 2*1024

typedef struct
{
	char buf[UDP_PACKET_SIZE];
	int len;
}PACKET_NODE_T;

typedef std::list<PACKET_NODE_T*> PACKET_BUF_LIST;


class H264RTPParser
{
public:
    H264RTPParser();
    virtual ~H264RTPParser();

	int SetSize(int w,int h);
	int SetFPS(int nfps);
	//!设置打包模式
	//!flag:1-udp打包，0-tcp打包
	int SetPackMode(int flag);
    int SetSSrc(int ssrc)
    {
        if(ssrc !=0)
        {
            m_iRtpSsrc = ssrc;
            return 0;
        }
        return -1;
    }
    int GetSSrc()
    {
        return m_iRtpSsrc;
    }
	int clear();
	//!pFrame-一帧数据
	//!len:帧长
	//flag:帧标志
    int AddFrame(const char* pFrame, const int len, const PackTypeFlag ucFrameType,bool ps =true);
	//!可取出的rtp包数目
	int GetPacketCount();
	//!成功-返回包长度
	int GetRtpPacket(char* pOutBuf, int size);
protected:
    void PlaySendFrame(unsigned char iFrameType, char* buf, int len,char* fui = NULL,char* fuh = NULL, bool bMarker = true);
	/* 媒体流入队列*/
	int sendFU_A(int iCh,unsigned int frameType, char* fui,char* fuh,char* data, int len, bool bMarker);

	unsigned short getSequenceNum();
	void setTimestamp(int fps);
private:
	int m_ucFrameRate;
	int m_packet_num;


	int m_pic_width;
	int m_pic_height;
	PACKET_BUF_LIST m_Packet_list;

	int m_rtpModel; //rtp over rtsp 2,other 0
	unsigned short m_usSeq;
	unsigned long m_iTimeStep;
	unsigned long m_iRtpSsrc;
	bool m_bFindFirstIFrame;
protected:
    //I帧封装，传进来的pDestBuf务必清零
    int Packet_I_frame(const char* pSrcBuf , int nSrcLen, char* pDestBuf , int& nDestLen , int nFrameRate , int nWidth , int nHeight,int Compression);
    int Packet_P_frame(const char* pSrcBuf , int nSrcLen, char* pDestBuf , int& nDestLen);
    int Packet_Video_frame(const char* pSrcBuf , int nSrcLen , char* pDestBuf, int& nLen , bool bFirst = true);       //封装视频帧
    int GeneratePacketsFromFrame(const char* pSrcBuf , int nSrcLen , char* pDestBuf , int& nLen); //拆分NAL包
private: //变量

    int m_IFrameCount;
};



#endif
