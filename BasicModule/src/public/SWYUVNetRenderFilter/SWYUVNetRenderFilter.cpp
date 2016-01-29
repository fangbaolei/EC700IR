#include "SWFC.h"
#include "SWYUVNetRenderFilter.h"
#include "SWPosImage.h"

CSWYUVNetRenderFilter::CSWYUVNetRenderFilter():CSWBaseFilter(1,0)
{
	m_hClientList = swpa_list_create();
	GetIn(0)->AddObject(CLASSID(CSWImage));
}

CSWYUVNetRenderFilter::~CSWYUVNetRenderFilter()
{
	swpa_list_destroy(m_hClientList);
}

HRESULT CSWYUVNetRenderFilter::Run()
{
	if(S_OK == CSWBaseFilter::Run()
	&& S_OK == m_tcpServer.Create()
	&& S_OK == m_tcpServer.Bind(NULL, 9988)
	&& S_OK == m_tcpServer.Listen()
	)
	{
		SW_TRACE_NORMAL("create yuv sender server:9988 is ok\n");
		m_cThreadServer.Start(OnListen, this);
		return S_OK;
	}
	else
	{
		SW_TRACE_NORMAL("create yuv sender server:9999 is failed\n");
		return E_FAIL;
	}
}

HRESULT CSWYUVNetRenderFilter::Stop()
{
	m_tcpServer.Close();
	CSWTCPSocket *pClient = NULL;
	while(NULL != (pClient = (CSWTCPSocket *)swpa_list_remove(m_hClientList)))
	{
		pClient->Release();
	}
	m_cThreadServer.Stop();
	return CSWBaseFilter::Stop();
}

HRESULT CSWYUVNetRenderFilter::Receive(CSWObject* obj)
{
	if(IsDecendant(CSWImage, obj))
	{
		CSWImage *pImage = (CSWImage *)obj;
		INT iWidth = pImage->GetWidth();
		INT iHeight = pImage->GetHeight();
		INT iSize = pImage->GetSize();		
		for(int pos = swpa_list_open(m_hClientList); pos != -1; pos = swpa_list_next(m_hClientList))
		{
			CSWTCPSocket *pClient = (CSWTCPSocket *)swpa_list_value(m_hClientList, pos);
#if 0			
			SendYUV422Image(pClient, pImage);
#else
			if(FAILED(pClient->Send(&iWidth, 4))
			|| FAILED(pClient->Send(&iHeight, 4))
			|| FAILED(pClient->Send(&iSize, 4))
			|| FAILED(pClient->Send(pImage->GetImageBuffer(), iSize)))
			{
				char szIP[255];
				pClient->GetPeerName(szIP);
				SW_TRACE_NORMAL("send data failed, disconet %s.\n", szIP);
				swpa_list_delete(m_hClientList, pos);
				pClient->Release();
			}	
#endif			
		}		
		swpa_list_close(m_hClientList);
	}
	return S_OK;
}

HRESULT  CSWYUVNetRenderFilter::SendYUV422Image(CSWTCPSocket *pClient, CSWImage *pImage)
{
	SW_COMPONENT_IMAGE sComponentImage;
	pImage->GetImage(&sComponentImage);   
	
	pClient->Send(&sComponentImage.iWidth, 4);
	pClient->Send(&sComponentImage.iHeight, 4);
	
	BYTE *pSrc = sComponentImage.rgpbData[0]; //Y
	pClient->Send(pSrc, sComponentImage.iHeight * sComponentImage.rgiStrideWidth[0]);
	
	pSrc = sComponentImage.rgpbData[1]; //U
	pClient->Send(pSrc, sComponentImage.iHeight * sComponentImage.rgiStrideWidth[1]);
	
	pSrc = sComponentImage.rgpbData[2]; //V
	pClient->Send(pSrc, sComponentImage.iHeight * sComponentImage.rgiStrideWidth[2]);
	
	/*
	pSrc = sComponentImage.rgpbData[1];	//U
	for (INT iHeight = 0; iHeight < sComponentImage.iHeight; iHeight++, pSrc += (sComponentImage.rgiStrideWidth[1]))
	{
	    pClient->Send(pSrc, sComponentImage.iWidth/2);
	}
	
	pSrc = sComponentImage.rgpbData[2];	//V
	for (INT iHeight = 0; iHeight < sComponentImage.iHeight; iHeight++, pSrc += (sComponentImage.rgiStrideWidth[2]))
	{
	    pClient->Send(pSrc, sComponentImage.iWidth/2);
	}
	*/		    
	return S_OK;
}

PVOID CSWYUVNetRenderFilter::OnListen(PVOID pvParam)
{
	CSWYUVNetRenderFilter *pThis = (CSWYUVNetRenderFilter *)pvParam;
	while(S_OK == pThis->m_cThreadServer.IsValid() && pThis->GetState() == FILTER_RUNNING)
	{
		SWPA_SOCKET_T sock;
		if(S_OK == pThis->m_tcpServer.Accept(sock))
		{
			CSWTCPSocket *pClient = new CSWTCPSocket();
			pClient->Attach(sock);
			
			char szIP[255];
			pClient->GetPeerName(szIP);
			SW_TRACE_NORMAL("receive a connect from %s\n", szIP);
			
			if(0 < swpa_list_add(pThis->m_hClientList, pClient))
			{
				SW_TRACE_NORMAL("swpa_list_add %s ok.\n", szIP);
			}
			else
			{
				SW_TRACE_NORMAL("swpa_list_add %s failed.\n", szIP);
				pClient->Release();
			}
		}
	}
}
