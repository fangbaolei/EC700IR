// RtpParser.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "SWH264RTPParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "assert.h"
#include <netinet/in.h>

#ifdef WIN32
#include <WinSock2.h>
#endif




typedef struct _RTPHDR_
{
	// little endian
	unsigned char csrccount:4;
	unsigned char extension:1;
	unsigned char padding:1;
	unsigned char version:2;

	unsigned char payloadtype:7;
	unsigned char marker:1;

	unsigned short seqnum;

	unsigned int timestamp;
	unsigned int ssrc;

}RTPHDR;

/*媒体数据头*/
typedef struct _RTPHDR_EXT_
{
	// little endian
	unsigned char csrccount:4;
	unsigned char extension:1;
	unsigned char padding:1;
	unsigned char version:2;

	unsigned char payloadtype:7;
	unsigned char marker:1;

	unsigned short seqnum;

	unsigned int timestamp;
	unsigned int ssrc;
	//unsigned short profile;
	//unsigned short extlen;
}RTPHDR_EXT;

//中星微平台扩展字段
typedef struct _RTP_EXT_HEADER1
{
	unsigned short profile;
	unsigned short len;
	unsigned char Reserved[3];
	unsigned char Framemarker:4;
	unsigned char FrameType:4;
}RTP_EXT_Header;

#define NAL_RTP_PACKET_SIZE 1400  //此大小必须小于rtp封包大小
#define RTPHDR_EXT_SIZE (sizeof(RTPHDR_EXT))
#define RTPHDR_SIZE (sizeof(RTPHDR))

#ifdef WIN32
#define w_rand rand
#else
#define w_rand random
#endif

//namespace NAMEA_SH
//{

H264RTPParser::H264RTPParser()
{
	m_ucFrameRate = 25;
	m_packet_num = 0;
	m_pic_width = 0;
	m_pic_height = 0;
	m_rtpModel = 1;
	m_usSeq = 0;
	m_iTimeStep = 0;
	m_iRtpSsrc = w_rand();
    m_IFrameCount = 0;


	m_bFindFirstIFrame = false;
}

H264RTPParser::~H264RTPParser()
{

}


FILE *f;
//static int first = 0;
int H264RTPParser::AddFrame( const char* pFrame, const int len, const PackTypeFlag ucFrameType, bool ps)
{
	if ( !m_bFindFirstIFrame )
	{
		if ( ucFrameType == PACK_TYPE_FRAME_I )
		{
			m_bFindFirstIFrame = true;
		}
		else
		{
			//SIP_Printf("rtpparser discard frame[%d] before get IFrame!\n", ucFrameType);
			return -1;
		}
	}
	if (PACK_TYPE_FRAME_I == ucFrameType)
	{
		if (m_ucFrameRate <= 0 || m_ucFrameRate >= 30)
		{
			m_ucFrameRate = 25;
		}
	}

	assert(len <= 1024*1024);
	static unsigned char s_buf[2*1024*1024];
	unsigned char* pcOutBuf = s_buf;
	//memset(pcOutBuf,0,1024*1024);
	int iOutLen = 0;
	int iRet = -1;
    switch(ucFrameType)
    {
        case PACK_TYPE_FRAME_I:
        iRet =  Packet_I_frame( pFrame, len, (char* )pcOutBuf, iOutLen, m_ucFrameRate, m_pic_width, m_pic_height, 7);
        break;
        case PACK_TYPE_FRAME_P:
        iRet =  Packet_P_frame(pFrame, len, (char*)pcOutBuf, iOutLen);
        break;
        default:
        return -4;
        break;
    }
	if ( iRet  < 0 )
	{
		return -3;
	}
	PlaySendFrame(ucFrameType, (char* )pcOutBuf,  iOutLen, NULL ,NULL ,true);
	setTimestamp(m_ucFrameRate);
	return 0;
}

int H264RTPParser::GetPacketCount()
{
	return m_Packet_list.size();
}

int H264RTPParser::GetRtpPacket( char* pOutBuf, int size )
{
	if( m_Packet_list.size() > 0 )
	{
		PACKET_NODE_T* pNode = (*m_Packet_list.begin());
		if ( pNode->len <= size )
		{

			int len = pNode->len;
			memcpy(pOutBuf, pNode->buf, pNode->len);
			delete pNode;
			m_Packet_list.pop_front();
			return len;
		}
		else
		{
			return -2;
		}

	}
	return -1;
}

