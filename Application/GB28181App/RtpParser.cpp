// RtpParser.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include "RtpParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "assert.h"
#include <netinet/in.h>

#ifdef WIN32
#include <WinSock2.h>
#endif

//extern void  SIP_Printf( const char *format, ... );



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

CRtpParser::CRtpParser()
{
	m_ucFrameRate = 25;
	m_packet_num = 0;
	m_pic_width = 0;
	m_pic_height = 0;
	m_rtpModel = 1;
	m_usSeq = 0;
	m_iTimeStep = 0;
	m_iRtpSsrc = w_rand();

	// m_pPsPackt = new CPSPackaging();
	// assert(m_pPsPackt);
	m_pPsMuxer = new CSWPSMuxer();
	INIT_PSMUXER_PARAM_T cPsMuxerParam;
	cPsMuxerParam.lSCRBase = 60000;
	cPsMuxerParam.iFrameRate = 25;
	cPsMuxerParam.iBitRate = 8 * (1 << 20);		//8Mbps
	m_pPsMuxer->InitMuxer((VOID *)&cPsMuxerParam);

	m_bFindFirstIFrame = false;
}

CRtpParser::~CRtpParser()
{
	//delete m_pPsPackt;
	SAFE_DELETE(m_pPsMuxer);
}


FILE *f;
//static int first = 0;
int CRtpParser::AddFrame( const char* pFrame, const int len, const PackTypeFlag ucFrameType )
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

	if ( m_pic_width <= 0 || m_pic_height <= 0 )
	{
		//SIP_Printf("CRtpParser:AddFrame failed, m_pic_width[%d], m_pic_height[%d]\n", m_pic_width, m_pic_height);
		return -2;
	}
	/* 先转换成PS流，再发送 */
//	assert(len <= 1024*1024);
	static unsigned char s_buf[1024*1024];
	const unsigned int s_buf_size = 1024 * 1024;
	unsigned char* pcOutBuf = s_buf;
	//memset(pcOutBuf,0,1024*1024);
	int iOutLen = 0;
	int iRet = -1;


	BLOCK_T *pTmpBlock = NULL;
	unsigned int iOutBufSize = 0;
	BLOCK_T *pVideoFrame = new BLOCK_T;
	if(NULL == pVideoFrame)
	{
		return E_OUTOFMEMORY;
	}
	CHAR *pFrameBuffer = new CHAR[len];
	if(NULL == pFrameBuffer)
	{
		SAFE_DELETE(pVideoFrame);
		return E_OUTOFMEMORY;
	}
	swpa_memcpy(pFrameBuffer, pFrame, len);
	pVideoFrame->pBuffer = (CHAR *)pFrameBuffer;
	pVideoFrame->iBuffer = len;
	pVideoFrame->pNext = NULL;
	pVideoFrame->iType = (BLOCK_TYPE_T)ucFrameType;	
	m_pPsMuxer->Mux(&pVideoFrame);			//PS复用封装
	pTmpBlock = pVideoFrame;
	/*
	  FIXME：
	  从GB28181Filter进来的数据有多处拷贝，现在RTP复用没有重构，
	  导致目前只能这个现状，待RTP重构的时候修复这个问题。
	 */
	while(pTmpBlock != NULL)
	{

		if((iOutBufSize + pTmpBlock->iBuffer) < s_buf_size)
		{
			swpa_memcpy(pcOutBuf + iOutBufSize, pTmpBlock->pBuffer, pTmpBlock->iBuffer);
		}
		else
		{
			break;
		}
		iOutBufSize += pTmpBlock->iBuffer;
		pTmpBlock = pTmpBlock->pNext;
	}
	iOutLen = iOutBufSize;
	m_pPsMuxer->BlockChainFree(pVideoFrame);
	pVideoFrame = NULL;
	
#if 1
	PlaySendFrame(ucFrameType, (char* )pcOutBuf,  iOutLen, NULL ,NULL ,true);
#else

	int res = 0;
	while (1)
	{
		PACKET_NODE_T* node = new PACKET_NODE_T;
		if ( (iOutLen-res) > 1400 )
		{
			node->len = 1400;
		}
		else
		{
			node->len = iOutLen-res;
		}
		memcpy(node->buf, pcOutBuf+res, node->len);
		m_Packet_list.push_back(node);
		res += node->len;
		if ( res >= iOutLen)
		{
			break;
		}
	}
#endif

	//delete []pcOutBuf;
	//pcOutBuf = 0;

	setTimestamp(m_ucFrameRate);
	return 0;
}

int CRtpParser::GetPacketCount()
{
	return m_Packet_list.size();
}

int CRtpParser::GetRtpPacket( char* pOutBuf, int size )
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

