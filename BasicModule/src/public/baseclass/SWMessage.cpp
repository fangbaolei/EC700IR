///////////////////////////////////////////////////////////
//  CSWMessage.cpp
//  Implementation of the Class CSWMessage
//  Created on:      27-二月-2013 16:29:45
//  Original author: huanggc
///////////////////////////////////////////////////////////
#include "SWFC.h"
#include "SWMessage.h"

#define SW_TRACE_DEBUG 

CSWThread* CSWMessage::m_pMsgThread = NULL;
CSWThread* CSWMessage::m_pRemoteMsgThread = NULL;
CSWObjectList CSWMessage::m_objMSGList;
CSWList<CSWMessage *>CSWMessage::m_clsMSGList;
CSWRWMutex CSWMessage::m_rwLock;
CSWUDPSocket CSWMessage::m_udpSocket;
/* BEGIN: Modified by huangdch, 2015/8/14 
*新增加进程间相关的变量*/
CSWMutex CSWMessage::m_tMsgMutex; //注册锁 MSG_SHM_PROCESS_MUTEX
STSharedMemRegisterNode* CSWMessage::m_SharedSharedMemRegister = NULL;
CSWList<STMessageRangNode*> CSWMessage::m_MSGRangList;
STProcessManageNode* CSWMessage::m_ProcessManageNode = NULL;
STProcessManageCur CSWMessage::m_ProcessManageCur;
STSWShmMessage* CSWMessage::m_pShm = NULL;
CSWMutex CSWMessage::tMutex;
CSWRWMutex CSWMessage::m_rwProcLock;
CSWSemaphore CSWMessage::m_ArySem[MSG_SHM_SEM_TOTAIL];
CSWThread* CSWMessage::m_pMsgProcThread = NULL;
char CSWMessage::m_ProcessName[MSG_SHM_PROCESS_LEN] = {0};
CSWMessage::CSWMessage(DWORD dwStartID, DWORD dwEndID)
:m_dwStartID(dwStartID)
,m_dwEndID(dwEndID)
{
	m_rwLock.WLock(0);
	m_clsMSGList.AddTail(this);
	/* BEGIN: Modified by huangdch, 2015/8/14 
     *第一个对象完成共享内存相关的初始化操作*/
	if (1 == m_clsMSGList.GetCount())
	{
	    InitShmMsg();
	}
	m_rwLock.Unlock();
	
	if(NULL == m_pMsgThread)
	{
		m_pMsgThread = new CSWThread();
		m_pMsgThread->Start(OnMSG, NULL);
	}
	else
	{
		m_pMsgThread->AddRef();
	}

	if (NULL == m_pRemoteMsgThread)
	{
		m_udpSocket.Create();
		int iOpt = 0;
		m_udpSocket.SetSockOpt(SWPA_SOL_SOCKET, SWPA_SO_REUSEADDR, &iOpt, sizeof(iOpt));
		m_udpSocket.SetRecvTimeout(4000);
		m_udpSocket.SetSendTimeout(4000);
		for(int i = 0; i < 10; i++)
		{
			if(S_OK == m_udpSocket.Bind(NULL, 8890 + i))
			{
				SW_TRACE_NORMAL("[%s]bind port %d", CSWApplication::GetExeName(), 8890 + i);
				break;
			}
		}
		m_pRemoteMsgThread = new CSWThread();
		m_pRemoteMsgThread->Start(OnRemoteMSG, NULL);
	}
	else
	{
		m_pRemoteMsgThread->AddRef();
	}

	/* BEGIN: Modified by huangdch, 2015/8/14
	*增加一个进程间通信的消息处理线程*/
	if(NULL == m_pMsgProcThread)
	{
		m_pMsgProcThread = new CSWThread;
		m_pMsgProcThread->Start(OnProMSG, NULL);
	}
	else
	{
		m_pMsgProcThread->AddRef();
	}

}


CSWMessage::~CSWMessage(){

	m_rwLock.WLock(0);
	SW_POSITION pos = m_clsMSGList.Find(this, NULL);

	if(m_clsMSGList.IsValid(pos))
	{
		m_clsMSGList.RemoveAt(pos);
	}
	m_rwLock.Unlock();

	/* BEGIN: Modified by huangdch, 2015/9/5 
	*这里退出会有异常,先去掉*/
	m_pMsgThread->Release();
	m_pMsgProcThread->Release();
	m_pRemoteMsgThread->Release();
	
	if (0 == m_clsMSGList.GetCount())
	{
		if (NULL != m_SharedSharedMemRegister)
		{
			swpa_normal_sharedmem_free(MSG_SHM_REGISTER_KEY, (void*)m_SharedSharedMemRegister);
		}
	
		if (m_ProcessManageCur.pShm != NULL)
		{
			swpa_normal_sharedmem_free(m_ProcessManageCur.dwKey, (void*)m_ProcessManageCur.pShm);
		}
	
		if (m_ProcessManageNode != NULL)
		{
			swpa_mem_free(m_ProcessManageNode);
		}
	}
	
}


/**
 * @brief 消息接收虚函数
 * @param [in] id 消息id号
 * @param [in] wParam 消息体参数1
 * @param [in] lParam 消息体参数2
 * @return 成功返回S_OK，其他值代表失败
 */
HRESULT CSWMessage::OnMessage(DWORD id, WPARAM wParam, LPARAM lParam){
	
	return E_NOTIMPL;
}

HRESULT CSWMessage::OnRemoteMessage(DWORD id, PVOID pvBuffer, INT iSize)
{
	//SW_TRACE_NORMAL("[%s]CSWMessage::OnRemoteMessage(%d,0x%08x,%d)", CSWApplication::GetExeName(), id, pvBuffer, iSize);
	return E_NOTIMPL;
}

/**
 * @brief 非阻塞式发送消息
 * @param [in] id 消息的id号
 * @param [in] wParam 消息体参数1
 * @param [in] lParam 消息体参数2
 * @return S_OK代表成功，其他值为错误代码
 */
HRESULT CSWMessage::PostMessage(DWORD id, WPARAM wParam, LPARAM lParam){
	
	m_objMSGList.Push(new CMSG(id, wParam, lParam));
	return S_OK;
}


/**
 * @brief 阻塞式发送消息
 * @param [in] id 消息的id号
 * @param [in] wParam 消息体参数1
 * @param [in] lParam 消息体参数2
 * @return S_OK代表成功，其他值为错误代码
 */
HRESULT CSWMessage::SendMessage(DWORD id, WPARAM wParam, LPARAM lParam)
{
	BOOL fProcessFlag = TRUE;
	CSWMessage* msg = NULL;
	HRESULT hr = E_FAIL;

	m_rwLock.RLock();
	SW_POSITION pos = m_clsMSGList.GetHeadPosition();
	while ((msg = m_clsMSGList.GetNext(pos)))
	{
		if (id > msg->m_dwStartID && id < msg->m_dwEndID)
		{
			hr = msg->OnMessage(id, wParam, lParam);
			fProcessFlag = FALSE;
			break;
		}
	}
	m_rwLock.Unlock();

	/* BEGIN: Modified by huangdch, 2015/8/14 
	* 进程间通信*/
	if (fProcessFlag && MSG_SHM_PROCESS_NUMBER > 0)
	{
		HRESULT result = -1;
		SW_TRACE_DEBUG("send id=[%u] access process.\n", id);
		hr = SendProcessMessage(id, wParam, lParam, result);
		if (FAILED(hr) || 0 != result)
		{
			SW_TRACE_NORMAL("call SendProcessMessage(%d) failed,[%d].\n", id, result);
			hr = E_FAIL;
		}
	}
	else if (fProcessFlag && MSG_SHM_PROCESS_NUMBER == 0)
	{
		SW_TRACE_DEBUG("id=[%u] not need define access process.\n", id);
	}

	return  hr;
}

