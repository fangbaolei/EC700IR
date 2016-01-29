// FFmpeg.cpp : implementation file
//

#include "stdafx.h"
#include "HvDeviceAx.h"
#include "FFmpeg.h"
#include "d3d_render.h"
#include "seh_exception_base.h"


//得到程序当前目录
CString GetCurrentDir(void)
{
	CString strPath;

	TCHAR szFileName[ MAX_PATH ] = {0};
	TCHAR szIniName[MAX_PATH] = {0};
	TCHAR szBakName[MAX_PATH] = {0};
	GetModuleFileName( NULL, szFileName, MAX_PATH );	//取得包括程序名的全路径
	PathRemoveFileSpec( szFileName );				//去掉程序名

	strPath = szFileName;
	strPath += "\\";
	return strPath;
}


void WriteTempLog(char *szLog)
{
	if (szLog == NULL)
		return ;

	CString strPath = GetCurrentDir();
	CTime tm = CTime::GetCurrentTime();
	MakeSureDirectoryPathExists(strPath.GetBuffer());
	strPath.ReleaseBuffer();
	CString szFileName;

	//szFileName.Format("%sError_%s.log", strPath, tm.Format("%Y%m%d"));

	szFileName.Format("%sError.log", strPath);

	FILE *myfile = fopen(szFileName, "w");
	if (myfile)
	{
		CTime tm = CTime::GetCurrentTime();
		fprintf(myfile, "%s : %s \n", tm.Format("%Y-%m-%d %H:%M:%S"), szLog);
		fclose(myfile);
	}

	return ;
}



// CFFmpeg

IMPLEMENT_DYNAMIC(CFFmpeg, CWnd)
CFFmpeg::CFFmpeg()
:m_video_st(NULL)
,m_video_oc(NULL)
,m_fOpenFile(FALSE)
{
}

CFFmpeg::~CFFmpeg()
{
}


BEGIN_MESSAGE_MAP(CFFmpeg, CWnd)
END_MESSAGE_MAP()


int CFFmpeg::H264VideoSaverInit()
{
	av_register_all();
	return 0;
}

// 说明：支持mp4、avi、mkv等视频封装格式。
int CFFmpeg::H264VideoSaverOpen(const char* szOutputFile, int iFrameNum, int iWith, int iHeight)
{
	char* szOutputFilename = new char[512];
	memset(szOutputFilename, 0, 512);
	ANSIToUTF8(szOutputFile, szOutputFilename);

	AVOutputFormat *fmt = av_guess_format(NULL,szOutputFilename,NULL);
	if (!fmt)
	{
		if (szOutputFilename)
		{
			delete[] szOutputFilename; 
		}
		return -1;
	}

	m_video_oc = avformat_alloc_context();
	if (!m_video_oc)
	{
		if (szOutputFilename)
		{
			delete[] szOutputFilename; 
		}
		return -1;
	}

	m_video_oc->oformat = fmt;
	strcpy(m_video_oc->filename, szOutputFile);

	m_video_st = avformat_new_stream(m_video_oc,0);
	if (!m_video_st)
	{
		if (szOutputFilename)
		{
			delete[] szOutputFilename; 
		}
		return -1;
	}

	m_video_st->codec->codec_id = CODEC_ID_H264;
	m_video_st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
	m_video_st->codec->bit_rate = 8000000;
	m_video_st->codec->width = iWith;
	m_video_st->codec->height = iHeight;
	AVRational time_base;
	time_base.num = 1;
	time_base.den = iFrameNum;
	m_video_st->codec->time_base = time_base;
	m_video_st->codec->gop_size = 12;
	m_video_st->codec->pix_fmt = PIX_FMT_YUV420P;
	m_video_st->codec->max_b_frames = 0;
	AVRational r_frame_rate;
	r_frame_rate.num = iFrameNum;
	r_frame_rate.den = 1;
	m_video_st->r_frame_rate = r_frame_rate;

	/*if (av_set_parameters(m_video_oc, NULL) < 0)
	{
		if (szOutputFilename)
		{
			delete[] szOutputFilename; 
		}
		return -1;
	}*/

	/*if (url_exist(szOutputFilename))
	{
	return -1;
	}*/
	if (!(m_video_oc->flags & AVFMT_NOFILE))
	{
		/* if (url_fopen(&m_video_oc->pb,szOutputFilename,URL_WRONLY)<0) */
		if (avio_open(&m_video_oc->pb,szOutputFilename,AVIO_FLAG_READ_WRITE))
		{
			if (szOutputFilename)
			{
				delete[] szOutputFilename; 
			}
			return -1;
		}
	}
	if (!m_video_oc->nb_streams)
	{
		if (szOutputFilename)
		{
			delete[] szOutputFilename; 
		}
		return -1;
	}

	if (avformat_write_header(m_video_oc , NULL)<0)
	{
		if (szOutputFilename)
		{
			delete[] szOutputFilename; 
		}
		return -1;
	}
	m_fOpenFile = TRUE;
	if (szOutputFilename)
	{
		delete[] szOutputFilename; 
	}
	return 0;
}