void H264RTPParser::PlaySendFrame( unsigned char iFrameType, char* buf, int len,char* fui /*= NULL*/,char* fuh /*= NULL*/, bool bMarker /*= true*/ )
{
	//直接用rtp发送
    //SIP_Printf("H264RTPParser:PlaySendFrame len=%d,NAL_RTP_PACKET_SIZE=%d\n", len,NAL_RTP_PACKET_SIZE);
	int total = len;
	if (total <= NAL_RTP_PACKET_SIZE )
	{
        //printf("PlaySendFrame %d\n",__LINE__);
		sendFU_A(-1, iFrameType,fui,fuh,buf,len,bMarker);
	}
	else
	{
		int curr = 0;
        unsigned char* fu_buf = (unsigned char*)buf;
		int fu_len = NAL_RTP_PACKET_SIZE;
		bool marker = false;

        //SIP_Printf("H264RTPParser:sendFU_A curr=%d,total=%d\n", curr,total);
		while( curr < total )
		{
			if( total - curr <= NAL_RTP_PACKET_SIZE )
			{
				fu_len = total - curr ;
				marker = true;
			}
			sendFU_A(-1, iFrameType, (char *)0, (char *)0, (char *)fu_buf, fu_len,marker);
			curr = curr + fu_len;
			fu_buf = fu_buf + fu_len;
		}
	}
}

int H264RTPParser::sendFU_A( int iCh,unsigned int frameType, char* fui,char* fuh,char* data, int len, bool bMarker )
{
	if (len <= 0 || NULL == data)
	{
        printf("H264RTPParser:sendFU_A len=%d,data=%d\n", len,data);
		return -1;
	}

	PACKET_NODE_T* node = new PACKET_NODE_T;
	int& rtp_size = node->len;
	char *pRtpBuf = node->buf;
	if(m_rtpModel)
	{
		RTPHDR_EXT* pszHdr = (RTPHDR_EXT* )(pRtpBuf );
		memset((char *)pszHdr, 0, RTPHDR_EXT_SIZE);
		pszHdr->version = 2;
		//pszHdr->extension = 1;
		pszHdr->extension = 0;
		pszHdr->payloadtype = 96;

		pszHdr->marker = bMarker;
		pszHdr->seqnum = ntohs((unsigned short)getSequenceNum());
		pszHdr->timestamp = ntohl(m_iTimeStep);
		pszHdr->ssrc = ntohl(m_iRtpSsrc);
		//pszHdr->profile = ntohs(1);
		//pszHdr->extlen = ntohs(2);

		if(NULL == fui)
		{
			memcpy(pRtpBuf+RTPHDR_EXT_SIZE,data, len);
			/*
			padding 添加方式，如果需要填充字节的话
			1)pszHdr->padding = 1;
			2)最后一个字节，如果长度为1，只需要填充0x01
			如果2个字节，填充 00 0x02
			如果3字节，填充 00 00 0x03
			一般4字节对齐，填充方式比较简单
			*/
			rtp_size = RTPHDR_EXT_SIZE + len;
		}
		else
		{
			//memcpy(pRtpBuf + RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader), fui, 1);
			//memcpy(pRtpBuf + RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader) + 1, fuh, 1);
			//memcpy(pRtpBuf + RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader) + 2, data, len);

			memcpy(pRtpBuf + RTPHDR_EXT_SIZE, fui, 1);
			memcpy(pRtpBuf + RTPHDR_EXT_SIZE + 1, fuh, 1);
			//memcpy(pRtpBuf + RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader) + 2, data, len);
			/*
			padding 添加方式，如果需要填充字节的话
			1)pszHdr->padding = 1;
			2)最后一个字节，如果长度为1，只需要填充0x01
			如果2个字节，填充 00 0x02
			如果3字节，填充 00 00 0x03
			一般4字节对齐，填充方式比较简单
			*/
			rtp_size = RTPHDR_EXT_SIZE + 2 + len;
		}
	}
	else  //tcp连接,需要在RTP包之前加2个字节的长度
	{
		RTPHDR_EXT pszHdr;

		memset(&pszHdr, 0, RTPHDR_EXT_SIZE);
		pszHdr.version = 2;
		pszHdr.extension = 1;
		pszHdr.payloadtype = 96;

		pszHdr.marker = bMarker;
		pszHdr.seqnum = ntohs((unsigned short)getSequenceNum());
		pszHdr.timestamp = ntohl(m_iTimeStep);
		pszHdr.ssrc = ntohl(m_iRtpSsrc);
		//pszHdr.profile = ntohs(1);
		//pszHdr.extlen = ntohs(2);

		//扩展字段
		RTP_EXT_Header RTPEXTHeader;
		memset(&RTPEXTHeader, 0,sizeof(RTP_EXT_Header));
		RTPEXTHeader.profile = ntohs(1);
		RTPEXTHeader.len = ntohs(1);
        switch (frameType) {
        case PACK_TYPE_FRAME_I:RTPEXTHeader.FrameType = 0;break;
        case PACK_TYPE_FRAME_P:RTPEXTHeader.FrameType = 1;break;
        case PACK_TYPE_FRAME_B:RTPEXTHeader.FrameType = 2;break;
        default:
            return -1;
            break;
        }
		if(bMarker)
		{
			RTPEXTHeader.Framemarker = 1;
		}
		else
		{
			RTPEXTHeader.Framemarker = 0;
		}

		unsigned short packetlen = ntohs((unsigned short)(RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader) + len));
		//trace("------------------------------------------- %d \n",(RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader) + len));
		memcpy(pRtpBuf,(void *)&packetlen,2);
		//memcpy(pRtpBuf+2,(void *)&pszHdr,sizeof(RTPHDR_EXT));
		memcpy(pRtpBuf+2,(void *)&pszHdr,sizeof(RTPHDR_EXT));
		memcpy(pRtpBuf+2+RTPHDR_EXT_SIZE,(void *)&RTPEXTHeader,sizeof(RTP_EXT_Header));
		memcpy(pRtpBuf+2+RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader),data, len);
		rtp_size = RTPHDR_EXT_SIZE+sizeof(RTPEXTHeader) + len + 2;
	}

	m_Packet_list.push_back(node);
	return 0;
}