HRESULT CSWMessage::SendRemoteMessage(DWORD id, PVOID pvBuffer, INT iSize)
{
	//SW_TRACE_NORMAL("[%s]SendRemoteMessage(%d,0x%08x,%d)", CSWApplication::GetExeName(), id, pvBuffer, iSize);
	CSWMemory *pMemory = CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY)->Alloc(iSize + sizeof(DWORD));
	if(pMemory)
	{
		swpa_memcpy(pMemory->GetBuffer(), &id, sizeof(DWORD));
		if(iSize && pvBuffer)
		{
			swpa_memcpy((CHAR *)pMemory->GetBuffer() + sizeof(DWORD), pvBuffer, iSize);
		}		
		for(int i = 0; i < 10; i++)
		{
			DWORD dwLen;
			m_udpSocket.SendTo(pMemory->GetBuffer(), iSize + sizeof(DWORD), "127.0.0.1", 8890 + i, &dwLen);
		}
		CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY)->Free(pMemory);
		return S_OK;
	}
	return E_FAIL;
}

/**
 * @brief 消息处理线程函数
 * @param [in] pvParam 线程函数
 */
void *CSWMessage::OnMSG(void *pvParam)
{
	SW_TRACE_NORMAL("<SWMessage:OnMSG>run.");
	
	while(S_OK == m_pMsgThread->IsValid() && !CSWApplication::IsExited())
	{
		CMSG *pMSG = (CMSG *)m_objMSGList.Pop(100);
		if(NULL != pMSG)
		{
			//SW_TRACE_NORMAL("<123>ONmsg pop.ID:%d,lp:%08x.", pMSG->m_dwID, (INT)pMSG->m_lParam);
			SendMessage(pMSG->m_dwID, pMSG->m_wParam, pMSG->m_lParam);
			pMSG->Release();
			pMSG = NULL;
		}
	}
	
	SW_TRACE_NORMAL("<SWMessage::OnMSG>exit.");
	return 0;
}

void* CSWMessage::OnRemoteMSG(void *pvParam)
{
	SW_TRACE_NORMAL("<SWMessage:OnRemoteMSG>run.");

	CSWMemory *pMemory = CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY)->Alloc(1024);

	while(S_OK == m_pRemoteMsgThread->IsValid() && !CSWApplication::IsExited())
	{
		DWORD dwSize = 0;
		if(S_OK == m_udpSocket.RecvFrom(pMemory->GetBuffer(), pMemory->GetSize(), &dwSize))
		{
			dwSize -= sizeof(DWORD);
			m_rwLock.RLock();
			SW_POSITION pos = m_clsMSGList.GetHeadPosition();
			CSWMessage* msg = NULL;
			HRESULT hr = E_NOTIMPL;
			BOOL fOk = FALSE;
			while((msg = m_clsMSGList.GetNext(pos)))
			{			
				fOk = TRUE;	
				hr = msg->OnRemoteMessage(*(DWORD *)pMemory->GetBuffer(), dwSize ? (CHAR *)pMemory->GetBuffer() + sizeof(DWORD) : NULL, dwSize);
			}
			m_rwLock.Unlock();
			
			if(!fOk)
			{
				SW_TRACE_NORMAL("[%s]not find any handle message function.", CSWApplication::GetExeName());
			}
		}
	}
	CSWMemoryFactory::GetInstance(SW_NORMAL_MEMORY)->Free(pMemory);

	SW_TRACE_NORMAL("<SWMessage::OnRemoteMSG>exit.");
	return 0;
}

/*****************************************************************************
 Prototype    : CSWMessage.InitShmMsg
 Description  : 初始化共享内存
 Input        : None
 Output       : None
 Return Value : HRESULT
 Calls        : 
 Note         : 
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
HRESULT CSWMessage::InitShmMsg()
{
//	int iRet = -1;
//	int iKey = -1;

	//不需要进程间的通信
	if (0 == MSG_SHM_PROCESS_NUMBER)
	{
		SW_TRACE_DEBUG("MSG_SHM_PROCESS_NUMBER=[0].\n");
		return S_OK;
	}

	SW_TRACE_DEBUG("start<InitShmMsg()>.\n");

	//进程锁
	if (0 != m_tMsgMutex.Create(MSG_SHM_PROCESS_MUTEX))
	{
		SW_TRACE_DEBUG("create mutex=[%d] failed.\n", MSG_SHM_PROCESS_MUTEX);
		return E_FAIL;
	}

	if (0 != m_tMsgMutex.Lock())
	{
		SW_TRACE_DEBUG("lock mutex=[%d] failed.\n", MSG_SHM_PROCESS_MUTEX);
		return E_FAIL;
	}

	//注册共享内存区的创建
	int state = 0;
	int size = MSG_SHM_PROCESS_NUMBER * sizeof(STSharedMemRegisterNode) + MSG_MAX * sizeof(char);
	m_SharedSharedMemRegister = (STSharedMemRegisterNode *)swpa_normal_sharedmem_alloc(MSG_SHM_REGISTER_KEY, size, &state);
	if (NULL == m_SharedSharedMemRegister)
	{
		SW_TRACE_DEBUG("create shm key_id=0x%08x(%d) size=[%d] failed.\n", MSG_SHM_REGISTER_KEY, MSG_SHM_REGISTER_KEY, size);
		if (0 != m_tMsgMutex.Unlock())
		{
			SW_TRACE_DEBUG("unlock mutex=[%d] failed.\n", MSG_SHM_PROCESS_MUTEX);
			return E_FAIL;
		}
		return E_FAIL;
	}
	if (0 == state)
	{
		memset(m_SharedSharedMemRegister, 0, size);
	}
	SW_TRACE_DEBUG("create shm key_id=0x%08x(%d) size=[%d] success.\n", MSG_SHM_REGISTER_KEY, MSG_SHM_REGISTER_KEY, size);

	if (0 != m_tMsgMutex.Unlock())
	{
		SW_TRACE_DEBUG("unlock mutex=[%d] failed.\n", MSG_SHM_PROCESS_MUTEX);
		return E_FAIL;
	}

	//初始化平台相关的信息
	m_rwProcLock.WLock(0);
	m_ProcessManageNode = (STProcessManageNode*)swpa_mem_alloc(MSG_SHM_PROCESS_NUMBER * sizeof(STProcessManageNode));
	if (NULL == m_ProcessManageNode)
	{
	    return E_FAIL;
	}
	for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
	{
		m_ProcessManageNode[k].dwKey = 0;
		m_ProcessManageNode[k].iProcessId = 0;
		memset(m_ProcessManageNode[k].szProcessName, 0, sizeof(m_ProcessManageNode[k].szProcessName) - 1);
		m_ProcessManageNode[k].pShm = NULL;
		m_ProcessManageNode[k].pMutex = NULL;
		for (int j = 0 ; j < MSG_SHM_SEM_TOTAIL ; j++)
		{
			m_ProcessManageNode[k].pSem[j] = NULL;
		}
	}
	m_rwProcLock.Unlock();
	
	memset(m_ProcessManageCur.szProcessName, 0, sizeof(m_ProcessManageCur.szProcessName));
	m_ProcessManageCur.iProcessId = 0;
	m_ProcessManageCur.dwKey = 0;
	m_ProcessManageCur.pShm = NULL;

	//获取进程名
	char* pszTemp = (char*)swpa_mem_alloc(MSG_SHM_PROCESS_LEN + 1);
	if (NULL == pszTemp)
	{
		return E_OUTOFMEMORY;
	}
	memset(pszTemp, 0, MSG_SHM_PROCESS_LEN + 1);
	int iProcessId = swpa_utils_process_get_id();
	swpa_utils_process_name_by_id(iProcessId, pszTemp, MSG_SHM_PROCESS_LEN);
	swpa_utils_path_getfilename(pszTemp, m_ProcessName);
	swpa_mem_free(pszTemp);

	SW_TRACE_DEBUG("end<InitShmMsg()>.\n");

	return S_OK;
}

/*****************************************************************************
 Prototype    : CSWMessage.AnalyResponse
 Description  : 解析广播应答的结果
 Input        : CHAR* szResult MassSend返回的结果  
 Output       : None
 Return Value : HRESULT
 Calls        : 
 Note         : 
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
HRESULT CSWMessage::AnalyResponse(CHAR* szResult)
{
	int iRet = 0;
	int iCire = 0;//第几次
	int iItem = 0;//第几项
	int iKey = 0;//key值
	int iFindFlag = 0; //是否重复,0不重复 1重复

	char* psToken = NULL;
	char* psDest = NULL;
	char* psSecToken = NULL;
	char* psSecDest = NULL;

	char szSection[32] =
	{
		0
	};
	STMessageRangNode RangNode;//应答消息节点

	psToken = swpa_strtok_r(szResult, "|", &psDest);
	while (NULL != psToken)
	{
		iCire = 0;//初始化
		++iItem;
		psSecToken = NULL;
		psSecDest = NULL;
		swpa_memset(&RangNode, 0, sizeof(RangNode));
		swpa_memset(szSection, 0, sizeof(szSection));

		swpa_strncpy(szSection, psToken, sizeof(szSection) - 1);
		psSecToken = swpa_strtok_r(szSection, "-", &psSecDest);
		while (NULL != psSecToken)
		{
			iCire++;
			if (1 == iItem)
			{
				iKey = swpa_atoi(psSecToken);//key值
			}
			else
			{
				if (iCire == 1)
				{
					RangNode.dwStartID = swpa_atoi(psSecToken);//最小值
				}
				else
				{
					RangNode.dwEndID = swpa_atoi(psSecToken);//最大值
				}
			}
			psSecToken = swpa_strtok_r(NULL, "-", &psSecDest);
		}
		psToken = swpa_strtok_r(NULL, "|", &psDest);
		if (iItem > 1)
		{
			RangNode.dwKey = iKey;
			iFindFlag = 0;
			//判断是否重复
			SW_POSITION pos = m_MSGRangList.GetHeadPosition();
			STMessageRangNode* msgRan = NULL;
			while (m_MSGRangList.IsValid(pos))
			{
				msgRan = m_MSGRangList.GetNext(pos);
				if (msgRan->dwStartID == RangNode.dwStartID && msgRan->dwEndID == RangNode.dwEndID)
				{
					msgRan->dwKey = iKey;
					iFindFlag = 1;
					break;
				}
			}
			if (1 != iFindFlag)
			{
				STMessageRangNode* pNode = (STMessageRangNode*)swpa_mem_alloc(sizeof(STMessageRangNode));
				if ( NULL == pNode )
				{
				    iRet = -1;
				    break;
				}
				pNode->dwKey = RangNode.dwKey;
				pNode->dwStartID = RangNode.dwStartID;
				pNode->dwEndID = RangNode.dwEndID;
				m_MSGRangList.AddTail(pNode);
			}
		}
	}
	
	return iRet;
}

/*****************************************************************************
 Prototype    : CSWMessage.MessageRegister
 Description  : d
 Input        : INT id           
                CHAR* szFunName OnSet开头表示Set消息,OnGet开头表示Get消息    
                INT msgFlag  自定义消息类型标识,默认根据函数名识别,1表示set消息,2表示get消息    
 Output       : None
 Return Value : HRESULT
 Calls        : 
 Note         : 如果函数名不满足要求,就要传入第三个参数
 				调用者自己保证id的大小不超过枚举变量中的MSG_MAX
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
HRESULT CSWMessage::MessageRegister(INT id, const CHAR* szFunName, INT msgFlag /*= -1*/)
{
	if (0 == MSG_SHM_PROCESS_NUMBER)
	{
		return 0;
	}
	if (NULL != m_SharedSharedMemRegister)
	{
		char* pszFuncInfo = ((char*)m_SharedSharedMemRegister + MSG_SHM_PROCESS_NUMBER * sizeof(STSharedMemRegisterNode));
		if (msgFlag == -1)
		{
			//不区分大小写的比较
			if (0 == swpa_strnicmp(szFunName, "OnSet", 5))
			{
				pszFuncInfo[id] = MSG_TYPE_SET;
			}
			else if (0 == swpa_strnicmp(szFunName, "OnGet", 5))
			{
				pszFuncInfo[id] = MSG_TYPE_GET;
			}
		}
		else if (msgFlag == MSG_TYPE_SET || msgFlag == MSG_TYPE_GET)
		{
			//根据指定的方式进行处理
			pszFuncInfo[id] = msgFlag;
		}
		SW_TRACE_DEBUG("id=[%d] fun_name=[%s] type=[%d]\n", id, szFunName, pszFuncInfo[id]);
	}

	return 0;
}

