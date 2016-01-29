/** ==================================================================
 *  @file   SW_Alg_Aewb.h
 *
 *	@date	2015.08.05
 *
 *  @author   lujy
 * ===================================================================*/

#ifndef _SW_ALG_AEWB_H_
#define _SW_ALG_AEWB_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*SW_2A_PLATPORM_PRINTF)(const char *string);

typedef void* (*SW_2A_PLATPORM_MEM_ALLOC)(int nSize);

typedef void (*SW_2A_PLATPORM_MEM_FREE)(void *Point);

typedef int (*SW_2A_PLATPORM_SPI_READ)(unsigned int addr,unsigned int *value);

typedef int  (*SW_2A_PLATPORM_SPI_WRITE)(unsigned int addr,unsigned int value);

typedef int  (*SW_2A_PLATPORM_SPI_WRITE_Mult)(unsigned int *addr,unsigned int *value,unsigned int count);

typedef void (*SW_2A_PLATPORM_SET_DENOISE_STATUS)(unsigned int Tnf_status,unsigned int Snf_status);

typedef void (*SW_2A_PLATPORM_SET_PWM_DC)(unsigned int duty,unsigned int ConstDuty);


typedef struct {
    SW_2A_PLATPORM_PRINTF Printf;

    SW_2A_PLATPORM_MEM_ALLOC Malloc;

    SW_2A_PLATPORM_MEM_FREE Free;

    SW_2A_PLATPORM_SPI_READ Spi_Read;

    SW_2A_PLATPORM_SPI_WRITE Spi_Write;
    
    SW_2A_PLATPORM_SPI_WRITE_Mult Spi_Write_Mult;

    SW_2A_PLATPORM_SET_DENOISE_STATUS Set_Denoise_status;

    SW_2A_PLATPORM_SET_PWM_DC Set_PWM_DC;    
}SW_2A_Platform_Operator;

typedef enum SW_2A_CMD_e
{
    //Set
    SW2A_CMD_SET_SHUTTER = 0,//设置快门
    SW2A_CMD_SET_GAIN,//设置增益
    SW2A_CMD_SET_RGB_GAIN,//设置RGB增益
    SW2A_CMD_SET_AE_STATE,//设置AE使能
    SW2A_CMD_SET_AE_THRESHOLD,//设置AE基准值
    SW2A_CMD_SET_AE_EXP_RANGE,//设置快门范围
    SW2A_CMD_SET_AE_ZONE,//设置测光区域
    SW2A_CMD_SET_AE_GAIN_RANGE,//设置增益范围
    SW2A_CMD_SET_SCENE,//设置场景模式
    SW2A_CMD_SET_AWB_STATE,//设置AWB使能
    SW2A_CMD_SET_WDR_STRENGHT,//设置wdr强度
    SW2A_CMD_SET_COLOR_ENABLE,//设置图像增强使能
    SW2A_CMD_SET_SATURATION,//设置饱和度
    SW2A_CMD_SET_CONTRAST,//设置对比度
    SW2A_CMD_SET_TRAFFIC_LIGHT_ENHANCE_REGION,//设置红灯加红区域
    SW2A_CMD_SET_TRAFFIC_LIGHT_LUM_TH,//设置红灯加红阈值
    SW2A_CMD_SET_GAMMA_ENABLE,//设置gamma使能
    SW2A_CMD_SET_GAMMA,//设置gamma值
    SW2A_CMD_SET_AUTO_DC_STATE,//使能自动dc光圈
    SW2A_CMD_SET_AC_SYNC_STATE,//电网同步使能
    SW2A_CMD_SET_AC_SYNC_PARAM,//电网同步延时
    SW2A_CMD_SET_IO_ARG,//抓拍参数
    SW2A_CMD_SET_EDGE_MOD,//触发沿
    SW2A_CMD_SET_GRAY_IMAGE_STATE,//黑白图
    SW2A_CMD_SET_WORK_MODE,//工作模式
    SW2A_CMD_SET_SOFT_CAPTURE,//软触发抓拍
    SW2A_CMD_SET_FLASH_GATE,//栏杆机控制
    SW2A_CMD_SET_CAPT_SHUTTER,//抓拍快门
    SW2A_CMD_SET_CAPT_GAIN,//抓拍增益
    SW2A_CMD_SET_CAPT_SHARPEN,//抓拍锐度
    SW2A_CMD_SET_CAPT_AWB_ENABLE,//抓拍自动白平衡使能
    SW2A_CMD_SET_CAPT_RGB,//抓拍RGB增益
    SW2A_CMD_SET_BACK_LIGHT,//背光
    SW2A_CMD_SET_AWB_MODE,//白平衡模式
    SW2A_CMD_SET_TNF_STATUS,//时域降噪开关
    SW2A_CMD_SET_SNF_STATUS,//空域降噪开关
	SW2A_CMD_SET_EDGE_ENHANCE,//边缘增强
	SW2A_CMD_SET_NIGHT_DELAY,//设置晚上状态延迟时间，816方案有效
	SW2A_CMD_SET_REG,//设置fpga寄存器-------[31 - 20]:addr,[19 - 0]:value
    //Get
    SW2A_CMD_GET_SHUTTER,
    SW2A_CMD_GET_GAIN,
    SW2A_CMD_GET_RGB_GAIN,
    SW2A_CMD_GET_AE_STATE,
    SW2A_CMD_GET_AE_THRESHOLD,
    SW2A_CMD_GET_AE_EXP_RANGE,
    SW2A_CMD_GET_AE_ZONE,
    SW2A_CMD_GET_AE_GAIN_RANGE,
    SW2A_CMD_GET_AWB_STATE,
    SW2A_CMD_GET_WDR_STRENGHT,
    SW2A_CMD_GET_COLOR_ENABLE,
    SW2A_CMD_GET_SATURATION,
    SW2A_CMD_GET_CONTRAST,
    SW2A_CMD_GET_GAMMA_ENABLE,
    SW2A_CMD_GET_GAMMA,
    SW2A_CMD_GET_AUTO_DC_STATE,
    SW2A_CMD_GET_AC_SYNC_STATE,
    SW2A_CMD_GET_AC_SYNC_PARAM,
    SW2A_CMD_GET_IO_ARG,
    SW2A_CMD_GET_EDGE_MOD,
    SW2A_CMD_GET_GRAY_IMAGE_STATE,
    SW2A_CMD_GET_DAY_LIGHT,
    SW2A_CMD_GET_EDGE_ENHANCE,
	SW2A_CMD_GET_REG,//读取fpga寄存器-------[31 - 20]:addr(传入),[31 - 0]:value(传出)
    SW2A_CMD_END
}SW_2A_CMD;
/* ===================================================================
 *  @func     SW_2A_Get_Version                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
char * SW_2A_Get_Version();

/* ===================================================================
 *  @func     SW_2A_Init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SW_2A_Init(SW_2A_Platform_Operator Operator);

/* ===================================================================
 *  @func     SW_2A_Ctrl                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SW_2A_Ctrl(unsigned cmd, void *data,int len);

/* ===================================================================
 *  @func     SW_2A_Process                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SW_2A_Process(void *pH3aMem);

/* ===================================================================
 *  @func     SW_2A_Release                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int SW_2A_Release(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // _SW_ALG_AEWB_H_
