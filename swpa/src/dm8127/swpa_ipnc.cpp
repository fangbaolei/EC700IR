#include "swpa.h"
#include "swpa_private.h"
#include "swpa_ipnc.h"
#include "arch.h"
#include "drv_device.h"

#define FP_FW_VPSSM3  	"/opt/ipnc/bin/ipnc_rdk_fw_m3vpss.xem3"
#define FP_FW_VIDEOM3 	"/opt/ipnc/bin/ipnc_rdk_fw_m3video.xem3"
#define FP_FW_DSP     	"/tmp/ipnc_rdk_fw_c6xdsp.xe674"		//todo:只读文件系统，解密后位于/tmp目录
//#define FP_FW_DSP     	"/opt/ipnc/bin/ipnc_rdk_fw_c6xdsp.xe674"
#define FP_BIN_RTDBG  	"/opt/ipnc/bin/remote_debug_client.out"
#define FP_BIN_SYSPRI 	"/opt/ipnc/bin/sys_pri.out"
#define FP_BIN_FWLOAD 	"/opt/ipnc/bin/fw_load.out"
#define FP_BIN_PRCM   	"/opt/ipnc/bin/linux_prcm_ipcam"
#define FP_BIN_MEMRDWR	"/opt/ipnc/bin/mem_rdwr.out"
#define FP_KO_OSA     	"/opt/ipnc/ko/osa_kermod.ko"
#define FP_KO_SYSLINK 	"/opt/ipnc/ko/syslink.ko"
#define FP_KO_FB      	"/opt/ipnc/ko/ti81xxfb.ko"
#define FP_KO_HDMI    	"/opt/ipnc/ko/ti81xxhdmi.ko"
#define FP_KO_VPSS    	"/opt/ipnc/ko/vpss.ko"
//debug
#define SWPA_IPNC_VNF_MATCH_INFO(fmt, ...) //SWPA_PRINT(fmt, ##__VA_ARGS__)

#define SWPA_IPNC_WARN(fmt, ...) SWPA_PRINT(fmt, ##__VA_ARGS__)
//#define SWPA_IPNC
#ifdef SWPA_IPNC
#define SWPA_IPNC_PRINT(fmt, ...) SWPA_PRINT(fmt, ##__VA_ARGS__)
#define SWPA_IPNC_CHECK(arg)      {if (!(arg)){SWPA_PRINT("[%s:%d]Check failed : %s [%d]\n", __FILE__, __LINE__, #arg, SWPAR_INVALIDARG);return SWPAR_INVALIDARG;}}
#else
#define	SWPA_IPNC_PRINT(fmt, ...)
#define SWPA_IPNC_CHECK(arg)
#endif

/*app:0-30000us
*fpga:0-2703
*11.1=30000/2703
*/
#define IMX178_SHUTTER_UNIT 11.1f
#define IMX178_GAIN_UNIT 1.0f

/*app:0-30000us
*fpga:0-1013
*29.6=30000/1013
*/
#define IMX185_SHUTTER_UNIT 29.6f
#define IMX185_GAIN_UNIT 3.0f

/*app:0-30000us
*fpga:0-1013
*29.6=30000/1013
*/
#define IMX249_SHUTTER_UNIT 29.6f
#define IMX249_GAIN_UNIT 1.0f

/*app:0-30000us
*fpga:0-765
*39.2=30000/765
*/
#define ICX816_SHUTTER_UNIT 39.2f
#define ICX816_GAIN_UNIT 0.359f

/*app:0-255
*fpga:0-114688
*449=114688/255
*/
#define FPGA_RGB_GAIN_STEP 449


// 使用的USECASE
static int s_iUseIMX = IMX185;
// 使用的Sensor
static int s_iSensorType = SENSOR_IMX185;


////////////////////////////////////////////////////////////////
typedef struct tagipc_node
{
	int   link_id;
	int   ref;
	void* addr;
	void* phys;
	int   ptr_type; //0:frame, 1:bit;
	void* ptr;
	int   resample;
	int   width;
	int   height;
	// todo.
	// 截图需要保存的信息。
	// 当前截图需要修改内存指针首地址，不能通过设置偏移量来实现。
	int   resize;
	int   offsetY;
	int   offsetUV;
	void* addr_resize;
	void* phys_resize;
	// cvbs
	void* addr_cvbs;
	void* phys_cvbs;
	int cvbs_width;
	int cvbs_height;
	int cvbsFlag;
	//fpga frame info, for match
	unsigned int fpga_time_stamp;
	int channel;
	tagipc_node()
	{
		swpa_memset(this, 0, sizeof(*this));
	}
}ipc_node;

typedef struct tagfunc_node
{
	int (*callback_func)(void *pContext, int type, void *struct_ptr);
	void *func_param;
	tagfunc_node()
	{
		swpa_memset(this, 0, sizeof(*this));
	}
}func_node;

func_node g_func[SWPA_LINK_MAX][SWPA_VPSS_CHANNEL_MAX];

metadata_callback_func g_metadata_callback_func = NULL;
void *g_metadata_func_param = NULL;


int g_hlst_memory = 0;
int g_hlst_release_vnf = 0;		//vnf对应的未降噪帧释放队列，vnf通道数据返回比较慢，当未降噪数据已经释放时，vnf帧还未返回时，放入该队列

int g_vnflistlock = 0; 
//debug
int g_vnfframecnt = 0;

int g_finitialize = 0;

int get_vnf_frame(unsigned int timestamp, ipc_node **vnfnode);


//将未能释放vnf加入队列
int add_vnf_release(unsigned int timestamp);

//判断是否已经释放
bool is_vnf_already_release(unsigned int timestamp);

int swpa_ipnc_mode(void)
{
	return s_iUseIMX;
}

int swpa_get_sensor_type(void)
{
	return s_iSensorType;
}

////////////////////////////////////////////////////////////////
//todo:下面的函数仅支持YUV420SP
int Convert_VIDFrame_Buf_to_Data(VIDEO_FRAMEBUF_S *buf, IPNC_DATA *data)
{
	if(buf && data)
	{	//数据
		data->addr[0] = (char *)buf->addr[0][0] + buf->framePitch[0]*buf->startY + buf->startX;
		data->addr[1] = (char *)buf->addr[0][1] + buf->framePitch[1]*buf->startY/2 + buf->startX;
		data->addr[2] = NULL;
		data->phys[0] = (char *)buf->phyAddr[0][0] + buf->framePitch[0]*buf->startY + buf->startX;
		data->phys[1] = (char *)buf->phyAddr[0][1] + buf->framePitch[1]*buf->startY/2 + buf->startX;
		data->phys[2] = NULL;
		data->size = 1.5*buf->frameWidth * buf->frameHeight;
		data->appData = buf->pUserSpecDataVirAddr;
		data->dataSize = buf->nUserSpecDataSize;
		return SWPAR_OK;
	}
	return SWPAR_FAIL;
}

