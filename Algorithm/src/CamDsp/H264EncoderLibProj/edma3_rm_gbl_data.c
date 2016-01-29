#include <edma3resmgr.h>

#define MAX_EDMA3_RM_INSTANCES (8u)

const unsigned int EDMA3_MAX_RM_INSTANCES = 8u;

EDMA3_RM_InstanceInitConfig userInstInitConfigArray[EDMA3_MAX_EDMA3_INSTANCES][MAX_EDMA3_RM_INSTANCES];
EDMA3_RM_Instance resMgrInstanceArray[EDMA3_MAX_EDMA3_INSTANCES][MAX_EDMA3_RM_INSTANCES];

EDMA3_RM_Instance *resMgrInstance = (EDMA3_RM_Instance *)resMgrInstanceArray;
EDMA3_RM_InstanceInitConfig *userInitConfig = (EDMA3_RM_InstanceInitConfig *)userInstInitConfigArray;

EDMA3_RM_Instance *ptrRMIArray = (EDMA3_RM_Instance *)resMgrInstanceArray;
EDMA3_RM_InstanceInitConfig *ptrInitCfgArray = (EDMA3_RM_InstanceInitConfig *)userInstInitConfigArray;
