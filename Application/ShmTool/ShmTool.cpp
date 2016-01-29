#include "SWFC.h"
#include "swpa.h"
#include "SWMessage.h"

int OperShm(int agc, char* agv[]);

int main(int agc, char* agv[])
{
	OperShm(agc, agv);
    return 0;
}

/*****************************************************************************
 函 数 名  : OperShm
 功能描述  : 释放进程间通信的共享内存
 输入参数  : int agc      
             char* agv[]  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 注意事项  : 
 
 修改历史      :
  1.日    期   : 2015年9月6日
    作    者   : huangdch
    修改内容   : 新生成函数

*****************************************************************************/
int OperShm(int agc, char* agv[])
{
	int iRet = 0;
	int keyId = -1;

	//连接管理区
	int state = -1;

	for ( int k = 0 ; k < MSG_SHM_PROCESS_NUMBER; k++ )
	{
		keyId = MSG_SHM_REGISTER_START_KEY + (k * 20);
	    iRet = swpa_normal_sharedmem_free(keyId, NULL);
		if  ( iRet != 0 )
		{
		    //SW_TRACE_DEBUG("destroy shm key=0x%08x(%d) failed, ret=[%d].\n", keyId, keyId, iRet);
		}
		else
		{
		    SW_TRACE_NORMAL("destroy shm key=0x%08x(%d) success.\n", keyId, keyId);
		}
	}

	for ( int k = 0 ; k < 4; k++ )
	{
		keyId = 10000 + k;
	    iRet = swpa_normal_sharedmem_free(keyId, NULL);
		if  ( iRet != 0 )
		{
		    //SW_TRACE_DEBUG("destroy shm key=0x%08x(%d) failed, ret=[%d].\n", keyId, keyId, iRet);
		}
		else
		{
		    SW_TRACE_NORMAL("destroy shm key=0x%08x(%d) success.\n", keyId, keyId);
		}
	}

	//删除其它参数传入的id
	int* init = (int*)swpa_mem_alloc((agc) * sizeof(int));
	if (NULL == init)
	{
	    return -1;
	}

	for ( int k = 1 ; k < agc ; k++ )
	{
		keyId = atoi(agv[k]);
		iRet = swpa_normal_sharedmem_free(keyId, NULL);
		if  ( iRet != 0 )
		{
		    SW_TRACE_DEBUG("destroy shm key=0x%08x(%d) failed, ret=[%d].\n", keyId, keyId, iRet);
		}
		else
		{
		    SW_TRACE_NORMAL("destroy shm key=0x%08x(%d) success.\n", keyId, keyId);
		}
	}
	swpa_mem_free(init);
	
	return 0;
}