HRESULT CSWMessage::SharedMemRegister()
{
	SW_TRACE_DEBUG("start<SharedMemRegister()>.\n");

	if (0 == MSG_SHM_PROCESS_NUMBER)
	{
		return 0;
	}

	int id = MSG_SHM_REGISTER_START_KEY;
	int reg_id = 0;

	int iRet = 0;
	char szTemp[MSG_SHM_PROCESS_LEN] =
	{
		0
	};
	char szProcName[MSG_SHM_PROCESS_LEN] =
	{
		0
	};
	int iProcessId = swpa_utils_process_get_id();
	swpa_utils_process_name_by_id(iProcessId, szTemp, MSG_SHM_PROCESS_LEN);
	swpa_utils_path_getfilename(szTemp, szProcName);

	//向共享内存注册进程相关的信息,每个id至少相距20以上,要在这里进行限制
	m_tMsgMutex.Lock();

	if (NULL != m_SharedSharedMemRegister)
	{
		int k = 0;
		int flag = -1;
		int node = -1;
		for (k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
		{
			if (0 != m_SharedSharedMemRegister[k].id && 0 == swpa_strcmp(szProcName, m_SharedSharedMemRegister[k].szProcName))
			{
				flag = 1; /* 已经注册 */
				node = k;
				break;
			}
		}

		if (-1 == flag)
		{
			//先找最大的那个id出来
			DWORD dwMaxId = 0;
			for (k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
			{
				if (m_SharedSharedMemRegister[k].id > dwMaxId)
				{
					dwMaxId = m_SharedSharedMemRegister[k].id;
				}
			}
		
			for (k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
			{
				if (0 == m_SharedSharedMemRegister[k].id)
				{
					node = k;
					//每次以之前的最大值的基本之上加20
					m_SharedSharedMemRegister[node].id = ((dwMaxId == 0)? id: (dwMaxId+20));
					reg_id = m_SharedSharedMemRegister[node].id;
					m_SharedSharedMemRegister[node].iProcessId = swpa_utils_process_get_id();
					swpa_strncpy(m_SharedSharedMemRegister[node].szProcName, szProcName, sizeof(m_SharedSharedMemRegister[node].szProcName) - 1);
					flag = 2;
					break;
				}
			}
		}
		else
		{
			;//已经注册的不处理,只以第一个id为准
		}
		if (-1 == flag)
		{
			iRet = -1; //找不到注册的节点   
		}
		
		SW_TRACE_DEBUG("sharedmemregister:id=[%d] fun_name=[%s] node=[%d] flag=[%d]\n", reg_id, szProcName, node, flag);
	}

	m_tMsgMutex.Unlock();

	SW_TRACE_DEBUG("end<SharedMemRegister()>.\n");

	return iRet;
}

HRESULT CSWMessage::FindProcByKeyId(const DWORD dwKeyId)
{
	int iRet = -1;

	m_rwProcLock.RLock();

	for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
	{
		if (dwKeyId == m_ProcessManageNode[k].dwKey)
		{
			iRet = k;
			break;
		}
	}
	
	m_rwProcLock.Unlock();

	return iRet;
}

HRESULT CSWMessage::UpdateProcByKeyId(const DWORD dwKeyId, INT iFlag)
{
	int iRet = -1;
	int code = -1;

	if (iFlag < 0)
	{
		if (0 != m_tMsgMutex.Lock())
		{
			return -1;
		}

		for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
		{
			if (dwKeyId == m_SharedSharedMemRegister[k].id && 0 != swpa_strcmp(m_ProcessName, m_SharedSharedMemRegister[k].szProcName))
			{
				code = k;
				break;
			}
		}
		
		if (0 != m_tMsgMutex.Unlock())
		{
			return -1;
		}
	}
	else
	{
	    code = iFlag;
	}

	//在注册区如果有数据就更新内存
	if (-1 != code)
	{
		int find = -1;

		m_rwProcLock.WLock(0);
		for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
		{
			if (dwKeyId == m_ProcessManageNode[k].dwKey)
			{
				find = 1;
				break;
			}
		}

		//如果内存找不到就更新
		if (-1 == find)
		{
			for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
			{
				if (0 == m_ProcessManageNode[k].dwKey)
				{
					m_ProcessManageNode[k].dwKey = dwKeyId;
					m_ProcessManageNode[k].iProcessId = m_SharedSharedMemRegister[code].iProcessId;
					swpa_strncpy(m_ProcessManageNode[k].szProcessName, m_SharedSharedMemRegister[code].szProcName, sizeof(m_ProcessManageNode[k].szProcessName) - 1);
					SW_TRACE_DEBUG("UpdateProcByKeyId:id=[%d] proc_name=[%s] node=[%d]\n", dwKeyId, m_ProcessManageNode[k].szProcessName, k);
					iRet = k;
					break;
				}
			}
		}
		m_rwProcLock.Unlock();
	}
	
	return iRet;
}


HRESULT CSWMessage::SendProcessMessage(DWORD id, DWORD wParam, DWORD lParam, INT& iRet)
{
	int iKey = -1;
	HRESULT result = E_FAIL;

	iRet = 0;//函数执行返回的结果
	iKey = FindKey(id);//通过id查找内存key

	if (iKey > 0)
	{
		SW_TRACE_DEBUG("send id=[%u] to key=[%d]\n", id, iKey);
		iRet = SingleSend(iKey, id, wParam, lParam, result);//给指定的key发消息
	}
	else if (iKey == 0)
	{
		//先发一个0的消息id,为广播Id
		SW_TRACE_DEBUG("send id=[0] to all.\n");
		iRet = MassSend(result);//第一次发送广播消息
		if (0 == iRet)
		{
			iKey = FindKey(id);//发送广播之后,这里面肯定是能找得到的
			if (iKey > 0)
			{
				SW_TRACE_DEBUG("send id=[%d] to key=[%d]\n", id, iKey);
				iRet = SingleSend(iKey, id, wParam, lParam, result);
			}
			else
			{
				SW_TRACE_DEBUG("send id=[%u] to all failed.\n", id);
			}
		}
		else
		{
			SW_TRACE_DEBUG("send id=[0] to all failed.\n");
		}
	}
	else
	{
		//消息Id异常
		SW_TRACE_DEBUG("[error]id=[%u] not define.\n", id);
	}
	return result; //调用对方返回的结果
}

/*****************************************************************************
 Prototype    : CSWMessage.FindKey
 Description  : 根据消息Id查找对应的共享内存key值
 Input        : DWORD id  
 Output       : None
 Return Value : HRESULT
 Calls        : 
 Note         : 
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
HRESULT CSWMessage::FindKey(DWORD id)
{
//	int iRet = 0;
	int iNode = 0;
	int iDefine = 1; //表示消息是否合法的范围
	int iKey = -1; //表示异常消息

	//测试:把缓冲区的内容打印出来
	SW_POSITION pos = m_MSGRangList.GetHeadPosition();
	STMessageRangNode* msgRan = NULL;
	//	while (m_MSGRangList.IsValid(pos))
	//	{
	//		msgRan = m_MSGRangList.GetNext(pos);
	//		//SW_TRACE_DEBUG("rang:key=[%d][%d-%d]\n", msgRan->dwKey, msgRan->dwStartID, msgRan->dwEndID);
	//	}

	//遍历查找消息Id对应的Key值
	//pos = m_MSGRangList.GetHeadPosition();
	while (m_MSGRangList.IsValid(pos))
	{
		msgRan = m_MSGRangList.GetNext(pos);
		if (id > msgRan->dwStartID && id < msgRan->dwEndID)
		{
			iNode = 1; //找到可发送的模块
			break;
		}
		else if (id == msgRan->dwStartID || id == msgRan->dwEndID || id < 1 || id > MSG_MAX - 1)
		{
			iDefine = 0; //消息非法
			break;
		}
	}
	if (1 == iNode)
	{
		iKey = msgRan->dwKey;//表示给当前的key发消息
	}
	else
	{
		if (iDefine)
		{
			iKey = 0; //表示要群发
		}
	}
	
	return iKey;
}

/*****************************************************************************
 Prototype    : CSWMessage.SingleSend
 Description  : 向指定共享内存块发消息
 Input        : INT iKeyId    
                DWORD id      
                DWORD wParam  
                DWORD lParam  
                INT result    
 Output       : None
 Return Value : HRESULT
 Calls        : 
 Note         : 
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
HRESULT CSWMessage::SingleSend(INT iKeyId, DWORD id, DWORD wParam, DWORD lParam, INT &result)
{
	HRESULT iRet = 0;
	int iNode = -1;
	int ifTimeOut = 0;

	STProcessManageNode* Promsg = NULL;

	int keyNode = FindProcByKeyId(iKeyId);

	//SW_TRACE_DEBUG("#SingleSend:FindProcByKeyId:KeyId=[%d],KeyNode=[%d]\n", iKeyId, keyNode);

	if (-1 == keyNode)
	{
		SW_TRACE_DEBUG("KeyId=[%d],KeyNode=[%d]\n", iKeyId, keyNode);
		return -1;
	}

	char* pszFuncInfo = ((char*)m_SharedSharedMemRegister + MSG_SHM_PROCESS_NUMBER * sizeof(STSharedMemRegisterNode));
	/* 1.连接共享内存 */
	Promsg = &m_ProcessManageNode[keyNode];
	if (NULL == Promsg->pShm)
	{
		//连接共享内存
		int state = -1;
		Promsg->pShm = swpa_normal_sharedmem_alloc(iKeyId, sizeof(STSWShmMessage), &state);
		if (NULL == Promsg->pShm)
		{
			iRet = -1;
			return iRet;
		}
		//创建锁
		Promsg->pMutex = new CSWMutex(iKeyId + 1);

		//信号量初始化
		int* init = (int*)swpa_mem_alloc(MSG_SHM_SEM_TOTAIL*sizeof(int));
		init[0] = 0;
		init[1] = MSG_SHM_NODE_NUMBER;
		for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
		{
			init[MSG_SHM_SEM_FULL_RET_START + k] = 0;
		}
		for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
		{
			init[MSG_SHM_SEM_EMPTRY_RET_START + k] = 1;
		}
		for (int k = 0 ; k < MSG_SHM_SEM_TOTAIL ; k++)
		{
			Promsg->pSem[k] = new CSWSemaphore();
			if (0 != Promsg->pSem[k]->Create(init[k], 256, iKeyId + 2 + k))
			{
				iRet = -1;
				return iRet;
			}
		}
		swpa_mem_free(init);
	}


	STSWShmMessage* pShm = (STSWShmMessage*)Promsg->pShm;
	CSWMutex* pMutex = Promsg->pMutex;

	/* 2.发送消息 */
	if (0 != Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Pend(MSG_SHM_WAIT_RESP_TIME))
	{
		SW_TRACE_DEBUG("shm is full,emptry=[%d] full=[%d].\n",\
				Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Value(),\
				Promsg->pSem[MSG_SHM_SEM_FULL_START]->Value());
		iRet = -2;
		return iRet;
	}

	//锁
	if (0 != pMutex->Lock())
	{
		SW_TRACE_DEBUG("Lock() failed.\n");
		return -1;
	}

	iNode = -1;
	//遍历找可用的节点
	for (int i = 0 ; i < MSG_SHM_NODE_NUMBER ; i++)
	{
		if (pShm->m_shmMsgNode[i].iState == 0)
		{
			iNode = i;//找到可插入的节点i
			break;
		}
	}	
	//填发送请求相关的字段
	ULONG lInsertTime = 0;
	if (iNode != -1)
	{
		pShm->m_shmMsgNode[iNode].iProcessId = swpa_utils_process_get_id();
		pShm->m_shmMsgNode[iNode].lInsertTime = swpa_datetime_gettick();//毫秒数
		lInsertTime = pShm->m_shmMsgNode[iNode].lInsertTime; //记录这个时间作为消息应用的标识判断
		pShm->m_shmMsgNode[iNode].dwId = id;
		pShm->m_shmMsgNode[iNode].iRet = 0;//默认需要返回结果
		//Set类型
		if (pszFuncInfo[id] == MSG_TYPE_SET)
		{
			//第二个参数lParam表示长度,如果为0,表示第一个参数是整形,否则表示指针
			if (0 == lParam)
			{
				//入参为整数
				snprintf(pShm->m_shmMsgNode[iNode].szParamBuf, MSG_SHM_DATA_BUF_LEN, "%u", wParam);
				pShm->m_shmMsgNode[iNode].iParamLen = 0; //表示整形
			}
			else
			{
				//入参为指针
				int iBufLen = (lParam > MSG_SHM_DATA_BUF_LEN) ? MSG_SHM_DATA_BUF_LEN : lParam;//入参长度
				swpa_memset(pShm->m_shmMsgNode[iNode].szParamBuf, 0, sizeof(pShm->m_shmMsgNode[iNode].szParamBuf));
				swpa_memcpy(pShm->m_shmMsgNode[iNode].szParamBuf, (CHAR *)wParam, iBufLen);//内存拷贝
				pShm->m_shmMsgNode[iNode].iParamLen = iBufLen;//表示指针
			}
		}
		pShm->m_shmMsgNode[iNode].iState = 1; //表示发送请求
		SW_TRACE_DEBUG("send:id=[%d] type=[%d] key=[%d].\n", id, pszFuncInfo[id], iKeyId);
	}

	if (0 != pMutex->Unlock())
	{
		SW_TRACE_DEBUG("Unlock() failed.\n");
		return -1;
	}
	
	if (iNode == -1)
	{
		SW_TRACE_DEBUG("[error]node=[-1].\n");
		return iRet; //注意:正常不应该走到这个流程
	}

	if (0 != Promsg->pSem[MSG_SHM_SEM_FULL_START]->Post()) //通知消息处理进程
	{
		SW_TRACE_DEBUG("call sem_full.Post() failed.\n");
		return -1;
	}			

	/* 3.等待处理结果 */
	if (0 != Promsg->pSem[MSG_SHM_SEM_FULL_RET_START + iNode]->Pend(MSG_SHM_WAIT_RESP_TIME))//最多等1秒钟
	{
		SW_TRACE_DEBUG("call sem_emptry.Pend(%d-%d) failed.\n",\
				Promsg->pSem[MSG_SHM_SEM_FULL_RET_START + iNode]->Value(),\
				Promsg->pSem[MSG_SHM_SEM_EMPTRY_RET_START + iNode]->Value());
		ifTimeOut = 1;
	}
	if (0 == ifTimeOut)
	{
		//消息正常返回
		if (0 != pMutex->Lock())
		{
			SW_TRACE_DEBUG("call Lock() failed.\n");
			return iRet; //直接退出
		} 
		if (pShm->m_shmMsgNode[iNode].iState == 3) //每次只取一个值出来
		{
			if (pShm->m_shmMsgNode[iNode].dwId == id && lInsertTime == pShm->m_shmMsgNode[iNode].lInsertTime)
			{
				result = pShm->m_shmMsgNode[iNode].iRet; //保存处理的返回值
				SW_TRACE_DEBUG("receive id=[%d] type=[%d] pid=[%d] iRet=[%d].\n",
					pShm->m_shmMsgNode[iNode].dwId,
					pszFuncInfo[id],
					pShm->m_shmMsgNode[iNode].iProcessId,
					pShm->m_shmMsgNode[iNode].iRet);

				//函数为Get型且需要返回值
				if (pszFuncInfo[id] == MSG_TYPE_GET && wParam > 0 && lParam > 0)
				{
					int iLen = (wParam > MSG_SHM_DATA_BUF_LEN) ? MSG_SHM_DATA_BUF_LEN : wParam;
					swpa_memcpy((CHAR *)lParam, pShm->m_shmMsgNode[iNode].szParamBuf, iLen);//把共享内存的数据拷贝回返回值的地址
				}
			}
			else
			{
				SW_TRACE_DEBUG("receive id=[%d] type=[%d] pid=[%d] insert_time=[%ld-%ld] iRet=[%d] failed.\n",
					pShm->m_shmMsgNode[iNode].dwId,
					pszFuncInfo[id],
					pShm->m_shmMsgNode[iNode].iProcessId,
					lInsertTime, pShm->m_shmMsgNode[iNode].lInsertTime,
					pShm->m_shmMsgNode[iNode].iRet);
				result = -1;//返回失败	
			}
			pShm->m_shmMsgNode[iNode].iState = 0;//标记又可以写入了
		}
		if (0 != pMutex->Unlock())
		{
			SW_TRACE_DEBUG("Unlock() failed.\n");
			return -1;
		}
		//释放当前节点
		if (0 != Promsg->pSem[MSG_SHM_SEM_EMPTRY_RET_START + iNode]->Post())
		{
			return -1;
		}
	}
	else
	{
		//如果消息返回超时处理流程
		if (0 != pMutex->Lock())
		{
			SW_TRACE_DEBUG("Lock() failed.\n");
			return -1;
		}
		pShm->m_shmMsgNode[iNode].iState = 0;//释放节点
		if (0 != pMutex->Unlock())
		{
			SW_TRACE_DEBUG("Unlock() failed.\n");
			return -1;
		}
		//表示这个消息已经被处理了
		if (0 != Promsg->pSem[MSG_SHM_SEM_FULL_START]->Pend(100))
		{
			SW_TRACE_DEBUG("call sem_full.Pend(100) failed.\n");
		}
		SW_TRACE_DEBUG("wait:id=[%d] key=[%d] time out.\n", id, iKeyId);
	}
	//唤醒下一个处理线程放数据
	if (0 != Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Post())
	{
		SW_TRACE_DEBUG("call sem_emptry.Post() failed,emptry=[%d] full=[%d].\n",\
					Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Value(),\
					Promsg->pSem[MSG_SHM_SEM_FULL_START]->Value());
	}
	return iRet;
}

/*****************************************************************************
 Prototype    : CSWMessage.MassSend
 Description  : 广播发送
 Input        : INT& result  
 Output       : None
 Return Value : HRESULT
 Calls        : 
 Note         : 
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
HRESULT CSWMessage::MassSend(INT& result)
{
	int iRet = -1; //表示发送失败
	int iNode = -1; //可用节点下标
	int ifTimeOut = 0;//是否超时,0不超时,1表示超时
	DWORD dwKeyId = -1; //共享内存key值
	int MassFlag = 0;

	DWORD* pdwKey = (DWORD*)swpa_mem_alloc(MSG_SHM_PROCESS_NUMBER*sizeof(int));
	if (NULL == pdwKey)
	{
		return -1;
	}
	swpa_memset(pdwKey, 0, MSG_SHM_PROCESS_NUMBER * sizeof(int));

	//SW_TRACE_DEBUG("start<MassSend>\n");

	//遍历找出已经注册的进程
	m_tMsgMutex.Lock();
	if (NULL != m_SharedSharedMemRegister)
	{
		int k = 0;
		for (k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
		{
			//SW_TRACE_DEBUG("#shm[%d]:id=[%d]name=[%s]\n", k, m_SharedSharedMemRegister[k].id, m_SharedSharedMemRegister[k].szProcName);
			if (0 != m_SharedSharedMemRegister[k].id && 0 != strcmp(m_ProcessName, m_SharedSharedMemRegister[k].szProcName))
			{
				pdwKey[k] = m_SharedSharedMemRegister[k].id;
			}
		}
	}
	m_tMsgMutex.Unlock();

	STProcessManageNode* Promsg = NULL;

	int m = 0;
	for (m = 0 ; m < MSG_SHM_PROCESS_NUMBER ; m++)
	{
		//SW_TRACE_DEBUG("#k=[%d]id=[%d]\n", m, m_SharedSharedMemRegister[m].id);
		if (pdwKey[m] == 0)
		{
			continue;
		}

		//SW_TRACE_DEBUG("####k=[%d]id=[%d]\n", m, m_SharedSharedMemRegister[m].id);

		dwKeyId = pdwKey[m];

		//优化:只要iKeyId在这里面找得到的,就不用再发广播消息了
		MassFlag = 0;
		SW_POSITION posM = m_MSGRangList.GetHeadPosition();
		STMessageRangNode* msgRan = NULL;
		while (m_MSGRangList.IsValid(posM))
		{
			msgRan = m_MSGRangList.GetNext(posM);
			if (dwKeyId == msgRan->dwKey)
			{
				MassFlag = 1;
				break;
			}
		}
		if (1 == MassFlag)
		{
			continue;
		}

		//根据keyId找到平台的相关信息
		int keyNode = FindProcByKeyId(dwKeyId);

		//SW_TRACE_DEBUG("#MassSend:FindProcByKeyId:KeyId=[%d],KeyNode=[%d]\n", iKeyId, keyNode);

		if (-1 == keyNode)
		{
			//SW_TRACE_DEBUG("#keyNode=[%d]\n", keyNode);
			if (-1 == (keyNode = UpdateProcByKeyId(dwKeyId)))
			{
				continue;
			}
		}

		/*1. 连接共享内存*/
		Promsg = &m_ProcessManageNode[keyNode];

		if (NULL == Promsg->pShm)
		{
			//连接共享内存
			int state = -1;
			Promsg->pShm = swpa_normal_sharedmem_alloc((INT)dwKeyId, sizeof(STSWShmMessage), &state);
			if (NULL == Promsg->pShm)
			{
				SW_TRACE_DEBUG("#alloc iKeyId=[%d] failed\n", dwKeyId);
				iRet = -1;
				continue;
			}
			
			//连接锁
			Promsg->pMutex = new CSWMutex(dwKeyId + 1);
			SW_TRACE_DEBUG("#alloc mutex_key_id =[%d] success\n", dwKeyId + 1);

			//信号量初始化
			int* init = (int*)swpa_mem_alloc(MSG_SHM_SEM_TOTAIL*sizeof(int));
			init[0] = 0;
			init[1] = MSG_SHM_NODE_NUMBER;
			for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
			{
				init[MSG_SHM_SEM_FULL_RET_START + k] = 0;
			}
			for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
			{
				init[MSG_SHM_SEM_EMPTRY_RET_START + k] = 1;
			}		
			int con_flag = 0;
			for (int k = 0 ; k < MSG_SHM_SEM_TOTAIL ; k++)
			{
				Promsg->pSem[k] = new CSWSemaphore();
				if (0 != Promsg->pSem[k]->Create(init[k], 256, dwKeyId + 2 + k))
				{
					iRet = -1;
					SW_TRACE_DEBUG("#alloc sem_key_id =[%d] failed.\n", dwKeyId + 2 + k);
					con_flag = 1;
					break;
				}
			}
			swpa_mem_free(init);

			if (1 == con_flag)
			{
				continue;
			}
		}

		STSWShmMessage* pShm = (STSWShmMessage*)Promsg->pShm;
		CSWMutex* pMutex = Promsg->pMutex;

		/*2. 申请发送消息*/
		if (0 != Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Pend(MSG_SHM_WAIT_RESP_TIME))
		{
			SW_TRACE_DEBUG("send broadcast time out, emptry=%d, full=%d.\n",\
						Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Value(),\
						Promsg->pSem[MSG_SHM_SEM_FULL_START]->Value());
			continue;
		}

		iNode = -1;

		if (0 != pMutex->Lock())
		{
			SW_TRACE_DEBUG("lock failed.\n");
			iRet = -5;
			goto SEND_END;
		}

		//遍历共享内存找一个可用的节点,并写入数据
		for (int i = 0 ; i < MSG_SHM_NODE_NUMBER ; i++)
		{
			if (pShm->m_shmMsgNode[i].iState == 0)
			{
				iNode = i;
				break;
			}
		}	
		ULONG lInsertTime = 0;
		if (iNode != -1)
		{
			pShm->m_shmMsgNode[iNode].iProcessId = swpa_utils_process_get_id();//埴调用者的进程Id
			pShm->m_shmMsgNode[iNode].lInsertTime = swpa_datetime_gettick();//插入时间
			lInsertTime = pShm->m_shmMsgNode[iNode].lInsertTime;
			pShm->m_shmMsgNode[iNode].dwId = 0; //0表示广播消息
			pShm->m_shmMsgNode[iNode].iParamLen = MSG_SHM_DATA_BUF_LEN;//填长度
			pShm->m_shmMsgNode[iNode].iState = 1; //表示发送请求
			pShm->m_shmMsgNode[iNode].iRet = 0;//默认需要返回结果
		}
		else
		{
			if (0 != pMutex->Unlock())
			{
				SW_TRACE_DEBUG("unlock failed.\n");
			}

			SW_TRACE_DEBUG("send broadcast node = -1\n");
			continue;
		}
		if (0 != pMutex->Unlock())
		{
			SW_TRACE_DEBUG("unlock failed.\n");
		}

		if (0 != Promsg->pSem[MSG_SHM_SEM_FULL_START]->Post()) //通知消息处理进程
		{
			SW_TRACE_DEBUG("call full failed.\n");
			iRet = -5;
			goto SEND_END;
		}			

		/*3. 等待结果*/
		if (0 != Promsg->pSem[MSG_SHM_SEM_FULL_RET_START + iNode]->Pend(MSG_SHM_WAIT_RESP_TIME))//最多等1秒钟
		{
			SW_TRACE_DEBUG("wait broadcast time out. node=%d, full_ret=%d, emptry_ret=%d.\n", \
					iNode,\
					Promsg->pSem[MSG_SHM_SEM_FULL_RET_START + iNode]->Value(),\
					Promsg->pSem[MSG_SHM_SEM_EMPTRY_RET_START + iNode]->Value());
			ifTimeOut = 1;//超时
		}
		if (0 == ifTimeOut)
		{
			//消息正常返回
			if (0 != pMutex->Lock())
			{
				SW_TRACE_DEBUG("lock failed.\n");
				iRet = -5;
				goto SEND_END;
			}

			if (pShm->m_shmMsgNode[iNode].iState == 3)
			{
				if (0 == pShm->m_shmMsgNode[iNode].dwId && lInsertTime == pShm->m_shmMsgNode[iNode].lInsertTime)
				{
					char* pszBuf = (char*)swpa_mem_alloc(MSG_SHM_DATA_BUF_LEN + 1);
					swpa_memset(pszBuf, 0, MSG_SHM_DATA_BUF_LEN + 1);
					int iLen = pShm->m_shmMsgNode[iNode].iParamLen < MSG_SHM_DATA_BUF_LEN ? pShm->m_shmMsgNode[iNode].iParamLen : MSG_SHM_DATA_BUF_LEN;
					swpa_memcpy(pszBuf, pShm->m_shmMsgNode[iNode].szParamBuf, iLen);
					SW_TRACE_DEBUG("receive:type=[broadcast] key=[%d] node=[%d] id=[%d] len=[%d] buf=[%s]\n",\
						dwKeyId,\
						iNode, pShm->m_shmMsgNode[iNode].dwId,\
						pShm->m_shmMsgNode[iNode].iParamLen,\
						pShm->m_shmMsgNode[iNode].szParamBuf);

					AnalyResponse(pszBuf);//解析广播消息
					swpa_mem_free(pszBuf);
				}
				else
				{
					SW_TRACE_DEBUG("receive:type=[broadcast] key=[%d] state=%d, node=[%d] id=[%d] cmp time=[%ld-%ld]\n",\
						dwKeyId,\
						pShm->m_shmMsgNode[iNode].iState,\
						iNode, pShm->m_shmMsgNode[iNode].dwId,\
						lInsertTime, pShm->m_shmMsgNode[iNode].lInsertTime);
				}
				pShm->m_shmMsgNode[iNode].iState = 0;//释放节点
			}
			if (0 != pMutex->Unlock())
			{
				SW_TRACE_DEBUG("Unlock failed.\n");
				iRet = -5;
				goto SEND_END;
			}

			//释放当前节点
			if (0 != Promsg->pSem[MSG_SHM_SEM_EMPTRY_RET_START + iNode]->Post())
			{
				iRet = -5;
				goto SEND_END;
			}
		}
		else
		{
			//应答超时处理
			if (0 != pMutex->Lock())
			{
				SW_TRACE_DEBUG("lock failed.\n");
				iRet = -5;
				goto SEND_END;
			}
			pShm->m_shmMsgNode[iNode].iState = 0;//释放节点
			if (0 != pMutex->Unlock())
			{
				SW_TRACE_DEBUG("Unlock failed.\n");
				iRet = -5;
				goto SEND_END;
			}

			//表示这个消息已经被处理了
			if (0 != Promsg->pSem[MSG_SHM_SEM_FULL_START]->Pend(100))
			{
				SW_TRACE_DEBUG("call sem_full.Pend(-1) failed.\n");
			}
			SW_TRACE_DEBUG("wait:node=%d, id=[%d] key=[%d] time out.\n", iNode, 0, dwKeyId);
		}
		//唤醒下一个处理线程放数据
		if (0 != Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Post())
		{
			SW_TRACE_DEBUG("call sem_emptry.Pend(%d-%d) failed.\n",\
					Promsg->pSem[MSG_SHM_SEM_EMPTRY_START]->Value(),\
					Promsg->pSem[MSG_SHM_SEM_FULL_START]->Value());
		}	
		iRet = 0; //成功
	}

	SEND_END ://发送统一出口

	swpa_mem_free(pdwKey);

	return iRet;
}

