
#include "swpa_mem.h"
#include "SWTask.h"
#include "SWImageStatisticsInfo.h"
#include "SWLog.h"

typedef struct STATISTICS_INFO{
    STATISTICS_INFO(){
        INT nSize = CSWImageStatisticsInfo::MAX_IMAGEBLOCK_INFO;
        for( int i = 0 ; i < nSize; i++)
        {
            CSWImageStatisticsInfo::fpBlockList[i] = true;
        }

        swpa_vpif_set_imginfo_callback( CSWImageStatisticsInfo::ReceiverCallBack);
    }
}STATISTICS_INFO;


BOOL *CSWImageStatisticsInfo::fpBlockList = new BOOL[ CSWImageStatisticsInfo::MAX_IMAGEBLOCK_INFO ];

// C通道数据
CAPTURE_IMAGE_INFO_C CSWImageStatisticsInfo::m_fImageInfoC;
// Y通道数据
CAPTURE_IMAGE_INFO_Y CSWImageStatisticsInfo::m_fImageInfoY;
// 数据读写互锁
CSWRWMutex CSWImageStatisticsInfo::m_fSWRWMutex;
// 差异性统计互锁
CSWRWMutex CSWImageStatisticsInfo::m_fDiffSWRWMutex;


FLOAT CSWImageStatisticsInfo::m_fVedioDiffFactor = 1.0;
FLOAT CSWImageStatisticsInfo::m_fCaptureDiffFactor = 1.0;

BOOL CSWImageStatisticsInfo::m_fCaptureUpdate = false;

BOOL CSWImageStatisticsInfo::fIsAllNoSelect = false;

// AB通道稳定陀螺效应因子
FLOAT CSWImageStatisticsInfo::m_fVedioConsisIndex = 0;

// 亮度值更新标志
BOOL CSWImageStatisticsInfo::m_fImageAgrYUpdateFlag = false;

INT CSWImageStatisticsInfo::m_nCurrentCounters = 0;

BOOL CSWImageStatisticsInfo::m_bIsOK = false;

/**
* 输入图像数据进行统计,该方式测试所用
*/
BOOL CSWImageStatisticsInfo::InputData( PVOID pData , INT nWidth , INT nHeight )
{
    void* pImageYUV422Data = pData;

    DWORD fYL , fYR ; fYL = fYR = 0 ;
    for( int i = 0 ; i < (nHeight - 1); i++)
    {
        BYTE *pYDataBuffer = (BYTE*)pImageYUV422Data + i*nWidth;

        fYL += (BYTE)(pYDataBuffer[nWidth / 2 - 1]);
        fYR += (BYTE)(pYDataBuffer[ nWidth / 2]);

        fYL += (BYTE)(pYDataBuffer[nWidth / 2 - 2]);
        fYR += (BYTE)(pYDataBuffer[nWidth / 2 + 1]);

        /*fYL += (BYTE)(pYDataBuffer[974 - 1]);
        fYR += (BYTE)(pYDataBuffer[ 974]);

        fYL += (BYTE)(pYDataBuffer[974 - 2]);
        fYR += (BYTE)(pYDataBuffer[974 + 1]);*/
    }

    CAPTURE_IMAGE_INFO_C stChannelInfo;

    INT *pAvgy = &stChannelInfo.avg_y_15;

    char *pAWBValueData = (char*)pData + nWidth*(2*nHeight - 1) + sizeof(INT)*25;
    INT *AWBR =  &stChannelInfo.sum_r;

    // 获取AWB统计信息
    for( int k = 0 ; k < 6; k++)
    {
        /*UINT uVlaue = ((pAWBValueData[k*2*4 ]) << 24);
        uVlaue |= ((pAWBValueData[k*2*4 + 1]) << 16);
        uVlaue |= ((pAWBValueData[k*2*4 + 2]) << 8);
        uVlaue |= (pAWBValueData[k*2*4 + 3]);*/

        UINT uVlaue = (pAWBValueData[ k*2*4 ]);
        uVlaue |= ((pAWBValueData[ k*2*4 + 1]) << 8 );
        uVlaue |= ((pAWBValueData[ k*2*4 + 2]) << 16);
        uVlaue |= ((pAWBValueData[ k*2*4 + 3]) << 24);

        *AWBR = uVlaue; AWBR += 2;

    }

    char *pYValueData = (char*)pData + nWidth*(2*nHeight - 1) + sizeof(INT)*32;

    DWORD dwSum = 0 ;

    // 获取AGC统计信息
    for( int j = 0 ; j < 16; j++)
    {
        UINT uValue = pYValueData[ j*2*4 ];
        *pAvgy = uValue; pAvgy += 2; dwSum += uValue;
    }

    char buffer[128];

    //
    ReceiverCallBack( NULL , &stChannelInfo , fYL , fYR);

}

