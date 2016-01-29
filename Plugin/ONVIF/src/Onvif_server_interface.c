/*
 * onvif_server_interface.c
 *
 *  Created on: 2014年7月10日
 *      Author: Yinj
 */

#include <stdio.h>
#include "soapStub.h"
#include "stdsoap2.h"
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>

#include "onvif_function.h"	
#include "onvifLib.h"
#include "wsseapi.h"

#define MANUFACTURER "www.hunda.com"
#define DEV_MODEL "Dinion"
#define DEV_FIRMWAREVERSION "auto"

#define MACH_ADDR_LENGTH 6
#define INFO_LENGTH 512
#define LARGE_INFO_LENGTH 1024
#define SMALL_INFO_LENGTH 512

const int USER_MANAGE_GET_LIST = 1;
const int USER_MANAGE_ADD_USER = 2;
const int USER_MANAGE_DELETE_USER = 3;
const int USER_MANAGE_MODIFY_USER = 4;


extern int PTZControl(const int iCmdID, void* pvArg);
extern int UserManage(const int iCmdID, void * pvArg);
extern int GetUser(void* psize, void* pvArg);
extern int g_AuthenticateEnable;




int GetUserList(int* piConut,UserArray_t* psUserlist)
{
	char cUserNamePasswd[128] = {0};
	snprintf(cUserNamePasswd,128,"<CMD>%d</CMD>;<UserName>NULL</UserName>;<PassWord>NULL</PassWord>\r\n",USER_MANAGE_GET_LIST);
	int len = strlen(cUserNamePasswd);

	if(0 != UserManage(len,cUserNamePasswd))
	{
		printf("err: get username list failed\n");
		return SOAP_ERR;
	}
	
//	usleep(10*1000);    //休眠40ms

	if (0 != GetUser(piConut, psUserlist))
	{
		printf("[line %d]err: GetUser failed\n",__LINE__);
		return SOAP_ERR;
	}

	return SOAP_OK;
}


static int GetPTZConfig(struct soap *soap, struct tt__PTZConfiguration * PTZConfiguration)
{
	ONVIF_PTZ_PARAM sParam;
	memset(&sParam, 0, sizeof(ONVIF_PTZ_PARAM));
	
	if (0 != GetPTZParam(&sParam) || !sParam.fInited)
	{
		return -1;
	}

	//required info
	PTZConfiguration->Name = "Profile_name_0";
	PTZConfiguration->UseCount = 1; //todo: what's this?
	PTZConfiguration->token = "Profile_name_0";
	PTZConfiguration->NodeToken = "Profile_name_0";

	//DefaultPTZSpeed  -- optional
	{
		PTZConfiguration->DefaultPTZSpeed =(struct tt__PTZSpeed *)soap_malloc(soap, sizeof(struct tt__PTZSpeed));
		memset(PTZConfiguration->DefaultPTZSpeed, 0, sizeof(struct tt__PTZSpeed));
		
		PTZConfiguration->DefaultPTZSpeed->PanTilt = (struct tt__Vector2D *)soap_malloc(soap, sizeof(struct tt__Vector2D));
		memset(PTZConfiguration->DefaultPTZSpeed->PanTilt, 0, sizeof(struct tt__Vector2D));
		PTZConfiguration->DefaultPTZSpeed->PanTilt->space = NULL;
		PTZConfiguration->DefaultPTZSpeed->PanTilt->x = (sParam.iAbsolutePRangeMax + sParam.iAbsolutePRangeMin) >> 1;
		PTZConfiguration->DefaultPTZSpeed->PanTilt->y = (sParam.iAbsoluteTRangeMax + sParam.iAbsoluteTRangeMin) >> 1;

		PTZConfiguration->DefaultPTZSpeed->Zoom = (struct tt__Vector1D *)soap_malloc(soap, sizeof(struct tt__Vector1D));
		memset(PTZConfiguration->DefaultPTZSpeed->Zoom, 0, sizeof(struct tt__Vector1D));
		PTZConfiguration->DefaultPTZSpeed->Zoom->space = NULL;
		PTZConfiguration->DefaultPTZSpeed->Zoom->x = (sParam.iAbsoluteZRangeMax + sParam.iAbsoluteZRangeMin) >> 1;
	}

	//PanTiltLimits -- optional
	{
		PTZConfiguration->PanTiltLimits =(struct tt__PanTiltLimits *)soap_malloc(soap, sizeof(struct tt__PanTiltLimits));
		memset(PTZConfiguration->PanTiltLimits, 0, sizeof(struct tt__PanTiltLimits));

		PTZConfiguration->PanTiltLimits->Range =(struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
		memset(PTZConfiguration->PanTiltLimits->Range, 0, sizeof(struct tt__Space2DDescription));

		PTZConfiguration->PanTiltLimits->Range->URI = "pantiltlimits"; //todo

		PTZConfiguration->PanTiltLimits->Range->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
		memset(PTZConfiguration->PanTiltLimits->Range->XRange, 0, sizeof(struct tt__FloatRange));
		PTZConfiguration->PanTiltLimits->Range->XRange->Max = sParam.iAbsolutePRangeMax;
		PTZConfiguration->PanTiltLimits->Range->XRange->Min = sParam.iAbsolutePRangeMin;

		PTZConfiguration->PanTiltLimits->Range->YRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
		memset(PTZConfiguration->PanTiltLimits->Range->YRange, 0, sizeof(struct tt__FloatRange));
		PTZConfiguration->PanTiltLimits->Range->YRange->Max = sParam.iAbsoluteTRangeMax;
		PTZConfiguration->PanTiltLimits->Range->YRange->Min = sParam.iAbsoluteTRangeMin;
	}

	//ZoomLimits  -- optional
	{
		PTZConfiguration->ZoomLimits =(struct tt__ZoomLimits *)soap_malloc(soap, sizeof(struct tt__ZoomLimits));
		memset(PTZConfiguration->ZoomLimits, 0, sizeof(struct tt__ZoomLimits));

		PTZConfiguration->ZoomLimits->Range =(struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
		memset(PTZConfiguration->ZoomLimits->Range, 0, sizeof(struct tt__Space1DDescription));

		PTZConfiguration->ZoomLimits->Range->URI = "zoomlimits";//todo

		PTZConfiguration->ZoomLimits->Range->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
		memset(PTZConfiguration->ZoomLimits->Range->XRange, 0, sizeof(struct tt__FloatRange));
		PTZConfiguration->ZoomLimits->Range->XRange->Max = sParam.iAbsoluteZRangeMax;
		PTZConfiguration->ZoomLimits->Range->XRange->Min = sParam.iAbsoluteZRangeMin;
	}
	
	return 0;
}


static int GetPTZNode(struct soap *soap, struct tt__PTZNode *PTZNode)
{
	ONVIF_PTZ_PARAM sParam;
	memset(&sParam, 0, sizeof(ONVIF_PTZ_PARAM));
	
	if (0 != GetPTZParam(&sParam) || !sParam.fInited)
	{
		return -1;
	}

	//required info
	PTZNode->token = "Profile_name_0";
	PTZNode->MaximumNumberOfPresets = sParam.iPresetCount;
	PTZNode->HomeSupported = sParam.fHomeSupported ? xsd__boolean__true_ : xsd__boolean__false_;;
	
	PTZNode->Name = "Profile_name_0"; //todo
	PTZNode->FixedHomePosition = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
	*PTZNode->FixedHomePosition = sParam.fFixedHomePos ? xsd__boolean__true_ : xsd__boolean__false_;
	

	//SupportedPTZSpaces -- required
	{
		PTZNode->SupportedPTZSpaces =(struct tt__PTZSpaces *)soap_malloc(soap, sizeof(struct tt__PTZSpaces));
		memset(PTZNode->SupportedPTZSpaces, 0, sizeof(struct tt__PTZSpaces));

		//ContinuousMoveSpace
		if (sParam.fContinuousMoveSupport)
		{
			PTZNode->SupportedPTZSpaces->__sizeContinuousPanTiltVelocitySpace = 1;

			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace =(struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
			memset(PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace, 0, sizeof(struct tt__Space2DDescription));
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->URI = "c_pantiltspeed"; //todo
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Max = 30;
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->XRange->Min = 0;
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Max = 30;
			PTZNode->SupportedPTZSpaces->ContinuousPanTiltVelocitySpace->YRange->Min = 0;
			
			PTZNode->SupportedPTZSpaces->__sizeContinuousZoomVelocitySpace = 1;
			PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace =(struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
			memset(PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace, 0, sizeof(struct tt__Space1DDescription));
			PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->URI = "c_zoomspeed"; //todo
			PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Max = 7;
			PTZNode->SupportedPTZSpaces->ContinuousZoomVelocitySpace->XRange->Min = 0;
		}

		//AbsoluteMoveSpace
		if (sParam.fAbsoluteMoveSupport)
		{
			PTZNode->SupportedPTZSpaces->__sizeAbsolutePanTiltPositionSpace = 1;

			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace =(struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
			memset(PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace, 0, sizeof(struct tt__Space2DDescription));
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->URI = "c_pantiltposition"; //todo
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Max = sParam.iAbsolutePRangeMax;
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->XRange->Min = sParam.iAbsolutePRangeMin;
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Max = sParam.iAbsoluteTRangeMax;
			PTZNode->SupportedPTZSpaces->AbsolutePanTiltPositionSpace->YRange->Min = sParam.iAbsoluteTRangeMin;
			
			PTZNode->SupportedPTZSpaces->__sizeAbsoluteZoomPositionSpace = 1;
			PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace =(struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
			memset(PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace, 0, sizeof(struct tt__Space1DDescription));
			PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->URI = "c_zoomposition"; //todo
			PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Max = sParam.iAbsoluteZRangeMax;
			PTZNode->SupportedPTZSpaces->AbsoluteZoomPositionSpace->XRange->Min = sParam.iAbsoluteZRangeMin;
		}

		//RelativeMoveSpace -- not supported
		if (0)
		{
			PTZNode->SupportedPTZSpaces->__sizeRelativePanTiltTranslationSpace = 1;

			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace =(struct tt__Space2DDescription *)soap_malloc(soap, sizeof(struct tt__Space2DDescription));
			memset(PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace, 0, sizeof(struct tt__Space2DDescription));
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->URI = "c_pantiltposition"; //todo
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Max = sParam.iAbsolutePRangeMax;
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->XRange->Min = sParam.iAbsolutePRangeMin;
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Max = sParam.iAbsoluteTRangeMax;
			PTZNode->SupportedPTZSpaces->RelativePanTiltTranslationSpace->YRange->Min = sParam.iAbsoluteTRangeMin;
			
			PTZNode->SupportedPTZSpaces->__sizeRelativeZoomTranslationSpace = 1;
			PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace =(struct tt__Space1DDescription *)soap_malloc(soap, sizeof(struct tt__Space1DDescription));
			memset(PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace, 0, sizeof(struct tt__Space1DDescription));
			PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->URI = "c_zoomposition"; //todo
			PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange =(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange));
			memset(PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange, 0, sizeof(struct tt__FloatRange));
			PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Max = sParam.iAbsoluteZRangeMax;
			PTZNode->SupportedPTZSpaces->RelativeZoomTranslationSpace->XRange->Min = sParam.iAbsoluteZRangeMin;
		}
	}

	//optional
	PTZNode->__sizeAuxiliaryCommands = 0;
	PTZNode->AuxiliaryCommands = NULL;
	PTZNode->__anyAttribute = NULL;
	PTZNode->Extension = NULL;
	
	return 0;
}


SOAP_FMAC5 int SOAP_FMAC6 soap_send___wsdd__ProbeMatches(struct soap *soap, const char *soap_endpoint, const char *soap_action, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{	struct __wsdd__ProbeMatches soap_tmp___wsdd__ProbeMatches;
	if (soap_action == NULL)
		soap_action = "http://docs.oasis-open.org/ws-dd/ns/discovery/2009/01/ProbeMatches";
	soap_begin(soap);
	soap->encodingStyle = NULL;
	soap_tmp___wsdd__ProbeMatches.wsdd__ProbeMatches = wsdd__ProbeMatches;
	soap_set_version(soap, 2); /* SOAP1.2 */
	soap_serializeheader(soap);
	soap_serialize___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_url(soap, soap_endpoint, NULL), soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___wsdd__ProbeMatches(soap, &soap_tmp___wsdd__ProbeMatches, "-wsdd:ProbeMatches", NULL)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}



SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Probe(struct soap* soap, struct wsdd__ProbeType *wsdd__Probe)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	 

    unsigned char macaddr[6] = { 0 };
    char _IPAddr[INFO_LENGTH] = { 0 };
    char _HwId[1024] = { 0 };

    wsdd__ProbeMatchesType ProbeMatches;
    ProbeMatches.ProbeMatch = (struct wsdd__ProbeMatchType *) soap_malloc(soap, sizeof(struct wsdd__ProbeMatchType));
    ProbeMatches.ProbeMatch->XAddrs = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    ProbeMatches.ProbeMatch->Types = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    ProbeMatches.ProbeMatch->Scopes = (struct wsdd__ScopesType*) soap_malloc( soap, sizeof(struct wsdd__ScopesType));
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties = (struct wsa__ReferencePropertiesType*) soap_malloc(soap, sizeof(struct wsa__ReferencePropertiesType));
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters = (struct wsa__ReferenceParametersType*) soap_malloc(soap, sizeof(struct wsa__ReferenceParametersType));
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName = (struct wsa__ServiceNameType*) soap_malloc(soap, sizeof(struct wsa__ServiceNameType));
    ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType = (char **) soap_malloc(soap, sizeof(char *) * SMALL_INFO_LENGTH);
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__any = (char **) soap_malloc(soap, sizeof(char*) * SMALL_INFO_LENGTH);
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__anyAttribute = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    ProbeMatches.ProbeMatch->wsa__EndpointReference.Address = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);

    netGetMac("eth0", macaddr); //eth0  根据实际情况填充
    sprintf(_HwId, "urn:uuid:11223344-5566-7788-99aa-%02X%02X%02X%02X%02X%02X",
            macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4],
            macaddr[5]);


    unsigned int localIp = 0;
    netGetIp("eth0", &localIp); //eth0 根据实际情况填充
    sprintf(_IPAddr, "http://%s:%d/onvif/device_service",inet_ntoa(*((struct in_addr *)&localIp)),g_onvif_response_port); //inet_ntoa(*((struct in_addr *)&localIp)));
    printf("_IPAddr ==== %s [%d] \n", _IPAddr, __LINE__);

    

    ProbeMatches.__sizeProbeMatch = 1;
    ProbeMatches.ProbeMatch->Scopes->__item = (char *) soap_malloc(soap, 1024);
    memset(ProbeMatches.ProbeMatch->Scopes->__item, 0, sizeof(ProbeMatches.ProbeMatch->Scopes->__item));

    //Scopes MUST BE
    strcat(ProbeMatches.ProbeMatch->Scopes->__item, "onvif://www.onvif.org/type/video_encoder onvif://www.onvif.org/type/audio_encoder onvif://www.onvif.org/hardware/IPC-model onvif://www.onvif.org/name/IPC-model");

//	printf("client input param wsdd__Probe->Scopes->MatchBy = %s,wsdd__Probe->Scopes->__item = %s \n ",
//			wsdd__Probe->Scopes->MatchBy,wsdd__Probe->Scopes->__item);
	
	
    ProbeMatches.ProbeMatch->Scopes->MatchBy = NULL;
    strcpy(ProbeMatches.ProbeMatch->XAddrs, _IPAddr);
    strcpy(ProbeMatches.ProbeMatch->Types, wsdd__Probe->Types);
    printf("[%d]wsdd__Probe->Types=%s\n", __LINE__,wsdd__Probe->Types);
    ProbeMatches.ProbeMatch->MetadataVersion = 1;

    //ws-discovery规定 为可选项
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties->__size = 0;
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceProperties->__any = NULL;
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters->__size = 0;
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ReferenceParameters->__any = NULL;

    ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType[0] = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    //ws-discovery规定 为可选项
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.PortType[0], "ttl");
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName->__item = NULL;
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName->PortName = NULL;
    ProbeMatches.ProbeMatch->wsa__EndpointReference.ServiceName->__anyAttribute = NULL;
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__any[0] = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.__any[0], "Any");
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.__anyAttribute, "Attribute");
    ProbeMatches.ProbeMatch->wsa__EndpointReference.__size = 0;
    strcpy(ProbeMatches.ProbeMatch->wsa__EndpointReference.Address, _HwId);


/*注释的部分为可选，注释掉onvif test也能发现ws-d*/
    soap->header->wsa__To = (char*)"http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous";
    soap->header->wsa__Action = (char*)"http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches";
    soap->header->wsa__RelatesTo = (struct wsa__Relationship*) soap_malloc(soap, sizeof(struct wsa__Relationship));
    soap->header->wsa__RelatesTo->__item = soap->header->wsa__MessageID;
    soap->header->wsa__RelatesTo->RelationshipType = NULL;
    soap->header->wsa__RelatesTo->__anyAttribute = NULL;

    soap->header->wsa__MessageID = (char *) soap_malloc(soap, sizeof(char) * INFO_LENGTH);
    strcpy(soap->header->wsa__MessageID, _HwId + 4);

    if (SOAP_OK == soap_send___wsdd__ProbeMatches(soap, "http://", NULL, &ProbeMatches))
    {
        printf("send ProbeMatches success !\n");
        return SOAP_OK;
    }

    printf("[%d] soap error: %d, %s, %s\n", __LINE__, soap->error, *soap_faultcode(soap), *soap_faultstring(soap));

	
    return soap->error;
}


SOAP_FMAC5 int SOAP_FMAC6 __tds__GetScopes(struct soap* soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse)
{
		/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
		

		tds__GetScopesResponse->__sizeScopes=1;
		tds__GetScopesResponse->Scopes=(struct tt__Scope*)soap_malloc(soap, sizeof(struct tt__Scope));
		memset(tds__GetScopesResponse->Scopes,0,sizeof(struct tt__Scope));
		tds__GetScopesResponse->Scopes->ScopeDef=tt__ScopeDefinition__Configurable;
		tds__GetScopesResponse->Scopes->ScopeItem=(char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
		strcpy(tds__GetScopesResponse->Scopes->ScopeItem,"onvif://www.onvif.org/type/video_encoder onvif://www.onvif.org/type/audio_encoder onvif://www.onvif.org/hardware/IPC-model onvif://www.onvif.org/name/IPC-model");
		
		return SOAP_OK; 
}



SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServices(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse) 
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	
	
	char _IPAddr[INFO_LENGTH] = { 0 };
	unsigned int localIp = 0;
	netGetIp("eth0", &localIp); //eth0 根据实际情况填充
	sprintf(_IPAddr, "http://%s:%d/onvif/device_service",inet_ntoa(*((struct in_addr *)&localIp)),g_onvif_response_port);

    tds__GetServicesResponse->__sizeService = 1;
    tds__GetServicesResponse->Service = (struct tds__Service *)soap_malloc(soap, sizeof(struct tds__Service));
    tds__GetServicesResponse->Service[0].Namespace = (char *)soap_malloc(soap, sizeof(char)* INFO_LENGTH);
    strcpy(tds__GetServicesResponse->Service[0].Namespace, "http://www.onvif.org/ver10/events/wsdl");
    tds__GetServicesResponse->Service[0].XAddr = (char *)soap_malloc(soap, sizeof(char)* INFO_LENGTH);
    strcpy(tds__GetServicesResponse->Service[0].XAddr,_IPAddr);

    tds__GetServicesResponse->Service[0].Capabilities = NULL;
    tds__GetServicesResponse->Service[0].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
    tds__GetServicesResponse->Service[0].Version->Major = 0;
    tds__GetServicesResponse->Service[0].Version->Minor = 3;

    tds__GetServicesResponse->Service[0].__size = 0;
    tds__GetServicesResponse->Service[0].__any = NULL;
    tds__GetServicesResponse->Service[0].__anyAttribute = NULL;

    return SOAP_OK;
}



