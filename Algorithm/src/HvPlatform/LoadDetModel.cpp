#include "hvutils.h"

#include "detectormodel.h"

//信息
typedef struct _DET_MODEL_INFO
{
	DWORD32 dwType;
	const char* szName;
	DWORD32 dwFileID;
} DET_MODEL_INFO;

static DET_MODEL_INFO g_rgDetModelInfo[] =
{
	{ 0,	"BlueDetectorModel",		11		},
	{ 1,	"YellowDetectorModel",		12		},
	{ 2,	"DYellowDetectorModel",		13		},
	{ 3,	"WJDetectorModel",			14		},
	{ 4,	"FiveCharDetectorModel",	15		},
	{ 5,	"DBGreenDetectorModel",		16		},
	{ 6,	"CarDetectorModel",		   17		},
	{ 7,	"FaceDetectorModel",		18		}		// 新加入人脸检测模型	20110506
};

#define DET_MODEL_NUM		ARRSIZE(	g_rgDetModelInfo)	//检测模型的个数.

DET_MODEL_INFO* GetDetModelInfo( const char* szName )
{
	DET_MODEL_INFO* pModelInfo(NULL);

	for (int i = 0; i < DET_MODEL_NUM; i++)
	{
		if ( strcmp(szName, g_rgDetModelInfo[i].szName) == 0 )
		{
			pModelInfo = &g_rgDetModelInfo[i];
			break;
		}
	}

	return pModelInfo;
}

int GetDetFileID( const char* szName)
{
	DET_MODEL_INFO* pModelInfo = GetDetModelInfo( szName );

	return (pModelInfo == NULL)?-1:(int)pModelInfo->dwFileID;
}

HRESULT GetDetModelList(LPCSTR* rgName, DWORD32* pdwCount )
{
	if (pdwCount == NULL) return E_POINTER;

	if (rgName == NULL)
	{
		*pdwCount = DET_MODEL_NUM;
		return S_FALSE;
	}

	DWORD32 dwCount = 0;
	DWORD32 dwMaxCount = *pdwCount;

	for ( int i = 0; i < DET_MODEL_NUM && dwCount < dwMaxCount; i++)
	{
		rgName[dwCount++] = g_rgDetModelInfo[i].szName;
	}

	*pdwCount = dwCount;

	return S_OK;
}