int Convert_VIDFrame_Buf_to_Image(VIDEO_FRAMEBUF_S *buf, IMAGE *image)
{
	if(buf && image)
	{	//数据
		swpa_memset(image, 0,sizeof(IMAGE));
		VIDEO_FRAMEBUF_S framebufinfo;
		swpa_memcpy(&framebufinfo, buf, sizeof(VIDEO_FRAMEBUF_S));
		
		if (SWPA_VPSS_JPEG_SECOND_CHANNEL == buf->channelNum	//todo 这一路为采集之后直接引用，VNF降噪前会进行扩边，应用层相应减掉
			&& (ICX816 == s_iUseIMX 
				|| ICX816_600W == s_iUseIMX 
				|| ICX816_640W == s_iUseIMX
				|| ICX816_SINGLE == s_iUseIMX)
			)
		{
			framebufinfo.frameWidth -= 256;
			framebufinfo.frameHeight -= 40;
			framebufinfo.startX = 16;
			framebufinfo.startY = 18;
			//debug
			/*static int count = 0;
			if (count++%100 == 0)
				printf("channel:%d,w:%d,h:%d,pitch[0]=%d,pitch[1]=%d,startx:%d,starty:%d\n",
					buf->channelNum,buf->frameWidth,buf->frameHeight,buf->framePitch[0],buf->framePitch[1],
					buf->startX,buf->startY);*/
		}
		

		Convert_VIDFrame_Buf_to_Data(&framebufinfo, &image->data);
		image->channel  = framebufinfo.channelNum;
		image->type = SWPA_IMAGE_YUV;
		image->pitch = framebufinfo.framePitch[0];
		image->width = framebufinfo.frameWidth;
		image->height = framebufinfo.frameHeight;

		//图像附加信息
		if(buf->pImageSpecDataVirAddr && buf->nImageSpecDataSize >= sizeof(IPC_CAPTURE_IMAGE_INFO_Y) )
		{
			IPC_CAPTURE_IMAGE_INFO_Y *info = (IPC_CAPTURE_IMAGE_INFO_Y *)buf->pImageSpecDataVirAddr;

			// 需要转换
			static int siMode = swpa_get_sensor_type();
			
			float fltShutterUnit = IMX185_SHUTTER_UNIT;
			float fltGainUnit = IMX185_GAIN_UNIT;
			int iRGBUnit = FPGA_RGB_GAIN_STEP;

			switch (siMode)
			{
				case SENSOR_IMX185:
					fltShutterUnit = IMX185_SHUTTER_UNIT;
					fltGainUnit = IMX185_GAIN_UNIT;
					break;
				case SENSOR_IMX178:
					fltShutterUnit = IMX178_SHUTTER_UNIT;
					fltGainUnit = IMX178_GAIN_UNIT;
					break;
				case SENSOR_IMX174:
				case SENSOR_IMX249:
					fltShutterUnit = IMX249_SHUTTER_UNIT;
					fltGainUnit = IMX249_GAIN_UNIT;
					break;
				case SENSOR_ICX816:
					fltShutterUnit = ICX816_SHUTTER_UNIT;
					fltGainUnit = ICX816_GAIN_UNIT;
					break;
				default:
					break;
			}

			image->shutter = (int)((float)(info->shutter_v) * fltShutterUnit);
			image->gain = (int)((float)(info->vgagain_a) * fltGainUnit);
			image->rGain = info->r_gain_v / iRGBUnit;
			image->gGain = info->g_gain_v / iRGBUnit;
			image->bGain = info->b_gain_v / iRGBUnit;

			image->iCaptureEdgeExt 		= info->capture_edge_ext;
			image->isCapture 			= info->capture_en;
			image->iCaptureFlag 		= info->capture_inf;
			image->iCaptureEnableFlag 	= info->capture_en_flag;
			image->iCaptureShutter		= (int)((float)(info->shutter_c) * fltShutterUnit);
			image->iCaptureGain			= (int)((float)(info->vga_gain_c_1) * fltGainUnit);
			image->iCaptureRGain		= info->r_gain_c / iRGBUnit;
			image->iCaptureGGain		= info->g_gain_c / iRGBUnit;
			image->iCaptureBGain		= info->b_gain_c / iRGBUnit;
			image->uTimeStamp 			= info->time_cnt_out_vd;

	
			//todo:临时做法，过滤多余抓拍图
			if (1 == image->channel/*JPEG 通道*/
				&& image->isCapture)
			{	
				static int image_time_stamp[2] = {0,0};		//上两张抓拍图的时间信息
				//与上一张抓拍图时间戳相同或者与上上一张抓拍图时间戳相同则过滤掉
				if (info->time_cnt_out_vd == image_time_stamp[1] || info->time_cnt_out_vd == image_time_stamp[0])
				{
					/*printf("=== fiter capture image, last image:0x%x,0x%x . cur image:0x%x ===",
						image_time_stamp[0],image_time_stamp[1],info->time_cnt_out_vd);*/
					image->isCapture = 0;
				}
				image_time_stamp[0] = image_time_stamp[1];
				image_time_stamp[1] = info->time_cnt_out_vd;
			}

			unsigned int uAvgY = 0;
            unsigned int rgAvgY[] = {info->avg_y_0,info->avg_y_1,info->avg_y_2,info->avg_y_3,
                    info->avg_y_4,info->avg_y_5,info->avg_y_6,info->avg_y_7,
                    info->avg_y_8,info->avg_y_9,info->avg_y_10,info->avg_y_11,
                    info->avg_y_12,info->avg_y_13,info->avg_y_14,info->avg_y_15};
            int i = 0;
            int iCount = 16;
            int iAvgY = 0;
            for( i = 0; i < 16; ++i )
            {
                iAvgY += (rgAvgY[i]);
            }
            uAvgY = iAvgY / iCount;
            image->iAvgY = uAvgY;


        	// 计算RAW图（未解压数据）的亮度值
        	int iRawY = 0.59 * (info->Sum_G_raw >> 1)
        					   + 0.30 * info->Sum_R_raw
        				       + 0.11 * info->Sum_B_raw;
        	// 取出亮度平均值
        	iRawY /= ((image->width * image->height) >> 2);
        	image->iRawAvgY = iRawY;


			if( image->channel == 1
					&& (0 != info->sum_y || info->sum_u > 1100) )
			{
				//printf("<123ooyy>++++++++++++++++++++++++++++++++++++++++++++++++++<<<<\n");
				//printf("<123ooyy>++++++++++data_ddr_W:%d++++V_counter_syn:%d++++++++++<<<<\n",
				//		info->sum_y, info->sum_u);
				//printf("<123ooyy>++++++++++++++++++++++++++++++++++++++++++++++++++<<<<\n");
				//printf("<123ooyy>++++++++++++++++++++++++++++++++++++++++++++++++++<<<<\n");

				//printf("<123fpga>data_ddr_W:%08x,V_counter_syn:%d.", info->sum_y, info->sum_u);
			}

		}
		return SWPAR_OK;
	}
	return SWPAR_FAIL;
}

int Convert_BitFrame_Buf_to_Image(VCODEC_BITSBUF_S *buf, IMAGE *image)
{
	if(buf && image)
	{
		swpa_memset(image, 0, sizeof(IMAGE));
		image->data.addr[0] = (char *)buf->bufVirtAddr + buf->startOffset;
		image->data.phys[0] = (char *)buf->bufPhysAddr + buf->startOffset;		
		image->data.size = buf->filledBufSize;
		image->data.appData = NULL;
		image->data.dataSize = NULL;
		image->channel = buf->chnId;
		image->frameType = buf->frameType;
		image->width = buf->frameWidth;
		image->height = buf->frameHeight;
		
		switch(buf->codecType)
		{
		case VCODEC_TYPE_MJPEG:
			image->type = SWPA_IMAGE_JPEG;
			break;
		case VCODEC_TYPE_H264BP:
		case VCODEC_TYPE_H264MP:
		case VCODEC_TYPE_H264HP:
			image->type = SWPA_IMAGE_H264;
			break;
		case VCODEC_TYPE_MPEG4:
		case VCODEC_TYPE_MPEG2:
			break;
		}
		SWPA_IPNC_PRINT("buf->codecType=%d, image->type=%d\n", buf->codecType, image->type);
		return SWPAR_OK;
	}
	return SWPAR_FAIL;
}

int call_callback_func(int link_id, int channel, int type, void *struct_ptr)
{
	if(SWPA_LINK_ALL < link_id 
		&& link_id < SWPA_LINK_MAX 
		&& channel < SWPA_VPSS_CHANNEL_MAX 
		&& g_func[link_id][channel].callback_func 
		&& g_func[link_id][channel].callback_func(g_func[link_id][channel].func_param, type, struct_ptr))
	{
		return SWPAR_OK;
	}
	return SWPAR_FAIL;
}

