#include "stdafx.h"
//#include <Windows.h>
#include "SaveH264Video.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SaveH264Video.h"

/*
extern int H264VideoSaverInit();
extern int H264VideoSaverOpen(const char* szOutputFile);
extern int H264VideoSaverWirteOneFrame(PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame);
extern int H264VideoSaverClose();
*/

namespace H264
{
	#pragma comment(lib, "avutil.lib")
	#pragma comment(lib, "avcodec.lib")
	#pragma comment(lib, "avformat.lib")
	#pragma comment(lib, "swscale.lib")
		extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavformat/avformat.h"
	#include "libswscale/swscale.h"
		}

	class H264DateSream
	{
	public:
		H264DateSream(void);
		~H264DateSream(void);
		int H264VideoSaverClose();
		int H264VideoSaverWirteOneFrame(PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame);
		// 说明：支持mp4、avi、mkv等视频封装格式。
		//int H264VideoSaverOpen(const char* szOutputFile, int iFrameNum);
		int H264VideoSaverOpen(const char* szOutputFile, int iFrameNum=12,int iWidth=1600, int iHeight=1088, int videoCodeRate=8000000);
		int H264VideoSaverInit();
		int H264VideoInit();
		int H264VideoOpen();
		int H264VideoClose();
	private:
		AVFormatContext *oc;
		AVStream *video_st;
		AVPacket avpkt;
		AVCodecContext *c;
		AVFrame *picture;
		int m_iFrame;
		CString m_strFileName;
	};

	H264DateSream::H264DateSream()
	{
		oc = NULL;
		video_st = NULL;
		c = NULL;
		picture = NULL;
	}

	H264DateSream::~H264DateSream(void)
	{

	}

	int H264DateSream::H264VideoSaverClose()
	{
		if ( oc != NULL )
		{
			__try
			{
				av_write_trailer(oc);
				for (unsigned int i = 0; i < oc->nb_streams; ++i)
				{
					av_freep(&oc->streams[i]->codec);
					av_freep(&oc->streams[i]);
				}
				url_fclose(oc->pb);
				av_free(oc);
				oc = NULL;
			}
			__except (EXCEPTION_EXECUTE_HANDLER) 
			{
				return -1;
			}
		}

		return 0;
	}

	int H264DateSream::H264VideoSaverWirteOneFrame(PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame)
	{
		AVPacket pkt;
		int ret = 0;
		__try 
		{
			av_init_packet(&pkt);
			if (fIsKeyFrame)
			{
				pkt.flags |= PKT_FLAG_KEY;
			}
			pkt.stream_index = video_st->index;
			pkt.data = pbH264BitStream;
			pkt.size = iSize;
			ret = av_write_frame(oc, &pkt);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)   
		{
			//out("get err frame %d\n", frame);
			//fprintf(stderr, "Error while decoding frame %d\n", frame);
			return -1;
		}
		return (ret!=0) ? (-1) : (0);
	}

	int H264DateSream::H264VideoSaverOpen(const char* szOutputFile, int iFrameNum,int iWidth, int iHeight, int videoCodeRate)
	{
		AVOutputFormat *fmt = guess_format(NULL,szOutputFile,NULL);
		if (!fmt)
		{
			return -1;
		}

		oc = av_alloc_format_context();
		if (!oc)
		{
			return -1;
		}

		oc->oformat = fmt;
		strcpy(oc->filename, szOutputFile);

		video_st = av_new_stream(oc,0);
		if (!video_st)
		{
			return -1;
		}

		video_st->codec->codec_id=CODEC_ID_H264;
		video_st->codec->codec_type=CODEC_TYPE_VIDEO;
		//video_st->codec->bit_rate=8000000;
		//video_st->codec->width=1600;
		//video_st->codec->height=1088;

		video_st->codec->bit_rate=videoCodeRate;
		video_st->codec->width=iWidth;
		video_st->codec->height=iHeight;
		
		AVRational time_base;
		time_base.num = 1;
		time_base.den = iFrameNum;
		video_st->codec->time_base=time_base;
		video_st->codec->gop_size=12;
		video_st->codec->pix_fmt=PIX_FMT_YUV422P;
		video_st->codec->max_b_frames=0;
		AVRational r_frame_rate;
		r_frame_rate.num = iFrameNum;
		r_frame_rate.den = 1;
		video_st->r_frame_rate = r_frame_rate;

		if (av_set_parameters(oc, NULL) < 0)
		{
			return -1;
		}
		strcpy(oc->title, "Dm6467-Video");
		strcpy(oc->author, "Shaorg");
		strcpy(oc->copyright, "Signalway");
		strcpy(oc->comment, "try it!");
		strcpy(oc->album, "NULL");
		oc->year = 2011;
		oc->track = 0;
		strcpy(oc->genre, "NULL");

		/*if (url_exist(szOutputFile))
		{
		return -1;
		}*/
		if (!(oc->flags & AVFMT_NOFILE))
		{
			if (url_fopen(&oc->pb,szOutputFile,URL_WRONLY)<0)
			{
				return -1;
			}
		}
		if (!oc->nb_streams)
		{
			return -1;
		}

		if (av_write_header(oc)<0)
		{
			return -1;
		}

		return 0;
	}

	int H264DateSream::H264VideoSaverInit()
	{
		av_register_all();
		return 0;
	}

	int H264DateSream::H264VideoInit()
	{
		avcodec_init();
		avcodec_register_all();
		return 0;
	}

	int H264DateSream::H264VideoOpen()
	{
		H264VideoClose();

		av_init_packet(&avpkt);

		/* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
		//memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

		AVCodec *codec = NULL;
		codec = avcodec_find_decoder(CODEC_ID_H264);
		if (!codec) {
			return -1;
		}

		c= avcodec_alloc_context();
		picture= avcodec_alloc_frame();

		if(codec->capabilities&CODEC_CAP_TRUNCATED)
			c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

		if (avcodec_open(c, codec) < 0) {
			return -1;
		}

		m_iFrame = 0;
		return 0;
	}

	int H264DateSream::H264VideoClose()
	{
		if ( c != NULL )
		{
			avcodec_close(c);
			av_free(c);
			av_free(picture);
			c = NULL;
			return 0;
		}
		return -1;
	}

}






//AVFormatContext *oc = NULL;
//AVStream *video_st = NULL;
//AVPacket avpkt;
//AVCodecContext *c = NULL;
//AVFrame *picture = NULL;
//
//int frame = 0;
//
//_H264_API int H264VideoSaverInit()
//{
//	av_register_all();
//	return 0;
//}
//
//// 说明：支持mp4、avi、mkv等视频封装格式。
//_H264_API int H264VideoSaverOpen(const char* szOutputFile)
//{
//	AVOutputFormat *fmt = guess_format(NULL,szOutputFile,NULL);
//	if (!fmt)
//	{
//		return -1;
//	}
//
//	oc = av_alloc_format_context();
//	if (!oc)
//	{
//		return -1;
//	}
//
//	oc->oformat = fmt;
//	strcpy(oc->filename, szOutputFile);
//
//	video_st = av_new_stream(oc,0);
//	if (!video_st)
//	{
//		return -1;
//	}
//
//	video_st->codec->codec_id=CODEC_ID_H264;
//	video_st->codec->codec_type=CODEC_TYPE_VIDEO;
//	video_st->codec->bit_rate=8000000;
//	video_st->codec->width=1600;
//	video_st->codec->height=1088;
//	AVRational time_base;
//	time_base.num = 1;
//	time_base.den = 12;
//	video_st->codec->time_base=time_base;
//	video_st->codec->gop_size=12;
//	video_st->codec->pix_fmt=PIX_FMT_YUV420P;
//	video_st->codec->max_b_frames=0;
//	AVRational r_frame_rate;
//	r_frame_rate.num = 12;
//	r_frame_rate.den = 1;
//	video_st->r_frame_rate = r_frame_rate;
//
//	if (av_set_parameters(oc, NULL) < 0)
//	{
//		return -1;
//	}
//	strcpy(oc->title, "Dm6467-Video");
//	strcpy(oc->author, "Shaorg");
//	strcpy(oc->copyright, "Signalway");
//	strcpy(oc->comment, "try it!");
//	strcpy(oc->album, "NULL");
//	oc->year = 2011;
//	oc->track = 0;
//	strcpy(oc->genre, "NULL");
//
//	/*if (url_exist(szOutputFile))
//	{
//		return -1;
//	}*/
//	if (!(oc->flags & AVFMT_NOFILE))
//	{
//		if (url_fopen(&oc->pb,szOutputFile,URL_WRONLY)<0)
//		{
//			return -1;
//		}
//	}
//	if (!oc->nb_streams)
//	{
//		return -1;
//	}
//
//	if (av_write_header(oc)<0)
//	{
//		return -1;
//	}
//
//	return 0;
//}
//
//_H264_API int H264VideoSaverWirteOneFrame(PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame)
//{
//	AVPacket pkt;
//	av_init_packet(&pkt);
//	if (fIsKeyFrame)
//	{
//		pkt.flags |= PKT_FLAG_KEY;
//	}
//	pkt.stream_index = video_st->index;
//	pkt.data = pbH264BitStream;
//	pkt.size = iSize;
//	int ret = av_write_frame(oc, &pkt);
//	return (ret!=0) ? (-1) : (0);
//}
//
//_H264_API int H264VideoSaverClose()
//{
//	if ( oc != NULL )
//	{
//		av_write_trailer(oc);
//		for (unsigned int i = 0; i < oc->nb_streams; ++i)
//		{
//			av_freep(&oc->streams[i]->codec);
//			av_freep(&oc->streams[i]);
//		}
//		url_fclose(oc->pb);
//		av_free(oc);
//		oc = NULL;
//	}
//
//	return 0;
//}
//
//_H264_API int H264VideoInit()
//{
//	avcodec_init();
//	avcodec_register_all();
//	return 0;
//}
//
//_H264_API int H264VideoOpen()
//{
//	H264VideoClose();
//
//	av_init_packet(&avpkt);
//
//	/* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
//	//memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);
//
//	AVCodec *codec = NULL;
//	codec = avcodec_find_decoder(CODEC_ID_H264);
//	if (!codec) {
//		return -1;
//	}
//
//	c= avcodec_alloc_context();
//	picture= avcodec_alloc_frame();
//
//	if(codec->capabilities&CODEC_CAP_TRUNCATED)
//		c->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
//
//	if (avcodec_open(c, codec) < 0) {
//		return -1;
//	}
//
//	frame = 0;
//	return 0;
//}
//
//_H264_API int H264VideoClose()
//{
//	if ( c != NULL )
//	{
//		avcodec_close(c);
//		av_free(c);
//		av_free(picture);
//		c = NULL;
//		return 0;
//	}
//	return -1;
//}

H264::H264DateSream* g_rgpH264Handle[256];
int g_iHandleIndex = 0;
CRITICAL_SECTION g_csSync;

_H264_API int H264VideoSaverInit()
{
	InitializeCriticalSection( &g_csSync );
	for (int i = 0; i < 256; i++)
	{
		g_rgpH264Handle[i] = NULL;
	}
	return 0;
}

_H264_API int H264VideoSaverCoInit()
{
	DeleteCriticalSection(&g_csSync);
	return 0;
}

//_H264_API void* H264VideoSaverOpen(const char* szOutputFile)
_H264_API void* H264VideoSaverOpen(const char* szOutputFile, int iFrameNum, int iWidth, int iHeight, int videoCodeRate)
{
	void* pvHandle = NULL;
	EnterCriticalSection(&g_csSync);
	int iIndex;
	for (iIndex = 0; iIndex < 256; iIndex++)
	{
		if (!g_rgpH264Handle[ iIndex ])
		{
			break;
		}
	}
	if (iIndex < 256)
	{
		g_rgpH264Handle[iIndex] = new H264::H264DateSream();
		pvHandle = (void*)g_rgpH264Handle[ iIndex ];
	}
	LeaveCriticalSection(&g_csSync);
	if (pvHandle)
	{
		((H264::H264DateSream*)pvHandle)->H264VideoSaverInit();
		((H264::H264DateSream*)pvHandle)->H264VideoSaverOpen(szOutputFile, iFrameNum,iWidth,iHeight, videoCodeRate);
	}
	
	return pvHandle;
}

_H264_API int H264VideoSaverWirteOneFrame(void* pvHandle, PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame)
{
	H264::H264DateSream* pH264DateSream = (H264::H264DateSream*)pvHandle;
	return pH264DateSream->H264VideoSaverWirteOneFrame(pbH264BitStream, iSize, fIsKeyFrame);
}

_H264_API int H264VideoSaverClose(void* pvHandle)
{
	__try
	{
		H264::H264DateSream* pH264DateSream = (H264::H264DateSream*)pvHandle;
		pH264DateSream->H264VideoSaverClose();
		for (int i = 0; i < 256; i++)
		{
			if (pH264DateSream == g_rgpH264Handle[i])
			{
				delete g_rgpH264Handle[i];
				g_rgpH264Handle[i] = NULL;
				return 1;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}

_H264_API int H264CheckDistSpace(char *pszDrive)
{
	//FARPROC pGetDiskFreeSpaceEx  =  GetProcAddress(GetModuleHandle( "kernel32.dll "), "GetDiskFreeSpaceExA ");
	//BOOL fResult;

	//if   (pGetDiskFreeSpaceEx) 
	//{ 
	//	LONG i64FreeBytesToCaller,i64TotalBytes,i64FreeBytes;
	//	fResult   =   pGetDiskFreeSpaceEx(pszDrive, 
	//		(PULARGE_INTEGER)&i64FreeBytesToCaller, 
	//		(PULARGE_INTEGER)&i64TotalBytes,
	//		(PULARGE_INTEGER)&i64FreeBytes); 

	//	//   Process   GetDiskFreeSpaceEx   results. 
	//} 

	//else   
	//{ 
		//DWORD dwSectPerClust,dwBytesPerSect,dwFreeClusters,dwTotalClusters;
		//fResult  =  GetDiskFreeSpace(pszDrive,   
		//	&dwSectPerClust,   
		//	&dwBytesPerSect, 
		//	&dwFreeClusters,   
		//	&dwTotalClusters);
			//   Process   GetDiskFreeSpace   results. 
	//}
    ULARGE_INTEGER nFreeBytesAvailable;
    ULARGE_INTEGER nTotalNumberOfBytes;
    ULARGE_INTEGER nTotalNumberOfFreeBytes;

    if (GetDiskFreeSpaceEx(_T(pszDrive),
        &nFreeBytesAvailable,
        &nTotalNumberOfBytes,
        &nTotalNumberOfFreeBytes))
    {
        TCHAR chBuf[256];
        wsprintf(chBuf,_T("Av=%I64d,Total=%I64d,Free=%I64d\r\n"),
            nFreeBytesAvailable,
            nTotalNumberOfBytes,
            nTotalNumberOfFreeBytes);
		DWORD dwHigh,dwLow;
		dwHigh = nFreeBytesAvailable.HighPart;
		dwLow = nFreeBytesAvailable.LowPart;
		DWORD dwFreeDist = dwHigh * 4 * 1024 + dwLow/ (1024 * 1024);
        //OutputDebugString(chBuf);
		return dwFreeDist;
    }

	return 0;

}
