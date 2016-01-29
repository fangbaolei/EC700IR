#include "swpa.h"
#include "drv_ipnc.h"

IpcCbParams param[20];
typedef struct tagMEM
{
	char* addr;
	char* phys;
	int size;
	int width;
	int height;
	char* appData;
	int dataSize;
}MEM;

#define APP_SIZE 1048576

int g_lstMEM = 0;
int g_reuselstMEM = 0;
int g_dspFile;

int drv_ipnc_init(void)
{
	g_lstMEM = swpa_list_create(0);
	g_reuselstMEM = swpa_list_create(0);
	g_dspFile = swpa_file_open("DSPLINK/1", "w");
	return 0;
}

int drv_ipnc_deinit(void)
{
	return 0;
}

int drv_ipnc_start(void)
{
	return 0;
}

int drv_ipnc_stop(void)
{
	return 0;
}

int drv_ipnc_control(int linkId, int cmd, void *pPrm, int prmSize, int timeout)
{
	printf("linkId:%d,cmd:%d,pPrm:0x%08x, prmSize:%d, timeout:%d\n", linkId, cmd, pPrm, prmSize, timeout);
	if(NULL != pPrm && g_dspFile)
	{
		swpa_file_ioctl(g_dspFile, SWPA_FILE_SET_READ_TIMEOUT, &timeout);
		if(!swpa_file_write(g_dspFile, pPrm, prmSize, NULL))
		{
			if(cmd)
			{
				param[3].CbFunc(param[3].CbParam);
			}
			return 0;
		}
	}	
	return -1;
}

int drv_ipnc_set_in_callback_params(int nCoreId, IpcCbParams *ipcCbParams)
{
	swpa_memcpy(&param[nCoreId], ipcCbParams, sizeof(IpcCbParams));
	return 0;
}

int drv_ipnc_set_out_callback_params(int nCoreId, IpcCbParams *ipcCbParams)
{
	return 0;
}

int drv_ipnc_vpss_get_full_frames(VIDFrame_BufList *bufs)
{
	MEM* mem =(MEM *)swpa_list_remove(g_lstMEM);
	bufs->numFrames = 0;
	if(NULL != mem)
	{
		bufs->numFrames = 1;
		bufs->frames[0].addr[0][0] = mem->addr;
		bufs->frames[0].phyAddr[0][0] = mem->phys;
		bufs->frames[0].frameWidth = mem->width;
		bufs->frames[0].frameHeight = mem->height;
		bufs->frames[0].appData = mem->appData;
		bufs->frames[0].dataSize = mem->dataSize;
	
		swpa_list_add(g_reuselstMEM, mem);
	}
	return 0;
}

int drv_ipnc_vpss_put_empty_frames(VIDFrame_BufList *bufs)
{
	for(int i = 0; i < bufs->numFrames; i++)
	{
		for(int pos = swpa_list_open(g_reuselstMEM); pos != -1; pos = swpa_list_next(g_reuselstMEM))
		{
			MEM* mem = (MEM*)swpa_list_value(g_reuselstMEM, pos);
			if((int)mem->addr == (int)bufs->frames[i].addr[0][0])
			{
				printf("swpa_sharedmem_free(0x%08x)\n", mem->addr);
				swpa_sharedmem_free(mem->addr, 0);
				swpa_list_delete(g_reuselstMEM, pos);				
				delete mem;
				break;
			}
		}
		swpa_list_close(g_reuselstMEM);
	}
	return 0;
}
int drv_ipnc_video_get_full_bits(void* pBitBufList)
{
	return 0;
}

int drv_ipnc_video_put_empty_bits(void* pBitBufList)
{
	return 0;
}
int drv_ipnc_dsp_get_full_data (VIDFrame_BufList *bufs)
{
	swpa_memset(bufs, 0, sizeof(VIDFrame_BufList));
	bufs->numFrames = 1;
	printf("drv_ipnc_dsp_get_full_data\n");
	return 0;
}

int drv_ipnc_dsp_put_empty_data (void* pDspDataList)
{
	printf("drv_ipnc_dsp_put_empty_data\n");
	return 0;
}

int drv_ipnc_put_full_frames(int nCoreId, VIDFrame_BufList *bufs)
{
	printf("drv_ipnc_put_full_frames\n");
	if(bufs)
	{
		for(int i = 0; i < bufs->numFrames; i++)
		{
			drv_ipnc_control(nCoreId, 1, (void *)((int)bufs->frames[i].phyAddr[0][0] + (int)bufs->frames[i].appData - (int)bufs->frames[i].addr[0][0]), 4, 4000);
		}
	}
	return 0;
}

int drv_ipnc_get_empty_frames(int nCoreId, VIDFrame_BufList *bufs)
{
	return 0;
}

int drv_ipnc_add_image(void* addr, void* phys, int width, int height, int size)
{
	MEM *mem = new MEM;
	if(!swpa_sharedmem_alloc((void**)&mem->addr, (void**)&mem->phys, size + APP_SIZE))
	{
		mem->size = size;
		mem->width = width;
		mem->height = height;
		swpa_memcpy(mem->addr, addr, size);	
		mem->appData = mem->addr + size;
		mem->dataSize = APP_SIZE;	
		swpa_list_add(g_lstMEM, mem);
		param[1].CbFunc(param[1].CbParam);
	}
	else
	{
		delete mem;
	}
	return 0;
}