SOAP_FMAC5 int SOAP_FMAC6 __tds__GetUsers(struct soap* soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse) 
{
	if(1 != g_AuthenticateEnable)
	{
		printf("__tds__GetUsers-> g_AuthenticateEnable is not enable!!!  \n");
		return SOAP_OK;
	}
	
	int sizeUser = 0;
	UserArray_t m_tUserInfo[8];
	if(SOAP_OK != GetUserList(&sizeUser,m_tUserInfo))
	{
		printf("__tds__GetUsers:GetUserList failed !!! \n");
		return SOAP_ERR; 
	}
	tds__GetUsersResponse->__sizeUser = sizeUser;
	tds__GetUsersResponse->User= (struct tt__User *)soap_malloc(soap, sizeof(struct tt__User)*sizeUser);
    memset(tds__GetUsersResponse->User, 0, sizeof(struct tt__User)*sizeUser);

	int i;
	for(i = 0;i<sizeUser;i++)
	{
		tds__GetUsersResponse->User[i].Username = (char *) soap_malloc(soap, sizeof(char) * 32);
	//	tds__GetUsersResponse->User[i].Password = NULL;
		tds__GetUsersResponse->User[i].Password = (char *) soap_malloc(soap, sizeof(char) * 16);
		memset(tds__GetUsersResponse->User[i].Username,0,32);
		memset(tds__GetUsersResponse->User[i].Password,0,16);
		strcpy(tds__GetUsersResponse->User[i].Username,m_tUserInfo[i].user);
		strcpy(tds__GetUsersResponse->User[i].Password,"******");	//

		if(5 == m_tUserInfo[i].authority)		//设备端和onvif 对用等级定义不一致，所以进行转化，不然可能出现反序列化失败的情况
			m_tUserInfo[i].authority = 0;
			
		tds__GetUsersResponse->User[i].UserLevel = m_tUserInfo[i].authority;    //enum { 'Administrator', 'Operator', 'User', 'Anonymous', 'Extended' }
		tds__GetUsersResponse->User[i].Extension = NULL;
		tds__GetUsersResponse->User[i].__anyAttribute = NULL;
	}

	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateUsers(struct soap* soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ 

	if(!tds__CreateUsers)
	{
		printf("tds__CreateUsers is NULL \n");
		return SOAP_ERR;
	}

	if(1 != g_AuthenticateEnable)
	{
		printf("tds__CreateUsers-> g_AuthenticateEnable is not enable!!!  \n");
		return SOAP_OK;
	}
		

	if( 0 != soap->iUserLevel)		//必须是admin 用户才可以进行创建用户，并且创建的用户不能是admin等级
	{
		printf("tds__CreateUsers user level is too low !!\n");
		return SOAP_ERR;
	}


	

	//用户名长度限制 ，4~32
	//密码限制 4~16
	int i;
	char cUserNamePasswd[128] = {0};
	
	for(i=0; i<tds__CreateUsers->__sizeUser;i++)
	{
		if(tt__UserLevel__Administrator == tds__CreateUsers->User[i].UserLevel)
		{
			printf("err: con't create admin level user \n");
			return SOAP_ERR;
		}
			
		snprintf(cUserNamePasswd,128,"<CMD>%d</CMD>;<UserName>%s</UserName>;<PassWord>%s</PassWord>;<UserAuthority>%d</UserAuthority>\r\n",
									 USER_MANAGE_ADD_USER,
									 tds__CreateUsers->User[i].Username,
									 tds__CreateUsers->User[i].Password,
									 tds__CreateUsers->User[i].UserLevel);
		
		int len = strlen(cUserNamePasswd);
		if(0 != UserManage(len,(char*)cUserNamePasswd))
		{
			printf("err: add user  failed\n");
			return SOAP_ERR;
		}
	}

	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteUsers(struct soap* soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse) 
{
	if(!tds__DeleteUsers)
	{
		printf("tds__DeleteUsers is NULL \n");
		return SOAP_ERR;
	}

	if(1 != g_AuthenticateEnable)
	{
		printf("__tds__DeleteUsers-> g_AuthenticateEnable is not enable!!!  \n");
		return SOAP_OK;
	}

	if( 0 != soap->iUserLevel)
	{
		printf("__tds__DeleteUsers user level is too low !!\n");
		return SOAP_ERR;
	}


	char *servername = NULL;
	if(soap->header && soap->header->wsse__Security )
	{
		servername = soap_wsse_get_Username(soap);
		printf("__tds__DeleteUsers:soap->header and  soap->header->wsse__Security  username %s \n",servername);
	}
	else if(soap->userid && soap->authrealm)
	{
		servername = soap->userid;
		printf("__tds__DeleteUsers:soap->userid and  soap->authrealm  username %s \n",servername);
	}

	printf("tds__DeleteUsers size is %d \n",tds__DeleteUsers->__sizeUsername);

	if(strcmp(tds__DeleteUsers->Username[0],servername) == 0)
	{
		printf("%s has logined,so it can't be delete! \n",tds__DeleteUsers->Username[0]);
		return 400;
	}

	int i;
	char cUserNamePasswd[128] = {0};
	for(i=0;i<tds__DeleteUsers->__sizeUsername;i++)
	{
		snprintf(cUserNamePasswd,128,"<CMD>%d</CMD>;<UserName>%s</UserName>;<PassWord>NULL</PassWord>\r\n",
									 USER_MANAGE_DELETE_USER,
									 tds__DeleteUsers->Username[i]);
		int len = strlen(cUserNamePasswd);
		if(0 != UserManage(len,(char*)cUserNamePasswd))
		{
			printf("err: Delete user  failed\n");
			return SOAP_ERR;
		}
	}
	
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ 
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetUser(struct soap* soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse) 
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ 

	if(!tds__SetUser)
	{
		printf("tds__SetUser is NULL \n");
		return SOAP_ERR;
	}

	if(1 != g_AuthenticateEnable)
	{
		printf("__tds__SetUser-> g_AuthenticateEnable is not enable!!!  \n");
		return SOAP_OK;
	}
	
	//只有administrator等级才可以进行用户名密码修改
	if( 0 != soap->iUserLevel)
	{
		printf("__tds__SetUser user level is too low !!\n");
		return SOAP_ERR;
	}

	//不可以改已登录的用户名密码
	char *servername = NULL;
	if(soap->header && soap->header->wsse__Security )
	{
		servername = soap_wsse_get_Username(soap);
		printf("__tds__SetUser:soap->header and  soap->header->wsse__Security  username %s \n",servername);
	}
	else if(soap->userid && soap->authrealm)
	{
		servername = soap->userid;
		printf("__tds__SetUser:soap->userid and  soap->authrealm  username %s \n",servername);
	}

	printf("__tds__SetUser size is %d \n",tds__SetUser->__sizeUser);

	if(strcmp(tds__SetUser->User[0].Username,servername) == 0)
	{
		printf("%s has logined,so it can't be set! \n",tds__SetUser->User[0].Username);
		return 400;
	}

	int i;
	char cUserNamePasswd[128] = {0};
	
	for(i=0; i<tds__SetUser->__sizeUser;i++)
	{
		if(tt__UserLevel__Administrator == tds__SetUser->User[i].UserLevel)  //不可以设置为admin 等级
		{
			printf("err:__tds__SetUser con't create admin level user \n");
			return SOAP_ERR;
		}
		
		snprintf(cUserNamePasswd,128,"<CMD>%d</CMD>;<UserName>%s</UserName>;<PassWord>%s</PassWord>;<UserAuthority>%d</UserAuthority>\r\n",
									 USER_MANAGE_MODIFY_USER,
									 tds__SetUser->User[i].Username,
									 tds__SetUser->User[i].Password,
									 tds__SetUser->User[i].UserLevel);
		int len = strlen(cUserNamePasswd);
		if(0 != UserManage(len,(char*)cUserNamePasswd))
		{
			printf("err: modify user  failed\n");
			return SOAP_ERR;
		}
	}
	
	return SOAP_OK;
}




SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDeviceInformation(struct soap* soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse) 
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	 

	char _HwId[32] = { 0 };
	unsigned char macaddr[6] = { 0 };
	netGetMac("eth0", macaddr); //eth0  根据实际情况填充

	sprintf(_HwId, "%02X%02X%02X%02X%02X%02X",
            macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4],
            macaddr[5]);
	

    tds__GetDeviceInformationResponse->Manufacturer = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    tds__GetDeviceInformationResponse->Model = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    tds__GetDeviceInformationResponse->FirmwareVersion = (char *) soap_malloc( soap, sizeof(char) * SMALL_INFO_LENGTH);
    tds__GetDeviceInformationResponse->SerialNumber = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
    tds__GetDeviceInformationResponse->HardwareId = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);

    strcpy(tds__GetDeviceInformationResponse->Manufacturer, MANUFACTURER);
    strcpy(tds__GetDeviceInformationResponse->Model, DEV_MODEL);
    strcpy(tds__GetDeviceInformationResponse->FirmwareVersion, DEV_FIRMWAREVERSION);
    strcpy(tds__GetDeviceInformationResponse->SerialNumber, _HwId);
    strcpy(tds__GetDeviceInformationResponse->HardwareId, "Hunda");
		
		
	return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCapabilities(struct soap* soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	

	char _IPAddr[INFO_LENGTH] = { 0 };
	unsigned int localIp = 0;
	netGetIp("eth0", &localIp); //eth0 根据实际情况填充
	sprintf(_IPAddr, "http://%s:%d/onvif/device_service",inet_ntoa(*((struct in_addr *)&localIp)),g_onvif_response_port);

	char _IPXAddr[INFO_LENGTH] = { 0 };
	sprintf(_IPXAddr, "http://%s:%d",inet_ntoa(*((struct in_addr *)&localIp)),g_onvif_response_port);

	
	#define MAX_64_LEN 1024
    tds__GetCapabilitiesResponse->Capabilities = (struct tt__Capabilities *) soap_malloc(soap, sizeof(struct tt__Capabilities));
    memset(tds__GetCapabilitiesResponse->Capabilities, 0 , sizeof(struct tt__Capabilities));

	//Device
	tds__GetCapabilitiesResponse->Capabilities->Device = (struct tt__DeviceCapabilities *) soap_malloc(soap, sizeof(struct tt__DeviceCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Device, 0, sizeof(struct tt__DeviceCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Device->XAddr = (char *) soap_malloc(soap, sizeof(char) * MAX_64_LEN);
	memset(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr, 0, sizeof(char) * MAX_64_LEN);
	//	sprintf(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr, "%s/onvif/device_service", "172.18.10.233:8080");
	strcpy(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr,_IPAddr);
	tds__GetCapabilitiesResponse->Capabilities->Device->Network = (struct tt__NetworkCapabilities *) soap_malloc(soap, sizeof(struct tt__NetworkCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Device->Network, 0, sizeof(struct tt__NetworkCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter = (enum xsd__boolean *) soap_malloc(soap, sizeof(enum xsd__boolean));
	tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration = (enum xsd__boolean *) soap_malloc(soap, sizeof(enum xsd__boolean));
	tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6 = (enum xsd__boolean *) soap_malloc(soap, sizeof(enum xsd__boolean));
	tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS = (enum xsd__boolean *) soap_malloc(soap, sizeof(enum xsd__boolean));
	*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter) = xsd__boolean__false_; // xsd__boolean__true_
	*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration) = xsd__boolean__false_; // xsd__boolean__false_
	*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6) = xsd__boolean__false_; // xsd__boolean__true_
	*(tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS) = xsd__boolean__true_; // xsd__boolean__false_
	tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Device->Network->__anyAttribute=NULL;

	tds__GetCapabilitiesResponse->Capabilities->Device->System = (struct tt__SystemCapabilities *) soap_malloc(soap, sizeof(struct tt__SystemCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Device->System, 0, sizeof(struct tt__SystemCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryResolve = xsd__boolean__true_;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryBye = xsd__boolean__true_;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->RemoteDiscovery = xsd__boolean__true_;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->SystemBackup = xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->SystemLogging = xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->FirmwareUpgrade = xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->__sizeSupportedVersions = 1;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions = (struct tt__OnvifVersion *) soap_malloc(soap, sizeof(struct tt__OnvifVersion));
	tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions->Major = 2;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions->Minor = 0;
	tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension=NULL;

	tds__GetCapabilitiesResponse->Capabilities->Device->IO =(struct tt__IOCapabilities*)soap_malloc(soap, sizeof(struct tt__IOCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Device->IO,0,sizeof(struct tt__IOCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Device->IO->InputConnectors=(int *)soap_malloc(soap, sizeof(int));
	*(tds__GetCapabilitiesResponse->Capabilities->Device->IO->InputConnectors)=2;
	tds__GetCapabilitiesResponse->Capabilities->Device->IO->RelayOutputs=(int *)soap_malloc(soap, sizeof(int));
	*(tds__GetCapabilitiesResponse->Capabilities->Device->IO->RelayOutputs)=1;
	tds__GetCapabilitiesResponse->Capabilities->Device->IO->Extension=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Device->IO->__anyAttribute=NULL;


	tds__GetCapabilitiesResponse->Capabilities->Device->Security = (struct tt__SecurityCapabilities*)soap_malloc(soap, sizeof(struct tt__SecurityCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Device->Security,0,sizeof(sizeof(struct tt__SecurityCapabilities)));	
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->AccessPolicyConfig=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->KerberosToken=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->OnboardKeyGeneration=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->RELToken=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->SAMLToken=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->TLS1_x002e1=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->TLS1_x002e2=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->X_x002e509Token=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->__any=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->__anyAttribute=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension=NULL;
	
	
	tds__GetCapabilitiesResponse->Capabilities->Device->Extension=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Device->__anyAttribute=NULL;

	tds__GetCapabilitiesResponse->Capabilities->Imaging=NULL;
    
  	//Imaging
  	/*
       tds__GetCapabilitiesResponse->Capabilities->Imaging = (struct tt__ImagingCapabilities *) soap_malloc(soap, sizeof(struct tt__ImagingCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Imaging, 0, sizeof(struct tt__ImagingCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr = (char *) soap_malloc(soap, sizeof(char) * MAX_64_LEN);
    	memset(tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr, '\0', sizeof(char) * MAX_64_LEN);
   	//sprintf(tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr, "%s/onvif/imaging_service", "172.18.18.144:8080");
   	sprintf(tds__GetCapabilitiesResponse->Capabilities->Imaging->XAddr, "%s", "http://172.18.10.233:8080");
	tds__GetCapabilitiesResponse->Capabilities->Imaging->__anyAttribute=NULL;
	*/
	
	//Media
	tds__GetCapabilitiesResponse->Capabilities->Media = (struct tt__MediaCapabilities *) soap_malloc(soap, sizeof(struct tt__MediaCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Media, 0, sizeof(struct tt__MediaCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Media->XAddr = (char *) soap_malloc(soap, sizeof(char) * MAX_64_LEN);
	memset(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, 0, sizeof(char) * MAX_64_LEN);
	//sprintf(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, "%s/onvif/media_service", "172.18.18.144:8080");
	// 	sprintf(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, "%s", "http://172.18.10.233:8080");
	strcpy(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr,_IPXAddr);
	tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities = (struct tt__RealTimeStreamingCapabilities *) soap_malloc(soap, sizeof(struct tt__RealTimeStreamingCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities, 0, sizeof(struct tt__RealTimeStreamingCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast = (enum xsd__boolean *) soap_malloc(soap, sizeof(int));
	tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = (enum xsd__boolean*) soap_malloc(soap, sizeof(int));		
	tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = (enum xsd__boolean*) soap_malloc(soap, sizeof(int));
	*(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast) = xsd__boolean__true_;    	
	*(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP) = xsd__boolean__true_;
	*(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP) = xsd__boolean__true_;
	tds__GetCapabilitiesResponse->Capabilities->Media->__any=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Media->__anyAttribute=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Media->Extension=NULL;

	//Analytics
	tds__GetCapabilitiesResponse->Capabilities->Analytics=(struct tt__AnalyticsCapabilities *)soap_malloc(soap, sizeof(struct tt__AnalyticsCapabilities));
	memset(tds__GetCapabilitiesResponse->Capabilities->Analytics,0,sizeof(struct tt__AnalyticsCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr=(char *) soap_malloc(soap, sizeof(char) * MAX_64_LEN);
	memset(tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr, 0, sizeof(char) * MAX_64_LEN);
//	sprintf(tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr, "%s", "http://172.18.10.233:8080");
	strcpy(tds__GetCapabilitiesResponse->Capabilities->Analytics->XAddr,_IPXAddr);
	tds__GetCapabilitiesResponse->Capabilities->Analytics->RuleSupport=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Analytics->AnalyticsModuleSupport=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Analytics->__any=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Analytics->__anyAttribute=NULL;

	//Events
	tds__GetCapabilitiesResponse->Capabilities->Events=(struct tt__EventCapabilities*)soap_malloc(soap, sizeof(struct tt__EventCapabilities));
	memset(	tds__GetCapabilitiesResponse->Capabilities->Events,0,sizeof(struct tt__EventCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->Events->XAddr=(char *) soap_malloc(soap, sizeof(char) * MAX_64_LEN);
	memset(tds__GetCapabilitiesResponse->Capabilities->Events->XAddr, 0, sizeof(char) * MAX_64_LEN);
//	sprintf(tds__GetCapabilitiesResponse->Capabilities->Events->XAddr, "%s", "http://172.18.10.233:8080");
	strcpy(tds__GetCapabilitiesResponse->Capabilities->Events->XAddr,_IPXAddr);
	tds__GetCapabilitiesResponse->Capabilities->Events->WSPausableSubscriptionManagerInterfaceSupport=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Events->WSPullPointSupport=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Events->WSSubscriptionPolicySupport=xsd__boolean__false_;
	tds__GetCapabilitiesResponse->Capabilities->Events->__any=NULL;
	tds__GetCapabilitiesResponse->Capabilities->Events->__anyAttribute=NULL;

	//PTZ
	tds__GetCapabilitiesResponse->Capabilities->PTZ=(struct tt__PTZCapabilities*)soap_malloc(soap, sizeof(struct tt__PTZCapabilities));
	memset(	tds__GetCapabilitiesResponse->Capabilities->PTZ,0,sizeof(struct tt__PTZCapabilities));
	tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr=(char *) soap_malloc(soap, sizeof(char) * MAX_64_LEN);
	memset(tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr, 0, sizeof(char) * MAX_64_LEN);
	
//	sprintf(tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr, "%s", "http://172.18.10.233:8080");
	strcpy(tds__GetCapabilitiesResponse->Capabilities->PTZ->XAddr,_IPXAddr);
	tds__GetCapabilitiesResponse->Capabilities->PTZ->__any=NULL;
	tds__GetCapabilitiesResponse->Capabilities->PTZ->__anyAttribute=NULL;


	//Extension 
	tds__GetCapabilitiesResponse->Capabilities->Extension=NULL;
	


	

    return SOAP_OK;


}





SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	 


    
    	trt__GetVideoSourcesResponse->__sizeVideoSources = size1;
    	trt__GetVideoSourcesResponse->VideoSources = (struct tt__VideoSource *)soap_malloc(soap, sizeof(struct tt__VideoSource) * size1);
	memset(trt__GetVideoSourcesResponse->VideoSources,0,sizeof(struct tt__VideoSource) * size1);
		//浮点数
	int i=0;	
	for(i=0;i<size1;i++)
	{
		trt__GetVideoSourcesResponse->VideoSources[i].Framerate = 25.000000;		
	    	trt__GetVideoSourcesResponse->VideoSources[i].Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
	    	
	    	trt__GetVideoSourcesResponse->VideoSources[i].token = (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
	     	trt__GetVideoSourcesResponse->VideoSources[i].Imaging =NULL;
		trt__GetVideoSourcesResponse->VideoSources[i].Extension=NULL;
		trt__GetVideoSourcesResponse->VideoSources[i].__anyAttribute=NULL;

		if(i==0)
		{
			trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Height = g_video_height;
		    	trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Width = g_video_width;
			strcpy(trt__GetVideoSourcesResponse->VideoSources[i].token,"VS_token_0"); //注意这里需要和GetProfile中的sourcetoken相同

		}
		else
		{
			trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Height = g_video_height_ex;
		    	trt__GetVideoSourcesResponse->VideoSources[i].Resolution->Width = g_video_width_ex;
			strcpy(trt__GetVideoSourcesResponse->VideoSources[i].token,"VS_token_1");
		}


	}
    	

	



	
/*
    	trt__GetVideoSourcesResponse->VideoSources->Imaging =(struct tt__ImagingSettings*)soap_malloc(soap, sizeof(struct tt__ImagingSettings));
	memset(trt__GetVideoSourcesResponse->VideoSources->Imaging,0,sizeof(struct tt__ImagingSettings));
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->Brightness = (float*)soap_malloc(soap,sizeof(float));
 //   *(trt__GetVideoSourcesResponse->VideoSources->Imaging->Brightness) = 128;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->ColorSaturation = (float*)soap_malloc(soap,sizeof(float));
//    *(trt__GetVideoSourcesResponse->VideoSources->Imaging->ColorSaturation) = 128;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->Contrast = (float*)soap_malloc(soap,sizeof(float));
//    *(trt__GetVideoSourcesResponse->VideoSources->Imaging->Contrast) = 128;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->IrCutFilter = (enum tt__IrCutFilterMode*)soap_malloc(soap,sizeof(int));
//    *(trt__GetVideoSourcesResponse->VideoSources->Imaging->IrCutFilter) = tt__IrCutFilterMode__ON;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->Sharpness = (float*)soap_malloc(soap,sizeof(float));
//    *(trt__GetVideoSourcesResponse->VideoSources->Imaging->Sharpness) = 128;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->BacklightCompensation = (struct tt__BacklightCompensation*)soap_malloc(soap, sizeof(struct tt__BacklightCompensation));
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->BacklightCompensation->Mode = tt__BacklightCompensationMode__OFF;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->BacklightCompensation->Level = 20;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->Exposure = NULL;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->Focus = NULL;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->WideDynamicRange = (struct tt__WideDynamicRange*)soap_malloc(soap, sizeof(struct tt__WideDynamicRange));
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->WideDynamicRange->Mode = tt__WideDynamicMode__OFF;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->WideDynamicRange->Level = 20;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->WhiteBalance = NULL;
	    trt__GetVideoSourcesResponse->VideoSources->Imaging->WhiteBalance = (struct tt__WhiteBalance*)soap_malloc(soap, sizeof(struct tt__WhiteBalance));
    trt__GetVideoSourcesResponse->VideoSources->Imaging->WhiteBalance->Mode = tt__WhiteBalanceMode__AUTO;
    trt__GetVideoSourcesResponse->VideoSources->Imaging->WhiteBalance->CrGain = 0;
    trt__GetVideoSourcesResponse->VideoSources->Imaging->WhiteBalance->CbGain = 0;
    	trt__GetVideoSourcesResponse->VideoSources->Imaging->Extension = NULL;

*********************/
  

    return SOAP_OK;
}


//If the profile token is already known, a profile can be fetched through the GetProfile command.
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfile(struct soap* soap, struct _trt__GetProfile *trt__GetProfile, struct _trt__GetProfileResponse *trt__GetProfileResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
    if((trt__GetProfile->ProfileToken) == NULL)
    {
        printf("the ProfileToken is null \n");
        return SOAP_FAULT;
    }

    //获取方案一配置
    if(strcmp((trt__GetProfile->ProfileToken),"Profile_token_0")== 0)
    {
    	trt__GetProfileResponse->Profile= (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
    	memset(trt__GetProfileResponse->Profile,0,sizeof(struct tt__Profile));

    	trt__GetProfileResponse->Profile->Name="Profile_name_0";
    	trt__GetProfileResponse->Profile->token="Profile_token_0";

    	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration=NULL;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration=NULL;
    	trt__GetProfileResponse->Profile->AudioEncoderConfiguration=NULL;
    	trt__GetProfileResponse->Profile->AudioSourceConfiguration=NULL;
    	trt__GetProfileResponse->Profile->PTZConfiguration=NULL;
    	trt__GetProfileResponse->Profile->MetadataConfiguration=NULL;
    	trt__GetProfileResponse->Profile->Extension=NULL;
    	trt__GetProfileResponse->Profile->fixed=NULL;
    	trt__GetProfileResponse->Profile->__anyAttribute=NULL;


    	trt__GetProfileResponse->Profile->VideoSourceConfiguration=(struct tt__VideoSourceConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name= (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->token= (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken=(char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds=(struct tt__IntRectangle *)soap_malloc(soap,sizeof(struct tt__IntRectangle));
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Extension=NULL;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->__any=NULL;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->__anyAttribute=NULL;


    	strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name,"VSC_Name_0");
    	strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->token,"VSC_Token_0");
    	strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken,"VS_token_0");
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height=g_video_height;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width=g_video_width;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x=0;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y=0;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount=0;
    	




    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration=(struct tt__VideoEncoderConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name= (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token=(char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    	strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name,"VEC_Name_0");
    	strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token,"VEC_token_0");
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding=tt__VideoEncoding__H264;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount=0;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality=50;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution=(struct tt__VideoResolution*)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height=g_video_height;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width=g_video_width;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl=(struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit=5000;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit=25;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval=1;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264=(struct tt__H264Configuration*)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->GovLength=0;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile=tt__H264Profile__Main;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->SessionTimeout= 1000;


    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->MPEG4=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__any=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__anyAttribute=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__size=0;
    }


    //获取方案二配置
    if(strcmp((trt__GetProfile->ProfileToken),"Profile_token_1")== 0)
    {
    	trt__GetProfileResponse->Profile= (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
    	memset(trt__GetProfileResponse->Profile,0,sizeof(struct tt__Profile));

    	trt__GetProfileResponse->Profile->Name="Profile_name_1";
    	trt__GetProfileResponse->Profile->token="Profile_token_1";

    	trt__GetProfileResponse->Profile->VideoAnalyticsConfiguration=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration=NULL;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration=NULL;
    	trt__GetProfileResponse->Profile->AudioEncoderConfiguration=NULL;
    	trt__GetProfileResponse->Profile->AudioSourceConfiguration=NULL;
    	trt__GetProfileResponse->Profile->PTZConfiguration=NULL;
    	trt__GetProfileResponse->Profile->MetadataConfiguration=NULL;
    	trt__GetProfileResponse->Profile->Extension=NULL;
    	trt__GetProfileResponse->Profile->fixed=NULL;
    	trt__GetProfileResponse->Profile->__anyAttribute=NULL;


    	trt__GetProfileResponse->Profile->VideoSourceConfiguration=(struct tt__VideoSourceConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name= (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->token= (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken=(char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds=(struct tt__IntRectangle *)soap_malloc(soap,sizeof(struct tt__IntRectangle));
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Extension=NULL;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->__any=NULL;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->__anyAttribute=NULL;


    	strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name,"VSC_Name_1");
    	strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->token,"VSC_Token_1");
    	strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken,"VS_token_1");
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height=g_video_height_ex;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width=g_video_width_ex;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x=0;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y=0;
    	trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount=0;
    	




    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration=(struct tt__VideoEncoderConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name= (char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token=(char *)soap_malloc(soap, sizeof(char)*INFO_LENGTH);
    	strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name,"VEC_Name_1");
    	strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token,"VEC_token_1");
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding=tt__VideoEncoding__H264;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount=0;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality=50;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution=(struct tt__VideoResolution*)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height=g_video_height_ex;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width=g_video_width_ex;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl=(struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit=5000;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit=25;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval=1;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264=(struct tt__H264Configuration*)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->GovLength=0;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile=tt__H264Profile__Main;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->SessionTimeout= 1000;


    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->MPEG4=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Multicast=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__any=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__anyAttribute=NULL;
    	trt__GetProfileResponse->Profile->VideoEncoderConfiguration->__size=0;
       }
	
	
	
	return SOAP_OK;
}




//Any endpoint can ask for the existing media profiles of a device using the GetProfiles command. Pre-configured or dynamically configured profiles can be retrieved using this command. 
//This command lists all configured profiles in a device. The client does not need to know the media profile in order to use the command.
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfiles(struct soap* soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *trt__GetProfilesResponse) 
{
        /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	 


	
	trt__GetProfilesResponse->__sizeProfiles = size1;
	trt__GetProfilesResponse->Profiles = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile)*size1);
    memset(trt__GetProfilesResponse->Profiles, 0, sizeof(struct tt__Profile)*size1);

	int i=0;
	for(i=0;i<size1;i++)
	{
		trt__GetProfilesResponse->Profiles[i].Name=NULL;
	  	trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration=NULL;
	  	trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration=NULL;
	  	trt__GetProfilesResponse->Profiles[i].AudioEncoderConfiguration=NULL;
	  	trt__GetProfilesResponse->Profiles[i].AudioSourceConfiguration=NULL;	
	  	trt__GetProfilesResponse->Profiles[i].Extension=NULL;
	  	trt__GetProfilesResponse->Profiles[i].MetadataConfiguration=NULL;
	  	trt__GetProfilesResponse->Profiles[i].PTZConfiguration=NULL;
	  	trt__GetProfilesResponse->Profiles[i].VideoAnalyticsConfiguration=NULL;
	  	trt__GetProfilesResponse->Profiles[i].__anyAttribute = NULL;
	

		if(i==0)
		{
			trt__GetProfilesResponse->Profiles[i].Name = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
			memset(trt__GetProfilesResponse->Profiles[i].Name,0,sizeof(char)*INFO_LENGTH);
			strcpy(trt__GetProfilesResponse->Profiles[i].Name,"Profile_name_0");
					
        	trt__GetProfilesResponse->Profiles[i].token= (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
			memset(trt__GetProfilesResponse->Profiles[i].token,0,sizeof(char)*INFO_LENGTH);
        	strcpy(trt__GetProfilesResponse->Profiles[i].token,"Profile_token_0");
		}
		else
		{
			trt__GetProfilesResponse->Profiles[i].Name = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
			memset(trt__GetProfilesResponse->Profiles[i].Name,0,sizeof(char)*INFO_LENGTH);
        	strcpy(trt__GetProfilesResponse->Profiles[i].Name,"Profile_name_1");
					
        	trt__GetProfilesResponse->Profiles[i].token= (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
			memset(trt__GetProfilesResponse->Profiles[i].token,0,sizeof(char)*INFO_LENGTH);
        	strcpy(trt__GetProfilesResponse->Profiles[i].token,"Profile_token_1");

		}
            

      
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap,sizeof(struct tt__VideoSourceConfiguration));
		memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration,0, sizeof(struct tt__VideoSourceConfiguration));			
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap,sizeof(struct tt__IntRectangle));
		memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name,0,sizeof(char)*INFO_LENGTH);	
		memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token,0,sizeof(char)*INFO_LENGTH);	
		memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken,0,sizeof(char)*INFO_LENGTH);	
		memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds,0,sizeof(struct tt__IntRectangle));	
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__any = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__anyAttribute = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Extension = NULL;

        /*注意SourceToken*/
		if(i==0)
		{
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name,"VSC_Name_0");
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token,"VSC_Token_0");
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken,"VS_token_0"); /*必须与__tmd__GetVideoSources中的token相同*/
		}
		else
		{
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name,"VSC_Name_1");
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token,"VSC_Token_1");
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken,"VS_token_1"); /*必须与__tmd__GetVideoSources中的token相同*/
		}
  
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->UseCount = 0;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->x = 0;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->y = 0;

		if(i==0)
		{
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->height =g_video_height;
	       	trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->width = g_video_width;
		}
		else
		{
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->height =g_video_height_ex;
			trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->width = g_video_width_ex;
		}
	       
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Extension=NULL;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__any=NULL;
		trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->__anyAttribute=NULL;
		

		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap,sizeof(struct tt__VideoEncoderConfiguration));
		memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration,0,sizeof(struct tt__VideoEncoderConfiguration));			
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token = (char *)soap_malloc(soap,sizeof(char)*INFO_LENGTH);
		memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name,0,sizeof(char)*INFO_LENGTH);
		memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token,0,sizeof(char)*INFO_LENGTH);

		if(i==0)
		{
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name,"VEC_Name_0");
    		strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token,"VEC_token_0");
		}
		else
		{
			strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name,"VEC_Name_1");
        	strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token,"VEC_token_1");
		}
        	
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount = 0;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Quality = 50;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding = tt__VideoEncoding__H264;//JPEG = 0, MPEG4 = 1, H264 = 2;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));

		if(i==0)
		{
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height = g_video_height;
	       	trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width = g_video_width;
		}
		else
		{
			trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height = g_video_height_ex;
	       	trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width = g_video_width_ex;
		}
	      
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit = 25;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval = 1;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit = 5000;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264=(struct tt__H264Configuration*)soap_malloc(soap, sizeof(struct tt__H264Configuration));
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->GovLength=0;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->H264Profile=tt__H264Profile__Main;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->SessionTimeout= 1000;


			
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->__any = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->__anyAttribute = NULL;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->MPEG4=NULL;
		trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast=NULL;
	


//	  	trt__GetProfilesResponse->Profiles[i].fixed = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
		trt__GetProfilesResponse->Profiles[i].fixed = NULL;

		trt__GetProfilesResponse->Profiles[i].PTZConfiguration =(struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
		memset(trt__GetProfilesResponse->Profiles[i].PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration));
		GetPTZConfig(soap, trt__GetProfilesResponse->Profiles[i].PTZConfiguration);
			
	}
  
	return SOAP_OK;
}



SOAP_FMAC5 int SOAP_FMAC6 __trt__GetStreamUri(struct soap* soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse)
{
    /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
     

	unsigned int localIp = 0;
	netGetIp("eth0", &localIp); //eth0 根据实际情况填充
	

	char *rtsp_uri;
	char *rtsp_uri_one;
    rtsp_uri = (char*)soap_malloc(soap,128*sizeof(char));
	rtsp_uri_one= (char*)soap_malloc(soap,128*sizeof(char));
	snprintf(rtsp_uri,128,"rtsp://%s:%d%s",inet_ntoa(*((struct in_addr *)&localIp)),g_rtsp_port,"/h264ESVideoTest");
	snprintf(rtsp_uri_one,128,"rtsp://%s:%d%s",inet_ntoa(*((struct in_addr *)&localIp)),g_rtsp_port,"/h264ESVideoTestSecond");

	trt__GetStreamUriResponse->MediaUri = (struct tt__MediaUri*) soap_malloc(soap, sizeof(struct tt__MediaUri));
	memset(trt__GetStreamUriResponse->MediaUri,0,sizeof(struct tt__MediaUri));
	trt__GetStreamUriResponse->MediaUri->Uri = (char *) soap_malloc(soap, sizeof(char) * SMALL_INFO_LENGTH);
	trt__GetStreamUriResponse->MediaUri->InvalidAfterConnect = xsd__boolean__false_;
	trt__GetStreamUriResponse->MediaUri->InvalidAfterReboot = xsd__boolean__false_;
	trt__GetStreamUriResponse->MediaUri->Timeout =  0;
	trt__GetStreamUriResponse->MediaUri->__any = NULL;
	trt__GetStreamUriResponse->MediaUri->__anyAttribute = NULL;

	if(NULL == trt__GetStreamUri->ProfileToken)
	{
		printf("trt__GetStreamUri->ProfileToken is full \n");
		return SOAP_OK;
	}
	
	printf("%s\n",trt__GetStreamUri->ProfileToken);
	if (strcmp (trt__GetStreamUri->ProfileToken, "Profile_token_0") == 0)
 	{
		//strcpy(trt__GetStreamUriResponse->MediaUri->Uri, "rtsp://172.18.10.233:8554/h264ESVideoTest");
 		strcpy(trt__GetStreamUriResponse->MediaUri->Uri, rtsp_uri);
 	}
  	else  if (strcmp (trt__GetStreamUri->ProfileToken, "Profile_token_1") == 0)
	{
		//strcpy(trt__GetStreamUriResponse->MediaUri->Uri, "rtsp://172.18.10.233:8554/h264ESVideoTest");
  		strcpy(trt__GetStreamUriResponse->MediaUri->Uri, rtsp_uri_one);
 	}
	else
	{
		printf("%s:invalid profile token !!!\n",__FUNCTION__);
		return SOAP_ERR;
	}

    return SOAP_OK;
}


#define MAX_PROF_TOKEN 64
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurations(struct soap* soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse) 
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	 

	
    	
	trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations=size1;
	trt__GetVideoSourceConfigurationsResponse->Configurations=(struct tt__VideoSourceConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration)*size1);
	memset(trt__GetVideoSourceConfigurationsResponse->Configurations,0,sizeof(struct tt__VideoSourceConfiguration)*size1);

	int i=0;
	for(i=0;i<size1;i++)
	{
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Name = (char*)soap_malloc(soap, sizeof(char) * MAX_PROF_TOKEN);
	       memset(trt__GetVideoSourceConfigurationsResponse->Configurations[i].Name, '\0', sizeof(char) * MAX_PROF_TOKEN);
		   
	       trt__GetVideoSourceConfigurationsResponse->Configurations[i].token = (char*)soap_malloc(soap, sizeof(char) * MAX_PROF_TOKEN);
	       memset(trt__GetVideoSourceConfigurationsResponse->Configurations[i].token, '\0', sizeof(char) * MAX_PROF_TOKEN);
	       
	       trt__GetVideoSourceConfigurationsResponse->Configurations[i].SourceToken = (char*)soap_malloc(soap, sizeof(char) * MAX_PROF_TOKEN);
	       memset(trt__GetVideoSourceConfigurationsResponse->Configurations[i].SourceToken, '\0', sizeof(char) * MAX_PROF_TOKEN);
	       
	       trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
	       memset(trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds, 0, sizeof(struct tt__IntRectangle));
	       trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->x      = 0;
	       trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->y      = 0;

		if(i==0)
		{
			strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[i].Name, "VSC_Name_0");
			strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[i].token, "VSC_Token_0");
			strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[i].SourceToken, "VS_token_0");
			trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->width  = g_video_width;
		       trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->height = g_video_height;
		}
		else
		{
			strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[i].Name, "VSC_Name_1");
			strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[i].token, "VSC_Token_1");
			strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[i].SourceToken, "VS_token_1");
			trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->width  = g_video_width_ex;
		       trt__GetVideoSourceConfigurationsResponse->Configurations[i].Bounds->height = g_video_height_ex;
		}
	       
	       trt__GetVideoSourceConfigurationsResponse->Configurations[i].UseCount = 0;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].Extension=NULL;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].__any=NULL;
		trt__GetVideoSourceConfigurationsResponse->Configurations[i].__anyAttribute=NULL;
	}


    return SOAP_OK;
}



SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurationOptions(struct soap* soap, struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions, struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse) 
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	
	trt__GetVideoEncoderConfigurationOptionsResponse->Options=(struct tt__VideoEncoderConfigurationOptions *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfigurationOptions ));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options,0,sizeof(struct tt__VideoEncoderConfigurationOptions));

	//QualityRange
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange=(struct tt__IntRange* )soap_malloc(soap, sizeof(struct tt__IntRange));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange,0,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange->Max=100;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->QualityRange->Min=0;

	//JPEG
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG=(struct tt__JpegOptions *)soap_malloc(soap, sizeof(struct tt__JpegOptions));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG,0,sizeof(struct tt__JpegOptions));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable=(struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution ));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange=(struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange=(struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable,0,sizeof(struct tt__VideoResolution ));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange,0,sizeof(struct tt__IntRange));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange,0,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable->Width=g_video_width;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->ResolutionsAvailable->Height=g_video_height;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange->Max=30;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->FrameRateRange->Min=12;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange->Max=255;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->EncodingIntervalRange->Min=1;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->JPEG->__sizeResolutionsAvailable=1;



	//H264
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264=(struct tt__H264Options *)soap_malloc(soap, sizeof(struct tt__H264Options ));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264,0,sizeof(struct tt__H264Options));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable=(struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution ));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange=(struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange=(struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange=(struct tt__IntRange *)soap_malloc(soap,sizeof(struct tt__IntRange));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported=(enum tt__H264Profile *)soap_malloc(soap, sizeof(enum tt__H264Profile ));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable,0,sizeof(struct tt__VideoResolution ));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange,0,sizeof(struct tt__IntRange));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange,0,sizeof(struct tt__IntRange));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange,0,sizeof(struct tt__IntRange));
	memset(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported,0,sizeof(enum tt__H264Profile));
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable->Width=g_video_width;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->ResolutionsAvailable->Height=g_video_height;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange->Max=30;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->FrameRateRange->Min=12;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange->Max=255;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->EncodingIntervalRange->Min=1;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange->Max=25;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->GovLengthRange->Min=2;
    *(trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->H264ProfilesSupported)=tt__H264Profile__Main;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeH264ProfilesSupported=1;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->H264->__sizeResolutionsAvailable=1;


	trt__GetVideoEncoderConfigurationOptionsResponse->Options->MPEG4=NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->Extension=NULL;
	trt__GetVideoEncoderConfigurationOptionsResponse->Options->__anyAttribute=NULL;
	
	

	
	return SOAP_OK;
}	




SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurations(struct soap* soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse) 
{ 
	///*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	

	
	
	trt__GetVideoEncoderConfigurationsResponse->__sizeConfigurations=size1;
	trt__GetVideoEncoderConfigurationsResponse->Configurations=(struct tt__VideoEncoderConfiguration*)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration)*size1);
	memset(trt__GetVideoEncoderConfigurationsResponse->Configurations,0, sizeof(struct tt__VideoEncoderConfiguration)*size1);

	int i=0;
	for(i=0;i<size1;i++)
	{
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name= (char *)soap_malloc(soap, sizeof(char)*MAX_PROF_TOKEN);
		memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name, '\0', sizeof(char)*MAX_PROF_TOKEN);
	       
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token = (char *)soap_malloc(soap, sizeof(char)*MAX_PROF_TOKEN);
	       memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token, '\0', sizeof(char)*MAX_PROF_TOKEN);
	      
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].UseCount=0;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Quality= 50.000000;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Encoding=(enum tt__VideoEncoding) 2;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution=(struct tt__VideoResolution*)soap_malloc(soap, sizeof(struct tt__VideoResolution));
		memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution,0,sizeof(struct tt__VideoResolution));

		if(i==0)
		{
			strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name, "VEC_Name_0");
			strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token, "VEC_token_0");
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Height=g_video_height;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Width=g_video_width;
		}
		else
		{
			strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name, "VEC_Name_1");
			strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token, "VEC_token_1");
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Height=g_video_height_ex;
			trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Width=g_video_width_ex;

		}
		
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl=(struct tt__VideoRateControl*)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
        trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->BitrateLimit=16384;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->EncodingInterval=1;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->FrameRateLimit=25;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264=(struct tt__H264Configuration*)soap_malloc(soap, sizeof(struct tt__H264Configuration));
		memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264,0,sizeof(struct tt__H264Configuration));
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->GovLength=1;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->H264Profile=(enum tt__H264Profile)1;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast=(struct tt__MulticastConfiguration*)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
		memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast,0, sizeof(struct tt__MulticastConfiguration));	
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address=(struct tt__IPAddress*)soap_malloc(soap, sizeof(struct tt__IPAddress));
		memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address,0,sizeof(struct tt__IPAddress));	
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address=NULL;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv6Address=NULL;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->Type=tt__IPType__IPv4;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Port=g_rtsp_port;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->TTL=0;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->AutoStart=xsd__boolean__false_;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->__any=NULL;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->__anyAttribute=NULL;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->__size=0;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].MPEG4=NULL;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].__any=NULL;
		trt__GetVideoEncoderConfigurationsResponse->Configurations[i].__anyAttribute=NULL;
	}


			
	return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfiguration(struct soap* soap, struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration, struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse) 
{
	///*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
    if((trt__GetVideoEncoderConfiguration->ConfigurationToken) == NULL)
    {
        printf("trt__GetVideoEncoderConfiguration->ConfigurationToken is null \n");
        return SOAP_FAULT;

    }

    if(strcmp((trt__GetVideoEncoderConfiguration->ConfigurationToken),"VEC_token_0") == 0)
    {

	    trt__GetVideoEncoderConfigurationResponse->Configuration=(struct tt__VideoEncoderConfiguration*)soap_malloc( soap, sizeof(struct tt__VideoEncoderConfiguration));
	    memset(trt__GetVideoEncoderConfigurationResponse->Configuration,0,sizeof(struct tt__VideoEncoderConfiguration));  

	   //请求的时候需要匹配的一些基本信息
        trt__GetVideoEncoderConfigurationResponse->Configuration->Name = (char *)soap_malloc(soap, sizeof(char)*MAX_PROF_TOKEN);
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Name, '\0', sizeof(char)*MAX_PROF_TOKEN);
        strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->Name, "VEC_Name_0");

        trt__GetVideoEncoderConfigurationResponse->Configuration->token = (char *)soap_malloc(soap, sizeof(char)*MAX_PROF_TOKEN);
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->token, '\0', sizeof(char)*MAX_PROF_TOKEN);
        strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->token, "VEC_token_0");

        trt__GetVideoEncoderConfigurationResponse->Configuration->UseCount = 0;
        trt__GetVideoEncoderConfigurationResponse->Configuration->Quality = 50.000000;
        //根据前端设备时间支持的编码格式选择对应的值，因为我测试的是设备只支持H264 ，所以选了2
        trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding = (enum tt__VideoEncoding) 2;   // JPEG = 0 , MPEG = 1, H264 = 2; 

        trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap,sizeof(struct tt__VideoResolution));
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution, 0 , sizeof(struct tt__VideoResolution));
        // 请求的视频的分辨率，对应前端设备填写对应的值，我这是1280 * 720
        trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Width  = g_video_width;
        trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Height = g_video_height;

        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl, 0, sizeof(struct tt__VideoRateControl));
        //请求的对应的编码信息.各个意思参考上面说明
        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->FrameRateLimit   = 25;
        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->EncodingInterval = 1;
        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->BitrateLimit     =16384;

        trt__GetVideoEncoderConfigurationResponse->Configuration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->H264, 0, sizeof(struct tt__H264Configuration));
        trt__GetVideoEncoderConfigurationResponse->Configuration->H264->GovLength  = 1;
        trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = (enum tt__H264Profile)1;

    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast=(struct tt__MulticastConfiguration *)soap_malloc(soap,sizeof(struct tt__MulticastConfiguration));
    	memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast,0,sizeof(struct tt__MulticastConfiguration));
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address=(struct tt__IPAddress *)soap_malloc(soap,sizeof(struct tt__IPAddress));
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->Type=tt__IPType__IPv4;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv6Address=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Port=g_rtsp_port;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->TTL=0;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->AutoStart=xsd__boolean__false_;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__any=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__anyAttribute=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__size=0;

    	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->__any=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->__anyAttribute=NULL;
    }

    if(strcmp((trt__GetVideoEncoderConfiguration->ConfigurationToken),"VEC_token_1") == 0)
    {

	    trt__GetVideoEncoderConfigurationResponse->Configuration=(struct tt__VideoEncoderConfiguration*)soap_malloc( soap, sizeof(struct tt__VideoEncoderConfiguration));
	    memset(trt__GetVideoEncoderConfigurationResponse->Configuration,0,sizeof(struct tt__VideoEncoderConfiguration));  

	   //请求的时候需要匹配的一些基本信息
        trt__GetVideoEncoderConfigurationResponse->Configuration->Name = (char *)soap_malloc(soap, sizeof(char)*MAX_PROF_TOKEN);
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Name, '\0', sizeof(char)*MAX_PROF_TOKEN);
        strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->Name, "VEC_Name_1");

        trt__GetVideoEncoderConfigurationResponse->Configuration->token = (char *)soap_malloc(soap, sizeof(char)*MAX_PROF_TOKEN);
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->token, '\0', sizeof(char)*MAX_PROF_TOKEN);
        strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->token, "VEC_token_1");

        trt__GetVideoEncoderConfigurationResponse->Configuration->UseCount = 0;
        trt__GetVideoEncoderConfigurationResponse->Configuration->Quality = 50.000000;
        //根据前端设备时间支持的编码格式选择对应的值，因为我测试的是设备只支持H264 ，所以选了2
        trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding = (enum tt__VideoEncoding) 2;   // JPEG = 0 , MPEG = 1, H264 = 2; 

        trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap,sizeof(struct tt__VideoResolution));
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution, 0 , sizeof(struct tt__VideoResolution));
        // 请求的视频的分辨率，对应前端设备填写对应的值，我这是1280 * 720
        trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Width  = g_video_width_ex;
        trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Height = g_video_height_ex;

        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl, 0, sizeof(struct tt__VideoRateControl));
        //请求的对应的编码信息.各个意思参考上面说明
        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->FrameRateLimit   = 25;
        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->EncodingInterval = 1;
        trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->BitrateLimit     =16384;

        trt__GetVideoEncoderConfigurationResponse->Configuration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
        memset(trt__GetVideoEncoderConfigurationResponse->Configuration->H264, 0, sizeof(struct tt__H264Configuration));
        trt__GetVideoEncoderConfigurationResponse->Configuration->H264->GovLength  = 1;
        trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = (enum tt__H264Profile)1;

    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast=(struct tt__MulticastConfiguration *)soap_malloc(soap,sizeof(struct tt__MulticastConfiguration));
    	memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast,0,sizeof(struct tt__MulticastConfiguration));
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address=(struct tt__IPAddress *)soap_malloc(soap,sizeof(struct tt__IPAddress));
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->Type=tt__IPType__IPv4;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv6Address=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Port=g_rtsp_port;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->TTL=0;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->AutoStart=xsd__boolean__false_;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__any=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__anyAttribute=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->__size=0;

    	trt__GetVideoEncoderConfigurationResponse->Configuration->MPEG4=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->__any=NULL;
    	trt__GetVideoEncoderConfigurationResponse->Configuration->__anyAttribute=NULL;
    }



    

	return SOAP_OK;
}	