/**
 * @brief 初始化对象
 * @return - 成功返回true,失败返回false
 */
BOOL CSWImageStatisticsInfo::Initialization( )
{
    INT nSize = CSWImageStatisticsInfo::MAX_IMAGEBLOCK_INFO;
    for( int i = 0 ; i < nSize; i++)
    {
        CSWImageStatisticsInfo::fpBlockList[i] = true;
    }

    swpa_vpif_set_imginfo_callback( CSWImageStatisticsInfo::ReceiverCallBack);
}

/**
 * @brief 接受图像Y通道及C通道数据信息的回调函数
 * @param [in] img_info_y : 需要输入的图像Y通道信息
 * @param [in] img_info_c : 需要输入的图像C通道信息
 * @return - 图像的平均亮度值
 */
void CSWImageStatisticsInfo::ReceiverCallBack(CAPTURE_IMAGE_INFO_Y * img_info_y
                                              , CAPTURE_IMAGE_INFO_C * img_info_c
                                              , INT sum_y1, INT sum_y2, INT sum_y3, INT sum_y4
                                              , INT sum_y5, INT sum_y6, INT sum_y7, INT sum_y8)
{


    CSWAutoLock objLock( &CSWImageStatisticsInfo::m_fSWRWMutex );

    static int siSumy3 = 0;
    static int siSumy4 = 0;
    static int siSumy5 = 0;
    static int siSumy6 = 0;
    static int siSumy7 = 0;

    if( img_info_y != NULL && img_info_y->capture_en == 0 )
    {
        if( img_info_y != NULL )
            swpa_memcpy( &CSWImageStatisticsInfo::m_fImageInfoY
                        , img_info_y ,sizeof( CAPTURE_IMAGE_INFO_Y));

        if( img_info_c != NULL ){
            swpa_memcpy( &CSWImageStatisticsInfo::m_fImageInfoC
                        , img_info_c ,sizeof( CAPTURE_IMAGE_INFO_C));

            CSWImageStatisticsInfo::m_fImageAgrYUpdateFlag = true;
        }
    }


    CSWAutoLock objDiffLock( &CSWImageStatisticsInfo::m_fDiffSWRWMutex );


    if( sum_y2 != 0 ){
        if( img_info_y != NULL && img_info_y->capture_en != 0 ){
            CSWImageStatisticsInfo::m_fCaptureDiffFactor = 1.0*sum_y1 / sum_y2;
            CSWImageStatisticsInfo::m_fCaptureUpdate = true;
        }
        else{


            // CSWImageStatisticsInfo::m_fVedioDiffFactor = 1.0*sum_y1 / sum_y2;
        	if( !CSWImageStatisticsInfo::m_bIsOK )
        	{
				siSumy3 += sum_y3;
				siSumy4 += sum_y4;
				siSumy5 += sum_y5;
				siSumy6 += sum_y6;
				siSumy7 += sum_y7;

				int iy1, iy2;
				if( sum_y4 > sum_y5 )
				{
				    iy1 = sum_y3;
				    iy2 = sum_y6;
				}
				else
				{
					iy1 = sum_y7;
					iy2 = sum_y8;
				}

				if( sum_y4 == 0 && sum_y5 == 0 )
				 {
					 iy1 = sum_y1;
					 iy2 = sum_y2;
				 }

				FLOAT fABChannelArg = 0.5*(iy1 + iy2) ;
				// 获取差异性分子
				FLOAT dbMolecular = swpa_sqrt(swpa_pow( iy1 - fABChannelArg , 2)
										 + swpa_pow( iy2 - fABChannelArg , 2));
				// AB通道稳定陀螺效应因子
				CSWImageStatisticsInfo::m_fVedioConsisIndex += dbMolecular / fABChannelArg *100;



				CSWImageStatisticsInfo::m_fVedioDiffFactor += 1.0*iy1 / iy2;


			   if( CSWImageStatisticsInfo::m_nCurrentCounters++ > 9){
				   CSWImageStatisticsInfo::m_fVedioDiffFactor = CSWImageStatisticsInfo::m_fVedioDiffFactor / CSWImageStatisticsInfo::m_nCurrentCounters;

				   CSWImageStatisticsInfo::m_fVedioConsisIndex = CSWImageStatisticsInfo::m_fVedioConsisIndex / CSWImageStatisticsInfo::m_nCurrentCounters;

				   SW_TRACE_DEBUG("<diff info>f:%f, c:%f. max:%d; min:%d.",
						   CSWImageStatisticsInfo::m_fVedioDiffFactor, CSWImageStatisticsInfo::m_fVedioConsisIndex,

						   siSumy4 / CSWImageStatisticsInfo::m_nCurrentCounters,

						   siSumy5 / CSWImageStatisticsInfo::m_nCurrentCounters

						   );

				   siSumy3 = siSumy4 = siSumy5 = siSumy6 = siSumy7 = 0;

				   CSWImageStatisticsInfo::m_bIsOK = true; CSWImageStatisticsInfo::m_nCurrentCounters = 0;


				}

        	}
        }
    }
}