void OnVpssOutData(void *pContext)
{
	int link_id = (int)pContext;
	VIDEO_FRAMEBUF_LIST_S bufFrames;
	swpa_memset(&bufFrames, 0, sizeof(bufFrames));

	switch(link_id)
	{
		case SWPA_LINK_VPSS  : //vpss
			{
				IPC_GetEmptyVpssM3Frames(&bufFrames);
				for(int i = 0; i < bufFrames.numFrames; i++)
				{
					swpa_ipnc_release(bufFrames.frames[i].phyAddr[0][0], 1);
				}
				break;
			}
		default:
			break;
	}
}
void OnIPNCInData(void *pContext)
{
	int link_id = (int)pContext;
	VIDEO_FRAMEBUF_LIST_S bufFrames;
	VCODEC_BITSBUF_LIST_S bufsBits;
	swpa_memset(&bufFrames, 0, sizeof(bufFrames));
	swpa_memset(&bufsBits,  0, sizeof(bufsBits));
	
	switch(link_id)
	{
	case SWPA_LINK_VPSS  : //vpss,yuv
		{	
			IPC_GetFullVpssM3Frames(&bufFrames);
			for(int i = 0; i < bufFrames.numFrames; i++)
			{	//回调数据
				IMAGE image;
				Convert_VIDFrame_Buf_to_Image(&bufFrames.frames[i], &image);
				//先放入链表
				ipc_node *node = new ipc_node;
				node->link_id = link_id;
				node->ref = 1;
				node->addr = image.data.addr[0];
				node->phys = bufFrames.frames[i].phyAddr[0][0];
				node->ptr_type = 0;
				node->fpga_time_stamp = image.uTimeStamp;
				node->channel = image.channel;
				node->ptr = new VIDEO_FRAMEBUF_S;
				//更换通道
				swpa_memcpy(node->ptr, &bufFrames.frames[i], sizeof(bufFrames.frames[i]));

				SWPA_IPNC_VNF_MATCH_INFO("channel:%d, addr:0x%x, phys:0x%x,ts:0x%x, tick:%d\n",
					image.channel, node->addr, node->phys, image.uTimeStamp, swpa_datetime_gettick());
				
				
				if (SWPA_VPSS_JPEG_CHANNEL == image.channel //vnf yuv to encode jpeg
 					&& (ICX816 == s_iUseIMX 
						|| ICX816_600W == s_iUseIMX 
						|| ICX816_640W == s_iUseIMX
						|| ICX816_SINGLE == s_iUseIMX))
				{
					swpa_mutex_lock(&g_vnflistlock,100);
					if (true == is_vnf_already_release(node->fpga_time_stamp))
					{
						//reture buf to m3 now
						VIDEO_FRAMEBUF_LIST_S bufs;
						bufs.numFrames = 1;
						swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));

						delete (VIDEO_FRAMEBUF_S *)node->ptr;
						node->ptr = NULL;
						
						SWPA_IPNC_VNF_MATCH_INFO("vnf release frame channel:%d,ts;0x%x\n",node->channel, node->fpga_time_stamp);
						delete node;
						IPC_PutEmptyVpssM3Frames(&bufs);
					}
					else
					{
						g_vnfframecnt++;
						SWPA_IPNC_VNF_MATCH_INFO("add vnf frame to list.addr:0x%x,phys:0x%x,ts:0x%x\n",
							node->addr, node->phys, node->fpga_time_stamp);
						swpa_list_add(g_hlst_memory, node);
					}
					swpa_mutex_unlock(&g_vnflistlock);
					
				}
				else
				{
					swpa_list_add(g_hlst_memory, node);

	            	// todo.
					if(0 == image.channel 
						&& (s_iUseIMX == IMX185_CVBSEXPORT || s_iUseIMX == IMX185_DUALVIDEO_CVBSEXPORT))
					{
						int iSize = (image.pitch*image.height/2);
						node->addr_cvbs = image.data.addr[1] + iSize;
						node->phys_cvbs = image.data.phys[1] + iSize;

						node->cvbs_width = 720;
						node->cvbs_height = 576;
						node->cvbsFlag = 4;
					}
					
					if(SWPAR_FAIL == call_callback_func(link_id, image.channel, CALLBACK_TYPE_IMAGE, &image))
					{
						SWPA_IPNC_PRINT("vpss call_callback_func failed release, channel:%d\n", image.channel);
						swpa_ipnc_release(node->phys, 1);
					}
				}
			}
			break;
		}
	case SWPA_LINK_VIDEO : //video,h264 or jpeg
		{
			IPC_GetFullVideoM3Bits(&bufsBits);
			for(int i = 0; i < bufsBits.numBufs; i++)
			{ //回调数据
				IMAGE image;
				Convert_BitFrame_Buf_to_Image(&bufsBits.bitsBuf[i], &image);
				//保存节点
				ipc_node *node = new ipc_node;
				node->link_id = link_id;
				node->ref = 1;
				node->addr = image.data.addr[0];
				node->phys = bufsBits.bitsBuf[i].bufPhysAddr;
				node->ptr_type = 1;
				node->ptr = new VCODEC_BITSBUF_S;
				swpa_memcpy(node->ptr, &bufsBits.bitsBuf[i], sizeof(bufsBits.bitsBuf[i]));
				swpa_list_add(g_hlst_memory, node);

				//回调
				if(SWPAR_FAIL == call_callback_func(link_id, image.channel, CALLBACK_TYPE_IMAGE, &image))
				{
					swpa_ipnc_release(node->phys, 1);
				}
			}
			break;
		}
	case SWPA_LINK_DSP   : //dsp,data
		{
			IPC_GetFullDspData(&bufFrames);
			for(int i = 0; i < bufFrames.numFrames; i++)
			{	//回调数据
				IPNC_DATA data;
				Convert_VIDFrame_Buf_to_Data(&bufFrames.frames[i], &data);
				//保存
				ipc_node *node = new ipc_node;
				node->link_id = link_id;
				node->ref = 1;
				node->addr = data.addr[0];
				node->phys = bufFrames.frames[i].phyAddr[0][0];
				node->ptr_type = 0;
				node->ptr = new VIDEO_FRAMEBUF_S;
				swpa_memcpy(node->ptr, &bufFrames.frames[i], sizeof(bufFrames.frames[i]));
				swpa_list_add(g_hlst_memory, node);
				//回调
				if(SWPAR_FAIL == call_callback_func(link_id, bufFrames.frames[i].channelNum, CALLBACK_TYPE_DATA, &data))
				{
					swpa_ipnc_release(node->phys, 1);
				}
			}
			break;
		}
	}
}

void OnIPNCOutData(void *pContext)
{
	int link_id = (int)pContext;
	SWPA_IPNC_PRINT("OnIPNCOutData link id:%d\n", link_id);
	VIDEO_FRAMEBUF_LIST_S bufFrames;
	swpa_memset(&bufFrames, 0, sizeof(bufFrames));
	
	switch(link_id)
	{
	case SWPA_LINK_VIDEO :
		{
			IPC_GetEmptyVideoM3Frames(&bufFrames);
			for(int i = 0; i < bufFrames.numFrames; i++)
			{
				SWPA_IPNC_PRINT("OnIPNCOutData SWPA_LINK_VIDEO, swpa_ipnc_release\n");
				swpa_ipnc_release(bufFrames.frames[i].phyAddr[0][0], 1);
			}
		}		
		break;
		
	case SWPA_LINK_DSP :
		{
			IPC_GetEmptyDspFrames(&bufFrames);
			for(int i = 0; i < bufFrames.numFrames; i++)
			{
				SWPA_IPNC_PRINT("OnIPNCOutData SWPA_LINK_DSP, swpa_ipnc_release\n");
				swpa_ipnc_release(bufFrames.frames[i].phyAddr[0][0], 1);
			}
		}
		break;
	}
}

void JPEGFrameInCb(void *pContext)
{
	VIDEO_FRAMEBUF_LIST_S bufFrames;
	IPC_GetFullVideoM3Frames(&bufFrames);
	for(int i = 0; i < bufFrames.numFrames; i++)
	{	//回调数据
		IMAGE image;
		Convert_VIDFrame_Buf_to_Image(&bufFrames.frames[i], &image);
		//先放入链表
		ipc_node *node = new ipc_node;
		node->link_id = SWPA_LINK_VIDEO;
		node->ref = 1;
		node->addr = image.data.addr[0];
		node->phys = bufFrames.frames[i].phyAddr[0][0];
		node->ptr_type = 0;
		node->ptr = new VIDEO_FRAMEBUF_LIST_S;
		//更换通道
		swpa_memcpy(node->ptr, &bufFrames.frames[i], sizeof(bufFrames.frames[i]));
		swpa_list_add(g_hlst_memory, node);
		//在回调
		if(SWPAR_FAIL == call_callback_func(SWPA_LINK_VIDEO, 2, CALLBACK_TYPE_IMAGE, &image))
		{
			swpa_ipnc_release(node->phys, 1);
		}
	}
}


void DspCbDummy(void *pContext)
{
	//!!this is a dummy function, only for test
	return;
}

void MetaDataCb(void *pData, unsigned int nLen)
{
	//printf("MetaDataCb call back!\n");
	if (g_metadata_callback_func && g_metadata_func_param)
	{
		//printf("call back func..........!\n");
		return g_metadata_callback_func(g_metadata_func_param, pData, nLen);
	}
	return ;
}


////////////////////////////////////////////////////////////////