/*****************************************************************************
 Prototype    : CSWMessage.OnProMSG
 Description  : 进程间消息处理线程
 Input        : void* pvParam  
 Output       : None
 Return Value : void*
 Calls        : 
 Note         : 
 
  History        :
  1.Date         : 2015/8/20
    Author       : huangdch
    Modification : Created function

*****************************************************************************/
void* CSWMessage::OnProMSG(void* pvParam)
{
	SW_TRACE_NORMAL("<SWMessage::OnProMSG>run.");

	if (0 == MSG_SHM_PROCESS_NUMBER)
	{
		SW_TRACE_DEBUG("MSG_SHM_PROCESS_NUMBER=[0]\n");
		return 0; //不需要进程间的通信
	}
	
	int iRet = -1;
	int iNode = -1;
	DWORD id = -1;
	int iCire = 0;
	int iDoFlag = 0; //是否真正被处理
	int cur_node = -1;

	SW_TRACE_DEBUG("OnProMSG,process_name=[%s]\n", m_ProcessName);

	char* pszFuncInfo = ((char*)m_SharedSharedMemRegister + MSG_SHM_PROCESS_NUMBER * sizeof(STSharedMemRegisterNode));
	
	ULONG lStartTime = 0;
	ULONG lEndTime = 0;
	CSWMessage* msg = NULL;
	while (S_OK == m_pMsgThread->IsValid() && !CSWApplication::IsExited())
	{
		if (cur_node != -1 && m_ProcessManageCur.dwKey > 0 && m_ProcessManageCur.pShm != NULL)
		{
			if (0 != m_ArySem[MSG_SHM_SEM_FULL_START].Pend(MSG_SHM_WAIT_RESP_TIME))
			{
				//每5秒更新一次平台的内存数据
				lEndTime = swpa_datetime_gettick();
				if (lEndTime - lStartTime > MSG_SHM_UPDATE_TIME * 1000)
				{
					lStartTime = lEndTime;
					//每次超时没数据的时候更新这个
					if (0 != m_tMsgMutex.Lock())
					{
						SW_TRACE_DEBUG("lock[%d] failed.\n", tMutex.GetId());
						break;
					}
					//int node = -1;
					for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
					{
						if (0 != m_SharedSharedMemRegister[k].id && 0 != swpa_strcmp(m_ProcessName, m_SharedSharedMemRegister[k].szProcName))
						{
							//更新非当前平台的内存信息
							if (-1 == FindProcByKeyId(m_SharedSharedMemRegister[k].id))
							{
								if (-1 != UpdateProcByKeyId(m_SharedSharedMemRegister[k].id, k))
								{
									SW_TRACE_DEBUG("update process info:key_id=[%d] success.\n", k);
								}
							}
						}
					}
					if (0 != m_tMsgMutex.Unlock())
					{
						SW_TRACE_DEBUG("Unlock[%d] failed.\n", tMutex.GetId());
						break;
					}
				}
				//SW_TRACE_DEBUG("wait msg time out,full=[%d] emptry=[%d]\n", m_ArySem[MSG_SHM_SEM_FULL_START].Value(), m_ArySem[MSG_SHM_SEM_EMPTRY_START].Value());
				continue;
			}

			//SW_TRACE_DEBUG("wait lock(%d)......\n", tMutex.GetId());

			if (0 != tMutex.Lock())
			{
				SW_TRACE_DEBUG("lock[%d] failed.\n", tMutex.GetId());
				break;
			}
			//SW_TRACE_DEBUG("get lock()......\n");

			iNode = -1;//记录时间最早的节点的下标
			ULONG iMinTime = ((ULONG) - 1) >> 1;
			ULONG lInsertTime = 0;
			//遍历取出插入时间最早的一个节点来处理,避免消息死节点
			for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
			{
				if (m_pShm->m_shmMsgNode[k].iState == 1)
				{
					if (m_pShm->m_shmMsgNode[k].lInsertTime < iMinTime)
					{
						iMinTime = m_pShm->m_shmMsgNode[k].lInsertTime;
						iNode = k;
					}
				}
			}	

			SW_TRACE_DEBUG("receive:node=[%d]msg_id=[%d]process_id=[%d]insert_time=[%lu]\n",
				iNode,
				m_pShm->m_shmMsgNode[iNode].dwId,
				m_pShm->m_shmMsgNode[iNode].iProcessId,
				m_pShm->m_shmMsgNode[iNode].lInsertTime);

			if (iNode != -1)
			{
				m_pShm->m_shmMsgNode[iNode].iState = 2; //表示正在处理中
			}
			else
			{
				if (0 != tMutex.Unlock())
				{
					SW_TRACE_DEBUG("Unlock[%d] failed.\n", tMutex.GetId());
					break;
				}
				SW_TRACE_DEBUG("node=%d\n", iNode);
				continue;
			}
			lInsertTime = m_pShm->m_shmMsgNode[iNode].lInsertTime;

			if (0 != tMutex.Unlock())
			{
				SW_TRACE_DEBUG("Unlock[%d] failed.\n", tMutex.GetId());
				break;
			}
			
			//消息处理:调用内部进程内通信的接口进去处理
			m_rwLock.RLock();
			id = m_pShm->m_shmMsgNode[iNode].dwId;
			//id = 0为广播消息
			if (id != 0)
			{
				SW_POSITION pos = m_clsMSGList.GetHeadPosition();
				iDoFlag = 0;
				iRet = -1;
				while (m_clsMSGList.IsValid(pos))
				{
					msg = m_clsMSGList.GetNext(pos);
					if (id > msg->m_dwStartID && id < msg->m_dwEndID)
					{
						iDoFlag = 1;
						if (pszFuncInfo[id] == MSG_TYPE_SET)//Set类型
						{
							if (m_pShm->m_shmMsgNode[iNode].iParamLen == 0)
							{
								//整形方式
								iRet = msg->OnMessage(id, swpa_atoi(m_pShm->m_shmMsgNode[iNode].szParamBuf), 0);//整形Set模式
							}
							else
							{
								//指针方式
								iRet = msg->OnMessage(id, (unsigned int) & m_pShm->m_shmMsgNode[iNode].szParamBuf, m_pShm->m_shmMsgNode[iNode].iParamLen);
							}
							iDoFlag = 1;
						}
						else if (pszFuncInfo[id] == MSG_TYPE_GET)
						{
							iRet = msg->OnMessage(id, m_pShm->m_shmMsgNode[iNode].iParamLen, (unsigned int) & m_pShm->m_shmMsgNode[iNode].szParamBuf);//Get模式
							iDoFlag = 1;
						}
						//m_pShm->m_shmMsgNode[iNode].iRet = iRet; //函数处理的返回结果
						break;
					}
				}
				m_pShm->m_shmMsgNode[iNode].iRet = iRet; //函数处理的返回结果
			}
			m_rwLock.Unlock();

			//返回结果
			if (0 != m_ArySem[MSG_SHM_SEM_EMPTRY_RET_START + iNode].Pend(-1))
			{
				SW_TRACE_DEBUG("response time out,node=%d, emptry_ret=%d, full_ret=%d.\n",\
							iNode,\
							m_ArySem[MSG_SHM_SEM_EMPTRY_RET_START + iNode].Value(),\
							m_ArySem[MSG_SHM_SEM_FULL_RET_START + iNode].Value());
				break;
			}

			if (0 != tMutex.Lock())
			{
				SW_TRACE_DEBUG("lock[%d] failed.\n", tMutex.GetId());
				break;
			}
			
			if (m_pShm->m_shmMsgNode[iNode].iState == 2 && lInsertTime == m_pShm->m_shmMsgNode[iNode].lInsertTime)
			{
				if (0 == id) //广播消息应答
				{
					int iLen = 0;
					char* pszBuf = (char*)swpa_mem_alloc(MSG_SHM_DATA_BUF_LEN + 1);
					swpa_memset(pszBuf, 0, MSG_SHM_DATA_BUF_LEN + 1);
					iLen += snprintf(pszBuf + iLen, MSG_SHM_DATA_BUF_LEN, "%d-%d|", m_ProcessManageCur.dwKey, m_ProcessManageCur.dwKey);
					SW_POSITION pos = m_clsMSGList.GetHeadPosition();
					while (m_clsMSGList.IsValid(pos))
					{
						msg = m_clsMSGList.GetNext(pos);
						iLen += swpa_snprintf(pszBuf + iLen, MSG_SHM_DATA_BUF_LEN, "%d-%d|", msg->m_dwStartID, msg->m_dwEndID);
					}					
					//去掉最后一"|"字符
					pszBuf[iLen - 1] = '\0';
					swpa_memset(m_pShm->m_shmMsgNode[iNode].szParamBuf, 0, sizeof(m_pShm->m_shmMsgNode[iNode].szParamBuf));
					m_pShm->m_shmMsgNode[iNode].iParamLen = iLen - 1; //返回参数的长度
					swpa_memcpy(m_pShm->m_shmMsgNode[iNode].szParamBuf, pszBuf, iLen - 1);//内容
					swpa_mem_free(pszBuf);
					m_pShm->m_shmMsgNode[iNode].iProcessId = swpa_utils_process_get_id();//这个要返回去
					SW_TRACE_DEBUG("response:type=[broadcast] key=[%d] node=[%d] msg_id=[%d] process_id=[%d] len=[%d] buf=[%s]\n", 
							m_pShm->m_KeyId,
							iNode, m_pShm->m_shmMsgNode[iNode].dwId,
							m_pShm->m_shmMsgNode[iNode].iProcessId, 
							m_pShm->m_shmMsgNode[iNode].iParamLen,
							m_pShm->m_shmMsgNode[iNode].szParamBuf);
				}
				else
				{
					//返回结果值已经在返回的地方处理了
					SW_TRACE_DEBUG("response:type=[normal] node=[%d] msg_id=[%d] process_id=[%d] do_flag=[%d] ret=[%d]\n",\
						iNode, m_pShm->m_shmMsgNode[iNode].dwId,
						m_pShm->m_shmMsgNode[iNode].iProcessId, iDoFlag,
						m_pShm->m_shmMsgNode[iNode].iRet);
				}

				m_pShm->m_shmMsgNode[iNode].iState = 3;//标记返回结果

				if (0 != tMutex.Unlock())
				{
					SW_TRACE_DEBUG("Unlock[%d] failed.\n", tMutex.GetId());
					break;
				}

				if (0 != m_ArySem[MSG_SHM_SEM_FULL_RET_START + iNode].Post())
				{
					SW_TRACE_DEBUG("call sem_full_ret.Pend(%d-%d) failed.\n",
								m_ArySem[MSG_SHM_SEM_FULL_RET_START + iNode].Value(),
								m_ArySem[MSG_SHM_SEM_EMPTRY_RET_START + iNode].Value());
					break;//退出线程
				}
			}
			else
			{
				if (0 != tMutex.Unlock())
				{
					SW_TRACE_DEBUG("Unlock[%d] failed.\n", tMutex.GetId());
					break;
				}
				//如果写不进去表示这个节点已经被超时了,就不能执行sem_full_ret.Post(),而且换成自己释放节点了
				if (0 != m_ArySem[MSG_SHM_SEM_EMPTRY_RET_START + iNode].Post())
				{
					break;
				}
				SW_TRACE_DEBUG("response:node=[%d] msg_id=[%d] process_id=[%d] state=[%d] insert_time=[%lu-%lu] time out.\n", 
					iNode,
					m_pShm->m_shmMsgNode[iNode].dwId,
					m_pShm->m_shmMsgNode[iNode].iProcessId,
					m_pShm->m_shmMsgNode[iNode].iState,
					lInsertTime, m_pShm->m_shmMsgNode[iNode].lInsertTime);
			}
			SW_TRACE_DEBUG("(%d)***********************************************************\n", ++iCire);
		}
		else
		{
			/* 遍历注册区,看有没有进程的注册信息 */
			int node = -1;

			if (0 != m_tMsgMutex.Lock())
			{
				SW_TRACE_DEBUG("lock[%d] failed.\n", tMutex.GetId());
				break;
			}

			if (NULL != m_SharedSharedMemRegister)
			{
				int k = 0;
				for (k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
				{
					if (0 != m_SharedSharedMemRegister[k].id && 0 == swpa_strcmp(m_ProcessName, m_SharedSharedMemRegister[k].szProcName))
					{
						node = k;
						cur_node = node;
						break;
					}
				}
			}

			int sleep_flag = 0;

			if (-1 != node)
			{
				sleep_flag = 1;
				int size = sizeof(STSWShmMessage);
				int key = m_SharedSharedMemRegister[node].id;

				SW_TRACE_DEBUG("shm key_id=[%d] size=[%d] start init.\n", key, size);

				//创建共享内存数据交换区
				int state = 0;
				m_pShm = (STSWShmMessage *)swpa_normal_sharedmem_alloc(key, size, &state);
				if (NULL == m_pShm)
				{
					SW_TRACE_DEBUG("alloc key=%d,size=%d failed.\n", key, size);
					m_tMsgMutex.Unlock();
					break;
				}
				if (0 == state)
				{
					memset(m_pShm, 0, size);
					SW_TRACE_DEBUG("init shm key=%d success.\n", key);
				}
				
				m_pShm->m_KeyId = key;
				m_ProcessManageCur.dwKey = key;
				m_ProcessManageCur.iProcessId = swpa_utils_process_get_id();
				m_ProcessManageCur.pShm = (void*)m_pShm;
				swpa_strncpy(m_ProcessManageCur.szProcessName, m_ProcessName, sizeof(m_ProcessManageCur.szProcessName) - 1);

				//初始化锁
				if (0 != tMutex.Create(key + 1))
				{
					SW_TRACE_DEBUG("create mutex id=%d failed.\n", key + 1);
					m_tMsgMutex.Unlock();
					break;
				}

				//初始化信号量
				int* init = (int*)swpa_mem_alloc(MSG_SHM_SEM_TOTAIL*sizeof(int));
				init[0] = 0;
				init[1] = MSG_SHM_NODE_NUMBER;


				for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
				{
					init[MSG_SHM_SEM_FULL_RET_START + k] = 0;
				}


				for (int k = 0 ; k < MSG_SHM_NODE_NUMBER ; k++)
				{
					init[MSG_SHM_SEM_EMPTRY_RET_START + k] = 1;
				}		

				for (int k = 0 ; k < MSG_SHM_SEM_TOTAIL ; k++)
				{
					if (0 != m_ArySem[k].Create(init[k], 256, key + 2 + k))
					{
						SW_TRACE_DEBUG("create sem id=%d failed.\n", key + 2 + k);
						break;
					}
				}

				swpa_mem_free(init);
				
				SW_TRACE_DEBUG("shm key_id=[%d] size=[%d] init success.\n", key, size);
				SW_TRACE_DEBUG("cur process info:id=[%d]name=[%s]\n", m_ProcessManageCur.dwKey, m_ProcessManageCur.szProcessName);
			}

			//更新平台内存的相关信息
			node = -1;
			for (int k = 0 ; k < MSG_SHM_PROCESS_NUMBER ; k++)
			{
				if (m_SharedSharedMemRegister != NULL && 0 != m_SharedSharedMemRegister[k].id && 0 != swpa_strcmp(m_ProcessName, m_SharedSharedMemRegister[k].szProcName))
				{
					//更新非当前平台的内存信息
					if (-1 == FindProcByKeyId(m_SharedSharedMemRegister[k].id))
					{
						if (-1 != UpdateProcByKeyId(m_SharedSharedMemRegister[k].id, k))
						{
							SW_TRACE_DEBUG("update process info:key_id=[%d] success.\n", k);
						}
					}
				}
			}

			m_tMsgMutex.Unlock();

			if (0 == sleep_flag)
			{
				CSWApplication::Sleep(5000);
			}

		}
	}
	SW_TRACE_NORMAL("<SWMessage::OnProMSG>exit.");

	return 0;
}
	

