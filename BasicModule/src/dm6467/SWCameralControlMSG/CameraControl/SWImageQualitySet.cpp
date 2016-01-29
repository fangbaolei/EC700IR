#include "SWImageQualitySet.h"
#include "swpa_camera.h"

CSWImageQualitySet::CSWImageQualitySet()
: m_nMode( MODE_CLOSE )
, m_nSharpenEnMode( MODE_CLOSE )
, m_nHazeMode( MODE_CLOSE )
, m_nContrast( 0 )
, m_nSaturation( 0 )
, m_nTemperature( 0 )
, m_nHue( 0 )
, m_nHazeThreshold( 8 )
, m_nSharpenThreshold( 7 )
{
    //ctor
}

CSWImageQualitySet::~CSWImageQualitySet()
{
    //dtor
}

/**
* @brief 设置色彩参数\n
*
* @param [in] mode 工作模式 MODE_CLOSE-关闭色彩调整功能；MODE_CAPTURE-作用于抓拍帧；
* MODE_VIDEO_CAPUTRUE-作用于所有帧
* @note .
* @see .
*/
INT CSWImageQualitySet::SetColorParamMode( INT nMode )
{
    return swpa_camera_imgproc_set_color_param( (m_nMode = nMode), m_nContrast , m_nSaturation
                                                , m_nTemperature , m_nHue);
}
/**
* @brief 设置色彩参数\n
*
* @param [in] contrast_value 对比度值，范围：[-100~100]，
* 默认值：0(此值针对应用层，计算得到的因子是FPGA寄存器的默认值)
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
INT CSWImageQualitySet::SetImageContrast( INT nContrast)
{
    return swpa_camera_imgproc_set_color_param( m_nMode, (m_nContrast = nContrast) , m_nSaturation
                                                , m_nTemperature , m_nHue);
}
/**
* @brief 设置色彩参数\n
*
* @param [in] saturation_value 饱和度值，范围：[-100~100]，
* 默认值：0(此值针对应用层，计算得到的r/g/b分量是FPGA寄存器的默认值)
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
 INT CSWImageQualitySet::SetImageSaturation( INT nSaturation)
 {
    return swpa_camera_imgproc_set_color_param( m_nMode, m_nContrast , (m_nSaturation = nSaturation)
                                                , m_nTemperature , m_nHue);
 }
/**
* @brief 设置色彩参数\n
*
* @param [in] color_temperature_value 色温值
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
 INT CSWImageQualitySet::SetImageTemperature( INT nTemperature)
 {
    return swpa_camera_imgproc_set_color_param( m_nMode, m_nContrast ,m_nSaturation
                                                , (m_nTemperature = nTemperature) , m_nHue);
 }
/**
* @brief 设置色彩参数\n
*
* @param [in] hue_value 色度值（色彩、色相）
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note .
* @see .
*/
 INT CSWImageQualitySet::SetImageHue( INT nHue)
 {
    return swpa_camera_imgproc_set_color_param( m_nMode, m_nContrast ,m_nSaturation
                                                , m_nTemperature , (m_nHue = nHue));
 }

 // 获取相关参数
INT CSWImageQualitySet::GetColorParam( INT &nMode , INT &nContrast
                   ,  INT &nSaturation , INT &nTemperature , INT &nHue )
{
    nMode = m_nMode;
    nContrast = m_nContrast;
    nSaturation = m_nSaturation;
    nTemperature = m_nTemperature;
    nHue = m_nHue;

    return S_OK;
}

/**
* @brief 设置锐化相关参数\n
*
*
* @param [in] mode 工作模式 1-关闭锐化功能；2-作用于抓拍帧；3-作用于所有帧
* @param [in] threshold 阈值 范围0-127
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note
* @see .
*/
INT CSWImageQualitySet::SetSharpenParam( INT threshold )
{
    return swpa_camera_imgproc_set_sharpen_param( m_nSharpenEnMode , (m_nSharpenThreshold = threshold) );
}
INT CSWImageQualitySet::SetSharpenMode( INT nMode )
{
    return swpa_camera_imgproc_set_sharpen_param((m_nSharpenEnMode = nMode) , m_nSharpenThreshold );

}
/**
* @brief 获取锐化相关参数\n
*
*
* @param [out] mode 工作模式 1-关闭锐化功能；2-作用于抓拍帧；3-作用于所有帧
* @param [out] threshold 阈值 范围0-127
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note
* @see .
*/
INT CSWImageQualitySet::GetSharpenParam( INT &nMode , INT &threshold )
{
    return swpa_camera_imgproc_get_sharpen_param( &nMode , &threshold );
}

VOID CSWImageQualitySet::SetGamma( INT *pData )
{
    swpa_memcpy(m_arrGamma,pData,GAMMA_ROW*GAMMA_COL*sizeof(INT));
    swpa_camera_imgproc_set_gamma( m_arrGamma );
}
/**
* @brief 设置除雾相关参数\n
*
*
* @param [in] mode 工作模式 MODE_CLOSE-关闭锐化调整功能；MODE_CAPTURE-作用于抓拍帧；
* MODE_VIDEO_CAPUTRUE-作用于所有帧
* @retval 0 : 执行成功
* @retval -1 : 执行失败
* @note
* @see .
*/
INT CSWImageQualitySet::SetHazeRemovalMode( INT nMode )
{
    return swpa_camera_imgproc_set_defog_param((m_nHazeMode = nMode) , m_nHazeThreshold);
}
/**
* @brief 设置除雾相关参数\n
*
* @param [in] threshold 阈值 范围0-10
*/
INT CSWImageQualitySet::SetHazeRemovalParam( INT threshold )
{
    return swpa_camera_imgproc_set_defog_param( m_nHazeMode
                                               , (m_nHazeThreshold = threshold) );
}