////数据生成
//HRESULT GetDetData(
//			   int iModelType,
//			   BYTE8 *pbData,
//			   DWORD32* pdwDatLen
//			   )
//{
//	const _FeatureGroup *pGroupSrc;
//	const _Feature *pFeatureSrc;
//	const ModelInfo *pModelInfo;
//	BYTE8 *pbTemp;
//	DET_MODEL detModel;
//	DWORD32 dwDetFileSize = 0;
//	DWORD32 dwFeatureHeaderSize, dwGroupHeaderSize;
//	int iMaxLayer, iMaxFeature, i;
//	SDWORD32 sdwTemp;
//	BYTE8 bTemp;
//
//	if ( pbData == NULL ) return( -1 );
//	pbTemp = pbData;
//	if ( iModelType > DET_MODEL_NUM ) return( -2 );
//	pModelInfo = &originModel[ iModelType ];
//	iMaxLayer = pModelInfo->nMaxLayer;
//	iMaxFeature = pModelInfo->nMaxFeature;
//
//	dwFeatureHeaderSize = 6 * sizeof( BYTE8 ) + 2 * sizeof( DWORD32 );
//	dwGroupHeaderSize = 2 * sizeof( DWORD32 );
//	dwDetFileSize = sizeof( DET_MODEL ) + ( iMaxLayer + 1 ) * dwGroupHeaderSize + iMaxFeature * dwFeatureHeaderSize;
//
//	if (dwDetFileSize > *pdwDatLen) return E_FAIL;
//	*pdwDatLen = dwDetFileSize;
//
//	detModel.dwHeaderSize = sizeof( DET_MODEL );
//	detModel.dwDetModelFileSize = dwDetFileSize;
//	detModel.dwFeatureHeaderSize = 8;
//	detModel.dwGroupHeaderSize = 13;
//	detModel.dwLayerCount = iMaxLayer + 1;
//	detModel.dwFeatureCount = iMaxFeature;
//	detModel.dwGroupOffset = sizeof( DET_MODEL );
//	detModel.dwFeatureOffset = detModel.dwGroupOffset + ( iMaxLayer + 1 ) * dwGroupHeaderSize;
//	detModel.dwModelWidth = pModelInfo->nModelWidth;
//	detModel.dwModelHeight = pModelInfo->nModelHeight;
//	detModel.dwModelPlateType = pModelInfo->PlateType;
//	detModel.dwReserved[0] = 0;
//	detModel.dwReserved[1] = 0;
//	memcpy( pbTemp, ( BYTE8 * )&detModel, sizeof( DET_MODEL ) );
//	pbTemp += sizeof( DET_MODEL );
//
//	pGroupSrc = pModelInfo->pFeatureGroup;
//	for ( i = 0; i < ( iMaxLayer + 1 ); i ++ )
//	{
//		sdwTemp = pGroupSrc->dwCascade;
//		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
//		pbTemp += sizeof( SDWORD32 );
//		sdwTemp = ( SDWORD32 )( pGroupSrc->fltFinalThreshold * SaveCoef );
//		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
//		pbTemp += sizeof( SDWORD32 );
//		pGroupSrc ++;
//	}
//
//	pFeatureSrc = pModelInfo->pFeature;
//	for ( i = 0; i < iMaxFeature; i ++ )
//	{
//		sdwTemp = ( SDWORD32 )( pFeatureSrc->fltAlpha * SaveCoef );
//		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
//		pbTemp += sizeof( SDWORD32 );
//		sdwTemp = ( SDWORD32 )( pFeatureSrc->fltFinalH * SaveCoef );
//		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
//		pbTemp += sizeof( SDWORD32 );
//		if ( pFeatureSrc->chFinalSign > 0 )
//		{
//			bTemp = 1;
//		}
//		else
//		{
//			bTemp = 0;
//		}
//		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
//		pbTemp += sizeof( BYTE8 );
//		bTemp = pFeatureSrc->FinalType.bType;
//		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
//		pbTemp += sizeof( BYTE8 );
//		bTemp = pFeatureSrc->FinalType.bPosX;
//		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
//		pbTemp += sizeof( BYTE8 );
//		bTemp = pFeatureSrc->FinalType.bPosY;
//		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
//		pbTemp += sizeof( BYTE8 );
//		bTemp = pFeatureSrc->FinalType.bWidth;
//		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
//		pbTemp += sizeof( BYTE8 );
//		bTemp = pFeatureSrc->FinalType.bHeight;
//		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
//		pbTemp += sizeof( BYTE8 );
//		pFeatureSrc ++;
//	}
//
//	return S_OK;
//}