int swpa_ipnc_create(IPNCCreateConfig *ipnc_create_config)
{
	if(ipnc_create_config == NULL)
	{
		printf("Err:ipnc_create invalid argument.");
		return SWPAR_FAIL;
	}
	int imx 				= ipnc_create_config->imx;
	int videoConfig 		= ipnc_create_config->videoConfig;
	int videoConfigSecond 	= ipnc_create_config->videoConfigSecond;
	int SecondVideoFrameRate= ipnc_create_config->secondVideoFrameRate;
	int Verbose 			= ipnc_create_config->verbose;
	int maxBitRate			= ipnc_create_config->maxBitRate;
	int secondMaxBitRate	= ipnc_create_config->secondMaxBitRate;
	if(!g_finitialize)
	{
		IpcInitParams param;
		swpa_memset(&param, 0, sizeof(param));
		param.cbSize = sizeof(param);

		unsigned char buffer[16] = {0};
		if (0 != drv_fpga_get_version(DEVID_SPI_FPGA, buffer, 16))
		{
			printf("Err: failed to get fpga version!!\n");
		}
		else
		{
			if (strncmp((char*)buffer, "VB", 2) == 0)
			{
				s_iSensorType = SENSOR_IMX185;
			}
			else if (strncmp((char*)buffer, "VC", 2) == 0)
			{
				s_iSensorType = SENSOR_IMX178;
			}
			else if (strncmp((char *)buffer, "VD", 2) == 0)
			{
				s_iSensorType = SENSOR_IMX249;
			}
			else if (strncmp((char *)buffer, "VE", 2) == 0)
			{
				s_iSensorType = SENSOR_ICX816;
			}
		}

		if( imx == -1 )
		{
			imx = (s_iSensorType == SENSOR_IMX178 ? IMX178_WITHOUTCVBS : IMX185);
		}

		switch(imx)
		{
		case IMX185 : 			printf("We got IMX185!!!\n"); break;
		case IMX178 : 			printf("We got IMX178!!!\n"); 
			param.vnfbufnum = 35; // 600W
			break;
		case JPEG_DEC:			printf("We got JPEG_DEC!!!\n"); break;
		case IMX185_DUALVIDEO:	printf("We got IMX185_DUALVIDEO!\n"); break;
		case IMX249 :			printf("We got IMX249!!! imx = %d\n", imx); 
			param.vnfbufnum = 25;			//降噪缓存数量
			break;
		case ICX816 :	
		case ICX816_640W:
		case ICX816_600W:
		case ICX816_SINGLE:
			printf("We got ICX816!!!\n");
			param.capbufnum = 10;
			param.vnfbufnum = 24; // 680W
			break;
		default :				printf("warnig: default\n"); break;
		}

		s_iUseIMX = (UseCase)imx;
		param.useCase = (UseCase)imx;    
        switch (videoConfig)
        {
            case SWPA_VIDEO_RES_1080P:
                param.videoConfig = VIDEO_RES_1080P;
                break;
            case SWPA_VIDEO_RES_720P:
                param.videoConfig = VIDEO_RES_720P;
                break;
            case SWPA_VIDEO_RES_540P:
                param.videoConfig = VIDEO_RES_540P;
                break;
            default:
                param.videoConfig = VIDEO_RES_1080P;
                break;       
        }
		//第二路H264参数
		switch (videoConfigSecond)
		{
			case SWPA_VIDEO_RES_1080P 	: param.videoConfig2 = VIDEO_RES_1080P; break;
			case SWPA_VIDEO_RES_720P	: param.videoConfig2 = VIDEO_RES_720P;	break;
			case SWPA_VIDEO_RES_576P	: param.videoConfig2 = VIDEO_RES_576P;	break;
			case SWPA_VIDEO_RES_480P	: param.videoConfig2 = VIDEO_RES_480P;	break;
			default : printf("Warning : Default video config!\n");param.videoConfig2 = VIDEO_RES_1080P;break;
		}
		
		switch (param.useCase)
		{
			case USECASE_FPGA_IMX249:						//15pfs
				param.sourceFpsConfig.nImageInputRate = 25;
				param.sourceFpsConfig.nImageOutputRate = 15;
				break;
			case USECASE_FPGA_ICX816_680W:	
			case USECASE_FPGA_ICX816_640W:
			case USECASE_FPGA_ICX816_600W:					//10pfs
				param.sourceFpsConfig.nImageInputRate = 25;
				param.sourceFpsConfig.nImageOutputRate = 10;
				break;
			case USECASE_FPGA_ICX816_SINGLE:
				param.sourceFpsConfig.nImageInputRate = 13;
				param.sourceFpsConfig.nImageOutputRate = 13;
				break;
			default:										//12.5pfs
				param.sourceFpsConfig.nImageInputRate = 30;	
				param.sourceFpsConfig.nImageOutputRate = 15;
				break;
		}
		
		param.sourceFpsConfig.nVideoInputRate2 = 25;//相机帧率
		param.sourceFpsConfig.nVideoOutputRate2 = SecondVideoFrameRate;//第二路H264输出帧率
		//VBR 最大码率
		param.maxBitRate[SWPA_VPSS_H264_CHANNEL] = maxBitRate;  
		param.maxBitRate[SWPA_VPSS_H264_SECOND_CHANNEL] = secondMaxBitRate; 

		printf("H264 second frame rate %d\n",SecondVideoFrameRate);
		//end

		param.FilePath.szFwVpssM3 = (char*)FP_FW_VPSSM3;
	    param.FilePath.szFwVideoM3 = (char*)FP_FW_VIDEOM3;
	    param.FilePath.szFwDsp = (char*)FP_FW_DSP;

	    param.FilePath.szBinRemoteDbg = (char*)FP_BIN_RTDBG;
	    param.FilePath.szBinSysPri = (char*)FP_BIN_SYSPRI; // SysPri可执行文件路径
	    param.FilePath.szBinFwLoad = (char*)FP_BIN_FWLOAD; // 固件加载程序可执行文件路径
	    param.FilePath.szBinPrcm = (char*)FP_BIN_PRCM; // 电源管理程序可执行文件路径
	    param.FilePath.szBinMemrdwr = (char*)FP_BIN_MEMRDWR; // 内存读写程序可执行文件路径

	    param.FilePath.szKoOsa = (char*)FP_KO_OSA; // OSA驱动文件路径
	    param.FilePath.szKoSyslink = (char*)FP_KO_SYSLINK; // Syslink驱动文件路径
	    param.FilePath.szKoFb = (char*)FP_KO_FB; // FB驱动文件路径
	    param.FilePath.szKoHdmi = (char*)FP_KO_HDMI; // HDMI驱动文件路径
	    param.FilePath.szKoVpss = (char*)FP_KO_VPSS; // VPSS驱动文件路径

		param.vpssInCb.CbFunc = OnIPNCInData;
		param.vpssInCb.CbParam = (void *)SWPA_LINK_VPSS;
		param.outVpssCb.CbFunc = OnVpssOutData;
		param.outVpssCb.CbParam = (void *)SWPA_LINK_VPSS;
		param.videoInCb.CbFunc = OnIPNCInData;
		param.videoInCb.CbParam = (void *)SWPA_LINK_VIDEO;
		param.outVideoCb.CbFunc = OnIPNCOutData;
		param.outVideoCb.CbParam = (void *)SWPA_LINK_VIDEO;
		param.dspInCb.CbFunc = OnIPNCInData;
		param.dspInCb.CbParam = (void *)SWPA_LINK_DSP;
		param.outDspCb.CbFunc = OnIPNCOutData;
		param.outDspCb.CbParam = (void *)SWPA_LINK_DSP;
		if(param.useCase == USECASE_JPEG_DEC)
		{
			param.videoFrameInCb.CbFunc = JPEGFrameInCb;
			param.videoFrameInCb.CbParam = NULL;
			param.dec_param.width = 3072;
			param.dec_param.height = 2048;
		}
		if (param.useCase == USECASE_DOME_CAMERA)
		{
			param.dspcallbackCb.CbFunc = DspCbDummy;
			param.dspcallbackCb.CbParam = NULL;
		}
		if (param.useCase == USECASE_FPGA_IMX249 
			|| param.useCase == USECASE_FPGA_ICX816_680W
			|| param.useCase == USECASE_FPGA_ICX816_640W
			|| param.useCase == USECASE_FPGA_ICX816_600W)	//2A algorithm in A8
		{
			param.bExternal2A = 1;
			param._2aMetadataCbFunc = MetaDataCb;
			param.extendJpegHight = 128;		//为图片外OSD需要，预留128行buffer
		}
		param.bEnableBIOSLog = Verbose == 0 ? 0 : 1;
		printf("start IPC_Init...\n");
		if (0 != IPC_Init(&param))
		{
			printf("err: IPC_Init failed.\n");
			return SWPAR_FAIL;
		}
		printf("IPC_Init ok...\n");
		
		g_finitialize = 1;
		g_hlst_memory = swpa_list_create(0);
		g_hlst_release_vnf = swpa_list_create(0);

		swpa_mutex_create(&g_vnflistlock,NULL);
		
		printf("start IPC_Start...\n");		
		IPC_Start();
		printf("IPC_Start ok...\n");
		
		return SWPAR_OK;
	}
	
	return SWPAR_FAIL;
}

void swpa_ipnc_setcallback(
	int link_id
	, int channel_id
	, int (*OnResult)(void *pContext, int type, void *struct_ptr)
	, void *pContext
)
{
	if(!g_finitialize)
	{
		return;
	}
	
	if(channel_id < SWPA_VPSS_CHANNEL_MAX)
	{
		g_func[link_id][channel_id].func_param = pContext;
		g_func[link_id][channel_id].callback_func = OnResult;	
	}
	else
	{
		g_func[link_id][0].func_param = pContext;
		g_func[link_id][0].callback_func = OnResult;
		g_func[link_id][1].func_param = pContext;
		g_func[link_id][1].callback_func = OnResult;
		g_func[link_id][2].func_param = pContext;
		g_func[link_id][2].callback_func = OnResult;
	}
}

void swpa_ipnc_setmetadatacallback(
	metadata_callback_func callbackfunc
	, void *pContext)
{
	if(!g_finitialize)
	{
		return;
	}
	//printf("set meta data call back!\n");
	g_metadata_callback_func = callbackfunc;
	g_metadata_func_param = pContext;
}

int swpa_ipnc_start(void)
{
	return 0;
}