unsigned short H264RTPParser::getSequenceNum()
{
	if (++m_usSeq >= 65535)
	{
		m_usSeq = 0;
	}
	return m_usSeq;
}

void H264RTPParser::setTimestamp( int fps )
{
	m_iTimeStep += 90000/fps;
}

int H264RTPParser::SetSize( int w,int h )
{
	m_pic_width = w;
	m_pic_height = h;
	return 0;
}

int H264RTPParser::SetFPS( int nfps )
{
	m_ucFrameRate = nfps;
	return 0;
}

int H264RTPParser::SetPackMode( int flag )
{
	m_rtpModel = flag;
	return 0;
}

int H264RTPParser::clear()
{
	PACKET_BUF_LIST::iterator it;
	for ( it = m_Packet_list.begin(); it != m_Packet_list.end(); it++ )
	{
		PACKET_NODE_T* pNode = (*it);
		delete pNode;
	}
    printf("clear\n");
	m_Packet_list.clear();
	m_bFindFirstIFrame = false;
	return 0;
}
int H264RTPParser::Packet_I_frame(const char* pSrcBuf , int nSrcLen, char* pDestBuf , int& nDestLen , int nFrameRate , int nWidth , int nHeight,int Compression)
{
    if ( NULL == pSrcBuf || NULL == pDestBuf )
    {
        return   -1;
    }

    int nTempLen = 0;
    int nTotalLen = 0;



    GeneratePacketsFromFrame(pSrcBuf , nSrcLen , pDestBuf + nTotalLen , nTempLen);
    nTotalLen += nTempLen;

    nDestLen = nTotalLen;

    m_IFrameCount++ ;
    return 0 ;
}

int H264RTPParser::Packet_P_frame(const char* pSrcBuf , int nSrcLen, char* pDestBuf , int& nDestLen)
{
    if ( NULL == pSrcBuf || NULL == pDestBuf )
    {
        return   -1;
    }

    int nTempLen = 0;
    int nTotalLen = 0;
    GeneratePacketsFromFrame(pSrcBuf , nSrcLen , pDestBuf + nTotalLen , nTempLen);
    nTotalLen += nTempLen;
    nDestLen = nTotalLen;

    return 0 ;
}


int H264RTPParser::Packet_Video_frame(const char* pSrcBuf, int nSrcLen, char* pDestBuf, int& nLen , bool bFirst /*= true*/)
{
    if ( NULL == pSrcBuf || NULL == pDestBuf )
    {
        return   -1;
    }
    memcpy(pDestBuf , pSrcBuf , nSrcLen);
    nLen = nSrcLen;
    return 0;
}


int H264RTPParser::GeneratePacketsFromFrame(const char* pSrcBuf , int nSrcLen , char* pDestBuf , int& nLen)
{
    int	nTotalLen = 0;
    int nPacketNalLen = 0;

    int nOldIndex = 0;
    int nPacketCount = 0;


    while( nSrcLen - nOldIndex > 60*1024)
    {
        if ( nPacketCount == 0 )
        {
            Packet_Video_frame(pSrcBuf + nOldIndex , 60*1024 , pDestBuf + nTotalLen , nPacketNalLen );
        }
        else
        {
            Packet_Video_frame(pSrcBuf + nOldIndex , 60*1024 , pDestBuf + nTotalLen , nPacketNalLen , false);
        }
        nOldIndex += 60*1024;
        nTotalLen += nPacketNalLen;
        nPacketCount++;
    }

    if ( nPacketCount == 0)
    {
        Packet_Video_frame(pSrcBuf + nOldIndex , nSrcLen - nOldIndex , pDestBuf + nTotalLen , nPacketNalLen);
    }
    else
    {
        Packet_Video_frame(pSrcBuf + nOldIndex , nSrcLen - nOldIndex , pDestBuf + nTotalLen , nPacketNalLen , false);
    }

    nTotalLen += nPacketNalLen;
    nPacketCount++;
    nLen = nTotalLen;

    return 0;
}