/**
 * @brief 获取差异统计值
 * @return - 为视频流差异因子
 */
VOID CSWImageStatisticsInfo::GetVedioDiffStatistValue( FLOAT& fDiffFactor , FLOAT& fConsisIndex )
{
	if( CSWImageStatisticsInfo::m_bIsOK == false ){
		fDiffFactor = 1.0; fConsisIndex  = 0.0; return;
	}


    CSWAutoLock objDiffLock( &CSWImageStatisticsInfo::m_fDiffSWRWMutex );
    fDiffFactor = CSWImageStatisticsInfo::m_fVedioDiffFactor;
    CSWImageStatisticsInfo::m_fVedioDiffFactor = 0.0; CSWImageStatisticsInfo::m_bIsOK  = false;
    // AB通道稳定陀螺效应因子
    fConsisIndex = CSWImageStatisticsInfo::m_fVedioConsisIndex;
    CSWImageStatisticsInfo::m_fVedioConsisIndex = 0.0;
}
 /**
 * @brief 获取差异统计值
 * @return - 为抓拍差异因子
 */
VOID CSWImageStatisticsInfo::GetCaptureDiffStatistValue( FLOAT& fDiffFactor , BOOL &IsUpdate  )
{
     CSWAutoLock objDiffLock( &CSWImageStatisticsInfo::m_fDiffSWRWMutex );
     fDiffFactor = CSWImageStatisticsInfo::m_fCaptureDiffFactor;
     IsUpdate = CSWImageStatisticsInfo::m_fCaptureUpdate;
     CSWImageStatisticsInfo::m_fCaptureUpdate = false;
}

/**
 * @brief 获取整张图片的平均亮度值,每一秒钟更新一次,且只有用户调用时才会更新
 * @return - 图像的平均亮度值
 */
INT CSWImageStatisticsInfo::GetYArgValue( )
{
    CSWAutoLock objLock( &CSWImageStatisticsInfo::m_fSWRWMutex );
    // 获取图像的平均亮度
    return CSWImageStatisticsInfo::CalculateArg( &CSWImageStatisticsInfo::m_fImageInfoC
                                                , CSWImageStatisticsInfo::fpBlockList , true );
}
/**
 * @brief 获取整张图片的平均亮度值,每一秒钟更新一次,且只有用户调用时才会更新
 * @return - 图像的平均亮度值,如果平均亮度值没有更新则返回-1,
 * 注意:该方法只为AGC提供服务
 */
INT CSWImageStatisticsInfo::GetAGCYArgValue( )
{
    CSWAutoLock objLock( &CSWImageStatisticsInfo::m_fSWRWMutex );
    // 获取图像的平均亮度
    return CSWImageStatisticsInfo::CalculateArg( &CSWImageStatisticsInfo::m_fImageInfoC
                                                , fpBlockList , true );
}
/**
 * @brief 获取图片的部分平均亮度值，局部位置由外部指定,每一秒钟更新一次,且只有用户调用时才更新
 * @return - 图像的局部或整张图片的平均亮度值
 */