int CFFmpeg::H264VideoSaverWirteOneFrame(PBYTE pbH264BitStream, int iSize, bool fIsKeyFrame)
{
	AVPacket pkt;
	int ret;
	__try
	{
		av_init_packet(&pkt);
		if (fIsKeyFrame)
		{
			pkt.flags |= AV_PKT_FLAG_KEY;
		}
		pkt.stream_index = m_video_st->index;
		pkt.data = pbH264BitStream;
		pkt.size = iSize;
		ret = av_write_frame(m_video_oc, &pkt);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}
	return (ret!= 0) ? (-1) : (0);
}

int CFFmpeg::H264VideoSaverClose()
{
	__try
	{
		if ( m_video_oc != NULL )
		{
			av_write_trailer(m_video_oc);
			for (unsigned int i = 0; i < m_video_oc->nb_streams; ++i)
			{
				av_freep(&m_video_oc->streams[i]->codec);
				av_freep(&m_video_oc->streams[i]);
			}
			//url_fclose(m_video_oc->pb);
			avio_close(m_video_oc->pb);
			av_free(m_video_oc);
			m_video_oc = NULL;
			m_video_st = NULL;
		}
		m_fOpenFile = FALSE;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return -1;
	}

	return 0;
}


CH264FrameDecode::CH264FrameDecode(void)
{
	m_pCodecCtx = NULL;
	m_pavfFrame = NULL;
	m_pVideoRender = NULL;
	m_iWidth = 1920;
	m_iHeight = 1080;
	m_showhWnd = NULL;
	Init();
}

CH264FrameDecode::~CH264FrameDecode(void)
{
	Release();
}

BOOL CH264FrameDecode::H264Env_Init()
{
	av_register_all();
	
	return TRUE;
}
BOOL CH264FrameDecode::H264Env_Release()
{
	return TRUE;
}



BOOL CH264FrameDecode::Init()
{
	Release();

	AVCodec *pCodec = avcodec_find_decoder(CODEC_ID_H264);
	if ( pCodec != NULL )
	{
		m_pCodecCtx = avcodec_alloc_context3(pCodec);
		m_pCodecCtx->time_base.num = 1;
		m_pCodecCtx->time_base.den = 25;
		m_pCodecCtx->bit_rate = 0;
		m_pCodecCtx->frame_number = 1;
		m_pCodecCtx->pix_fmt = PIX_FMT_YUV420P;
		//m_pCodecCtx->width = m_iWidth;
		//m_pCodecCtx->height = m_iHeight;
	}
	if (m_pCodecCtx != NULL && avcodec_open2(m_pCodecCtx, pCodec,NULL) >=0)
	{
		m_pavfFrame = avcodec_alloc_frame();
	}


	return TRUE;
}

BOOL CH264FrameDecode::Release()
{
	if ( m_pavfFrame != NULL )
	{
		av_free(m_pavfFrame);
		m_pavfFrame = NULL;
	}

	if ( m_pCodecCtx != NULL )
	{
		avcodec_close(m_pCodecCtx);
		av_free(m_pCodecCtx);

		m_pCodecCtx = NULL;
	}

	if ( m_pVideoRender != NULL)
	{
		delete m_pVideoRender;
		m_pVideoRender = NULL;
	}

	return TRUE;
}