void swpa_ipnc_release(void *addr, int fphys)
{
	if(!g_finitialize)
	{
		return;
	}

	SWPA_IPNC_PRINT("find swpa_ipnc_release(0x%08x, %d)\n", addr, fphys);
	bool fFound = false;
	bool fReleaseVnf = false;
	unsigned int timestamp;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if( node == NULL )
		{
			continue;
		}
		bool fIsResize = (node->resize == 1 && (fphys ? node->phys_resize == addr : node->addr_resize == addr));
		bool fIsCvbsCb = (fphys ? node->phys_cvbs == addr : node->addr_cvbs == addr);
		if( (fphys ? node->phys == addr : node->addr == addr) || fIsResize || fIsCvbsCb)
		{
			fFound = true;
			if(!--node->ref)
			{	
				swpa_list_delete(g_hlst_memory, pos);
				
				switch(node->link_id)
				{
				case SWPA_LINK_A8 : 
					{
						break;
					}
					
				case SWPA_LINK_VPSS : //vpss
					{
						SWPA_IPNC_PRINT("release SWPA_LINK_VPSS(0x%08x)\n", addr);
						
						if(!node->ptr_type)
						{
							VIDEO_FRAMEBUF_LIST_S bufs;
							bufs.numFrames = 1;
							swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));
							if(node->resample)
							{
								bufs.frames[0].frameWidth = node->width;
								bufs.frames[0].frameHeight = node->height;
								if( node->resize )
								{
									bufs.frames[0].addr[0][0] -= node->offsetY;
									bufs.frames[0].phyAddr[0][0] -= node->offsetY;
									bufs.frames[0].addr[0][1] -= node->offsetUV;
									bufs.frames[0].phyAddr[0][1] -= node->offsetUV;
								}
							}
							if (SWPA_VPSS_JPEG_CHANNEL == node->channel)
							{
								g_vnfframecnt--;
							}
							
							if (SWPA_VPSS_JPEG_SECOND_CHANNEL == node->channel 
								&& (ICX816 == s_iUseIMX 
									|| ICX816_600W == s_iUseIMX 
									|| ICX816_640W == s_iUseIMX
									|| ICX816_SINGLE == s_iUseIMX)
								)
							{
								fReleaseVnf = true;
								timestamp = node->fpga_time_stamp;
							}
							
							delete (VIDEO_FRAMEBUF_S *)node->ptr;
							SWPA_IPNC_VNF_MATCH_INFO("release frame in match channel:%d,ts;0x%x\n",node->channel, node->fpga_time_stamp);
							IPC_PutEmptyVpssM3Frames(&bufs);
						}
						break;
					}
					
				case SWPA_LINK_VIDEO : //video
					{
						if(!node->ptr_type)
						{
							VIDEO_FRAMEBUF_LIST_S bufs;
							bufs.numFrames = 1;
							swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));
							delete (VIDEO_FRAMEBUF_LIST_S *)node->ptr;
							IPC_PutEmptyVideoM3Frames(&bufs);
						}
						else if(1 == node->ptr_type)
						{
							VCODEC_BITSBUF_LIST_S bufs;
							bufs.numBufs = 1;
							SWPA_IPNC_PRINT("release SWPA_LINK_VIDEO(0x%08x)\n", addr);
							swpa_memcpy(&bufs.bitsBuf[0], node->ptr, sizeof(bufs.bitsBuf[0]));
							delete (VCODEC_BITSBUF_S *)node->ptr;
							IPC_PutEmptyVideoM3Bits(&bufs);
						}
						break;
					}
					
				case SWPA_LINK_DSP : //dsp
					{
						SWPA_IPNC_PRINT("release SWPA_LINK_DSP(0x%08x)\n", addr);
						VIDEO_FRAMEBUF_LIST_S bufs;
						bufs.numFrames = 1;
						swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));
						delete (VIDEO_FRAMEBUF_S *)node->ptr;
						IPC_PutEmptyDspData(&bufs);
						break;
					}
				default:
					{
						SWPA_IPNC_PRINT("not found node(0x%08x)\n", addr);
					}
				}
				delete node;
			}
			else
			{
				SWPA_IPNC_PRINT("found 0x%08x, ref:%d\n", addr, node->ref);
			}
			break;
		}
	}
	swpa_list_close(g_hlst_memory);
	if(!fFound)
	{
		SWPA_IPNC_PRINT("not found 0x%08x\n", addr);

		//printf("<123ipnc>not found 0x%08x\n", addr);
	}

	if (fReleaseVnf)
	{
		fFound = false;
		swpa_mutex_lock(&g_vnflistlock,100);
		for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
		{
			ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
			if(node 
				&& (timestamp == node->fpga_time_stamp)
				&& SWPA_VPSS_JPEG_CHANNEL == node->channel)
			{
				if(!--node->ref)
				{
					g_vnfframecnt--;
					swpa_list_delete(g_hlst_memory, pos);
					SWPA_IPNC_VNF_MATCH_INFO("release vnf frame ts:0x%x\n",timestamp);

					if(!node->ptr_type)
					{
						VIDEO_FRAMEBUF_LIST_S bufs;
						bufs.numFrames = 1;
						swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));
					
						delete (VIDEO_FRAMEBUF_S *)node->ptr;
						
						SWPA_IPNC_VNF_MATCH_INFO("release vnf frame in match channel:%d,ts;0x%x\n",node->channel, node->fpga_time_stamp);
						delete node;
						
						IPC_PutEmptyVpssM3Frames(&bufs);
					}
				}

				fFound = true;
				
				break;
			}
		}

		if (false == fFound)
		{
			SWPA_IPNC_VNF_MATCH_INFO("the without vnf frame already release,add timestamp to vnf release list.ts:0x%x\n",timestamp);
			add_vnf_release(timestamp);
		}
		
		swpa_list_close(g_hlst_memory);

		swpa_mutex_unlock(&g_vnflistlock);
		
	}
}

void* swpa_ipnc_get_app_ptr(void *addr)
{
	if(!g_finitialize)
	{
		return NULL;
	}

	void *appData = NULL;
	for(int pos = swpa_list_open(g_hlst_memory); NULL == appData && pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node && node->addr == addr)
		{
			switch(node->link_id)
			{
			case SWPA_LINK_VPSS : 
				appData = ((VIDEO_FRAMEBUF_S *)node->ptr)->pUserSpecDataVirAddr; 
				break;
			case SWPA_LINK_VIDEO : 
				appData = 0;
				break;
			case SWPA_LINK_DSP : 
				appData = ((VIDEO_FRAMEBUF_S *)node->ptr)->pUserSpecDataVirAddr; 
				break;
			}
		}
	}
	swpa_list_close(g_hlst_memory);
	return appData;
}

int swpa_ipnc_get_app_size(void *addr)
{	
	if(!g_finitialize)
	{
		return 0;
	}
	
	int size = 0;
	for(int pos = swpa_list_open(g_hlst_memory); !size && pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node && node->addr == addr)
		{
			switch(node->link_id)
			{
			case SWPA_LINK_VPSS : 
				size = ((VIDEO_FRAMEBUF_S *)node->ptr)->nUserSpecDataSize; 
				break;
			case SWPA_LINK_VIDEO : 
				size = 0; 
				break;
			case SWPA_LINK_DSP : 
				size = ((VIDEO_FRAMEBUF_S *)node->ptr)->nUserSpecDataSize; 
				break;
			}
		}
	}
	swpa_list_close(g_hlst_memory);
	return size;
}

//截图
int swpa_ipnc_Crop(void *addr, int left, int top, int right, int bottom, int* piDetWidth, int* piDetHeight)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}
	int ret = SWPAR_FAIL;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node && node->addr == addr)
		{
            SWPA_IPNC_PRINT("swpa_ipnc_Crop : link_id=%d, resample=%d, resize=%d###########################\n",
                            node->link_id, node->resample, node->resize);
            //if(SWPA_LINK_VPSS == node->link_id && node->resample && !node->resize )
			{
				VIDEO_FRAMEBUF_S* buf = (VIDEO_FRAMEBUF_S *)node->ptr;
				// zhaopy 截图
				if( left == 0 && right == 0 && top == 0 && bottom == 0)
				{
				}
				else
				{
					left = (left + 31) / 32 * 32;
                    right = (right + 31) / 32 * 32;
					top = (top + 7) / 8 * 8;
                    bottom = (bottom+7) / 8 * 8;

					int iNewW = right - left;
					int iNewH = bottom - top;

					buf->frameWidth = iNewW;
					buf->frameHeight = iNewH;
					node->resize = 1;
					node->offsetY = ((top * buf->framePitch[0] + left));
					node->offsetUV = ((top / 2) * buf->framePitch[1] + left);

					buf->addr[0][0] += node->offsetY;
					buf->phyAddr[0][0] += node->offsetY;
					buf->addr[0][1] += node->offsetUV;
					buf->phyAddr[0][1] += node->offsetUV;

					node->addr_resize = buf->addr[0][0];
					node->phys_resize = buf->phyAddr[0][0];
				}
				if( piDetWidth != 0 )
				{
					*piDetWidth = buf->frameWidth;
				}
				if( piDetHeight != 0 )
				{
					*piDetHeight = buf->frameHeight;
				}
				ret = SWPAR_OK;
				break;
			}
		}
	}
	swpa_list_close(g_hlst_memory);
	return ret;
}

