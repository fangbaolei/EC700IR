#ifndef CSWIMAGESTATISTICSINFO_H
#define CSWIMAGESTATISTICSINFO_H
#include "SWObject.h"
#include "SWAutoLock.h"


struct STATISTICS_INFO;
/**
* 图像统计信息类
*/
class CSWImageStatisticsInfo : public CSWObject
{

CLASSINFO(CSWImageStatisticsInfo,CSWObject)

    enum{ MAX_IMAGEBLOCK_INFO = 16};
    // 统计图像块列表
    static BOOL *fpBlockList;
    // 图像块是否都为飞选中状态
    static BOOL fIsAllNoSelect;
public:
    static BOOL InputData( PVOID pData , INT nWidth , INT nHeight );
    /**
     * @brief 初始化对象
     * @return - 成功返回true,失败返回false
     */
    static BOOL Initialization( );
    /**
     * @brief 获取整张图片的平均亮度值,每一秒钟更新一次,且只有用户调用时才会更新
     * @return - 图像的平均亮度值,失败-1
     */
    static INT GetYArgValue(  );

    /**
     * @brief 获取整张图片的平均亮度值,每一秒钟更新一次,且只有用户调用时才会更新
     * @return - 图像的平均亮度值,如果平均亮度值没有更新则返回-1,
     * 注意:该方法只为AGC提供服务
     */
    static INT GetAGCYArgValue(  );
    /**
     * @brief 获取图片的部分平均亮度值，局部位置由外部指定,每一秒钟更新一次,且只有用户调用时才更新
     * @return - 图像的局部或整张图片的平均亮度值,失败返回-1
     */
    static INT GetPartYArgValue( );
    /**
     * @brief 获取最大图像块数
     * @return - 返回最大可统计的图像块数
     */
    static DWORD GetMaxImageBlock( );
    /**
     * @brief 设置需要统计Y值的图像块，图像统计方框默认都设为需统计状态,
     * bIsStatistics = true表示需要统计，否则为不统计该图像Y值
     * @param [in] bIsStatistics : false不需要统计，true需要统计该图像块Y值
     * @param [in] nIndex : 需要统计或不统计的图像块位置
     * @return - S_OK : 成功 - S_FALSE : 失败
     */
    static HRESULT SetImageListInfo( BOOL bIsStatistics ,  INT nIndex );

     /**
     * @brief 获取图片像素通道的统计值
     * @param [out] nRSum : R通道的统计总值
     * @param [out] nGSum : G通道的统计总值
     * @param [out] nBSum : B通道的统计总值
     * @return - S_OK : 成功 - S_FALSE : 失败
     */
     static HRESULT GetRGBSum( INT& nRSum , INT& nGSum , INT& nBSum );

    /**
     * @brief 获取差异统计值
     * @return - 为视频流差异因子
     */

    static VOID GetVedioDiffStatistValue( FLOAT& fDiffFactor , FLOAT& fConsisIndex);
     /**
     * @brief 获取差异统计值
     * @return - 为抓拍差异因子
     */
    static VOID GetCaptureDiffStatistValue( FLOAT& fDiffFactor , BOOL &IsUpdate );
private:
    /**
     * @brief 统计图像的平均亮度值
     * @param [in] pImageInfo : 需要输入的图像信息
     * @param [in] pBlockList : 需要统计的图像块列表
     * @param [in] IsAll : 是否需要统计所有图像块Y值，默认为需要统计
     * 如果只需要统计部分图像块的Y值信息，则该值为false
     * @return - 返回图像的平均亮度值，如果统计失败，则返回-1
     */
    static INT CalculateArg( CAPTURE_IMAGE_INFO_C *pImageInfo
                              , const INT *pBlockList , BOOL IsAll = true );


    /**
     * @brief 接受图像Y通道及C通道数据信息的回调函数
     * @param [in] img_info_y : 需要输入的图像Y通道信息
     * @param [in] img_info_c : 需要输入的图像C通道信息
     * @return - 图像的平均亮度值
     */
    static void ReceiverCallBack(CAPTURE_IMAGE_INFO_Y * img_info_y
                                 , CAPTURE_IMAGE_INFO_C * img_info_c,
                                 INT sum_y1, INT sum_y2, INT sum_y3 = 0, INT sum_y4 = 0,
                                 INT sum_y5 = 0, INT sum_y6 = 0, INT sum_y7 = 0, INT sum_y8 = 0);


private:
    // 图像数据更新互锁
    static CSWRWMutex m_fSWRWMutex;
    // 差异性统计信息互锁
    static CSWRWMutex m_fDiffSWRWMutex;

    // C通道数据
    static CAPTURE_IMAGE_INFO_C m_fImageInfoC;
    // Y通道数据
    static CAPTURE_IMAGE_INFO_Y m_fImageInfoY;
    // AB通道稳定陀螺效应因子
    static FLOAT m_fVedioConsisIndex;
    // 视频流差异因子
    static FLOAT m_fVedioDiffFactor;
    // 抓拍流差异因子
    static FLOAT m_fCaptureDiffFactor;
    static BOOL m_fCaptureUpdate ;

    // Y值数据更新标准
    static BOOL m_fImageAgrYUpdateFlag;

    // 当前计数
    // 差异校正要连续统计多帧取平均再做判断。
    static INT m_nCurrentCounters;
    static BOOL m_bIsOK;

    friend class STATISTICS_INFO;
};

#endif // CSWIMAGESTATISTICSINFO_H

