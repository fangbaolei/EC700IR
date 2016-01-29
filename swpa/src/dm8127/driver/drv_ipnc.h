#ifndef __DRV_IPNC_H__
#define __DRV_IPNC_H__

#define VIDFRAME_MAX_FIELDS 1
#define VIDFRAME_MAX_PLANES 1
#define VIDFRAME_MAX_FRAME_BUFS 4

typedef struct tagIpcCbParams
{
	void (*CbFunc)(void *);
	void* CbParam;
}IpcCbParams;

typedef struct VIDFrame_Buf 
{
  unsigned int reserved[2];
  /**< First two 32 bit entries are reserved to allow use as Que element */
	void* addr[VIDFRAME_MAX_FIELDS][VIDFRAME_MAX_PLANES];
  /**< virtual address of vid frame buffer pointers */
 	void* phyAddr[VIDFRAME_MAX_FIELDS][VIDFRAME_MAX_PLANES];
  /**< virtual address of vid frame buffer pointers */
  unsigned int channelNum;
  /**< Coding type */
  unsigned int timeStamp;
  /**< Time stamp */
  unsigned int fid;
  /**< Field indentifier (TOP/BOTTOM/FRAME) */
  unsigned int frameWidth;
  /**< Width of the frame */
  unsigned int frameHeight;
  /**< Height of the frame */
  unsigned int framePitch[VIDFRAME_MAX_PLANES];
  /**< Pitch of the frame */
	void* linkPrivate;
  /**< Link private info. Application should preserve this value and not overwrite it */
  
  void *appData;
  unsigned int dataSize;
  
} VIDFrame_Buf;

typedef struct VIDFrame_BufList 
{
	unsigned int numFrames;
	/**< Number of valid VIDFrame_Buf entries */
	VIDFrame_Buf  frames[VIDFRAME_MAX_FRAME_BUFS];
	/**< Array of VIDFrame_Buf structures */

}VIDFrame_BufList;


int drv_ipnc_init(void);
int drv_ipnc_deinit(void);
int drv_ipnc_start(void);
int drv_ipnc_stop(void);
int drv_ipnc_control(int linkId, int cmd, void *pPrm, int prmSize, int timeout);
int drv_ipnc_set_in_callback_params(int nCoreId, IpcCbParams *ipcCbParams);
int drv_ipnc_set_out_callback_params(int nCoreId, IpcCbParams *ipcCbParams);
int drv_ipnc_vpss_get_full_frames(VIDFrame_BufList *buf);
int drv_ipnc_vpss_put_empty_frames(VIDFrame_BufList *buf);
int drv_ipnc_video_get_full_bits(void* pBitBufList);
int drv_ipnc_video_put_empty_bits(void* pBitBufList);
int drv_ipnc_dsp_get_full_data (VIDFrame_BufList *bufs);
int drv_ipnc_dsp_put_empty_data (void* pDspDataList);
int drv_ipnc_put_full_frames(int nCoreId, VIDFrame_BufList *bufs);
int drv_ipnc_get_empty_frames(int nCoreId, VIDFrame_BufList *bufs);


int drv_ipnc_add_image(void* addr, void* phys, int width, int height, int size);
#endif