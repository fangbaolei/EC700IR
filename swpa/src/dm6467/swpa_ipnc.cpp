#include "swpa.h"
#include "swpa_ipnc.h"

////////////////////////////////////////////////////////////////
int swpa_ipnc_create(int videoConfig,int videoConfigSecond, int SecondVideoFrameRate, int Verbose)
{
	return SWPAR_OK;
}

void swpa_ipnc_setcallback(
	int link_id
	, void (*OnResult)(void *pContext, int type, void *struct_ptr)
	, void *pContext
)
{
	
}

int swpa_ipnc_start(void)
{
	return SWPAR_OK;
}

void swpa_ipnc_release(void *addr)
{
	
}

void* swpa_ipnc_get_app_ptr(void *addr)
{
	return 0;
}

int swpa_ipnc_get_app_size(void *addr)
{
	return SWPAR_OK;
}

int swpa_ipnc_send_data(int link_id, void *addr, int size, void *appData, int dataSize)
{
	return SWPAR_OK;
}

int swpa_ipnc_get_vnf_frame(void *reg_frame_addr, IMAGE *vnf_image_info)
{
	return SWPAR_OK;
}

int swpa_ipnc_control(int linkId, int cmd, void *pPrm, int prmSize, int timeout)
{
	return SWPAR_OK;	
}

void swpa_ipnc_close(void)
{
}