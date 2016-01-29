/**
 * @file   drv_errno.h
 * @author lijj
 * @date   2013-02-28
 *
 * @brief  驱动层API错误码定义
 *
 */
#ifndef DRV_ERRNO_H
#define DRV_ERRNO_H

//返回值定义
///成功
#define DRV_OK          ( 0)

///失败
#define DRV_FAIL        (-1)

///非法参数
#define DRV_INVALIDARG  (-2)

///没有实现
#define DRV_NOTIMPL	    (-3)

///内存不足
#define DRV_OUTOFMEMORY (-4)

///没有初始化
#define DRV_NOTINITED   (-5)

#define DRV_TIMEOUT     (-6)

#endif // DRV_ERRNO_H
