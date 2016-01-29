#include "swpa.h"
#include "SWFC.h"
#include "SWClassFactory.h"

CSWClassFactory theClassFactory;

///////////////////////////////////////////////////////////////////////////////////////
CSWCreateClass::CSWCreateClass()
{
}

CSWCreateClass::~CSWCreateClass()
{
}

LPCSTR CSWCreateClass::GetObjectName(void)
{
	return NULL;
}

CSWObject* CSWCreateClass::CreateObject(const char *szclsName)
{
	return NULL;
}
///////////////////////////////////////////////////////////////////////////////////////
CSWClassFactory::CSWClassFactory()
{
}

CSWClassFactory::~CSWClassFactory()
{
}

CSWObject *CSWClassFactory::CreateObject(LPCSTR lpzclsName)
{
	SW_POSITION pos = m_lstClass.GetHeadPosition();
	while(m_lstClass.IsValid(pos))
	{
		CSWCreateClass* pClass = m_lstClass.GetNext(pos);
		if(!swpa_strcmp(pClass->GetObjectName(), lpzclsName))
		{
			return pClass->CreateObject(lpzclsName);
		}
	}
	return NULL;
}

HRESULT CSWClassFactory::Add(CSWCreateClass *pClass)
{
	if(NULL == m_lstClass.Find(pClass))
	{
		if(m_lstClass.IsFull())
		{
			m_lstClass.SetMaxCount(m_lstClass.GetCount() + 1);
		}
		m_lstClass.AddTail(pClass);
	}
	return S_OK;
}
	
void ClassInitialize()
{
	//摄像机控制命令模块
	LOAD_CLASS(CSWCameralControlMSG)
	LOAD_CLASS(CSWCameraControl2AMSG)

	//球机控制命令模块
	LOAD_CLASS(CSWDomeCameraControlMSG)

	
	//信号匹配模块
	LOAD_CLASS(CSWMatchTransformFilter)
	//后处理模块
	LOAD_CLASS(CSWResultFilter)
	//JPEG压缩模块
	LOAD_CLASS(CSWJPEGEncodeTransformFilter)
	//自动化控制模块
	LOAD_CLASS(CSWAutoControlRenderFilter)
	//H264保存模块
	LOAD_CLASS(CSWH264HDDTransformFilter)
	//H264网络发送模块
	LOAD_CLASS(CSWH264NetRenderFilter)
	//识别结果保存模块
	LOAD_CLASS(CSWResultHDDTransformFilter)
	//识别结果网络发送模块
	LOAD_CLASS(CSWResultNetRenderFilter)
	//JPEG调试码流网络发送模块
	LOAD_CLASS(CSWJPEGNetRenderFilter)
	//网络命令模块
	LOAD_CLASS(CSWNetCommandProcess)
	//识别模块
	LOAD_CLASS(CSWRecognizeTransformFilter)
	//识别模块
	LOAD_CLASS(CSWRecognizeTransformPTFilter)
	//识别模块
	LOAD_CLASS(CSWRecognizeTransformEPFilter)
    //识别模块
    LOAD_CLASS(CSWRecognizeTransformTollGateFilter)

	LOAD_CLASS(CSWRecognizeTGTransformFilter)
	//网络采集模块
	LOAD_CLASS(CSWNetSourceFilter)

	LOAD_CLASS(CSWYUVNetRenderFilter)
	// 触发输出模块
	LOAD_CLASS(CSWTriggerOut)
	//交通事件检测模块
	LOAD_CLASS(CSWEPoliceEventCheckerTransformFilter)

	//Record转换模块
	LOAD_CLASS(CSWRecordTransformFilter)

    //JPEGCach缓存模块
    LOAD_CLASS(CSWJPEGCachTransformFilter)
	


	//字符叠加模块
	LOAD_CLASS(CSWJPEGOverlayFilter)
	LOAD_CLASS(CSWH264OverlayFilter)
	LOAD_CLASS(CSWH264SecondOverlayFilter)
	LOAD_CLASS(CSWImage2SWPosImageTransformFilter)
	//JPEG解压模块
	LOAD_CLASS(CSWJPEGFilter)
#ifdef DM6467	
	//VPIF模块
	LOAD_CLASS(CSWVPIFSourceFilter)
	//H264采集模块
	LOAD_CLASS(CSWH264SourceFilter)
#endif

#ifdef DM8127
	LOAD_CLASS(CSWJPEGDecodeTransformFilter)
	LOAD_CLASS(CSWCentaurusSourceFilter)
	LOAD_CLASS(CSWIPNCTransformFilter)
	LOAD_CLASS(CSWCentaurusH264EncodeFilter)
	LOAD_CLASS(CSWCentaurusJpegEncodeFilter)
	LOAD_CLASS(CSWExtendDataTransformFilter)
#endif	
    //RTSP模块
    LOAD_CLASS(CSWRTSPTransformFilter)

    //GB28181模块
    LOAD_CLASS(CSWGBH264TransformFilter)

    //H264发送模块
    LOAD_CLASS(CSWH264Filter)

	//用户管理模块
    LOAD_CLASS(CSWUserManager)

	//球机控制模块
    LOAD_CLASS(CSWDomeRockerControlMSG)

	//FTP上传模块
	LOAD_CLASS(CSWResultFtpRenderFilter)

	//违章停车检测模块
	LOAD_CLASS(CSWRecognizeTransformPPFilter)

	//H264缓存模块
	LOAD_CLASS(CSWH264QueueRenderFilter)

	//结果取得录像模块
	LOAD_CLASS(CSWResultVideoTransformFilter)

	// MD5加密模块
	LOAD_CLASS(CSWMD5EncryptFilter)
}