void CRtpParser::PlaySendFrame( uchar iFrameType, char* buf, int len,char* fui /*= NULL*/,char* fuh /*= NULL*/, bool bMarker /*= true*/ )
{
	//直接用rtp发送
	//SIP_Printf("CRtpParser:PlaySendFrame len=%d,NAL_RTP_PACKET_SIZE=%d\n", len,NAL_RTP_PACKET_SIZE);
	int total = len;
	if (total <= NAL_RTP_PACKET_SIZE )
	{
        //printf("PlaySendFrame %d\n",__LINE__);
		sendFU_A(-1, iFrameType,fui,fuh,buf,len,bMarker);
	}
	else
	{
		int curr = 0;
		unsigned char* fu_buf = (uchar*)buf;
		int fu_len = NAL_RTP_PACKET_SIZE;
		bool marker = false;

        //SIP_Printf("CRtpParser:sendFU_A curr=%d,total=%d\n", curr,total);
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

int CRtpParser::sendFU_A( int iCh,unsigned int frameType, char* fui,char* fuh,char* data, int len, bool bMarker )
{
	if (len <= 0 || NULL == data)
	{
        printf("CRtpParser:sendFU_A len=%d,data=%d\n", len,data);
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
        case PACK_TYPE_FRAME_AUDIO:RTPEXTHeader.FrameType = 8;break;
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
     //printf("PlaySendFrame %d   %d \n",m_Packet_list.size(),node->len);
	//m_pRtpClient->SendRawStream(0, (char *)(pPacket->GetBuffer()), pPacket->GetLength());
	//pPacket->Release();
	return 0;
}

unsigned short CRtpParser::getSequenceNum()
{
	if (++m_usSeq >= 65535)
	{
		m_usSeq = 0;
	}
	return m_usSeq;
}

void CRtpParser::setTimestamp( int fps )
{
	m_iTimeStep += 90000/fps;
}

int CRtpParser::SetSize( int w,int h )
{
	m_pic_width = w;
	m_pic_height = h;
	return 0;
}

int CRtpParser::SetFPS( int nfps )
{
	//m_ucFrameRate = nfps;
	m_pPsMuxer->SetFrameRate(nfps);
	return 0;
}

int CRtpParser::SetPackMode( int flag )
{
	m_rtpModel = flag;
	return 0;
}

int CRtpParser::clear()
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
//}
//
//struct h264FRAME_INFO
//{
//	int framelen;
//	int frametype;
//};
//void GenerateFrameIndex()
//{
//
//	struct h264FRAME_INFO frameinfo;
//	FILE* fp_h264 = NULL;
//	int ret = 0;
//
//	fp_h264 = fopen("20120615143721.bcm", "rb");
//	assert(fp_h264 != NULL);
//	fseek(fp_h264, 0, SEEK_END);
//	int size_h264=ftell(fp_h264);
//	fseek(fp_h264, 0, SEEK_SET);
//	char* ph264Buf = new char[size_h264];
//	ret = fread(ph264Buf, 1, size_h264, fp_h264);
//	assert(ret == size_h264);
//
//	FILE* fp_index = fopen("20120615143721.index", "wb+");
//	assert(fp_index != NULL);
//	int last_pos = 0;
//	int frame_count = 0;
//	int frame_type = -1;
//	for ( int i = 0 ; i < size_h264 - 3 ; i++)
//	{
//		if ( ph264Buf[i] == 0x00 && ph264Buf[i+1] == 0x00 && ph264Buf[i+2] == 0x00 && ph264Buf[i+3] == 0x01)
//		{
//			if ( frame_type >= 0 )
//			{
//				frame_count++;
//				frameinfo.frametype = frame_type;
//				frameinfo.framelen = i-last_pos;
//				fwrite(&frameinfo, sizeof(frameinfo), 1, fp_index);
//				last_pos = i;
//			}
//
//			if ( (ph264Buf[i+4] & 0x1F) == 5 ) //i帧
//			{
//				frame_type = PACK_TYPE_FRAME_I;
//			}
//			else if ( (ph264Buf[i+4] & 0x1F) == 1 ) //p帧
//			{
//				frame_type = PACK_TYPE_FRAME_P;
//			}
//			else
//			{
//				frame_type = -1;
//			}
//
//			continue;
//		}
//	}
//	frame_count++;
//	frameinfo.frametype = frame_type;
//	frameinfo.framelen = size_h264-last_pos;
//	fwrite(&frameinfo, sizeof(frameinfo), 1, fp_index);
//
//	delete []ph264Buf;
//	fclose(fp_h264);
//	fclose(fp_index);
//
//	printf("genrate frame index sucess, frame count[%d]!\n",frame_count);
//}
//
//void h264es2ps()
//{
//	struct h264FRAME_INFO frameinfo;
//	//!es 转ps
//	CRtpParser rtpParser;
//	rtpParser.SetPackMode(1);
//	rtpParser.SetSize(1280, 720);
//	rtpParser.SetFPS(25);
//
//	FILE* fp_h264 = NULL;
//	int ret = 0;
//
//	char* pBuf= new char[1024*1024];
//	fp_h264 = fopen("fdaudio.pcm", "rb");
//	assert(fp_h264 != NULL);
//	FILE* fp_index = fopen("len_type.type", "rb");
//	assert(fp_index != NULL);
//	FILE* fp_ps = fopen("fdaudio.ps", "wb+");
//	assert(fp_ps != NULL);
//
//	int framecount = 0;
//	while (1)
//	{
//		ret = fread(&frameinfo, sizeof(frameinfo), 1, fp_index);
//		if (ret <= 0)
//		{
//			printf("read frame index over, total:%d\n", framecount);
//			break;
//		}
//		framecount++;
//		ret = fread(pBuf, 1, frameinfo.framelen, fp_h264);
//		assert(ret == frameinfo.framelen);
//		rtpParser.AddFrame((char*)pBuf, frameinfo.framelen, (enum PackTypeFlag)frameinfo.frametype);
//		char buf[1500];
//		while( 1)
//		{
//			ret = rtpParser.GetRtpPacket( buf, sizeof(buf) );
//			if ( ret <= 0 )
//			{
//				break;
//			}
//			int wlen = fwrite(buf, 1, ret, fp_ps);
//			assert(wlen == ret);
//		}
//	}
//
//	fclose(fp_h264);
//	fclose(fp_index);
//	fclose(fp_ps);
//	delete[] pBuf;
//}




//#ifdef WIN32
//int _tmain(int argc, _TCHAR* argv[])
//{
//	//GenerateFrameIndex();
//	h264es2ps();
//	return 0;
//}
//
//#endif
