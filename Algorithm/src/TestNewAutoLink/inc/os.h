#ifndef _OS_H_
#define _OS_H_

#include "HvUtils.h"

namespace QTCPIP {
	//空指针定义
#ifndef NULL
	const int NULL = 0;
#endif
	//
	//定长数据类型定义
	typedef unsigned char BYTE;
	typedef unsigned short WORD;
	typedef unsigned int DWORD;
	//
	//最大缓存包
#ifdef CHIP_6455
	const int MAX_PACKET_COUNT = 2000;
#else
	const int MAX_PACKET_COUNT = 500;
#endif
	
	//预留给系统的缓存包，数据发送不能占用这个缓存，即数据发送能占的最大缓存是MAX_PACKET_COUNT-30
	const int RESERVE_PACKET_COUNT = 30;
	//
	//WORD交换字节序
	inline WORD* ReverseWORD( WORD *dest, const WORD *source ) {
		*dest = ( *source >> 8 ) | ( *source << 8 );
		return dest;
	}
	//
	//DWORD交换字节序
	inline WORD* ReverseDWORD( WORD *dest, const WORD *source ) {
		dest[ 0 ] = ( source[ 1 ] >> 8 ) | ( source[ 1 ] << 8 );
		dest[ 1 ] = ( source[ 0 ] >> 8 ) | ( source[ 0 ] << 8 );
		return dest;
	}
	//
	//复制DWORD
	inline WORD* CopyDWORD( WORD *dest, const WORD *source ) {
		_mem4( dest ) = _mem4_const( source );
		return dest;
	}
	//
	//复制以太网地址
	inline WORD* CopyEtherAddr( WORD *dest, const WORD *source ) {
		_mem4( dest ) = _mem4_const( source );
		_amem2( dest + 2 ) = _amem2_const( source + 2 );
		return dest;
	}
	//
	//比较DWORD
	inline bool EqualDWORD( const WORD *dest, const WORD *source ) {
		return _mem4_const( dest ) == _mem4_const( source );
	}

	inline BYTE *OS_memcpy( BYTE *restrict dest, const BYTE *restrict source, int count )
	{
		return ( BYTE * )std::memcpy( dest, source, count );		
	//以下的写法在Debug下不能正常运行.
	/*	
		BYTE *pbTemp = dest;
		int iCount2 = count >> 3;
		for ( int i = 0; i < iCount2; i++ )
		{
			_memd8( dest ) = _memd8_const( source );
			dest += 8;
			source += 8;
		}
		if ( count & 4 )
		{
			_mem4( dest ) = _mem4_const( source );
			dest += 4;
			source += 4;
		}
		if ( count & 2 )
		{
			_mem2( dest ) = _mem2_const( source );
			dest += 2;
			source += 2;
		}
		if ( count & 1 )
		{
			*dest = *source;
		}
		return pbTemp;
	*/
	}
	
	//发送信号量
	inline void PostSem( DWORD SemHandle ) {
		SEM_post( reinterpret_cast< SEM_Handle >( SemHandle ) );
	}
	//
	//等待信号量
	inline void PendSem( DWORD SemHandle ) {
		SEM_pend( reinterpret_cast< SEM_Handle >( SemHandle ), SYS_FOREVER );
	}
	//
	//取得系统时间(毫秒)
	inline DWORD GetMS() {
		return static_cast< DWORD >( PRD_getticks() );
	}
	//
	//取得时间间隔(毫秒)
	inline DWORD GetInterval( DWORD begin_time ) {
		return GetMS() - begin_time;
	}
	//
	//
	//进程休眠
	inline void Sleep( DWORD ms ) {
		TSK_sleep( ms );
	}
	//
	//取得一个随机数
	inline DWORD OS_Random() {
		return GetMS();
	}
	//
}

#endif
