/// @file
/// @brief 内存操作
/// @author liaoy
/// @date 2013/8/26 10:46:40
///
/// 修改说明:
/// [2013/8/26 10:46:40 liaoy] 最初版本

#pragma once

#include "sv_basetype.h"
#include "sv_error.h"
#include "string.h"

namespace sv
{
    /// 申请内存
    /// @note 如果片内申请不到,会自动在片外申请
    void* memAlloc(
        int nSize,                      ///< 申请长度
        SV_BOOL fFastMem = FALSE        ///< 是否在片内申请,默认为FALSE
    );

    /// 释放内存
    void memFree(
        void* pAddr,                    ///< 要释放的地址
        int nSize,                      ///< 内存长度
        SV_BOOL fFastMem = FALSE        ///< 是否在片内申请,默认为FALSE
    );

    /// 内存拷贝
    __inline void* memCpy(
        void* pDest,                    ///< 目的地址
        const void* pSrc,               ///< 源地址
        int nLen                        ///< 长度
    )
    {
        return memcpy(pDest, pSrc, nLen);
    }

    /// 内存设置
    __inline void* memSet(
        void* pDest,                    ///< 目的地址
        int nVal,                       ///< 设置值
        int nLen                        ///< 长度
    )
    {
        return memset(pDest, nVal, nLen);
    }

    /// 内存比较
    /// @return 比较结果
    /// @retval 0 内容一致
    /// @retval 其他 内容不一致
    __inline int memCmp(
        const void* pSrc,           ///< 源地址
        const void* pDest,          ///< 目的地址
        int nLen                    ///< 长度
    )
    {
        return memcmp(pSrc, pDest, nLen);
    }

    /// 单块内存分配类
    class CMemAlloc
    {
    public:
        /// 构造函数
        CMemAlloc();

        /// 析构函数
        ~CMemAlloc();

        /// 内存分配
        /// @note 如果片内申请不到,会自动在片外申请
        void* Alloc(
            int nSize,                      ///< 申请的内存长度
            SV_BOOL fFastMem = FALSE        ///< 是否在片内申请,默认为FALSE
        );

        /// 手动释放内存
        void Free();

        /// 查询是否在片内分配
        SV_BOOL IsFastMem();
    private:
        /// 内存地址
        void* m_pAddr;

        /// 内存长度
        int m_nSize;

        /// 是否片内分配
        SV_BOOL m_fFastMem;
    };

    /// 多块内存分配类
    template<int N>
    class CMultiMemAlloc_T
    {
    public:
        /// 构造函数
        CMultiMemAlloc_T()
        {
            for(int i = 0; i < N; i++)
            {
                m_rgAddr[i] = NULL;
                m_rgSize[i] = 0;
                m_rgFastMem[i] = FALSE;
            }
        }

        /// 析构函数
        ~CMultiMemAlloc_T()
        {
            Free();
        }

        /// 内存分配
        /// @note 如果片内申请不到,会自动在片外申请
        void* Alloc(
            int nSize,                      ///< 申请的内存长度
            SV_BOOL fFastMem = FALSE        ///< 是否在片内申请,默认为FALSE
        )
        {
            // 查找可用位置
            int nPos = -1;

            for(int i = 0; i < N; i++)
            {
                if(m_rgAddr[i] == NULL)
                {
                    nPos = i;
                    break;
                }
            }

            if(nPos == -1)
            {
                return NULL;
            }

            // 分配内存
            void *pTemp = NULL;
            nSize = ((nSize + 1023) >> 10) << 10;     //1024字节对齐

            if(fFastMem)
            {
                pTemp = memAlloc(nSize, TRUE);
            }

            if(pTemp == NULL)
            {
                pTemp = memAlloc(nSize, FALSE);
                fFastMem = FALSE;
            }

            if(pTemp == NULL)
            {
                return NULL;
            }

            m_rgAddr[nPos] = pTemp;
            m_rgSize[nPos] = nSize;
            m_rgFastMem[nPos] = fFastMem;
            return pTemp;
        }

        /// 手动释放内存
        void Free(
            void* pAddr = NULL      ///< 要释放的内存地址,如果为空则为全部释放
        )
        {
            for(int i = 0; i < N; i++)
            {
                if(m_rgAddr[i] == NULL)
                {
                    continue;
                }

                if(pAddr == NULL || m_rgAddr[i] == pAddr)
                {
                    memFree(m_rgAddr[i], m_rgSize[i], m_rgFastMem[i]);
                    m_rgAddr[i] = NULL;
                }
            }
        }

        /// 获取最大可分配块数
        int GetMaxAlloc()
        {
            return N;
        }

    private:
        /// 地址数组
        void* m_rgAddr[N];

        /// 长度数组
        int m_rgSize[N];

        /// 片内内存标记
        SV_BOOL m_rgFastMem[N];
    };

    /// 默认多内存分配类(最大32块内存)
    typedef CMultiMemAlloc_T<32> CMultiMemAlloc;

} // sv