int swpa_ipnc_Crop_reset(void *addr, int left, int top, int right, int bottom, int iResetWidth, int iResetHeight, int* piDetWidth, int* piDetHeight)
{
    if(!g_finitialize)
    {
        return SWPAR_FAIL;
    }
    int ret = SWPAR_FAIL;
    for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
    {
        ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
        if(node && node->addr == addr)
        {
            SWPA_IPNC_PRINT("swpa_ipnc_Crop_reset : link_id=%d, resample=%d, resize=%d###########################\n",
                            node->link_id, node->resample, node->resize);
            //if(SWPA_LINK_VPSS == node->link_id && node->resample && !node->resize )
            {
                VIDEO_FRAMEBUF_S* buf = (VIDEO_FRAMEBUF_S *)node->ptr;
                // zhaopy 截图
                if( left == 0 && right == 0 && top == 0 && bottom == 0)
                {
                }
                else
                {
                    left = (left + 31) / 32 * 32;
                    right = (right + 31) / 32 * 32;
                    top = (top + 7) / 8 * 8;
                    bottom = (bottom+7) / 8 * 8;

                    buf->frameWidth = iResetWidth;
                    buf->frameHeight = iResetHeight;
                    node->resize = 0;
                    node->offsetY = ((top * buf->framePitch[0] + left));
                    node->offsetUV = ((top / 2) * buf->framePitch[1] + left);

                    buf->addr[0][0] -= node->offsetY;
                    buf->phyAddr[0][0] -= node->offsetY;
                    buf->addr[0][1] -= node->offsetUV;
                    buf->phyAddr[0][1] -= node->offsetUV;

                    node->addr_resize = buf->addr[0][0];
                    node->phys_resize = buf->phyAddr[0][0];
                }
                if( piDetWidth != 0 )
                {
                    *piDetWidth = buf->frameWidth;
                }
                if( piDetHeight != 0 )
                {
                    *piDetHeight = buf->frameHeight;
                }
                ret = SWPAR_OK;
                break;
            }
        }
    }
    swpa_list_close(g_hlst_memory);
    return ret;
}

// 拉伸与截图
int swpa_ipnc_resample(void *addr, void**y, void**uv)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}

	int ret = SWPAR_FAIL;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node && node->addr == addr)
		{
			if(SWPA_LINK_VPSS == node->link_id && !node->resample)
			{
				VIDEO_FRAMEBUF_S* buf = (VIDEO_FRAMEBUF_S *)node->ptr;
				node->resample = 1;
				node->width = buf->frameWidth;
				node->height = buf->frameHeight;
				
				int iOffsetY = buf->framePitch[0]*buf->startY + buf->startX;
				int iOffsetUV = buf->framePitch[1]*(buf->startY/2) + buf->startX;;
				
				char* yStartSrc = (char *)buf->addr[0][0] + iOffsetY;
				char* uvStartSrc = (char *)buf->addr[0][1] + iOffsetUV;
				
				char* yStartDst = yStartSrc;
				char* uvStartDst = yStartDst + buf->framePitch[0]*buf->frameHeight*2 + iOffsetUV;

				char* ySrc  = yStartSrc + buf->framePitch[0] * (buf->frameHeight - 1);
				char* uvSrc = uvStartSrc + buf->framePitch[1] * (buf->frameHeight/2 - 1);
				
				char* yDst  = yStartDst + buf->framePitch[0] * (buf->frameHeight*2 - 1);
				char* uvDst = uvStartDst + buf->framePitch[1] * (buf->frameHeight - 1);

				//先拷贝uv
				for(int h = buf->frameHeight/2; h > 0; h--)
				{
					swpa_memcpy(uvDst, uvSrc, buf->framePitch[1]); 
					uvDst -= buf->framePitch[1];
					swpa_memcpy(uvDst, uvSrc, buf->framePitch[1]); 
					uvDst -= buf->framePitch[1];
					uvSrc -= buf->framePitch[1];
				}

				//在拷贝y
				for(int h = buf->frameHeight; h > 0; h--)
				{
					swpa_memcpy(yDst, ySrc, buf->framePitch[0]); 
					yDst -= buf->framePitch[0];
					swpa_memcpy(yDst, ySrc, buf->framePitch[0]); 
					yDst -= buf->framePitch[0];
					ySrc -= buf->framePitch[0];
				}
				//地址偏移
				buf->frameHeight *= 2;
				buf->addr[0][1] = uvStartDst - iOffsetUV;
				buf->phyAddr[0][1] = (char *)buf->phyAddr[0][1] + ((int)uvStartDst - (int)uvStartSrc) - iOffsetUV;
				node->addr = yStartDst;

				if(y)
				{
					*y = yStartDst;
				}
				if(uv)
				{
					*uv = uvStartDst;
				}
				ret = SWPAR_OK;
				break;
			}
		}
	}
	swpa_list_close(g_hlst_memory);
	return ret;
}

//扩图，增加图像高度，上扩或者下扩,需要M3修改buffer大小来支持
int swpa_ipnc_scale_up(void *addr, int up, int offset, int *pnewheight)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}
	int ret = SWPAR_FAIL;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node && node->addr == addr)
		{
			if(SWPA_LINK_VPSS == node->link_id  && !node->resize )
			{
				VIDEO_FRAMEBUF_S* buf = (VIDEO_FRAMEBUF_S *)node->ptr;
				//扩图，增加图像高度，上扩或者下扩
				int iImageHeight = buf->frameHeight;	//真实图像数据高度
				offset = (offset+7) / 8 * 8;			//八的整数倍,不够的话补齐
				buf->frameHeight += offset;
				node->resize = 1;

				node->addr_resize = buf->addr[0][0];
				node->phys_resize = buf->phyAddr[0][0];
				
				if( pnewheight != NULL )
				{
					*pnewheight = buf->frameHeight;
				}
				
				if(up)	//往上扩，原始数据往下搬移
				{
					int iCopyYSize = buf->framePitch[0]*offset;
					int iImageYDataSize =  buf->framePitch[0]*iImageHeight;
					char *pYStartAddr = (char *)buf->addr[0][0] + buf->startY*buf->framePitch[0];
					char *pYDstAddr = pYStartAddr + iImageYDataSize;//指向图片最末尾
					char *pYSrcAddr = pYDstAddr - iCopyYSize;
					int iYDataLast = iImageYDataSize;
					int iLoopCount = (iImageHeight/offset) + 1;
					//拷贝y
					for(int i = 0; i<iLoopCount; i++)
					{
						swpa_memcpy(pYDstAddr, pYSrcAddr, iCopyYSize);
						iYDataLast -= iCopyYSize;

						iCopyYSize = (iCopyYSize < iYDataLast)?iCopyYSize:iYDataLast;
						pYDstAddr -= iCopyYSize;
						pYSrcAddr -= iCopyYSize;
					}
					swpa_memset(pYStartAddr,16,buf->framePitch[0]*offset);	//黑底
					
					int iUVDataSize = buf->framePitch[1]*(iImageHeight>>1);
					int iCopyUVSize = buf->framePitch[1]*(offset>>1);
					char *pUVStartAddr = (char *)buf->addr[0][1] + (buf->startY>>1)*buf->framePitch[1];
					char *pUVDstAddr = pUVStartAddr + iUVDataSize;
					char *pUVSrcAddr = pUVDstAddr - iCopyUVSize;
					int iUVDataLast = iUVDataSize;
					
					//拷贝uv
					for (int i=0; i<iLoopCount; i++)
					{
						swpa_memcpy(pUVDstAddr, pUVSrcAddr, iCopyUVSize);
						iUVDataLast -= iCopyUVSize;
						
						iCopyUVSize = (iCopyUVSize < iUVDataLast)?iCopyUVSize:iUVDataLast;
						pUVDstAddr -= iCopyUVSize;
						pUVSrcAddr -= iCopyUVSize;
					}
					swpa_memset(pUVStartAddr,128,buf->framePitch[1]*(offset>>1));	//黑底

				}
				else	//往下扩，只需将下扩区域背景变成黑色
				{
					char *pYStart = (char *)buf->addr[0][0] + buf->startY*buf->framePitch[0];
					char *pUVStart = (char *)buf->addr[0][1] + (buf->startY>>1)*buf->framePitch[1];
					char *pYSet = pYStart + buf->framePitch[0]*iImageHeight;
					char *pUVSet = pUVStart + buf->framePitch[1]*(iImageHeight>>1);
					swpa_memset(pYSet,16,buf->framePitch[0]*offset);	//黑底
					swpa_memset(pUVSet,128,buf->framePitch[1]*(offset>>1));	//黑底
				}

				ret = SWPAR_OK;
				break;
			}
		}
	}
	swpa_list_close(g_hlst_memory);
	return ret;
}