BOOL CH264FrameDecode::H264_Decode(const PBYTE pSrcData, const DWORD dwDataLen, int *pnWidth, int  *pnHeight, HWND hWnd)
{
	
	if ( m_pCodecCtx == NULL ||  m_pavfFrame == NULL)
	{
		Init();
	}

	BOOL nGot = FALSE;
	int len = 0;

	try
	{
		//填入待解码的比特流
		av_init_packet(&m_AVPkt);
		m_AVPkt.data = (uint8_t*)pSrcData;
		m_AVPkt.size = dwDataLen;

		avcodec_decode_video2(m_pCodecCtx, m_pavfFrame, (int*)&nGot, &m_AVPkt);

		if (nGot)
		{

			if ( pnWidth != NULL )
			 *pnWidth = m_pavfFrame->width;
			if ( pnHeight != NULL )
			 *pnHeight = m_pavfFrame->height;

			if ( hWnd== NULL )
				return true;

			if((m_pavfFrame->height != m_iHeight  &&  m_pavfFrame->width != m_iWidth) || m_showhWnd != hWnd )
			{
				m_iWidth = m_pavfFrame->width;
				m_iHeight = m_pavfFrame->height;
				m_showhWnd = hWnd;

				if ( m_pVideoRender != NULL )
				{
					delete[] m_pVideoRender;
					m_pVideoRender = NULL;
				}

				if  (m_pVideoRender == NULL )
				{
					m_pVideoRender = new d3d_render();
					if ( m_pVideoRender != NULL )
						m_pVideoRender->init_render(m_showhWnd, m_iWidth, m_iHeight, PIX_FMT_YUV420P);
				}
			
			}

			if ( m_pVideoRender != NULL )
			{
				//直接显示
				m_pVideoRender->render_one_frame(m_pavfFrame, PIX_FMT_YUV420P);
			}
			else
			{
				m_iWidth = 0;
				m_iHeight = 0;

				//m_pVideoRender->init_render(hWnd, m_iWidth, m_iHeight, PIX_FMT_YUV420P);
			}

		}
		else
		{
			OutputDebugString("avcodec_decode_video Fail\n");
		}
	}
	catch (CException* e)
	{
		TCHAR   szCause[255];
		CString strFormatted;
		e->GetErrorMessage(szCause, 255);

		char chTemp[1024] = {0};
		sprintf_s(chTemp, " H264_Decode 产生异常 %s", szCause);
		WriteTempLog(chTemp);
		e->Delete();
	}
	catch( seh_exception_access_violation& e ) 
	{ 
		char pchTemp[1024] = { 0 };
		sprintf_s(pchTemp, "H264_Decode Caught SEH_Exception. 错误原因： %s\n", e.what()); 
		OutputDebugString(pchTemp);
		WriteTempLog(pchTemp);

	} 
	catch(exception &r)
	{
		char pchTemp[1024] = { 0 };
		sprintf_s(pchTemp, "H264_Decode  exception 错误原因： %s\n", r.what()); 
		WriteTempLog(pchTemp);
	}
	catch (...)
	{
		WriteTempLog("H264_Decode 产生SHE异常");
	}

	return nGot;


/*a

	while (m_AVPkt.size > 0) 
	{
		//__try
		//{

			len = avcodec_decode_video2(m_pCodecCtx, m_pavfFrame, (int*)&nGot, &m_AVPkt);
			

		//}
		//__except (EXCEPTION_EXECUTE_HANDLER)
		//{
		//	return -1;
		//}

		m_pVideoRender->render_one_frame(m_pavfFrame, PIX_FMT_YUV420P);

		m_AVPkt.size -= len;
		m_AVPkt.data += len;
	}

	/* some codecs, such as MPEG, transmit the I and P frame with a
	latency of one frame. You must do the following to have a
	chance to get the last frame of the video */

	m_AVPkt.data = NULL;
	m_AVPkt.size = 0;

	len = avcodec_decode_video2(m_pCodecCtx, m_pavfFrame, (int*)&nGot, &m_AVPkt);

	/*if (nGot) {
		//my_size = picture->width * picture->height;
		//SaveYUV420P(m_AVCodeContext, m_AVFrame, pbYuv420p, nOutBufMaxSize);
		//A_TRACE_F(T_INFO, "dw: %d, dh: %d\n", picture->width, picture->height);
		m_nFrameNum++;
	}*/

	return true;

}







// CFFmpeg message handlers