//此函数要在__tds__GetCapabilities中设置imging选项就会强制性调用此接口
SOAP_FMAC5 int SOAP_FMAC6 __timg__GetOptions(struct soap* soap, struct _timg__GetOptions *timg__GetOptions, struct _timg__GetOptionsResponse *timg__GetOptionsResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	

	timg__GetOptionsResponse->ImagingOptions=(struct tt__ImagingOptions20*)soap_malloc(soap, sizeof(struct tt__ImagingOptions20));
	memset(timg__GetOptionsResponse->ImagingOptions,0,sizeof(struct tt__ImagingOptions20));
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation=(struct tt__BacklightCompensationOptions20*)soap_malloc(soap, sizeof(struct tt__BacklightCompensationOptions20));
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Level=(struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Level->Max=255;
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Level->Min=1;
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Mode=(enum tt__BacklightCompensationMode *)soap_malloc(soap, sizeof(enum tt__BacklightCompensationMode ));
    *(timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->Mode)=tt__BacklightCompensationMode__OFF;
	timg__GetOptionsResponse->ImagingOptions->BacklightCompensation->__sizeMode=0;
	timg__GetOptionsResponse->ImagingOptions->Brightness=(struct tt__FloatRange *)soap_malloc(soap, sizeof(struct tt__FloatRange ));
	timg__GetOptionsResponse->ImagingOptions->Brightness->Max=255;
	timg__GetOptionsResponse->ImagingOptions->Brightness->Min=1;
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation=(struct tt__FloatRange*)soap_malloc( soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation->Max=255;
	timg__GetOptionsResponse->ImagingOptions->ColorSaturation->Min=1;
	timg__GetOptionsResponse->ImagingOptions->Contrast=(struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Contrast->Max=255;
	timg__GetOptionsResponse->ImagingOptions->Contrast->Min=1;
	timg__GetOptionsResponse->ImagingOptions->Exposure=NULL;
	timg__GetOptionsResponse->ImagingOptions->Extension=NULL;
	timg__GetOptionsResponse->ImagingOptions->Focus=NULL;
	timg__GetOptionsResponse->ImagingOptions->IrCutFilterModes=(enum tt__IrCutFilterMode *)soap_malloc( soap, sizeof(enum tt__IrCutFilterMode ));
    *(timg__GetOptionsResponse->ImagingOptions->IrCutFilterModes)=tt__IrCutFilterMode__ON;
	timg__GetOptionsResponse->ImagingOptions->Sharpness=(struct tt__FloatRange *)soap_malloc( soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Max=255;
	timg__GetOptionsResponse->ImagingOptions->Sharpness->Min=1;
	timg__GetOptionsResponse->ImagingOptions->WhiteBalance=NULL;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange=(struct tt__WideDynamicRangeOptions20*)soap_malloc(soap, sizeof(struct tt__WideDynamicRangeOptions20));
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Level=(struct tt__FloatRange*)soap_malloc(soap, sizeof(struct tt__FloatRange));
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Level->Max=255;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Level->Min=1;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode=(enum tt__WideDynamicMode *)soap_malloc(soap, sizeof(enum tt__WideDynamicMode ));
    *(timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->Mode)=tt__WideDynamicMode__OFF;
	timg__GetOptionsResponse->ImagingOptions->WideDynamicRange->__sizeMode=1;
	timg__GetOptionsResponse->ImagingOptions->__anyAttribute=NULL;
	timg__GetOptionsResponse->ImagingOptions->__sizeIrCutFilterModes=0;


	return SOAP_OK;
}




SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	 
    return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemDateAndTime(struct soap* soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse)
{ 

	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ 
	return SOAP_OK; 
}










SOAP_FMAC5 int SOAP_FMAC6 SOAP_ENV__Fault(struct soap* soap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Hello(struct soap* soap, struct wsdd__HelloType *wsdd__Hello) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Bye(struct soap* soap, struct wsdd__ByeType *wsdd__Bye) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__ProbeMatches(struct soap* soap, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Resolve(struct soap* soap, struct wsdd__ResolveType *wsdd__Resolve) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__ResolveMatches(struct soap* soap, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 SOAP_ENV__Fault_Yinj(struct soap* soap, char *faultcode, char *faultstring, char *faultactor, struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *SOAP_ENV__Code, struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node, char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__GetSupportedActions(struct soap* soap, struct _ns1__GetSupportedActions *ns1__GetSupportedActions, struct _ns1__GetSupportedActionsResponse *ns1__GetSupportedActionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__GetActions(struct soap* soap, struct _ns1__GetActions *ns1__GetActions, struct _ns1__GetActionsResponse *ns1__GetActionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__CreateActions(struct soap* soap, struct _ns1__CreateActions *ns1__CreateActions, struct _ns1__CreateActionsResponse *ns1__CreateActionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__DeleteActions(struct soap* soap, struct _ns1__DeleteActions *ns1__DeleteActions, struct _ns1__DeleteActionsResponse *ns1__DeleteActionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__ModifyActions(struct soap* soap, struct _ns1__ModifyActions *ns1__ModifyActions, struct _ns1__ModifyActionsResponse *ns1__ModifyActionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__GetServiceCapabilities(struct soap* soap, struct _ns1__GetServiceCapabilities *ns1__GetServiceCapabilities, struct _ns1__GetServiceCapabilitiesResponse *ns1__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__GetActionTriggers(struct soap* soap, struct _ns1__GetActionTriggers *ns1__GetActionTriggers, struct _ns1__GetActionTriggersResponse *ns1__GetActionTriggersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__CreateActionTriggers(struct soap* soap, struct _ns1__CreateActionTriggers *ns1__CreateActionTriggers, struct _ns1__CreateActionTriggersResponse *ns1__CreateActionTriggersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__DeleteActionTriggers(struct soap* soap, struct _ns1__DeleteActionTriggers *ns1__DeleteActionTriggers, struct _ns1__DeleteActionTriggersResponse *ns1__DeleteActionTriggersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns1__ModifyActionTriggers(struct soap* soap, struct _ns1__ModifyActionTriggers *ns1__ModifyActionTriggers, struct _ns1__ModifyActionTriggersResponse *ns1__ModifyActionTriggersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__GetServiceCapabilities(struct soap* soap, struct _ns2__GetServiceCapabilities *ns2__GetServiceCapabilities, struct _ns2__GetServiceCapabilitiesResponse *ns2__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__GetAccessPointInfoList(struct soap* soap, struct _ns2__GetAccessPointInfoList *ns2__GetAccessPointInfoList, struct _ns2__GetAccessPointInfoListResponse *ns2__GetAccessPointInfoListResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__GetAccessPointInfo(struct soap* soap, struct _ns2__GetAccessPointInfo *ns2__GetAccessPointInfo, struct _ns2__GetAccessPointInfoResponse *ns2__GetAccessPointInfoResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__GetAreaInfoList(struct soap* soap, struct _ns2__GetAreaInfoList *ns2__GetAreaInfoList, struct _ns2__GetAreaInfoListResponse *ns2__GetAreaInfoListResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__GetAreaInfo(struct soap* soap, struct _ns2__GetAreaInfo *ns2__GetAreaInfo, struct _ns2__GetAreaInfoResponse *ns2__GetAreaInfoResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__GetAccessPointState(struct soap* soap, struct _ns2__GetAccessPointState *ns2__GetAccessPointState, struct _ns2__GetAccessPointStateResponse *ns2__GetAccessPointStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__EnableAccessPoint(struct soap* soap, struct _ns2__EnableAccessPoint *ns2__EnableAccessPoint, struct _ns2__EnableAccessPointResponse *ns2__EnableAccessPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__DisableAccessPoint(struct soap* soap, struct _ns2__DisableAccessPoint *ns2__DisableAccessPoint, struct _ns2__DisableAccessPointResponse *ns2__DisableAccessPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns2__ExternalAuthorization(struct soap* soap, struct _ns2__ExternalAuthorization *ns2__ExternalAuthorization, struct _ns2__ExternalAuthorizationResponse *ns2__ExternalAuthorizationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__GetServiceCapabilities(struct soap* soap, struct _ns4__GetServiceCapabilities *ns4__GetServiceCapabilities, struct _ns4__GetServiceCapabilitiesResponse *ns4__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__GetDoorInfoList(struct soap* soap, struct _ns4__GetDoorInfoList *ns4__GetDoorInfoList, struct _ns4__GetDoorInfoListResponse *ns4__GetDoorInfoListResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__GetDoorInfo(struct soap* soap, struct _ns4__GetDoorInfo *ns4__GetDoorInfo, struct _ns4__GetDoorInfoResponse *ns4__GetDoorInfoResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__GetDoorState(struct soap* soap, struct _ns4__GetDoorState *ns4__GetDoorState, struct _ns4__GetDoorStateResponse *ns4__GetDoorStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__AccessDoor(struct soap* soap, struct _ns4__AccessDoor *ns4__AccessDoor, struct _ns4__AccessDoorResponse *ns4__AccessDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__LockDoor(struct soap* soap, struct _ns4__LockDoor *ns4__LockDoor, struct _ns4__LockDoorResponse *ns4__LockDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__UnlockDoor(struct soap* soap, struct _ns4__UnlockDoor *ns4__UnlockDoor, struct _ns4__UnlockDoorResponse *ns4__UnlockDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__BlockDoor(struct soap* soap, struct _ns4__BlockDoor *ns4__BlockDoor, struct _ns4__BlockDoorResponse *ns4__BlockDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__LockDownDoor(struct soap* soap, struct _ns4__LockDownDoor *ns4__LockDownDoor, struct _ns4__LockDownDoorResponse *ns4__LockDownDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__LockDownReleaseDoor(struct soap* soap, struct _ns4__LockDownReleaseDoor *ns4__LockDownReleaseDoor, struct _ns4__LockDownReleaseDoorResponse *ns4__LockDownReleaseDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__LockOpenDoor(struct soap* soap, struct _ns4__LockOpenDoor *ns4__LockOpenDoor, struct _ns4__LockOpenDoorResponse *ns4__LockOpenDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__LockOpenReleaseDoor(struct soap* soap, struct _ns4__LockOpenReleaseDoor *ns4__LockOpenReleaseDoor, struct _ns4__LockOpenReleaseDoorResponse *ns4__LockOpenReleaseDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns4__DoubleLockDoor(struct soap* soap, struct _ns4__DoubleLockDoor *ns4__DoubleLockDoor, struct _ns4__DoubleLockDoorResponse *ns4__DoubleLockDoorResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetServiceCapabilities(struct soap* soap, struct _ns5__GetServiceCapabilities *ns5__GetServiceCapabilities, struct _ns5__GetServiceCapabilitiesResponse *ns5__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__CreateRSAKeyPair(struct soap* soap, struct _ns5__CreateRSAKeyPair *ns5__CreateRSAKeyPair, struct _ns5__CreateRSAKeyPairResponse *ns5__CreateRSAKeyPairResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetKeyStatus(struct soap* soap, struct _ns5__GetKeyStatus *ns5__GetKeyStatus, struct _ns5__GetKeyStatusResponse *ns5__GetKeyStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetPrivateKeyStatus(struct soap* soap, struct _ns5__GetPrivateKeyStatus *ns5__GetPrivateKeyStatus, struct _ns5__GetPrivateKeyStatusResponse *ns5__GetPrivateKeyStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetAllKeys(struct soap* soap, struct _ns5__GetAllKeys *ns5__GetAllKeys, struct _ns5__GetAllKeysResponse *ns5__GetAllKeysResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__DeleteKey(struct soap* soap, struct _ns5__DeleteKey *ns5__DeleteKey, struct _ns5__DeleteKeyResponse *ns5__DeleteKeyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__CreatePKCS10CSR(struct soap* soap, struct _ns5__CreatePKCS10CSR *ns5__CreatePKCS10CSR, struct _ns5__CreatePKCS10CSRResponse *ns5__CreatePKCS10CSRResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__CreateSelfSignedCertificate(struct soap* soap, struct _ns5__CreateSelfSignedCertificate *ns5__CreateSelfSignedCertificate, struct _ns5__CreateSelfSignedCertificateResponse *ns5__CreateSelfSignedCertificateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__UploadCertificate(struct soap* soap, struct _ns5__UploadCertificate *ns5__UploadCertificate, struct _ns5__UploadCertificateResponse *ns5__UploadCertificateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetCertificate(struct soap* soap, struct _ns5__GetCertificate *ns5__GetCertificate, struct _ns5__GetCertificateResponse *ns5__GetCertificateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetAllCertificates(struct soap* soap, struct _ns5__GetAllCertificates *ns5__GetAllCertificates, struct _ns5__GetAllCertificatesResponse *ns5__GetAllCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__DeleteCertificate(struct soap* soap, struct _ns5__DeleteCertificate *ns5__DeleteCertificate, struct _ns5__DeleteCertificateResponse *ns5__DeleteCertificateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__CreateCertificationPath(struct soap* soap, struct _ns5__CreateCertificationPath *ns5__CreateCertificationPath, struct _ns5__CreateCertificationPathResponse *ns5__CreateCertificationPathResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetCertificationPath(struct soap* soap, struct _ns5__GetCertificationPath *ns5__GetCertificationPath, struct _ns5__GetCertificationPathResponse *ns5__GetCertificationPathResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetAllCertificationPaths(struct soap* soap, struct _ns5__GetAllCertificationPaths *ns5__GetAllCertificationPaths, struct _ns5__GetAllCertificationPathsResponse *ns5__GetAllCertificationPathsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__DeleteCertificationPath(struct soap* soap, struct _ns5__DeleteCertificationPath *ns5__DeleteCertificationPath, struct _ns5__DeleteCertificationPathResponse *ns5__DeleteCertificationPathResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__AddServerCertificateAssignment(struct soap* soap, struct _ns5__AddServerCertificateAssignment *ns5__AddServerCertificateAssignment, struct _ns5__AddServerCertificateAssignmentResponse *ns5__AddServerCertificateAssignmentResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__RemoveServerCertificateAssignment(struct soap* soap, struct _ns5__RemoveServerCertificateAssignment *ns5__RemoveServerCertificateAssignment, struct _ns5__RemoveServerCertificateAssignmentResponse *ns5__RemoveServerCertificateAssignmentResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__ReplaceServerCertificateAssignment(struct soap* soap, struct _ns5__ReplaceServerCertificateAssignment *ns5__ReplaceServerCertificateAssignment, struct _ns5__ReplaceServerCertificateAssignmentResponse *ns5__ReplaceServerCertificateAssignmentResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __ns5__GetAssignedServerCertificates(struct soap* soap, struct _ns5__GetAssignedServerCertificates *ns5__GetAssignedServerCertificates, struct _ns5__GetAssignedServerCertificatesResponse *ns5__GetAssignedServerCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetServiceCapabilities(struct soap* soap, struct _tad__GetServiceCapabilities *tad__GetServiceCapabilities, struct _tad__GetServiceCapabilitiesResponse *tad__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__DeleteAnalyticsEngineControl(struct soap* soap, struct _tad__DeleteAnalyticsEngineControl *tad__DeleteAnalyticsEngineControl, struct _tad__DeleteAnalyticsEngineControlResponse *tad__DeleteAnalyticsEngineControlResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__CreateAnalyticsEngineControl(struct soap* soap, struct _tad__CreateAnalyticsEngineControl *tad__CreateAnalyticsEngineControl, struct _tad__CreateAnalyticsEngineControlResponse *tad__CreateAnalyticsEngineControlResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__SetAnalyticsEngineControl(struct soap* soap, struct _tad__SetAnalyticsEngineControl *tad__SetAnalyticsEngineControl, struct _tad__SetAnalyticsEngineControlResponse *tad__SetAnalyticsEngineControlResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineControl(struct soap* soap, struct _tad__GetAnalyticsEngineControl *tad__GetAnalyticsEngineControl, struct _tad__GetAnalyticsEngineControlResponse *tad__GetAnalyticsEngineControlResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineControls(struct soap* soap, struct _tad__GetAnalyticsEngineControls *tad__GetAnalyticsEngineControls, struct _tad__GetAnalyticsEngineControlsResponse *tad__GetAnalyticsEngineControlsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngine(struct soap* soap, struct _tad__GetAnalyticsEngine *tad__GetAnalyticsEngine, struct _tad__GetAnalyticsEngineResponse *tad__GetAnalyticsEngineResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngines(struct soap* soap, struct _tad__GetAnalyticsEngines *tad__GetAnalyticsEngines, struct _tad__GetAnalyticsEnginesResponse *tad__GetAnalyticsEnginesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__SetVideoAnalyticsConfiguration(struct soap* soap, struct _tad__SetVideoAnalyticsConfiguration *tad__SetVideoAnalyticsConfiguration, struct _tad__SetVideoAnalyticsConfigurationResponse *tad__SetVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__SetAnalyticsEngineInput(struct soap* soap, struct _tad__SetAnalyticsEngineInput *tad__SetAnalyticsEngineInput, struct _tad__SetAnalyticsEngineInputResponse *tad__SetAnalyticsEngineInputResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineInput(struct soap* soap, struct _tad__GetAnalyticsEngineInput *tad__GetAnalyticsEngineInput, struct _tad__GetAnalyticsEngineInputResponse *tad__GetAnalyticsEngineInputResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsEngineInputs(struct soap* soap, struct _tad__GetAnalyticsEngineInputs *tad__GetAnalyticsEngineInputs, struct _tad__GetAnalyticsEngineInputsResponse *tad__GetAnalyticsEngineInputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsDeviceStreamUri(struct soap* soap, struct _tad__GetAnalyticsDeviceStreamUri *tad__GetAnalyticsDeviceStreamUri, struct _tad__GetAnalyticsDeviceStreamUriResponse *tad__GetAnalyticsDeviceStreamUriResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetVideoAnalyticsConfiguration(struct soap* soap, struct _tad__GetVideoAnalyticsConfiguration *tad__GetVideoAnalyticsConfiguration, struct _tad__GetVideoAnalyticsConfigurationResponse *tad__GetVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__CreateAnalyticsEngineInputs(struct soap* soap, struct _tad__CreateAnalyticsEngineInputs *tad__CreateAnalyticsEngineInputs, struct _tad__CreateAnalyticsEngineInputsResponse *tad__CreateAnalyticsEngineInputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__DeleteAnalyticsEngineInputs(struct soap* soap, struct _tad__DeleteAnalyticsEngineInputs *tad__DeleteAnalyticsEngineInputs, struct _tad__DeleteAnalyticsEngineInputsResponse *tad__DeleteAnalyticsEngineInputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tad__GetAnalyticsState(struct soap* soap, struct _tad__GetAnalyticsState *tad__GetAnalyticsState, struct _tad__GetAnalyticsStateResponse *tad__GetAnalyticsStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetSupportedRules(struct soap* soap, struct _tan__GetSupportedRules *tan__GetSupportedRules, struct _tan__GetSupportedRulesResponse *tan__GetSupportedRulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__CreateRules(struct soap* soap, struct _tan__CreateRules *tan__CreateRules, struct _tan__CreateRulesResponse *tan__CreateRulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__DeleteRules(struct soap* soap, struct _tan__DeleteRules *tan__DeleteRules, struct _tan__DeleteRulesResponse *tan__DeleteRulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetRules(struct soap* soap, struct _tan__GetRules *tan__GetRules, struct _tan__GetRulesResponse *tan__GetRulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__ModifyRules(struct soap* soap, struct _tan__ModifyRules *tan__ModifyRules, struct _tan__ModifyRulesResponse *tan__ModifyRulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetServiceCapabilities(struct soap* soap, struct _tan__GetServiceCapabilities *tan__GetServiceCapabilities, struct _tan__GetServiceCapabilitiesResponse *tan__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetSupportedAnalyticsModules(struct soap* soap, struct _tan__GetSupportedAnalyticsModules *tan__GetSupportedAnalyticsModules, struct _tan__GetSupportedAnalyticsModulesResponse *tan__GetSupportedAnalyticsModulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__CreateAnalyticsModules(struct soap* soap, struct _tan__CreateAnalyticsModules *tan__CreateAnalyticsModules, struct _tan__CreateAnalyticsModulesResponse *tan__CreateAnalyticsModulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__DeleteAnalyticsModules(struct soap* soap, struct _tan__DeleteAnalyticsModules *tan__DeleteAnalyticsModules, struct _tan__DeleteAnalyticsModulesResponse *tan__DeleteAnalyticsModulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__GetAnalyticsModules(struct soap* soap, struct _tan__GetAnalyticsModules *tan__GetAnalyticsModules, struct _tan__GetAnalyticsModulesResponse *tan__GetAnalyticsModulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tan__ModifyAnalyticsModules(struct soap* soap, struct _tan__ModifyAnalyticsModules *tan__ModifyAnalyticsModules, struct _tan__ModifyAnalyticsModulesResponse *tan__ModifyAnalyticsModulesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Hello(struct soap* soap, struct wsdd__HelloType tdn__Hello, struct wsdd__ResolveType *tdn__HelloResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Bye(struct soap* soap, struct wsdd__ByeType tdn__Bye, struct wsdd__ResolveType *tdn__ByeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tdn__Probe(struct soap* soap, struct wsdd__ProbeType tdn__Probe, struct wsdd__ProbeMatchesType *tdn__ProbeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServiceCapabilities(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemDateAndTime(struct soap* soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemFactoryDefault(struct soap* soap, struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__UpgradeSystemFirmware(struct soap* soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SystemReboot(struct soap* soap, struct _tds__SystemReboot *tds__SystemReboot, struct _tds__SystemRebootResponse *tds__SystemRebootResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__RestoreSystem(struct soap* soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemBackup(struct soap* soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemLog(struct soap* soap, struct _tds__GetSystemLog *tds__GetSystemLog, struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemSupportInformation(struct soap* soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetScopes(struct soap* soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__AddScopes(struct soap* soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveScopes(struct soap* soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDiscoveryMode(struct soap* soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDiscoveryMode(struct soap* soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteDiscoveryMode(struct soap* soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteDiscoveryMode(struct soap* soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDPAddresses(struct soap* soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetEndpointReference(struct soap* soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteUser(struct soap* soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteUser(struct soap* soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetWsdlUrl(struct soap* soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDPAddresses(struct soap* soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetHostname(struct soap* soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostname(struct soap* soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostnameFromDHCP(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDNS(struct soap* soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDNS(struct soap* soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNTP(struct soap* soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNTP(struct soap* soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDynamicDNS(struct soap* soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDynamicDNS(struct soap* soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkInterfaces(struct soap* soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkInterfaces(struct soap* soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkProtocols(struct soap* soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkProtocols(struct soap* soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkDefaultGateway(struct soap* soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkDefaultGateway(struct soap* soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetZeroConfiguration(struct soap* soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetZeroConfiguration(struct soap* soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetIPAddressFilter(struct soap* soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetIPAddressFilter(struct soap* soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__AddIPAddressFilter(struct soap* soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveIPAddressFilter(struct soap* soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetAccessPolicy(struct soap* soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetAccessPolicy(struct soap* soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateCertificate(struct soap* soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificates(struct soap* soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificatesStatus(struct soap* soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetCertificatesStatus(struct soap* soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteCertificates(struct soap* soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetPkcs10Request(struct soap* soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificates(struct soap* soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetClientCertificateMode(struct soap* soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetClientCertificateMode(struct soap* soap, struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputSettings(struct soap* soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SendAuxiliaryCommand(struct soap* soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCACertificates(struct soap* soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificateWithPrivateKey(struct soap* soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificateInformation(struct soap* soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCACertificates(struct soap* soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateDot1XConfiguration(struct soap* soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDot1XConfiguration(struct soap* soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfiguration(struct soap* soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfigurations(struct soap* soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteDot1XConfiguration(struct soap* soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Capabilities(struct soap* soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Status(struct soap* soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__ScanAvailableDot11Networks(struct soap* soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemUris(struct soap* soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__StartFirmwareUpgrade(struct soap* soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__StartSystemRestore(struct soap* soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServices_(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServiceCapabilities_(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDeviceInformation_(struct soap* soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemDateAndTime_(struct soap* soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemDateAndTime_(struct soap* soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemFactoryDefault_(struct soap* soap, struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__UpgradeSystemFirmware_(struct soap* soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SystemReboot_(struct soap* soap, struct _tds__SystemReboot *tds__SystemReboot, struct _tds__SystemRebootResponse *tds__SystemRebootResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__RestoreSystem_(struct soap* soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemBackup_(struct soap* soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemLog_(struct soap* soap, struct _tds__GetSystemLog *tds__GetSystemLog, struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemSupportInformation_(struct soap* soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetScopes_(struct soap* soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetScopes_(struct soap* soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__AddScopes_(struct soap* soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveScopes_(struct soap* soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDiscoveryMode_(struct soap* soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDiscoveryMode_(struct soap* soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteDiscoveryMode_(struct soap* soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteDiscoveryMode_(struct soap* soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDPAddresses_(struct soap* soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetEndpointReference_(struct soap* soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteUser_(struct soap* soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteUser_(struct soap* soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetUsers_(struct soap* soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateUsers_(struct soap* soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteUsers_(struct soap* soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetUser_(struct soap* soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetWsdlUrl_(struct soap* soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCapabilities_(struct soap* soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDPAddresses_(struct soap* soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetHostname_(struct soap* soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostname_(struct soap* soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostnameFromDHCP_(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDNS_(struct soap* soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDNS_(struct soap* soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNTP_(struct soap* soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNTP_(struct soap* soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDynamicDNS_(struct soap* soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDynamicDNS_(struct soap* soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkInterfaces_(struct soap* soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkInterfaces_(struct soap* soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkProtocols_(struct soap* soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkProtocols_(struct soap* soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkDefaultGateway_(struct soap* soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkDefaultGateway_(struct soap* soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetZeroConfiguration_(struct soap* soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetZeroConfiguration_(struct soap* soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetIPAddressFilter_(struct soap* soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetIPAddressFilter_(struct soap* soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__AddIPAddressFilter_(struct soap* soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveIPAddressFilter_(struct soap* soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetAccessPolicy_(struct soap* soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetAccessPolicy_(struct soap* soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateCertificate_(struct soap* soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificates_(struct soap* soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificatesStatus_(struct soap* soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetCertificatesStatus_(struct soap* soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteCertificates_(struct soap* soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetPkcs10Request_(struct soap* soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificates_(struct soap* soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetClientCertificateMode_(struct soap* soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetClientCertificateMode_(struct soap* soap, struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRelayOutputs_(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputSettings_(struct soap* soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputState_(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SendAuxiliaryCommand_(struct soap* soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCACertificates_(struct soap* soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificateWithPrivateKey_(struct soap* soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificateInformation_(struct soap* soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCACertificates_(struct soap* soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateDot1XConfiguration_(struct soap* soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDot1XConfiguration_(struct soap* soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfiguration_(struct soap* soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfigurations_(struct soap* soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteDot1XConfiguration_(struct soap* soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Capabilities_(struct soap* soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Status_(struct soap* soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__ScanAvailableDot11Networks_(struct soap* soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemUris_(struct soap* soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__StartFirmwareUpgrade_(struct soap* soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tds__StartSystemRestore_(struct soap* soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__PullMessages(struct soap* soap, struct _tev__PullMessages *tev__PullMessages, struct _tev__PullMessagesResponse *tev__PullMessagesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Seek(struct soap* soap, struct _tev__Seek *tev__Seek, struct _tev__SeekResponse *tev__SeekResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__SetSynchronizationPoint(struct soap* soap, struct _tev__SetSynchronizationPoint *tev__SetSynchronizationPoint, struct _tev__SetSynchronizationPointResponse *tev__SetSynchronizationPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__GetServiceCapabilities(struct soap* soap, struct _tev__GetServiceCapabilities *tev__GetServiceCapabilities, struct _tev__GetServiceCapabilitiesResponse *tev__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__CreatePullPointSubscription(struct soap* soap, struct _tev__CreatePullPointSubscription *tev__CreatePullPointSubscription, struct _tev__CreatePullPointSubscriptionResponse *tev__CreatePullPointSubscriptionResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__GetEventProperties(struct soap* soap, struct _tev__GetEventProperties *tev__GetEventProperties, struct _tev__GetEventPropertiesResponse *tev__GetEventPropertiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Renew(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Unsubscribe(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Subscribe(struct soap* soap, struct _wsnt__Subscribe *wsnt__Subscribe, struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__GetCurrentMessage(struct soap* soap, struct _wsnt__GetCurrentMessage *wsnt__GetCurrentMessage, struct _wsnt__GetCurrentMessageResponse *wsnt__GetCurrentMessageResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__GetMessages(struct soap* soap, struct _wsnt__GetMessages *wsnt__GetMessages, struct _wsnt__GetMessagesResponse *wsnt__GetMessagesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__DestroyPullPoint(struct soap* soap, struct _wsnt__DestroyPullPoint *wsnt__DestroyPullPoint, struct _wsnt__DestroyPullPointResponse *wsnt__DestroyPullPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Notify_(struct soap* soap, struct _wsnt__Notify *wsnt__Notify) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__CreatePullPoint(struct soap* soap, struct _wsnt__CreatePullPoint *wsnt__CreatePullPoint, struct _wsnt__CreatePullPointResponse *wsnt__CreatePullPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Renew_(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__Unsubscribe_(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__PauseSubscription(struct soap* soap, struct _wsnt__PauseSubscription *wsnt__PauseSubscription, struct _wsnt__PauseSubscriptionResponse *wsnt__PauseSubscriptionResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tev__ResumeSubscription(struct soap* soap, struct _wsnt__ResumeSubscription *wsnt__ResumeSubscription, struct _wsnt__ResumeSubscriptionResponse *wsnt__ResumeSubscriptionResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetServiceCapabilities(struct soap* soap, struct _timg__GetServiceCapabilities *timg__GetServiceCapabilities, struct _timg__GetServiceCapabilitiesResponse *timg__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetImagingSettings(struct soap* soap, struct _timg__GetImagingSettings *timg__GetImagingSettings, struct _timg__GetImagingSettingsResponse *timg__GetImagingSettingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __timg__SetImagingSettings(struct soap* soap, struct _timg__SetImagingSettings *timg__SetImagingSettings, struct _timg__SetImagingSettingsResponse *timg__SetImagingSettingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


SOAP_FMAC5 int SOAP_FMAC6 __timg__Move(struct soap* soap, struct _timg__Move *timg__Move, struct _timg__MoveResponse *timg__MoveResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __timg__Stop(struct soap* soap, struct _timg__Stop *timg__Stop, struct _timg__StopResponse *timg__StopResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetStatus(struct soap* soap, struct _timg__GetStatus *timg__GetStatus, struct _timg__GetStatusResponse *timg__GetStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __timg__GetMoveOptions(struct soap* soap, struct _timg__GetMoveOptions *timg__GetMoveOptions, struct _timg__GetMoveOptionsResponse *timg__GetMoveOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetServiceCapabilities(struct soap* soap, struct _tls__GetServiceCapabilities *tls__GetServiceCapabilities, struct _tls__GetServiceCapabilitiesResponse *tls__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetLayout(struct soap* soap, struct _tls__GetLayout *tls__GetLayout, struct _tls__GetLayoutResponse *tls__GetLayoutResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__SetLayout(struct soap* soap, struct _tls__SetLayout *tls__SetLayout, struct _tls__SetLayoutResponse *tls__SetLayoutResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetDisplayOptions(struct soap* soap, struct _tls__GetDisplayOptions *tls__GetDisplayOptions, struct _tls__GetDisplayOptionsResponse *tls__GetDisplayOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetPaneConfigurations(struct soap* soap, struct _tls__GetPaneConfigurations *tls__GetPaneConfigurations, struct _tls__GetPaneConfigurationsResponse *tls__GetPaneConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__GetPaneConfiguration(struct soap* soap, struct _tls__GetPaneConfiguration *tls__GetPaneConfiguration, struct _tls__GetPaneConfigurationResponse *tls__GetPaneConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__SetPaneConfigurations(struct soap* soap, struct _tls__SetPaneConfigurations *tls__SetPaneConfigurations, struct _tls__SetPaneConfigurationsResponse *tls__SetPaneConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__SetPaneConfiguration(struct soap* soap, struct _tls__SetPaneConfiguration *tls__SetPaneConfiguration, struct _tls__SetPaneConfigurationResponse *tls__SetPaneConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__CreatePaneConfiguration(struct soap* soap, struct _tls__CreatePaneConfiguration *tls__CreatePaneConfiguration, struct _tls__CreatePaneConfigurationResponse *tls__CreatePaneConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tls__DeletePaneConfiguration(struct soap* soap, struct _tls__DeletePaneConfiguration *tls__DeletePaneConfiguration, struct _tls__DeletePaneConfigurationResponse *tls__DeletePaneConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetServiceCapabilities(struct soap* soap, struct _tmd__GetServiceCapabilities *tmd__GetServiceCapabilities, struct _tmd__GetServiceCapabilitiesResponse *tmd__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetRelayOutputOptions(struct soap* soap, struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions, struct _tmd__GetRelayOutputOptionsResponse *tmd__GetRelayOutputOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSources(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }



SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoOutputs(struct soap* soap, struct _tmd__GetVideoOutputs *tmd__GetVideoOutputs, struct _tmd__GetVideoOutputsResponse *tmd__GetVideoOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSourceConfiguration(struct soap* soap, struct _tmd__GetVideoSourceConfiguration *tmd__GetVideoSourceConfiguration, struct _tmd__GetVideoSourceConfigurationResponse *tmd__GetVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoOutputConfiguration(struct soap* soap, struct _tmd__GetVideoOutputConfiguration *tmd__GetVideoOutputConfiguration, struct _tmd__GetVideoOutputConfigurationResponse *tmd__GetVideoOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSourceConfiguration(struct soap* soap, struct _tmd__GetAudioSourceConfiguration *tmd__GetAudioSourceConfiguration, struct _tmd__GetAudioSourceConfigurationResponse *tmd__GetAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputConfiguration(struct soap* soap, struct _tmd__GetAudioOutputConfiguration *tmd__GetAudioOutputConfiguration, struct _tmd__GetAudioOutputConfigurationResponse *tmd__GetAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetVideoSourceConfiguration(struct soap* soap, struct _tmd__SetVideoSourceConfiguration *tmd__SetVideoSourceConfiguration, struct _tmd__SetVideoSourceConfigurationResponse *tmd__SetVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetVideoOutputConfiguration(struct soap* soap, struct _tmd__SetVideoOutputConfiguration *tmd__SetVideoOutputConfiguration, struct _tmd__SetVideoOutputConfigurationResponse *tmd__SetVideoOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetAudioSourceConfiguration(struct soap* soap, struct _tmd__SetAudioSourceConfiguration *tmd__SetAudioSourceConfiguration, struct _tmd__SetAudioSourceConfigurationResponse *tmd__SetAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetAudioOutputConfiguration(struct soap* soap, struct _tmd__SetAudioOutputConfiguration *tmd__SetAudioOutputConfiguration, struct _tmd__SetAudioOutputConfigurationResponse *tmd__SetAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSourceConfigurationOptions(struct soap* soap, struct _tmd__GetVideoSourceConfigurationOptions *tmd__GetVideoSourceConfigurationOptions, struct _tmd__GetVideoSourceConfigurationOptionsResponse *tmd__GetVideoSourceConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoOutputConfigurationOptions(struct soap* soap, struct _tmd__GetVideoOutputConfigurationOptions *tmd__GetVideoOutputConfigurationOptions, struct _tmd__GetVideoOutputConfigurationOptionsResponse *tmd__GetVideoOutputConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSourceConfigurationOptions(struct soap* soap, struct _tmd__GetAudioSourceConfigurationOptions *tmd__GetAudioSourceConfigurationOptions, struct _tmd__GetAudioSourceConfigurationOptionsResponse *tmd__GetAudioSourceConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputConfigurationOptions(struct soap* soap, struct _tmd__GetAudioOutputConfigurationOptions *tmd__GetAudioOutputConfigurationOptions, struct _tmd__GetAudioOutputConfigurationOptionsResponse *tmd__GetAudioOutputConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetRelayOutputSettings(struct soap* soap, struct _tmd__SetRelayOutputSettings *tmd__SetRelayOutputSettings, struct _tmd__SetRelayOutputSettingsResponse *tmd__SetRelayOutputSettingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetDigitalInputs(struct soap* soap, struct _tmd__GetDigitalInputs *tmd__GetDigitalInputs, struct _tmd__GetDigitalInputsResponse *tmd__GetDigitalInputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetSerialPorts(struct soap* soap, struct _tmd__GetSerialPorts *tmd__GetSerialPorts, struct _tmd__GetSerialPortsResponse *tmd__GetSerialPortsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetSerialPortConfiguration(struct soap* soap, struct _tmd__GetSerialPortConfiguration *tmd__GetSerialPortConfiguration, struct _tmd__GetSerialPortConfigurationResponse *tmd__GetSerialPortConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetSerialPortConfiguration(struct soap* soap, struct _tmd__SetSerialPortConfiguration *tmd__SetSerialPortConfiguration, struct _tmd__SetSerialPortConfigurationResponse *tmd__SetSerialPortConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetSerialPortConfigurationOptions(struct soap* soap, struct _tmd__GetSerialPortConfigurationOptions *tmd__GetSerialPortConfigurationOptions, struct _tmd__GetSerialPortConfigurationOptionsResponse *tmd__GetSerialPortConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tmd__SendReceiveSerialCommand(struct soap* soap, struct _tmd__SendReceiveSerialCommand *tmd__SendReceiveSerialCommand, struct _tmd__SendReceiveSerialCommandResponse *tmd__SendReceiveSerialCommandResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetServiceCapabilities(struct soap* soap, struct _tptz__GetServiceCapabilities *tptz__GetServiceCapabilities, struct _tptz__GetServiceCapabilitiesResponse *tptz__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetConfigurations
	(
	struct soap* soap
	, struct _tptz__GetConfigurations *tptz__GetConfigurations
	, struct _tptz__GetConfigurationsResponse *tptz__GetConfigurationsResponse) 
{ 
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ 
	

	tptz__GetConfigurationsResponse = (struct _tptz__GetConfigurationsResponse *)soap_malloc(soap, sizeof(struct _tptz__GetConfigurationsResponse));
	memset(tptz__GetConfigurationsResponse, 0, sizeof(struct _tptz__GetConfigurationsResponse));

	tptz__GetConfigurationsResponse->__sizePTZConfiguration = 1;

	tptz__GetConfigurationsResponse->PTZConfiguration =(struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
	memset(tptz__GetConfigurationsResponse->PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration));
	GetPTZConfig(soap, tptz__GetConfigurationsResponse->PTZConfiguration);

	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresets
	( struct soap* soap
	, struct _tptz__GetPresets *tptz__GetPresets
	, struct _tptz__GetPresetsResponse *tptz__GetPresetsResponse)
{
	printf("[%d]%s: to be done.\n",  __LINE__,__FUNCTION__);
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SetPreset
	( struct soap* soap
	, struct _tptz__SetPreset *tptz__SetPreset
	, struct _tptz__SetPresetResponse *tptz__SetPresetResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	
	int iPresetID = -1;	
	int iArg[3] = {-1};
	char szName[32] = {0};

	if (NULL == tptz__SetPreset)
	{
		return SOAP_ERR;
	}

	if (NULL != tptz__SetPreset->ProfileToken)
	{
		printf("domeptz token = %s\n",  tptz__SetPreset->ProfileToken);
	}

	if (NULL != tptz__SetPreset->PresetToken)
	{
		char szStr[256] = {0};
		printf("domeptz presettoken = %s\n",  tptz__SetPreset->PresetToken);
		sscanf(tptz__SetPreset->PresetToken, "%[^1-9]", szStr, sizeof(szStr)-1);
		sscanf(tptz__SetPreset->PresetToken+strlen(szStr), "%d", &iPresetID);
	}

	if (iPresetID < 0 && NULL != tptz__SetPreset->PresetName)
	{
		char szStr[256] = {0};
		printf("domeptz PresetName = %s\n",  tptz__SetPreset->PresetName);
		strncpy(szName, tptz__SetPreset->PresetName, sizeof(szName)-1);
		sscanf(tptz__SetPreset->PresetName, "%[^1-9]", szStr, sizeof(szStr)-1);
		sscanf(tptz__SetPreset->PresetName+strlen(szStr), "%d", &iPresetID);
	}
	
	printf("domeptz iPresetID = %d\n", iPresetID);

	if (iPresetID < 0 || iPresetID > 255)
	{	
		return SOAP_ERR;
	}
	
	iArg[0] = iPresetID;
	iArg[1] = 0;
	if (strlen(szName) > 0 && strlen(szName) < 32)
	{
		iArg[1] = (int)szName;
	}
	
	if (0 != PTZControl(PTZ_PRESET_SET, iArg))
	{
		printf("err: set preset%d failed\n", iPresetID);
		return SOAP_ERR;
	}
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__RemovePreset
	( struct soap* soap
	, struct _tptz__RemovePreset *tptz__RemovePreset
	, struct _tptz__RemovePresetResponse *tptz__RemovePresetResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	int iPresetID = -1;	
	int iArg[3] = {-1};

	if (NULL == tptz__RemovePreset)
	{
		return SOAP_ERR;
	}

	if (NULL != tptz__RemovePreset->ProfileToken)
	{
		printf("domeptz token = %s:\n",  tptz__RemovePreset->ProfileToken);
	}

	if (NULL != tptz__RemovePreset->PresetToken)
	{
		char szStr[256] = {0};
		sscanf(tptz__RemovePreset->PresetToken, "%[^1-9]", szStr, sizeof(szStr)-1);
		sscanf(tptz__RemovePreset->PresetToken+strlen(szStr), "%d", &iPresetID);
	}

	if (iPresetID < 0 || iPresetID > 255)
	{	
		return SOAP_ERR;
	}

	iArg[0] = iPresetID;
	if (0 != PTZControl(PTZ_PRESET_REMOVE, iArg))
	{
		printf("err: remove preset%d failed\n", iPresetID);
		return SOAP_ERR;
	}	
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GotoPreset
	( struct soap* soap
	, struct _tptz__GotoPreset *tptz__GotoPreset
	, struct _tptz__GotoPresetResponse *tptz__GotoPresetResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/

	int iPresetID = -1;	
	int iArg[3] = {-1};

	if (NULL == tptz__GotoPreset)
	{
		return SOAP_ERR;
	}

	if (NULL != tptz__GotoPreset->ProfileToken)
	{
		printf("domeptz token = %s\n",  tptz__GotoPreset->ProfileToken);
	}

	if (NULL != tptz__GotoPreset->PresetToken)
	{
		char szStr[256] = {0};
		sscanf(tptz__GotoPreset->PresetToken, "%[^1-9]", szStr, sizeof(szStr)-1);
		sscanf(tptz__GotoPreset->PresetToken+strlen(szStr), "%d", &iPresetID);
	}

	printf("domeptz iPresetID = %d\n", iPresetID);

	if (iPresetID < 0 || iPresetID > 255)
	{
		return SOAP_ERR;
	}

	iArg[0] = iPresetID;
	//iArg[1] = iSpeed;//todo
	if (0 != PTZControl(PTZ_PRESET_CALL, iArg))
	{
		printf("err: goto preset%d failed\n", iPresetID);
		return SOAP_ERR;
	}	
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetStatus(struct soap* soap, struct _tptz__GetStatus *tptz__GetStatus, struct _tptz__GetStatusResponse *tptz__GetStatusResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetConfiguration
	(
	struct soap* soap
	, struct _tptz__GetConfiguration *tptz__GetConfiguration
	, struct _tptz__GetConfigurationResponse *tptz__GetConfigurationResponse) 
{ 
	printf("[%d]%s: token = %s\n",  __LINE__,__FUNCTION__, tptz__GetConfiguration->PTZConfigurationToken);

	if (NULL == tptz__GetConfiguration
		|| strlen(tptz__GetConfiguration->PTZConfigurationToken)
		|| 0 != strcmp("Profile_name_0", tptz__GetConfiguration->PTZConfigurationToken))
	{	
		return SOAP_ERR;
	}
		 
	tptz__GetConfigurationResponse =(struct _tptz__GetConfigurationResponse *)soap_malloc(soap, sizeof(struct _tptz__GetConfigurationResponse));
	memset(tptz__GetConfigurationResponse, 0, sizeof(struct _tptz__GetConfigurationResponse));

	tptz__GetConfigurationResponse->PTZConfiguration =(struct tt__PTZConfiguration *)soap_malloc(soap, sizeof(struct tt__PTZConfiguration));
	memset(tptz__GetConfigurationResponse->PTZConfiguration, 0, sizeof(struct tt__PTZConfiguration));

	GetPTZConfig(soap, tptz__GetConfigurationResponse->PTZConfiguration);
	
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetNodes
	(struct soap* soap, struct _tptz__GetNodes *tptz__GetNodes, struct _tptz__GetNodesResponse *tptz__GetNodesResponse)
{ 
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/

	tptz__GetNodesResponse = (struct _tptz__GetNodesResponse *)soap_malloc(soap, sizeof(struct _tptz__GetNodesResponse));
	memset(tptz__GetNodesResponse, 0, sizeof(struct _tptz__GetNodesResponse));

	tptz__GetNodesResponse->__sizePTZNode = 1;
	
	tptz__GetNodesResponse->PTZNode = (struct tt__PTZNode *)soap_malloc(soap, sizeof(struct tt__PTZNode));
	memset(tptz__GetNodesResponse->PTZNode, 0, sizeof(struct tt__PTZNode));

	GetPTZNode(soap, tptz__GetNodesResponse->PTZNode);
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetNode
	(struct soap* soap, struct _tptz__GetNode *tptz__GetNode, struct _tptz__GetNodeResponse *tptz__GetNodeResponse) 
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	printf("nodetoken: %s\n", tptz__GetNode->NodeToken);


	tptz__GetNodeResponse =(struct _tptz__GetNodeResponse *)soap_malloc(soap, sizeof(struct _tptz__GetNodeResponse));
	memset(tptz__GetNodeResponse, 0, sizeof(struct _tptz__GetNodeResponse));

	tptz__GetNodeResponse->PTZNode =(struct tt__PTZNode *)soap_malloc(soap, sizeof(struct tt__PTZNode));
	memset(tptz__GetNodeResponse->PTZNode, 0, sizeof(struct tt__PTZNode));

	GetPTZNode(soap, tptz__GetNodeResponse->PTZNode);

	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SetConfiguration(struct soap* soap, struct _tptz__SetConfiguration *tptz__SetConfiguration, struct _tptz__SetConfigurationResponse *tptz__SetConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetConfigurationOptions
	(
	struct soap* soap
	, struct _tptz__GetConfigurationOptions *tptz__GetConfigurationOptions
	, struct _tptz__GetConfigurationOptionsResponse *tptz__GetConfigurationOptionsResponse
	)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ 
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GotoHomePosition
	( struct soap* soap
	, struct _tptz__GotoHomePosition *tptz__GotoHomePosition
	, struct _tptz__GotoHomePositionResponse *tptz__GotoHomePositionResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/

	if (NULL == tptz__GotoHomePosition)
	{
		return SOAP_ERR;
	}

	//todo: speed?
	
	PTZControl(PTZ_GOTO_HOMEPOSITION, NULL);
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__SetHomePosition(struct soap* soap, struct _tptz__SetHomePosition *tptz__SetHomePosition, struct _tptz__SetHomePositionResponse *tptz__SetHomePositionResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__ContinuousMove
	(
	  struct soap* soap
	, struct _tptz__ContinuousMove *tptz__ContinuousMove
	, struct _tptz__ContinuousMoveResponse *tptz__ContinuousMoveResponse
	) 
{
	if (NULL == tptz__ContinuousMove)
	{
		return SOAP_ERR;
	}

	tptz__ContinuousMoveResponse = NULL;

	ONVIF_PTZ_PARAM sParam;
	memset(&sParam, 0, sizeof(ONVIF_PTZ_PARAM));
	
	if (0 != GetPTZParam(&sParam) || !sParam.fInited)
	{
		return SOAP_ERR;
	}	
	
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/
	printf("domeptz Token = %s\n",  tptz__ContinuousMove->ProfileToken);
	if (tptz__ContinuousMove->Timeout)
	{
		printf("domeptz Timeout = %lld:\n",  *tptz__ContinuousMove->Timeout);
	}

	int panspeed = 0;
	int tiltspeed = 0;
	int zoomspeed = 0;
	
	if (NULL != tptz__ContinuousMove->Velocity
		&& NULL != tptz__ContinuousMove->Velocity->PanTilt)
	{
		panspeed = sParam.iContinuousPSpeedMax * tptz__ContinuousMove->Velocity->PanTilt->x;
		tiltspeed = sParam.iContinuousTSpeedMax * tptz__ContinuousMove->Velocity->PanTilt->y;
	}

	if (NULL != tptz__ContinuousMove->Velocity
		&& NULL != tptz__ContinuousMove->Velocity->Zoom)
	{
		zoomspeed = sParam.iContinuousZSpeedMax * tptz__ContinuousMove->Velocity->Zoom->x;
	}

	printf("domeptz (%d, %d, %d)\n", panspeed, tiltspeed, zoomspeed);

	int arg[3] = {0};
	if (panspeed > 0)
	{
		if (tiltspeed > 0)
		{
			arg[0] = 3;
			arg[1] = panspeed;
			arg[2] = tiltspeed;
			PTZControl(PTZ_PANTILT, arg);
		}
		else if (tiltspeed < 0)
		{
			arg[0] = 2;
			arg[1] = panspeed;
			arg[2] = -tiltspeed;
			PTZControl(PTZ_PANTILT, arg);
		}
		else
		{
			arg[0] = 0;
			arg[1] = panspeed;
			PTZControl(PTZ_PAN, arg);
		}
	}
	else if (panspeed < 0)
	{
		if (tiltspeed > 0)
		{
			arg[0] = 1;
			arg[1] = -panspeed;
			arg[2] = tiltspeed;
			PTZControl(PTZ_PANTILT, arg);
		}
		else if (tiltspeed < 0)
		{
			arg[0] = 0;
			arg[1] = -panspeed;
			arg[2] = -tiltspeed;
			PTZControl(PTZ_PANTILT, arg);
		}
		else
		{
			arg[0] = 1;
			arg[1] = -panspeed;
			PTZControl(PTZ_PAN, arg);
		}
	}
	else 
	{
		if (tiltspeed > 0)
		{
			arg[0] = 1;
			arg[1] = tiltspeed;
			PTZControl(PTZ_TILT, arg);
		}
		else if (tiltspeed < 0)
		{
			arg[0] = 0;
			arg[1] = -tiltspeed;
			PTZControl(PTZ_TILT, arg);
		}
		else
		{
			//do nothing
		}
	}

	if (zoomspeed != 0)
	{
		arg[0] = zoomspeed > 0 ? 0 : 1;
		arg[1] = abs(zoomspeed);
		PTZControl(PTZ_ZOOM, arg);
	}
	
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__RelativeMove
	( struct soap* soap
	, struct _tptz__RelativeMove *tptz__RelativeMove
	, struct _tptz__RelativeMoveResponse *tptz__RelativeMoveResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/

	if (NULL == tptz__RelativeMove)
	{
		return SOAP_ERR;
	}
/*
	ONVIF_PTZ_PARAM sParam;
	memset(&sParam, 0, sizeof(ONVIF_PTZ_PARAM));
	
	if (0 != GetPTZParam(&sParam) || !sParam.fInited)
	{
		return SOAP_ERR;
	}

	int arg[3] = {0};
	
	//todo: speed
	if (NULL != tptz__RelativeMove->Translation)
	{	
		if (NULL != tptz__RelativeMove->Translation->PanTilt)
		{
			printf("domeptz panpos = %f\n", tptz__RelativeMove->Translation->PanTilt->x);
			printf("domeptz tiltpos = %f\n", tptz__RelativeMove->Translation->PanTilt->y);

			arg[0] = sParam.iAbsolutePRangeMax * tptz__RelativeMove->Translation->PanTilt->x;
			arg[1] = sParam.iAbsoluteTRangeMax * tptz__RelativeMove->Translation->PanTilt->y;
			PTZControl(PTZ_SET_PANTILT, arg);
		}

		if (NULL != tptz__RelativeMove->Translation->Zoom)
		{
			printf("domeptz Zoompos = %f\n", tptz__RelativeMove->Translation->Zoom->x);

			arg[0] = sParam.iAbsoluteZRangeMax * tptz__RelativeMove->Translation->Zoom->x;
			PTZControl(PTZ_SET_ZOOM, arg);
		}
	}
*/
	return SOAP_OK;
}


SOAP_FMAC5 int SOAP_FMAC6 __tptz__SendAuxiliaryCommand(struct soap* soap, struct _tptz__SendAuxiliaryCommand *tptz__SendAuxiliaryCommand, struct _tptz__SendAuxiliaryCommandResponse *tptz__SendAuxiliaryCommandResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__AbsoluteMove
	( struct soap* soap
	, struct _tptz__AbsoluteMove *tptz__AbsoluteMove
	, struct _tptz__AbsoluteMoveResponse *tptz__AbsoluteMoveResponse)
{
	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/

	if (NULL == tptz__AbsoluteMove)
	{
		return SOAP_ERR;
	}

	ONVIF_PTZ_PARAM sParam;
	memset(&sParam, 0, sizeof(ONVIF_PTZ_PARAM));
	
	if (0 != GetPTZParam(&sParam) || !sParam.fInited)
	{
		return SOAP_ERR;
	}

	int arg[3] = {0};

	//todo: speed
	if (NULL != tptz__AbsoluteMove->Position)
	{	
		if (NULL != tptz__AbsoluteMove->Position->PanTilt)
		{
			printf("domeptz panpos = %f\n", tptz__AbsoluteMove->Position->PanTilt->x);
			printf("domeptz tiltpos = %f\n", tptz__AbsoluteMove->Position->PanTilt->y);

			arg[0] = sParam.iAbsolutePRangeMax * tptz__AbsoluteMove->Position->PanTilt->x;
			arg[1] = sParam.iAbsoluteTRangeMax * tptz__AbsoluteMove->Position->PanTilt->y;
			PTZControl(PTZ_SET_PANTILT, arg);
		}

		if (NULL != tptz__AbsoluteMove->Position->Zoom)
		{
			printf("domeptz Zoompos = %f\n", tptz__AbsoluteMove->Position->Zoom->x);

			arg[0] = sParam.iAbsoluteZRangeMax * tptz__AbsoluteMove->Position->Zoom->x;
			PTZControl(PTZ_SET_ZOOM, arg);
		}
	}

	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__Stop
	(struct soap* soap
	, struct _tptz__Stop *tptz__Stop
	, struct _tptz__StopResponse *tptz__StopResponse) 
{
	int fStopAll = 1;

	/*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/

	if (NULL == tptz__Stop)
	{
		return SOAP_ERR;
	}

	if (NULL != tptz__Stop->ProfileToken)
	{
		printf("domeptz token = %s:\n",  tptz__Stop->ProfileToken);
	}

	
	if (NULL != tptz__Stop->PanTilt)
	{
		fStopAll = 0;
		if (xsd__boolean__true_ == *tptz__Stop->PanTilt)
		{
			PTZControl(PTZ_STOP_PAN, NULL);
			PTZControl(PTZ_STOP_TILT, NULL);
			PTZControl(PTZ_STOP_PANTILT, NULL);
		}
	}

	if (NULL != tptz__Stop->Zoom)
	{
		fStopAll = 0;
		if (xsd__boolean__true_ == *tptz__Stop->Zoom)
		{
			PTZControl(PTZ_STOP_ZOOM, NULL);
		}
	}

	if (fStopAll)
	{
		PTZControl(PTZ_STOP_PAN, NULL);
		PTZControl(PTZ_STOP_TILT, NULL);
		PTZControl(PTZ_STOP_PANTILT, NULL);
		PTZControl(PTZ_STOP_ZOOM, NULL);
	}
	
	
	return SOAP_OK; 
}

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresetTours(struct soap* soap, struct _tptz__GetPresetTours *tptz__GetPresetTours, struct _tptz__GetPresetToursResponse *tptz__GetPresetToursResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresetTour(struct soap* soap, struct _tptz__GetPresetTour *tptz__GetPresetTour, struct _tptz__GetPresetTourResponse *tptz__GetPresetTourResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetPresetTourOptions(struct soap* soap, struct _tptz__GetPresetTourOptions *tptz__GetPresetTourOptions, struct _tptz__GetPresetTourOptionsResponse *tptz__GetPresetTourOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__CreatePresetTour(struct soap* soap, struct _tptz__CreatePresetTour *tptz__CreatePresetTour, struct _tptz__CreatePresetTourResponse *tptz__CreatePresetTourResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__ModifyPresetTour(struct soap* soap, struct _tptz__ModifyPresetTour *tptz__ModifyPresetTour, struct _tptz__ModifyPresetTourResponse *tptz__ModifyPresetTourResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__OperatePresetTour(struct soap* soap, struct _tptz__OperatePresetTour *tptz__OperatePresetTour, struct _tptz__OperatePresetTourResponse *tptz__OperatePresetTourResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__RemovePresetTour(struct soap* soap, struct _tptz__RemovePresetTour *tptz__RemovePresetTour, struct _tptz__RemovePresetTourResponse *tptz__RemovePresetTourResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tptz__GetCompatibleConfigurations(struct soap* soap, struct _tptz__GetCompatibleConfigurations *tptz__GetCompatibleConfigurations, struct _tptz__GetCompatibleConfigurationsResponse *tptz__GetCompatibleConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetServiceCapabilities(struct soap* soap, struct _trc__GetServiceCapabilities *trc__GetServiceCapabilities, struct _trc__GetServiceCapabilitiesResponse *trc__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateRecording(struct soap* soap, struct _trc__CreateRecording *trc__CreateRecording, struct _trc__CreateRecordingResponse *trc__CreateRecordingResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteRecording(struct soap* soap, struct _trc__DeleteRecording *trc__DeleteRecording, struct _trc__DeleteRecordingResponse *trc__DeleteRecordingResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordings(struct soap* soap, struct _trc__GetRecordings *trc__GetRecordings, struct _trc__GetRecordingsResponse *trc__GetRecordingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingConfiguration(struct soap* soap, struct _trc__SetRecordingConfiguration *trc__SetRecordingConfiguration, struct _trc__SetRecordingConfigurationResponse *trc__SetRecordingConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingConfiguration(struct soap* soap, struct _trc__GetRecordingConfiguration *trc__GetRecordingConfiguration, struct _trc__GetRecordingConfigurationResponse *trc__GetRecordingConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingOptions(struct soap* soap, struct _trc__GetRecordingOptions *trc__GetRecordingOptions, struct _trc__GetRecordingOptionsResponse *trc__GetRecordingOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateTrack(struct soap* soap, struct _trc__CreateTrack *trc__CreateTrack, struct _trc__CreateTrackResponse *trc__CreateTrackResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteTrack(struct soap* soap, struct _trc__DeleteTrack *trc__DeleteTrack, struct _trc__DeleteTrackResponse *trc__DeleteTrackResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetTrackConfiguration(struct soap* soap, struct _trc__GetTrackConfiguration *trc__GetTrackConfiguration, struct _trc__GetTrackConfigurationResponse *trc__GetTrackConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetTrackConfiguration(struct soap* soap, struct _trc__SetTrackConfiguration *trc__SetTrackConfiguration, struct _trc__SetTrackConfigurationResponse *trc__SetTrackConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__CreateRecordingJob(struct soap* soap, struct _trc__CreateRecordingJob *trc__CreateRecordingJob, struct _trc__CreateRecordingJobResponse *trc__CreateRecordingJobResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__DeleteRecordingJob(struct soap* soap, struct _trc__DeleteRecordingJob *trc__DeleteRecordingJob, struct _trc__DeleteRecordingJobResponse *trc__DeleteRecordingJobResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobs(struct soap* soap, struct _trc__GetRecordingJobs *trc__GetRecordingJobs, struct _trc__GetRecordingJobsResponse *trc__GetRecordingJobsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingJobConfiguration(struct soap* soap, struct _trc__SetRecordingJobConfiguration *trc__SetRecordingJobConfiguration, struct _trc__SetRecordingJobConfigurationResponse *trc__SetRecordingJobConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobConfiguration(struct soap* soap, struct _trc__GetRecordingJobConfiguration *trc__GetRecordingJobConfiguration, struct _trc__GetRecordingJobConfigurationResponse *trc__GetRecordingJobConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__SetRecordingJobMode(struct soap* soap, struct _trc__SetRecordingJobMode *trc__SetRecordingJobMode, struct _trc__SetRecordingJobModeResponse *trc__SetRecordingJobModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trc__GetRecordingJobState(struct soap* soap, struct _trc__GetRecordingJobState *trc__GetRecordingJobState, struct _trc__GetRecordingJobStateResponse *trc__GetRecordingJobStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetServiceCapabilities(struct soap* soap, struct _trp__GetServiceCapabilities *trp__GetServiceCapabilities, struct _trp__GetServiceCapabilitiesResponse *trp__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetReplayUri(struct soap* soap, struct _trp__GetReplayUri *trp__GetReplayUri, struct _trp__GetReplayUriResponse *trp__GetReplayUriResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trp__GetReplayConfiguration(struct soap* soap, struct _trp__GetReplayConfiguration *trp__GetReplayConfiguration, struct _trp__GetReplayConfigurationResponse *trp__GetReplayConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trp__SetReplayConfiguration(struct soap* soap, struct _trp__SetReplayConfiguration *trp__SetReplayConfiguration, struct _trp__SetReplayConfigurationResponse *trp__SetReplayConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetServiceCapabilities(struct soap* soap, struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities, struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSources(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateProfile(struct soap* soap, struct _trt__CreateProfile *trt__CreateProfile, struct _trt__CreateProfileResponse *trt__CreateProfileResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }






SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoEncoderConfiguration(struct soap* soap, struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration, struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoSourceConfiguration(struct soap* soap, struct _trt__AddVideoSourceConfiguration *trt__AddVideoSourceConfiguration, struct _trt__AddVideoSourceConfigurationResponse *trt__AddVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioEncoderConfiguration(struct soap* soap, struct _trt__AddAudioEncoderConfiguration *trt__AddAudioEncoderConfiguration, struct _trt__AddAudioEncoderConfigurationResponse *trt__AddAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioSourceConfiguration(struct soap* soap, struct _trt__AddAudioSourceConfiguration *trt__AddAudioSourceConfiguration, struct _trt__AddAudioSourceConfigurationResponse *trt__AddAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddPTZConfiguration(struct soap* soap, struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration, struct _trt__AddPTZConfigurationResponse *trt__AddPTZConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoAnalyticsConfiguration(struct soap* soap, struct _trt__AddVideoAnalyticsConfiguration *trt__AddVideoAnalyticsConfiguration, struct _trt__AddVideoAnalyticsConfigurationResponse *trt__AddVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddMetadataConfiguration(struct soap* soap, struct _trt__AddMetadataConfiguration *trt__AddMetadataConfiguration, struct _trt__AddMetadataConfigurationResponse *trt__AddMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioOutputConfiguration(struct soap* soap, struct _trt__AddAudioOutputConfiguration *trt__AddAudioOutputConfiguration, struct _trt__AddAudioOutputConfigurationResponse *trt__AddAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioDecoderConfiguration(struct soap* soap, struct _trt__AddAudioDecoderConfiguration *trt__AddAudioDecoderConfiguration, struct _trt__AddAudioDecoderConfigurationResponse *trt__AddAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoEncoderConfiguration(struct soap* soap, struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration, struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoSourceConfiguration(struct soap* soap, struct _trt__RemoveVideoSourceConfiguration *trt__RemoveVideoSourceConfiguration, struct _trt__RemoveVideoSourceConfigurationResponse *trt__RemoveVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioEncoderConfiguration(struct soap* soap, struct _trt__RemoveAudioEncoderConfiguration *trt__RemoveAudioEncoderConfiguration, struct _trt__RemoveAudioEncoderConfigurationResponse *trt__RemoveAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioSourceConfiguration(struct soap* soap, struct _trt__RemoveAudioSourceConfiguration *trt__RemoveAudioSourceConfiguration, struct _trt__RemoveAudioSourceConfigurationResponse *trt__RemoveAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemovePTZConfiguration(struct soap* soap, struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration, struct _trt__RemovePTZConfigurationResponse *trt__RemovePTZConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoAnalyticsConfiguration(struct soap* soap, struct _trt__RemoveVideoAnalyticsConfiguration *trt__RemoveVideoAnalyticsConfiguration, struct _trt__RemoveVideoAnalyticsConfigurationResponse *trt__RemoveVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveMetadataConfiguration(struct soap* soap, struct _trt__RemoveMetadataConfiguration *trt__RemoveMetadataConfiguration, struct _trt__RemoveMetadataConfigurationResponse *trt__RemoveMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioOutputConfiguration(struct soap* soap, struct _trt__RemoveAudioOutputConfiguration *trt__RemoveAudioOutputConfiguration, struct _trt__RemoveAudioOutputConfigurationResponse *trt__RemoveAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioDecoderConfiguration(struct soap* soap, struct _trt__RemoveAudioDecoderConfiguration *trt__RemoveAudioDecoderConfiguration, struct _trt__RemoveAudioDecoderConfigurationResponse *trt__RemoveAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__DeleteProfile(struct soap* soap, struct _trt__DeleteProfile *trt__DeleteProfile, struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurations(struct soap* soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *trt__GetAudioSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurations(struct soap* soap, struct _trt__GetAudioEncoderConfigurations *trt__GetAudioEncoderConfigurations, struct _trt__GetAudioEncoderConfigurationsResponse *trt__GetAudioEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetVideoAnalyticsConfigurations *trt__GetVideoAnalyticsConfigurations, struct _trt__GetVideoAnalyticsConfigurationsResponse *trt__GetVideoAnalyticsConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfigurations(struct soap* soap, struct _trt__GetMetadataConfigurations *trt__GetMetadataConfigurations, struct _trt__GetMetadataConfigurationsResponse *trt__GetMetadataConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfigurations(struct soap* soap, struct _trt__GetAudioOutputConfigurations *trt__GetAudioOutputConfigurations, struct _trt__GetAudioOutputConfigurationsResponse *trt__GetAudioOutputConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfigurations(struct soap* soap, struct _trt__GetAudioDecoderConfigurations *trt__GetAudioDecoderConfigurations, struct _trt__GetAudioDecoderConfigurationsResponse *trt__GetAudioDecoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfiguration(struct soap* soap, struct _trt__GetVideoSourceConfiguration *trt__GetVideoSourceConfiguration, struct _trt__GetVideoSourceConfigurationResponse *trt__GetVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfiguration(struct soap* soap, struct _trt__GetAudioSourceConfiguration *trt__GetAudioSourceConfiguration, struct _trt__GetAudioSourceConfigurationResponse *trt__GetAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfiguration(struct soap* soap, struct _trt__GetAudioEncoderConfiguration *trt__GetAudioEncoderConfiguration, struct _trt__GetAudioEncoderConfigurationResponse *trt__GetAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__GetVideoAnalyticsConfiguration *trt__GetVideoAnalyticsConfiguration, struct _trt__GetVideoAnalyticsConfigurationResponse *trt__GetVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfiguration(struct soap* soap, struct _trt__GetMetadataConfiguration *trt__GetMetadataConfiguration, struct _trt__GetMetadataConfigurationResponse *trt__GetMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfiguration(struct soap* soap, struct _trt__GetAudioOutputConfiguration *trt__GetAudioOutputConfiguration, struct _trt__GetAudioOutputConfigurationResponse *trt__GetAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfiguration(struct soap* soap, struct _trt__GetAudioDecoderConfiguration *trt__GetAudioDecoderConfiguration, struct _trt__GetAudioDecoderConfigurationResponse *trt__GetAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations, struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoSourceConfigurations *trt__GetCompatibleVideoSourceConfigurations, struct _trt__GetCompatibleVideoSourceConfigurationsResponse *trt__GetCompatibleVideoSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *trt__GetCompatibleAudioEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *trt__GetCompatibleAudioSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoAnalyticsConfigurations *trt__GetCompatibleVideoAnalyticsConfigurations, struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse *trt__GetCompatibleVideoAnalyticsConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleMetadataConfigurations(struct soap* soap, struct _trt__GetCompatibleMetadataConfigurations *trt__GetCompatibleMetadataConfigurations, struct _trt__GetCompatibleMetadataConfigurationsResponse *trt__GetCompatibleMetadataConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioOutputConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioOutputConfigurations *trt__GetCompatibleAudioOutputConfigurations, struct _trt__GetCompatibleAudioOutputConfigurationsResponse *trt__GetCompatibleAudioOutputConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioDecoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioDecoderConfigurations *trt__GetCompatibleAudioDecoderConfigurations, struct _trt__GetCompatibleAudioDecoderConfigurationsResponse *trt__GetCompatibleAudioDecoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceConfiguration(struct soap* soap, struct _trt__SetVideoSourceConfiguration *trt__SetVideoSourceConfiguration, struct _trt__SetVideoSourceConfigurationResponse *trt__SetVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoEncoderConfiguration(struct soap* soap, struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration, struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioSourceConfiguration(struct soap* soap, struct _trt__SetAudioSourceConfiguration *trt__SetAudioSourceConfiguration, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioEncoderConfiguration(struct soap* soap, struct _trt__SetAudioEncoderConfiguration *trt__SetAudioEncoderConfiguration, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__SetVideoAnalyticsConfiguration *trt__SetVideoAnalyticsConfiguration, struct _trt__SetVideoAnalyticsConfigurationResponse *trt__SetVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetMetadataConfiguration(struct soap* soap, struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration, struct _trt__SetMetadataConfigurationResponse *trt__SetMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioOutputConfiguration(struct soap* soap, struct _trt__SetAudioOutputConfiguration *trt__SetAudioOutputConfiguration, struct _trt__SetAudioOutputConfigurationResponse *trt__SetAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioDecoderConfiguration(struct soap* soap, struct _trt__SetAudioDecoderConfiguration *trt__SetAudioDecoderConfiguration, struct _trt__SetAudioDecoderConfigurationResponse *trt__SetAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurationOptions(struct soap* soap, struct _trt__GetVideoSourceConfigurationOptions *trt__GetVideoSourceConfigurationOptions, struct _trt__GetVideoSourceConfigurationOptionsResponse *trt__GetVideoSourceConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurationOptions(struct soap* soap, struct _trt__GetAudioSourceConfigurationOptions *trt__GetAudioSourceConfigurationOptions, struct _trt__GetAudioSourceConfigurationOptionsResponse *trt__GetAudioSourceConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioEncoderConfigurationOptions *trt__GetAudioEncoderConfigurationOptions, struct _trt__GetAudioEncoderConfigurationOptionsResponse *trt__GetAudioEncoderConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfigurationOptions(struct soap* soap, struct _trt__GetMetadataConfigurationOptions *trt__GetMetadataConfigurationOptions, struct _trt__GetMetadataConfigurationOptionsResponse *trt__GetMetadataConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfigurationOptions(struct soap* soap, struct _trt__GetAudioOutputConfigurationOptions *trt__GetAudioOutputConfigurationOptions, struct _trt__GetAudioOutputConfigurationOptionsResponse *trt__GetAudioOutputConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioDecoderConfigurationOptions *trt__GetAudioDecoderConfigurationOptions, struct _trt__GetAudioDecoderConfigurationOptionsResponse *trt__GetAudioDecoderConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetGuaranteedNumberOfVideoEncoderInstances(struct soap* soap, struct _trt__GetGuaranteedNumberOfVideoEncoderInstances *trt__GetGuaranteedNumberOfVideoEncoderInstances, struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse *trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__StartMulticastStreaming(struct soap* soap, struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming, struct _trt__StartMulticastStreamingResponse *trt__StartMulticastStreamingResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__StopMulticastStreaming(struct soap* soap, struct _trt__StopMulticastStreaming *trt__StopMulticastStreaming, struct _trt__StopMulticastStreamingResponse *trt__StopMulticastStreamingResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetSynchronizationPoint(struct soap* soap, struct _trt__SetSynchronizationPoint *trt__SetSynchronizationPoint, struct _trt__SetSynchronizationPointResponse *trt__SetSynchronizationPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetSnapshotUri(struct soap* soap, struct _trt__GetSnapshotUri *trt__GetSnapshotUri, struct _trt__GetSnapshotUriResponse *trt__GetSnapshotUriResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceModes(struct soap* soap, struct _trt__GetVideoSourceModes *trt__GetVideoSourceModes, struct _trt__GetVideoSourceModesResponse *trt__GetVideoSourceModesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceMode(struct soap* soap, struct _trt__SetVideoSourceMode *trt__SetVideoSourceMode, struct _trt__SetVideoSourceModeResponse *trt__SetVideoSourceModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSDs(struct soap* soap, struct _trt__GetOSDs *trt__GetOSDs, struct _trt__GetOSDsResponse *trt__GetOSDsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSD(struct soap* soap, struct _trt__GetOSD *trt__GetOSD, struct _trt__GetOSDResponse *trt__GetOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSDOptions(struct soap* soap, struct _trt__GetOSDOptions *trt__GetOSDOptions, struct _trt__GetOSDOptionsResponse *trt__GetOSDOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetOSD(struct soap* soap, struct _trt__SetOSD *trt__SetOSD, struct _trt__SetOSDResponse *trt__SetOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateOSD(struct soap* soap, struct _trt__CreateOSD *trt__CreateOSD, struct _trt__CreateOSDResponse *trt__CreateOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__DeleteOSD(struct soap* soap, struct _trt__DeleteOSD *trt__DeleteOSD, struct _trt__DeleteOSDResponse *trt__DeleteOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetServiceCapabilities_(struct soap* soap, struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities, struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSources_(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSources_(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputs_(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateProfile_(struct soap* soap, struct _trt__CreateProfile *trt__CreateProfile, struct _trt__CreateProfileResponse *trt__CreateProfileResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfile_(struct soap* soap, struct _trt__GetProfile *trt__GetProfile, struct _trt__GetProfileResponse *trt__GetProfileResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfiles_(struct soap* soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *trt__GetProfilesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoEncoderConfiguration_(struct soap* soap, struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration, struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoSourceConfiguration_(struct soap* soap, struct _trt__AddVideoSourceConfiguration *trt__AddVideoSourceConfiguration, struct _trt__AddVideoSourceConfigurationResponse *trt__AddVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioEncoderConfiguration_(struct soap* soap, struct _trt__AddAudioEncoderConfiguration *trt__AddAudioEncoderConfiguration, struct _trt__AddAudioEncoderConfigurationResponse *trt__AddAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioSourceConfiguration_(struct soap* soap, struct _trt__AddAudioSourceConfiguration *trt__AddAudioSourceConfiguration, struct _trt__AddAudioSourceConfigurationResponse *trt__AddAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddPTZConfiguration_(struct soap* soap, struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration, struct _trt__AddPTZConfigurationResponse *trt__AddPTZConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoAnalyticsConfiguration_(struct soap* soap, struct _trt__AddVideoAnalyticsConfiguration *trt__AddVideoAnalyticsConfiguration, struct _trt__AddVideoAnalyticsConfigurationResponse *trt__AddVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddMetadataConfiguration_(struct soap* soap, struct _trt__AddMetadataConfiguration *trt__AddMetadataConfiguration, struct _trt__AddMetadataConfigurationResponse *trt__AddMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioOutputConfiguration_(struct soap* soap, struct _trt__AddAudioOutputConfiguration *trt__AddAudioOutputConfiguration, struct _trt__AddAudioOutputConfigurationResponse *trt__AddAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioDecoderConfiguration_(struct soap* soap, struct _trt__AddAudioDecoderConfiguration *trt__AddAudioDecoderConfiguration, struct _trt__AddAudioDecoderConfigurationResponse *trt__AddAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoEncoderConfiguration_(struct soap* soap, struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration, struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoSourceConfiguration_(struct soap* soap, struct _trt__RemoveVideoSourceConfiguration *trt__RemoveVideoSourceConfiguration, struct _trt__RemoveVideoSourceConfigurationResponse *trt__RemoveVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioEncoderConfiguration_(struct soap* soap, struct _trt__RemoveAudioEncoderConfiguration *trt__RemoveAudioEncoderConfiguration, struct _trt__RemoveAudioEncoderConfigurationResponse *trt__RemoveAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioSourceConfiguration_(struct soap* soap, struct _trt__RemoveAudioSourceConfiguration *trt__RemoveAudioSourceConfiguration, struct _trt__RemoveAudioSourceConfigurationResponse *trt__RemoveAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemovePTZConfiguration_(struct soap* soap, struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration, struct _trt__RemovePTZConfigurationResponse *trt__RemovePTZConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoAnalyticsConfiguration_(struct soap* soap, struct _trt__RemoveVideoAnalyticsConfiguration *trt__RemoveVideoAnalyticsConfiguration, struct _trt__RemoveVideoAnalyticsConfigurationResponse *trt__RemoveVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveMetadataConfiguration_(struct soap* soap, struct _trt__RemoveMetadataConfiguration *trt__RemoveMetadataConfiguration, struct _trt__RemoveMetadataConfigurationResponse *trt__RemoveMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioOutputConfiguration_(struct soap* soap, struct _trt__RemoveAudioOutputConfiguration *trt__RemoveAudioOutputConfiguration, struct _trt__RemoveAudioOutputConfigurationResponse *trt__RemoveAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioDecoderConfiguration_(struct soap* soap, struct _trt__RemoveAudioDecoderConfiguration *trt__RemoveAudioDecoderConfiguration, struct _trt__RemoveAudioDecoderConfigurationResponse *trt__RemoveAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__DeleteProfile_(struct soap* soap, struct _trt__DeleteProfile *trt__DeleteProfile, struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurations_(struct soap* soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurations_(struct soap* soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurations_(struct soap* soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *trt__GetAudioSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurations_(struct soap* soap, struct _trt__GetAudioEncoderConfigurations *trt__GetAudioEncoderConfigurations, struct _trt__GetAudioEncoderConfigurationsResponse *trt__GetAudioEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoAnalyticsConfigurations_(struct soap* soap, struct _trt__GetVideoAnalyticsConfigurations *trt__GetVideoAnalyticsConfigurations, struct _trt__GetVideoAnalyticsConfigurationsResponse *trt__GetVideoAnalyticsConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfigurations_(struct soap* soap, struct _trt__GetMetadataConfigurations *trt__GetMetadataConfigurations, struct _trt__GetMetadataConfigurationsResponse *trt__GetMetadataConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfigurations_(struct soap* soap, struct _trt__GetAudioOutputConfigurations *trt__GetAudioOutputConfigurations, struct _trt__GetAudioOutputConfigurationsResponse *trt__GetAudioOutputConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfigurations_(struct soap* soap, struct _trt__GetAudioDecoderConfigurations *trt__GetAudioDecoderConfigurations, struct _trt__GetAudioDecoderConfigurationsResponse *trt__GetAudioDecoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfiguration_(struct soap* soap, struct _trt__GetVideoSourceConfiguration *trt__GetVideoSourceConfiguration, struct _trt__GetVideoSourceConfigurationResponse *trt__GetVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfiguration_(struct soap* soap, struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration, struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfiguration_(struct soap* soap, struct _trt__GetAudioSourceConfiguration *trt__GetAudioSourceConfiguration, struct _trt__GetAudioSourceConfigurationResponse *trt__GetAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfiguration_(struct soap* soap, struct _trt__GetAudioEncoderConfiguration *trt__GetAudioEncoderConfiguration, struct _trt__GetAudioEncoderConfigurationResponse *trt__GetAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoAnalyticsConfiguration_(struct soap* soap, struct _trt__GetVideoAnalyticsConfiguration *trt__GetVideoAnalyticsConfiguration, struct _trt__GetVideoAnalyticsConfigurationResponse *trt__GetVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfiguration_(struct soap* soap, struct _trt__GetMetadataConfiguration *trt__GetMetadataConfiguration, struct _trt__GetMetadataConfigurationResponse *trt__GetMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfiguration_(struct soap* soap, struct _trt__GetAudioOutputConfiguration *trt__GetAudioOutputConfiguration, struct _trt__GetAudioOutputConfigurationResponse *trt__GetAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfiguration_(struct soap* soap, struct _trt__GetAudioDecoderConfiguration *trt__GetAudioDecoderConfiguration, struct _trt__GetAudioDecoderConfigurationResponse *trt__GetAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoEncoderConfigurations_(struct soap* soap, struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations, struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoSourceConfigurations_(struct soap* soap, struct _trt__GetCompatibleVideoSourceConfigurations *trt__GetCompatibleVideoSourceConfigurations, struct _trt__GetCompatibleVideoSourceConfigurationsResponse *trt__GetCompatibleVideoSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioEncoderConfigurations_(struct soap* soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *trt__GetCompatibleAudioEncoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioSourceConfigurations_(struct soap* soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *trt__GetCompatibleAudioSourceConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoAnalyticsConfigurations_(struct soap* soap, struct _trt__GetCompatibleVideoAnalyticsConfigurations *trt__GetCompatibleVideoAnalyticsConfigurations, struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse *trt__GetCompatibleVideoAnalyticsConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleMetadataConfigurations_(struct soap* soap, struct _trt__GetCompatibleMetadataConfigurations *trt__GetCompatibleMetadataConfigurations, struct _trt__GetCompatibleMetadataConfigurationsResponse *trt__GetCompatibleMetadataConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioOutputConfigurations_(struct soap* soap, struct _trt__GetCompatibleAudioOutputConfigurations *trt__GetCompatibleAudioOutputConfigurations, struct _trt__GetCompatibleAudioOutputConfigurationsResponse *trt__GetCompatibleAudioOutputConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioDecoderConfigurations_(struct soap* soap, struct _trt__GetCompatibleAudioDecoderConfigurations *trt__GetCompatibleAudioDecoderConfigurations, struct _trt__GetCompatibleAudioDecoderConfigurationsResponse *trt__GetCompatibleAudioDecoderConfigurationsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceConfiguration_(struct soap* soap, struct _trt__SetVideoSourceConfiguration *trt__SetVideoSourceConfiguration, struct _trt__SetVideoSourceConfigurationResponse *trt__SetVideoSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoEncoderConfiguration_(struct soap* soap, struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration, struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioSourceConfiguration_(struct soap* soap, struct _trt__SetAudioSourceConfiguration *trt__SetAudioSourceConfiguration, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioEncoderConfiguration_(struct soap* soap, struct _trt__SetAudioEncoderConfiguration *trt__SetAudioEncoderConfiguration, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoAnalyticsConfiguration_(struct soap* soap, struct _trt__SetVideoAnalyticsConfiguration *trt__SetVideoAnalyticsConfiguration, struct _trt__SetVideoAnalyticsConfigurationResponse *trt__SetVideoAnalyticsConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetMetadataConfiguration_(struct soap* soap, struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration, struct _trt__SetMetadataConfigurationResponse *trt__SetMetadataConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioOutputConfiguration_(struct soap* soap, struct _trt__SetAudioOutputConfiguration *trt__SetAudioOutputConfiguration, struct _trt__SetAudioOutputConfigurationResponse *trt__SetAudioOutputConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioDecoderConfiguration_(struct soap* soap, struct _trt__SetAudioDecoderConfiguration *trt__SetAudioDecoderConfiguration, struct _trt__SetAudioDecoderConfigurationResponse *trt__SetAudioDecoderConfigurationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurationOptions_(struct soap* soap, struct _trt__GetVideoSourceConfigurationOptions *trt__GetVideoSourceConfigurationOptions, struct _trt__GetVideoSourceConfigurationOptionsResponse *trt__GetVideoSourceConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurationOptions_(struct soap* soap, struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions, struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurationOptions_(struct soap* soap, struct _trt__GetAudioSourceConfigurationOptions *trt__GetAudioSourceConfigurationOptions, struct _trt__GetAudioSourceConfigurationOptionsResponse *trt__GetAudioSourceConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurationOptions_(struct soap* soap, struct _trt__GetAudioEncoderConfigurationOptions *trt__GetAudioEncoderConfigurationOptions, struct _trt__GetAudioEncoderConfigurationOptionsResponse *trt__GetAudioEncoderConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfigurationOptions_(struct soap* soap, struct _trt__GetMetadataConfigurationOptions *trt__GetMetadataConfigurationOptions, struct _trt__GetMetadataConfigurationOptionsResponse *trt__GetMetadataConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfigurationOptions_(struct soap* soap, struct _trt__GetAudioOutputConfigurationOptions *trt__GetAudioOutputConfigurationOptions, struct _trt__GetAudioOutputConfigurationOptionsResponse *trt__GetAudioOutputConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfigurationOptions_(struct soap* soap, struct _trt__GetAudioDecoderConfigurationOptions *trt__GetAudioDecoderConfigurationOptions, struct _trt__GetAudioDecoderConfigurationOptionsResponse *trt__GetAudioDecoderConfigurationOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetGuaranteedNumberOfVideoEncoderInstances_(struct soap* soap, struct _trt__GetGuaranteedNumberOfVideoEncoderInstances *trt__GetGuaranteedNumberOfVideoEncoderInstances, struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse *trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetStreamUri_(struct soap* soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__StartMulticastStreaming_(struct soap* soap, struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming, struct _trt__StartMulticastStreamingResponse *trt__StartMulticastStreamingResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__StopMulticastStreaming_(struct soap* soap, struct _trt__StopMulticastStreaming *trt__StopMulticastStreaming, struct _trt__StopMulticastStreamingResponse *trt__StopMulticastStreamingResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetSynchronizationPoint_(struct soap* soap, struct _trt__SetSynchronizationPoint *trt__SetSynchronizationPoint, struct _trt__SetSynchronizationPointResponse *trt__SetSynchronizationPointResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetSnapshotUri_(struct soap* soap, struct _trt__GetSnapshotUri *trt__GetSnapshotUri, struct _trt__GetSnapshotUriResponse *trt__GetSnapshotUriResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceModes_(struct soap* soap, struct _trt__GetVideoSourceModes *trt__GetVideoSourceModes, struct _trt__GetVideoSourceModesResponse *trt__GetVideoSourceModesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceMode_(struct soap* soap, struct _trt__SetVideoSourceMode *trt__SetVideoSourceMode, struct _trt__SetVideoSourceModeResponse *trt__SetVideoSourceModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSDs_(struct soap* soap, struct _trt__GetOSDs *trt__GetOSDs, struct _trt__GetOSDsResponse *trt__GetOSDsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSD_(struct soap* soap, struct _trt__GetOSD *trt__GetOSD, struct _trt__GetOSDResponse *trt__GetOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSDOptions_(struct soap* soap, struct _trt__GetOSDOptions *trt__GetOSDOptions, struct _trt__GetOSDOptionsResponse *trt__GetOSDOptionsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__SetOSD_(struct soap* soap, struct _trt__SetOSD *trt__SetOSD, struct _trt__SetOSDResponse *trt__SetOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateOSD_(struct soap* soap, struct _trt__CreateOSD *trt__CreateOSD, struct _trt__CreateOSDResponse *trt__CreateOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trt__DeleteOSD_(struct soap* soap, struct _trt__DeleteOSD *trt__DeleteOSD, struct _trt__DeleteOSDResponse *trt__DeleteOSDResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetServiceCapabilities(struct soap* soap, struct _trv__GetServiceCapabilities *trv__GetServiceCapabilities, struct _trv__GetServiceCapabilitiesResponse *trv__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceivers(struct soap* soap, struct _trv__GetReceivers *trv__GetReceivers, struct _trv__GetReceiversResponse *trv__GetReceiversResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceiver(struct soap* soap, struct _trv__GetReceiver *trv__GetReceiver, struct _trv__GetReceiverResponse *trv__GetReceiverResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__CreateReceiver(struct soap* soap, struct _trv__CreateReceiver *trv__CreateReceiver, struct _trv__CreateReceiverResponse *trv__CreateReceiverResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__DeleteReceiver(struct soap* soap, struct _trv__DeleteReceiver *trv__DeleteReceiver, struct _trv__DeleteReceiverResponse *trv__DeleteReceiverResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__ConfigureReceiver(struct soap* soap, struct _trv__ConfigureReceiver *trv__ConfigureReceiver, struct _trv__ConfigureReceiverResponse *trv__ConfigureReceiverResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__SetReceiverMode(struct soap* soap, struct _trv__SetReceiverMode *trv__SetReceiverMode, struct _trv__SetReceiverModeResponse *trv__SetReceiverModeResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __trv__GetReceiverState(struct soap* soap, struct _trv__GetReceiverState *trv__GetReceiverState, struct _trv__GetReceiverStateResponse *trv__GetReceiverStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetServiceCapabilities(struct soap* soap, struct _tse__GetServiceCapabilities *tse__GetServiceCapabilities, struct _tse__GetServiceCapabilitiesResponse *tse__GetServiceCapabilitiesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingSummary(struct soap* soap, struct _tse__GetRecordingSummary *tse__GetRecordingSummary, struct _tse__GetRecordingSummaryResponse *tse__GetRecordingSummaryResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingInformation(struct soap* soap, struct _tse__GetRecordingInformation *tse__GetRecordingInformation, struct _tse__GetRecordingInformationResponse *tse__GetRecordingInformationResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetMediaAttributes(struct soap* soap, struct _tse__GetMediaAttributes *tse__GetMediaAttributes, struct _tse__GetMediaAttributesResponse *tse__GetMediaAttributesResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindRecordings(struct soap* soap, struct _tse__FindRecordings *tse__FindRecordings, struct _tse__FindRecordingsResponse *tse__FindRecordingsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetRecordingSearchResults(struct soap* soap, struct _tse__GetRecordingSearchResults *tse__GetRecordingSearchResults, struct _tse__GetRecordingSearchResultsResponse *tse__GetRecordingSearchResultsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindEvents(struct soap* soap, struct _tse__FindEvents *tse__FindEvents, struct _tse__FindEventsResponse *tse__FindEventsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetEventSearchResults(struct soap* soap, struct _tse__GetEventSearchResults *tse__GetEventSearchResults, struct _tse__GetEventSearchResultsResponse *tse__GetEventSearchResultsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindPTZPosition(struct soap* soap, struct _tse__FindPTZPosition *tse__FindPTZPosition, struct _tse__FindPTZPositionResponse *tse__FindPTZPositionResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetPTZPositionSearchResults(struct soap* soap, struct _tse__GetPTZPositionSearchResults *tse__GetPTZPositionSearchResults, struct _tse__GetPTZPositionSearchResultsResponse *tse__GetPTZPositionSearchResultsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetSearchState(struct soap* soap, struct _tse__GetSearchState *tse__GetSearchState, struct _tse__GetSearchStateResponse *tse__GetSearchStateResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__EndSearch(struct soap* soap, struct _tse__EndSearch *tse__EndSearch, struct _tse__EndSearchResponse *tse__EndSearchResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__FindMetadata(struct soap* soap, struct _tse__FindMetadata *tse__FindMetadata, struct _tse__FindMetadataResponse *tse__FindMetadataResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }

SOAP_FMAC5 int SOAP_FMAC6 __tse__GetMetadataSearchResults(struct soap* soap, struct _tse__GetMetadataSearchResults *tse__GetMetadataSearchResults, struct _tse__GetMetadataSearchResultsResponse *tse__GetMetadataSearchResultsResponse) { /*printf("[%d]%s:\n",  __LINE__,__FUNCTION__);*/ return SOAP_OK; }