int swpa_ipnc_send_data(int link_id, void *addr, int size, void *appData, int dataSize)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}

	int ret = SWPAR_FAIL;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node &&
				((node->addr == addr) || (node->cvbsFlag != 0 && node->addr_cvbs == addr))
				)
		{
			switch(link_id)
			{
			case SWPA_LINK_DSP : 
				{
					node->ref++;
					VIDEO_FRAMEBUF_LIST_S bufs;
					bufs.numFrames = 1;

					swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));
					if(dataSize > bufs.frames[0].nUserSpecDataSize)
					{
						dataSize = bufs.frames[0].nUserSpecDataSize;
					}

					// todo.
					// 强制设成通道0，
					// DSP只处理通道0的数据。
					bufs.frames[0].channelNum = 0;

					if(NULL != appData && 0 < dataSize && appData != bufs.frames[0].pUserSpecDataVirAddr)
					{			
						swpa_memcpy(bufs.frames[0].pUserSpecDataVirAddr, appData, dataSize);
					}
					IPC_PutFullDspFrames(&bufs);
				}
				break;
				
			case SWPA_LINK_VIDEO:
				{
					node->ref++;
					VIDEO_FRAMEBUF_LIST_S bufs;
					bufs.numFrames = 1;
					swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));
					if(dataSize > bufs.frames[0].nUserSpecDataSize)
					{
						dataSize = bufs.frames[0].nUserSpecDataSize;
					}
					if(NULL != appData && 0 < dataSize && appData != bufs.frames[0].pUserSpecDataVirAddr)
					{			
						swpa_memcpy(bufs.frames[0].pUserSpecDataVirAddr, appData, dataSize);
					}
					SWPA_IPNC_PRINT("IPC_PutFullVideoM3Frames:0x%08x\n", addr);
					IPC_PutFullVideoM3Frames(&bufs);
				}
				break;

			case SWPA_LINK_VPSS:
				{
					node->ref++;
					VIDEO_FRAMEBUF_LIST_S bufs;
					bufs.numFrames = 1;
					swpa_memcpy(&bufs.frames[0], node->ptr, sizeof(bufs.frames[0]));

					bufs.frames[0].addr[0][0] = node->addr_cvbs;
					bufs.frames[0].phyAddr[0][0] = node->phys_cvbs;
					bufs.frames[0].addr[0][1] = node->addr_cvbs + 720 * 576;
					bufs.frames[0].phyAddr[0][1] = node->phys_cvbs  + 720 * 576;
					// todo.
					// w? h?
					bufs.frames[0].frameWidth = 720;
					bufs.frames[0].frameHeight = 576;
					bufs.frames[0].framePitch[0] = 720;
					bufs.frames[0].framePitch[1] = 720;
					bufs.frames[0].startY = 0;

					node->cvbsFlag |= 0x1;
					IPC_PutFullVpssM3Frames(&bufs);
				}
				break;
			}
			ret = SWPAR_OK;
			break;
		}
	}
	swpa_list_close(g_hlst_memory);
	return ret;
}

int swpa_ipnc_get_vnf_frame(void *reg_frame_addr, IMAGE *vnf_image_info, int image_ts)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}
	if (NULL == reg_frame_addr || NULL == vnf_image_info)
	{
		printf("arg is null\n");
		return SWPAR_INVALIDARG;
	}

	int ret = SWPAR_FAIL;
	unsigned int timestamp = 0;
	bool waitvnf = false;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		//找到对应的识别的帧信息，拿到时间戳
		if(node 
			&&(node->addr == reg_frame_addr)
			&&(SWPA_VPSS_JPEG_SECOND_CHANNEL == node->channel //without vnf frame
					&& (ICX816 == s_iUseIMX 
					|| ICX816_600W == s_iUseIMX 
					|| ICX816_640W == s_iUseIMX
					|| ICX816_SINGLE == s_iUseIMX))
				)
		{
			//get vnf yuv frame from match list to encode jpeg
			timestamp = node->fpga_time_stamp;

			SWPA_IPNC_VNF_MATCH_INFO("wait vnf frame, ts:0x%x, phys addr:0x%x,addr:0x%x \n",
				timestamp, node->phys, node->addr);
			waitvnf = true;
			
			ret = SWPAR_OK;
			break;
		}
	}
	swpa_list_close(g_hlst_memory);

	
	if (waitvnf)	//等待vnf降噪帧过来
	{
		ret = SWPAR_OK;
		ipc_node *vnfnode = NULL;
		do
		{
			if (ret != SWPAR_OK)
			{
				swpa_thread_sleep_ms(80);
			}
			ret = get_vnf_frame(timestamp, &vnfnode);
        }while(ret != SWPAR_OK && ((swpa_datetime_gettick() - image_ts) < 300));//vnf降噪帧比未降噪帧晚200多毫秒
		
		if (SWPAR_OK == ret)
		{
			vnfnode->ref++;
			
			//将vnf帧信息送到应用层，来创建Image
			VIDEO_FRAMEBUF_S frame;
			swpa_memcpy(&frame, vnfnode->ptr, sizeof(frame));
			Convert_VIDFrame_Buf_to_Image(&frame, vnf_image_info);
			
			SWPA_IPNC_VNF_MATCH_INFO("get vnf info addr:0x%x, phys:0x%x, ts:0x%x\n",
				vnf_image_info->data.addr[0], vnf_image_info->data.phys[0], vnf_image_info->uTimeStamp);
		}
		else
		{
			SWPA_IPNC_WARN("wait vnf frame time out. ts:0x%x,vnfcnt:%d\n", 
				timestamp, g_vnfframecnt);
		}

	}

	return ret;
}

//获取降噪帧
int get_vnf_frame(unsigned int timestamp, ipc_node **vnfnode)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}
	if (NULL == vnfnode)
	{
		return SWPAR_INVALIDARG;
	}
	
	int ret = SWPAR_FAIL;
	for(int pos = swpa_list_open(g_hlst_memory); pos != -1; pos = swpa_list_next(g_hlst_memory))
	{
		ipc_node *node = (ipc_node *)swpa_list_value(g_hlst_memory, pos);
		if(node 
			&& (timestamp == node->fpga_time_stamp)
			&&(SWPA_VPSS_JPEG_CHANNEL == node->channel //vnf frame
					&& (ICX816 == s_iUseIMX 
					|| ICX816_600W == s_iUseIMX 
					|| ICX816_640W == s_iUseIMX
					|| ICX816_SINGLE == s_iUseIMX)))
		{
			SWPA_IPNC_VNF_MATCH_INFO("get vnf frame ts:0x%x\n",timestamp);
			*vnfnode = node;
			ret = SWPAR_OK;
			break;
		}
	}
	swpa_list_close(g_hlst_memory);
	
	return ret;
}

//降噪的那帧对应的没有降噪的帧已经释放则加入直接释放队列
int add_vnf_release(unsigned int timestamp)
{
	if(!g_finitialize)
	{
		return SWPAR_FAIL;
	}
	unsigned int *ptimestamp = new (unsigned int);
	*ptimestamp = timestamp;

	return swpa_list_add(g_hlst_release_vnf, ptimestamp);
}

//降噪的那帧对应的没有降噪帧已经释放
bool is_vnf_already_release(unsigned int timestamp)
{
	if(!g_finitialize)
	{
		return false;
	}
	
	bool ret = false;

	for(int pos = swpa_list_open(g_hlst_release_vnf); pos != -1; pos = swpa_list_next(g_hlst_release_vnf))
	{
		unsigned int *node = (unsigned int *)swpa_list_value(g_hlst_release_vnf, pos);

		if(node && (timestamp == *node))
		{
			SWPA_IPNC_VNF_MATCH_INFO("vnf frame already release,not add to match list.ts:0x%x\n",timestamp);
			swpa_list_delete(g_hlst_release_vnf, pos);

			delete node;
			ret = true;
			break;
		}
	}
	swpa_list_close(g_hlst_release_vnf);

	return ret;
}