INT CSWImageStatisticsInfo::GetPartYArgValue( )
{
    CSWAutoLock objLock( &CSWImageStatisticsInfo::m_fSWRWMutex );
     // 获取图像的平均亮度
    return CSWImageStatisticsInfo::CalculateArg( &CSWImageStatisticsInfo::m_fImageInfoC
                                                , CSWImageStatisticsInfo::fpBlockList , false );

}
/**
 * @brief 获取最大图像块数
 * @return - 返回最大可统计的图像块数
 */
DWORD CSWImageStatisticsInfo::GetMaxImageBlock( )
{
    return MAX_IMAGEBLOCK_INFO;
}
/**
 * @brief 设置需要统计Y值的图像块，图像统计方框默认都设为需统计状态,
 * bIsStatistics = true表示需要统计，否则为不统计该图像Y值
 * @param [in] bIsStatistics : false不需要统计，true需要统计该图像块Y值
 * @param [in] nIndex : 需要统计或不统计的图像块位置
 * @return - S_OK : 成功 - S_FALSE : 失败
 */
HRESULT CSWImageStatisticsInfo::SetImageListInfo( BOOL bIsStatistics ,  INT nIndex )
{
    if( nIndex >= CSWImageStatisticsInfo::MAX_IMAGEBLOCK_INFO) return S_FALSE;

    CSWImageStatisticsInfo::fpBlockList[nIndex] = bIsStatistics;

    CSWImageStatisticsInfo::fIsAllNoSelect = true;
    for( int i = 0 ; i < CSWImageStatisticsInfo::MAX_IMAGEBLOCK_INFO; i++)
    {
        if( CSWImageStatisticsInfo::fpBlockList[i] ){
            CSWImageStatisticsInfo::fIsAllNoSelect = false; break;
        }
    }

    return S_OK;
}

/**ImageInfo , name
 * @brief 统计图像的平均亮度值
 * @param [in] pImageInfo : 需要输入的图像信息
 * @param [in] pBlockList : 需要统计的图像块列表
 * @param [in] IsAll : 是否需要统计所有图像块Y值，默认为需要统计
 * 如果只需要统计部分图像块的Y值信息，则该值为false
 * @return - 成功返回图像的平均亮度值,失败返回-1
 */
INT CSWImageStatisticsInfo::CalculateArg( CAPTURE_IMAGE_INFO_C *pImageInfo
                              , const INT *pBlockList , BOOL IsAll )
{
    if( CSWImageStatisticsInfo::fIsAllNoSelect && !IsAll ) return -2;
    if( CSWImageStatisticsInfo::m_fImageAgrYUpdateFlag == false) return -1;

    CSWImageStatisticsInfo::m_fImageAgrYUpdateFlag = false;

    if( pImageInfo == NULL ||  pBlockList == NULL ) return -1;

    INT nImageYArg = 0 ; INT nCalculateBlockCounts = 0;

    for( int i = 0; i < CSWImageStatisticsInfo::MAX_IMAGEBLOCK_INFO; i++)
    {
        if( pBlockList[i] || IsAll){
            nImageYArg +=  *((INT*)(&pImageInfo->avg_y_0) - i*2); nCalculateBlockCounts++;
        }

    }

    return (nCalculateBlockCounts == 0 ? -1 : (0.5 + 1.0*nImageYArg / nCalculateBlockCounts));
}
 /**
 * @brief 获取图片像素通道的统计值
 * @param [out] nRSum : R通道的统计总值
 * @param [out] nGSum : G通道的统计总值
 * @param [out] nBSum : B通道的统计总值
 * @return - S_OK : 成功 - S_FALSE : 失败
 */
 HRESULT CSWImageStatisticsInfo::GetRGBSum( INT& nRSum , INT& nGSum , INT& nBSum )
 {
    CSWAutoLock objLock( &CSWImageStatisticsInfo::m_fSWRWMutex );
    nRSum = CSWImageStatisticsInfo::m_fImageInfoC.sum_r;
    nGSum = CSWImageStatisticsInfo::m_fImageInfoC.sum_g;
    nBSum = CSWImageStatisticsInfo::m_fImageInfoC.sum_b;

    CSWImageStatisticsInfo::m_fImageInfoC.sum_r = CSWImageStatisticsInfo::m_fImageInfoC.sum_g = CSWImageStatisticsInfo::m_fImageInfoC.sum_b = 0;

    return S_OK;
 }



