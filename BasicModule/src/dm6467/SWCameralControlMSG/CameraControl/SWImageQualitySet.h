#ifndef CSWIMAGEQUALITYSET_H
#define CSWIMAGEQUALITYSET_H
#include "SWObject.h"


class CSWImageQualitySet: public CSWObject
{
CLASSINFO(CSWImageQualitySet,CSWObject)
    enum{ GAMMA_ROW = 8 , GAMMA_COL = 2};
    INT m_arrGamma[GAMMA_ROW][GAMMA_COL];

public:
    // MODE_CLOSE-关闭色彩调整功能；MODE_CAPTURE-作用于抓拍帧；MODE_VIDEO_CAPUTRUE-作用于所有帧
    enum{ MODE_CLOSE = 1, MODE_CAPTURE , MODE_VIDEO_CAPUTRUE};
public:
    CSWImageQualitySet();
    virtual ~CSWImageQualitySet();

    // 设置GAMMA值swpa_camera_imgproc_set_gamma
    VOID SetGamma( INT *pData );

    /**
    * @brief 设置色彩参数\n
    *
    * @param [in] mode 工作模式 MODE_CLOSE-关闭色彩调整功能；MODE_CAPTURE-作用于抓拍帧；
    * MODE_VIDEO_CAPUTRUE-作用于所有帧
    * @note .
    * @see .
    */
    INT SetColorParamMode( INT nMode );
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
    INT SetImageContrast( INT nContrast);
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
     INT SetImageSaturation( INT nSaturation);
    /**
    * @brief 设置色彩参数\n
    *
    * @param [in] color_temperature_value 色温值
    * @retval 0 : 执行成功
    * @retval -1 : 执行失败
    * @note .
    * @see .
    */
     INT SetImageTemperature( INT nTemperature);
    /**
    * @brief 设置色彩参数\n
    *
    * @param [in] hue_value 色度值（色彩、色相）
    * @retval 0 : 执行成功
    * @retval -1 : 执行失败
    * @note .
    * @see .
    */
     INT SetImageHue( INT nHue );
     // 获取相关参数
     INT GetColorParam( INT &nMode , INT &nContrast
                       ,  INT &nSaturation , INT &nTemperature , INT &nHue );

    /**
    * @brief 设置锐化工作模式
    *
    * @param [in] mode 工作模式 MODE_CLOSE-关闭锐化调整功能；MODE_CAPTURE-作用于抓拍帧；
    * MODE_VIDEO_CAPUTRUE-作用于所有帧
    * @note .
    * @see .
    */
    INT SetSharpenMode( INT nMode );

    /**
    * @brief 设置锐化相关参数\n
    *
    *
    * @param [in] mode 工作模式 1-关闭锐化功能；2-作用于抓拍帧；3-作用于所有帧
    * @param [in] threshold 阈值 范围1-3
    * @retval 0 : 执行成功
    * @retval -1 : 执行失败
    * @note
    * @see .
    */
    INT SetSharpenParam( INT threshold );
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
    INT GetSharpenParam( INT &nMode , INT &threshold );

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
    INT SetHazeRemovalMode( INT nMode );
    /**
    * @brief 设置除雾相关参数\n
    *
    * @param [in] threshold 阈值 范围0-10
    */
    INT SetHazeRemovalParam( INT threshold );

protected:

private:
    // 模式,默认值为MODE_VIDEO_CAPUTRUE
    INT m_nMode;
    // 锐化使能模式
    INT m_nSharpenEnMode;
    INT m_nSharpenThreshold;
    //除雾使能模式
    INT m_nHazeMode ;
    // 除雾阀值
    INT m_nHazeThreshold;
    // 对比度
    INT m_nContrast;
    // 饱和度
    INT m_nSaturation;
    // 色温值
    INT m_nTemperature;
    //色度值
    INT m_nHue;
};

#endif // CSWIMAGEQUALITYSET_H