#define CMD(cmd) case cmd : return IPC_CONTROL_##cmd
int MapCmd(int cmd)
{
	switch(cmd)
	{
		CMD(CMD_SET_EXP                             );
		CMD(CMD_GET_EXP                             );
		CMD(CMD_SET_GAIN                            );
		CMD(CMD_GET_GAIN                            );
		CMD(CMD_SET_RGB                             );
		CMD(CMD_GET_RGB                             );
		CMD(CMD_SET_AE_STATE                        );
		CMD(CMD_GET_AE_STATE                        );
		CMD(CMD_SET_AE_THRESHOLD                    );
		CMD(CMD_GET_AE_THRESHOLD                    );
		CMD(CMD_SET_AE_EXP_RANGE                    );
		CMD(CMD_GET_AE_EXP_RANGE                    );
		CMD(CMD_SET_AE_ZONE                         );
		CMD(CMD_GET_AE_ZONE                         );
		CMD(CMD_SET_AE_GAIN_RANGE                   );
		CMD(CMD_GET_AE_GAIN_RANGE                   );
		CMD(CMD_SET_AWB_STATE                       );
		CMD(CMD_GET_AWB_STATE                       );
		CMD(CMD_SET_WDR_STRENGTH                    );
		CMD(CMD_GET_WDR_STRENGTH                    );
		CMD(CMD_SET_SHARPNESS_STATE                 );
		CMD(CMD_GET_SHARPNESS_STATE                 );
		CMD(CMD_SET_SHARPNESS                       );
		CMD(CMD_GET_SHARPNESS                       );
		CMD(CMD_SET_SATURATION_CONTRAST_STATE       );
		CMD(CMD_GET_SATURATION_CONTRAST_STATE       );
		CMD(CMD_SET_SATURATION                      );
		CMD(CMD_GET_SATURATION                      );
		CMD(CMD_SET_CONTRAST                        );
		CMD(CMD_GET_CONTRAST                        );
		CMD(CMD_SET_TNF_STATE                       );
		CMD(CMD_GET_TNF_STATE                       );
		CMD(CMD_SET_SNF_STATE                       );
		CMD(CMD_GET_SNF_STATE                       );
		CMD(CMD_SET_TNF_SNF_STRENGTH                );
		CMD(CMD_GET_TNF_SNF_STRENGTH                );
		CMD(CMD_SET_TRAFFIC_LIGTH_ENHANCE_STATE     );
		CMD(CMD_GET_TRAFFIC_LIGTH_ENHANCE_STATE     );
		CMD(CMD_SET_TRAFFIC_LIGTH_ENHANCE_REGION    );
        CMD(CMD_SET_TRAFFIC_LIGTH_LUM_TH            );
        CMD(CMD_SET_GAMMA_ENABLE                    );
        CMD(CMD_GET_GAMMA_ENABLE                    );
        CMD(CMD_SET_GAMMA                           );
        CMD(CMD_GET_GAMMA                           );
        CMD(CMD_SET_EDGE_ENHANCE                    );
        CMD(CMD_GET_EDGE_ENHANCE                    );
		CMD(CMD_SET_DCIRIS_AUTO_STATE               );
		CMD(CMD_GET_DCIRIS_AUTO_STATE               );
		CMD(CMD_ZOOM_DCIRIS                         );
		CMD(CMD_SHRINK_DCIRIS                       );
		CMD(CMD_SET_FILTER_STATE                    );
		CMD(CMD_SET_RELAY_STATE                     );
		CMD(CMD_SET_AC_SYNC_STATE                   );
		CMD(CMD_GET_AC_SYNC_STATE                   );
		CMD(CMD_SET_AC_SYNC_DELAY                   );
		CMD(CMD_GET_AC_SYNC_DELAY                   );
		CMD(CMD_SET_IO_ARG                        	);
		CMD(CMD_GET_IO_ARG                        	);
		CMD(CMD_SET_EDGE_MOD                        );
		CMD(CMD_GET_EDGE_MOD                        );
		CMD(CMD_GET_H264_RES                        );
		CMD(CMD_SET_GRAY_IMAGE_STATE                );
		CMD(CMD_GET_GRAY_IMAGE_STATE                );
		CMD(CMD_SET_H264_BITRATE                    );
		CMD(CMD_GET_H264_BITRATE                    );
		CMD(CMD_SET_H264_BITRATE2					);
		CMD(CMD_GET_H264_BITRATE2					);
		CMD(CMD_SET_H264_IFRAME_INTERVAL            );
		CMD(CMD_GET_H264_IFRAME_INTERVAL            );
		CMD(CMD_SET_JPEG_QUALITY                    );
		CMD(CMD_GET_JPEG_QUALITY                    );
		CMD(CMD_SET_CVBS_STD                        );
		CMD(CMD_GET_CVBS_STD                        );
		CMD(CMD_SET_VIDEO_STREAM_RECOGNIZE_PARAMS   );
		CMD(CMD_SET_PICTURE_SNAP_RECOGNIZE_PARAMS   );
		CMD(CMD_SET_FPGA_REG                        );
		CMD(CMD_GET_FPGA_REG                        );
		CMD(CMD_SET_SCENE                        	);
		CMD(CMD_SET_WORKMODE						);
		CMD(CMD_SOFT_CAPTURE						);
		CMD(CMD_SET_FLASH_GATE						);
		CMD(CMD_GET_VERSION							);
		CMD(CMD_SET_CAP_SHUTTER                     );
		CMD(CMD_SET_CAP_GAIN                        );
		CMD(CMD_SET_CAP_SHARPEN                     );
		CMD(CMD_SET_CAP_RGB                         );
		CMD(CMD_GET_DAY_NIGHT                       );
		CMD(CMD_SET_BACKLIGHT                       );
		CMD(CMD_SET_AWB_MODE						);
		CMD(CMD_SET_H264_IFRAME_INTERVAL2           );
		CMD(CMD_GET_H264_IFRAME_INTERVAL2           );
	}
}

int swpa_ipnc_control(int linkId, int cmd, void *pPrm, int prmSize, int timeout)
{
	if(g_finitialize)
	{
		IPC_Control(MapCmd(cmd), pPrm, prmSize);
	}
	
	return SWPAR_OK;
}

void swpa_ipnc_close(void)
{
	if(g_finitialize)
	{
		IPC_Stop();
		SWPA_IPNC_PRINT("=======IPC_Stop Finish!========\n");
		IPC_Deinit();
		SWPA_IPNC_PRINT("=======IPC_Deinit Finish!========\n");
		g_finitialize = 0;
	}
}

void swpa_ipnc_cache_wbinv(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait)
{
	if(g_finitialize)
	{
		IPC_Cache_WbInv(blockPtr, byteCnt, type, wait);
	}
}

void swpa_ipnc_cache_inv(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait)
{
	if (g_finitialize)
	{
		IPC_Cache_Inv(blockPtr, byteCnt, type, wait);
	}
}

void swpa_ipnc_cache_wb(void* blockPtr, unsigned int byteCnt, unsigned short type, unsigned short wait)
{
	if(g_finitialize)
	{
		IPC_Cache_Wb(blockPtr, byteCnt, type, wait);
	}
}

int swpa_ipnc_add_jpg(void* buffer, int size)
{
	static VCODEC_BITSBUF_LIST_S jpgbufs;
	static int jpgIndex = 0;

	if(!jpgIndex)
	{
		if(IPC_GetEmptyVideoM3Bits(&jpgbufs))
		{
			//printf("IPC_GetEmptyVideoM3Bits failed.\n");
			return SWPAR_FAIL;
		}
	}
	
	VCODEC_BITSBUF_LIST_S bufs;
	bufs.numBufs = 1;
	swpa_memcpy(&bufs.bitsBuf[0], &jpgbufs.bitsBuf[jpgIndex], sizeof(bufs.bitsBuf[0]));
	swpa_memcpy(bufs.bitsBuf[0].bufVirtAddr, buffer, size);	
	bufs.bitsBuf[0].filledBufSize = size;
	IPC_PutFullVideoM3Bits(&bufs);
	jpgIndex = (jpgIndex + 1)%jpgbufs.numBufs;
	return SWPAR_OK;
}

void swpa_ipnc_printDetailedStatistics(void)
{
	IPC_printDetailedStatistics();
}
void swpa_ipnc_printBufferStatistics(void)
{
	IPC_printBufferStatistics();
}

void* swpa_ipnc_share_region_malloc(int index, int size)
{
	IPC_SHARE_REGION reg = (index == 1 ? IPC_SHARE_REGION_1 : IPC_SHARE_REGION_2);
	return IPC_ShareRegion_Malloc(reg, size);
}

void swpa_ipnc_share_region_free(int index, void* ptr, int size)
{
	IPC_SHARE_REGION reg = (index == 1 ? IPC_SHARE_REGION_1 : IPC_SHARE_REGION_2);
	return IPC_ShareRegion_Free(reg, ptr, size);
}

void swpa_ipnc_share_region_get_total_size(int *size)
{
	IPC_SHAREREGION_STATUS sr1_status, sr2_status;
	swpa_memset(&sr1_status, 0, sizeof(IPC_SHAREREGION_STATUS));
	swpa_memset(&sr2_status, 0, sizeof(IPC_SHAREREGION_STATUS));

	IPC_ShareRegion_Get_status(IPC_SHARE_REGION_1, &sr1_status);
	IPC_ShareRegion_Get_status(IPC_SHARE_REGION_2, &sr2_status);
	*size = sr1_status.TotalSize + sr2_status.TotalSize;
}

void swpa_ipnc_share_region_get_free_size(int *size)
{
	IPC_SHAREREGION_STATUS sr1_status, sr2_status;
	swpa_memset(&sr1_status, 0, sizeof(IPC_SHAREREGION_STATUS));
	swpa_memset(&sr2_status, 0, sizeof(IPC_SHAREREGION_STATUS));

	IPC_ShareRegion_Get_status(IPC_SHARE_REGION_1, &sr1_status);
	IPC_ShareRegion_Get_status(IPC_SHARE_REGION_2, &sr2_status);
	*size = sr1_status.TotalFreeSize + sr2_status.TotalFreeSize;
}

int swpa_ipnc_set_target_bitrate(int chID, int target_bitrate)
{
	return IPC_Enc_Set_TargetBitrate(chID, target_bitrate);
}

int swpa_ipnc_set_iframe_interval(int chID, int interval)
{
	return IPC_Enc_Set_IframeIntervel(chID, interval);
}

int swpa_ipnc_set_rate_control(int chID, int rate_control)
{
	return IPC_Enc_Set_RateControl(chID, rate_control);
}

int swpa_ipnc_set_vbr_duration(int chID, int duration)
{
	return IPC_Enc_Set_VbrDuration(chID, duration);
}

int swpa_ipnc_set_vbr_sensitivity(int chID, int sensitivity)
{
	return IPC_Enc_Set_VbrSensitivity(chID, sensitivity);
}

int swpa_ipnc_get_target_bitrate(int chID, int *target_rate)
{
	return IPC_Enc_Get_TargetBitrate(chID, target_rate);
}

int swpa_ipnc_get_iframe_interval(int chID, int *interval)
{
	return IPC_Enc_Get_IframeIntervel(chID, interval);
}
