/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

#ifndef _HVSTREAM_H_
#define _HVSTREAM_H_

#include "hvutils.h"

#define HVRESULT HRESULT
#define IStream CHvStream

#ifndef min
#define min(a,b) (a<b?a:b)
#endif

#if (RUN_PLATFORM == PLATFORM_WINDOWS)
const UINT g_kcCopyBuf=2048;
#elif (RUN_PLATFORM == PLATFORM_LINUX)
const UINT g_kcCopyBuf=2048;
#else
const UINT g_kcCopyBuf=64;
#endif

#include "hvinterface.h"
using namespace HvCore;

/**
* @file		HvStream.h
* @brief	抽象基类CHvStream的定义
* @author	Shaorg
* @date		2010-7-28
*/

namespace HiVideo
{
	/*! @class	CHvStream
	* @brief	各种流类的基类
	*/
	class CHvStream
	{
	public:
		/**
		* @brief	初始化
		* @param	lpParam		与初始化相关的参数
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT Initialize(
			PVOID lpParam
			) = 0;

		/**
		* @brief	关闭流
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT Close(
			void
			) = 0;

		/**
		* @brief	读取流中的数据
		* @param	pv			数据输出缓冲区
		* @param	cb			指定读取的字节数
		* @param	pcbRead		实际读取的字节数
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT Read(
			PVOID pv,
			UINT cb,
			PUINT pcbRead
			) = 0;

		/**
		* @brief	写入数据到流中
		* @param	pv			数据输入缓冲区
		* @param	cb			指定写入的字节数
		* @param	pcbWritten	实际写入的字节数
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT Write(
			LPCVOID pv,
			UINT cb,
			PUINT pcbWritten
			) = 0;

		/**
		* @brief	重定位
		* @param	dlibMove		移动的距离
		* @param	ssOrigin		相对起始位置
		* @param	pnNewPosition	移动后的位置
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT Seek(
			INT dlibMove,
			STREAM_SEEK ssOrigin,
			PUINT pnNewPosition
			) = 0;

		/**
		* @brief	设置流的大小
		* @param	nNewSize	指定的新的尺寸
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT SetSize(
			UINT nNewSize
			) = 0;

		/**
		* @brief	获取流缓冲区的指针
		* @param	ppBuf		指向该流的缓冲区的指针
		* @param	pSize		该流的大小
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT GetBuf(
			BYTE8** ppBuf,
			DWORD32* pSize
			);

		/**
		* @brief	拷贝流
		* @param	pstm		目的流
		* @param	cb			要拷贝的数据大小
		* @param	pcbRead		实际从源流读取出的数据大小
		* @param	pcbWritten	实际写入到目的流的数据大小
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT CopyTo(
			CHvStream *pstm,
			UINT cb,
			PUINT pcbRead,
			PUINT pcbWritten
			) = 0;

		/**
		* @brief	获取流的CRC校验码
		* @param	pCrc		获取到的校验码
		* @param	pSize		实际进行校验的数据大小
		* @return	成功：S_OK，失败：E_FAIL
		*/
		virtual HRESULT GetVerifyInfo(
			DWORD32* pCrc,
			DWORD32* pSize
			) = 0;
	};
}

#endif