//数据生成
HRESULT GetDetData(
				   int iModelType,
				   BYTE8 *pbData,
				   DWORD32* pdwDatLen
				   )
{
	const _FeatureGroup *pGroupSrc;
	const _Feature *pFeatureSrc;
	const ModelInfo *pModelInfo;
	BYTE8 *pbTemp;
	DET_MODEL detModel;
	DWORD32 dwDetFileSize = 0;
	DWORD32 dwFeatureHeaderSize, dwGroupHeaderSize;
	int iMaxLayer, iMaxFeature, i;
	SDWORD32 sdwTemp;
	BYTE8 bTemp;

	if ( pbData == NULL ) return( -1 );
	pbTemp = pbData;
	if ( iModelType > DET_MODEL_NUM ) return( -2 );
	pModelInfo = &originModel[ iModelType ];
	iMaxLayer = pModelInfo->nMaxLayer;
	iMaxFeature = pModelInfo->nMaxFeature;

	dwFeatureHeaderSize = 6 * sizeof( BYTE8 ) + 3 * sizeof( DWORD32 );
	dwGroupHeaderSize = 2 * sizeof( DWORD32 );
	dwDetFileSize = sizeof( DET_MODEL ) + ( iMaxLayer + 1 ) * dwGroupHeaderSize + iMaxFeature * dwFeatureHeaderSize;

	if (dwDetFileSize > *pdwDatLen) return E_FAIL;
	*pdwDatLen = dwDetFileSize;

	detModel.dwHeaderSize = sizeof( DET_MODEL );
	detModel.dwDetModelFileSize = dwDetFileSize;
	detModel.dwFeatureHeaderSize = 9;
	detModel.dwGroupHeaderSize = 13;
	detModel.dwLayerCount = iMaxLayer + 1;
	detModel.dwFeatureCount = iMaxFeature;
	detModel.dwGroupOffset = sizeof( DET_MODEL );
	detModel.dwFeatureOffset = detModel.dwGroupOffset + ( iMaxLayer + 1 ) * dwGroupHeaderSize;
	detModel.dwModelWidth = pModelInfo->nModelWidth;
	detModel.dwModelHeight = pModelInfo->nModelHeight;
	detModel.dwModelPlateType = pModelInfo->PlateType;
	detModel.dwReserved[0] = 0;
	detModel.dwReserved[1] = 0;
	memcpy( pbTemp, ( BYTE8 * )&detModel, sizeof( DET_MODEL ) );
	pbTemp += sizeof( DET_MODEL );

	pGroupSrc = pModelInfo->pFeatureGroup;
	for ( i = 0; i < ( iMaxLayer + 1 ); i ++ )
	{
		sdwTemp = pGroupSrc->dwCascade;
		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
		pbTemp += sizeof( SDWORD32 );
		sdwTemp = ( SDWORD32 )( pGroupSrc->fltFinalThreshold * SaveCoef );
		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
		pbTemp += sizeof( SDWORD32 );
		pGroupSrc ++;
	}

	pFeatureSrc = pModelInfo->pFeature;
	for ( i = 0; i < iMaxFeature; i ++ )
	{
		sdwTemp = ( SDWORD32 )( pFeatureSrc->fltAlpha * SaveCoef );
		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
		pbTemp += sizeof( SDWORD32 );
		sdwTemp = ( SDWORD32 )( pFeatureSrc->fltFinalL * SaveCoef );
		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
		pbTemp += sizeof( SDWORD32 );
		sdwTemp = ( SDWORD32 )( pFeatureSrc->fltFinalH * SaveCoef );
		memcpy( pbTemp, ( BYTE8 * )&sdwTemp, sizeof( SDWORD32 ) );
		pbTemp += sizeof( SDWORD32 );
		if ( pFeatureSrc->chFinalSign > 0 )
		{
			bTemp = 1;
		}
		else
		{
			bTemp = 0;
		}
		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
		pbTemp += sizeof( BYTE8 );
		bTemp = pFeatureSrc->FinalType.bType;
		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
		pbTemp += sizeof( BYTE8 );
		bTemp = pFeatureSrc->FinalType.bPosX;
		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
		pbTemp += sizeof( BYTE8 );
		bTemp = pFeatureSrc->FinalType.bPosY;
		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
		pbTemp += sizeof( BYTE8 );
		bTemp = pFeatureSrc->FinalType.bWidth;
		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
		pbTemp += sizeof( BYTE8 );
		bTemp = pFeatureSrc->FinalType.bHeight;
		memcpy( pbTemp, &bTemp, sizeof( BYTE8 ) );
		pbTemp += sizeof( BYTE8 );
		pFeatureSrc ++;
	}

	return S_OK;
}

HRESULT LoadDetDat(
				   LPCSTR szName,
				   void* pDat,
				   DWORD32* pdwDatLen,
				   DWORD32 dwFlag = 0
				   )
{
	if (pDat == NULL || pdwDatLen == NULL) return E_POINTER;

	DET_MODEL_INFO* pModelInfo = GetDetModelInfo( szName );

	if (pModelInfo == NULL) return E_FAIL;

	return GetDetData( pModelInfo->dwType, (BYTE8*)pDat, pdwDatLen );
}

