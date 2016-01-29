#pragma once

#include "FFmpeg.h"
#include "HvDeviceCommDef.h"
#include <list>
using namespace std;

//h264 每帧数据
typedef struct _VideoFrameData
{
	PBYTE pbVideoData;
	DWORD dwVideoDataLength;
	DWORD dwVideoType;

	_VideoFrameData()
	{
		pbVideoData = NULL;
		dwVideoDataLength = 0;
		dwVideoType = 0;
	}

	~_VideoFrameData()
	{
		//在写时释放内存
		if(pbVideoData)
		{
			delete[] pbVideoData;
			pbVideoData = NULL;
		}
	}
}VideoFrameData;

typedef list<VideoFrameData*> VideoFrameDataList;

//作用：把H264流缓存在内存中。
//调用：Save函数，则写入磁盘。
//add by zengmx at 2014/9/28
class CVideoMemory
{
public:
	CVideoMemory(void);
	~CVideoMemory(void);

	//初始化解码、编码器
	static int H264VideoCodeInit()
	{
		av_register_all();
		return 0;
	}

	//加入一帧
	int AddOneFrame(PBYTE pbH264BitStream, int iSize, DWORD dwVideoType, int iWidth, int iHeight);

	//保存到磁盘，并清空缓存
	int SaveToDisk(const char* szOutputFile, int iFrameNum);

	//清空队列缓存
	int ClearVideoFrame();

	//是否为空缓存
	bool IsVideoNull();
private:
	char m_strFileName[512];
	int m_iWidth;
	int m_iHeight;
	int m_iFrameNum;

	CFFmpeg m_VideoFile;
	VideoFrameDataList m_VideoFrameDataList;
};

