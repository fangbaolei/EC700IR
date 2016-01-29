/*=============================================================================
 TI Proprietary Information

 Copyright (c) 2005 Texas Instruments, Inc.

    This is an unpublished work created in the year stated above.
    Texas Instruments owns all rights in and to this work and
    intends to maintain and protect it as an unpublished copyright.
    In the event of either inadvertent or deliberate publication,
    the above stated date shall be treated as the year of first
    publication.  In the event of such publication, Texas Instruments
    intends to enforce its rights in the work under the copyright
    laws as a published work.
===============================================================================

 kaleido_mem_map.h

 Description :
     Kaleido ARM memory map macros

===============================================================================

 Revision History:

  Revision
   0.0.0    12/27/05  kurihara   Initial version
   0.0.1    01/11/06  kurihara   Fixed KLD_CFG_BFSWCREG_SZ
   0.0.2    01/27/06  kurihara   Changed regarding LPF, IPE, CLKC
   0.0.3    03/24/06  kurihara   Changed address mapping of ARMIMEM and ARMDMEM
   0.0.4    05/19/06  Rama       Changed ARMIMEM size and KLD_RSV0_SZ size 
   0.0.5    08/24/06  Abhijit    Added  ARMDMEM_SZ macro  
*/

#ifndef KLD_MEM_MAP
#define KLD_MEM_MAP

//=============================================================================
// Kaleido memory map (Byte address)
//=============================================================================
//-----------------------------------------------------------------------------
// Memory map from ARM
//-----------------------------------------------------------------------------
// Start address of the area
#define KLD_ITCM             0x00000000
#define KLD_RSV0             0x00004000
#define KLD_DTCM             0x00400000
#define KLD_RSV1             0x00402000
#define KLD_RSV2             0x00800000
#define KLD_CFG              0x01800000
#define KLD_RSV3             0x01A00000
#define KLD_EXT              0x01C00000

// Size of the area
#define KLD_ITCM_SZ          0x00004000
#define KLD_RSV0_SZ          0x003FC000
#define KLD_DTCM_SZ          0x00002000
#define KLD_RSV1_SZ          0x003FE000
#define KLD_RSV2_SZ          0x01000000
#define KLD_CFG_SZ           0x00200000
#define KLD_RSV3_SZ          0x00200000
#define KLD_EXT_SZ           0xFE400000

//-----------------------------------------------------------------------------
// Memory map for CFG Bus
//  The address is offset from KLD_CFG area
//-----------------------------------------------------------------------------
// Base address
#define KLD_CFG_BASE_FR_ARM  KLD_CFG

// Start address of the area
#define KLD_CFG_TOPREG       0x00000000
#define KLD_CFG_ECDREG       0x00000100
#define KLD_CFG_CALCREG      0x00000200
#define KLD_CFG_RSV0         0x00000220
#define KLD_CFG_BSREG        0x00000300
#define KLD_CFG_RSV1         0x00000340
#define KLD_CFG_MCREG        0x00000400
#define KLD_CFG_RSV2         0x00000410
#define KLD_CFG_LPFREG       0x00000500
#define KLD_CFG_RSV3         0x00000520
#define KLD_CFG_MEREG        0x00000600 // Only for Kaleido10
#define KLD_CFG_RSV4         0x00000620
#define KLD_CFG_IPEREG       0x00000700 // Only for Kaleido10
#define KLD_CFG_RSV5         0x00000720
#define KLD_CFG_RSV6         0x00000800
#define KLD_CFG_CLKCREG      0x00001000
#define KLD_CFG_RSV7         0x00001020
#define KLD_CFG_BFSWCREG     0x00001100
#define KLD_CFG_RSV8         0x00001140
#define KLD_CFG_INTCREG      0x00001200
#define KLD_CFG_ICECREG      0x00001300
#define KLD_CFG_RSV9         0x00001400
#define KLD_CFG_C2D          0x00100000
#define KLD_CFG_RSV10        0x00140000

// Size of the area
#define KLD_CFG_TOPREG_SZ    0x00000100
#define KLD_CFG_ECDREG_SZ    0x00000100
#define KLD_CFG_CALCREG_SZ   0x00000020
#define KLD_CFG_RSV0_SZ      0x000000E0
#define KLD_CFG_BSREG_SZ     0x00000040
#define KLD_CFG_RSV1_SZ      0x000000C0
#define KLD_CFG_MCREG_SZ     0x00000010
#define KLD_CFG_RSV2_SZ      0x000000F0
#define KLD_CFG_LPFREG_SZ    0x00000020
#define KLD_CFG_RSV3_SZ      0x000000E0
#define KLD_CFG_MEREG_SZ     0x00000020 // Only for Kaleido10
#define KLD_CFG_RSV4_SZ      0x000000E0
#define KLD_CFG_IPEREG_SZ    0x00000020 // Only for Kaleido10
#define KLD_CFG_RSV5_SZ      0x000000E0
#define KLD_CFG_RSV6_SZ      0x00000800
#define KLD_CFG_CLKCREG_SZ   0x00000020
#define KLD_CFG_RSV7_SZ      0x000000E0
#define KLD_CFG_BFSWCREG_SZ  0x00000040
#define KLD_CFG_RSV8_SZ      0x000000C0
#define KLD_CFG_INTCREG_SZ   0x00000100
#define KLD_CFG_ICECREG_SZ   0x00000100
#define KLD_CFG_RSV9_SZ      0x000FEC00
#define KLD_CFG_C2D_SZ       0x00040000
#define KLD_CFG_RSV10_SZ     0x000C0000

//-----------------------------------------------------------------------------
// Memory map for DMA Bus
//  The address is offset from C2D area
//-----------------------------------------------------------------------------
// Base address
#define KLD_DMA_BASE_FR_ARM  (KLD_CFG_BASE_FR_ARM + KLD_CFG_C2D)
#define KLD_DMA_BASE_FR_CFG  KLD_CFG_C2D

// Start address of the area
#define KLD_DMA_MEMCBUF0     0x00000000
#define KLD_DMA_MEMCBUF1     0x00002000
#define KLD_DMA_MEMCBUF2     0x00004000
#define KLD_DMA_MEMCBUF3     0x00006000
#define KLD_DMA_RSDBUF       0x00008000
#define KLD_DMA_RECONBUF     0x0000C000
#define KLD_DMA_CALCSBUF     0x0000E000
#define KLD_DMA_CALCMBUF     0x0000F000
#define KLD_DMA_IPRDBUF      0x00010000
#define KLD_DMA_BSWBUF       0x00010800
#define KLD_DMA_BSABUF       0x00011000
#define KLD_DMA_BSBUF        0x00012000
#define KLD_DMA_MBDTBUF      0x00013000
#define KLD_DMA_LPFDBUF      0x00013800
#define KLD_DMA_LPFWBUF0     0x00017000
#define KLD_DMA_LPFWBUF1     0x00018000
#define KLD_DMA_ECDABUF      0x00019000
#define KLD_DMA_STRBUF       0x0001C000
#define KLD_DMA_ECDSBUF      0x0001D000
#define KLD_DMA_ECDWBUF      0x0001E800
#define KLD_DMA_IPORGBUF     0x0001F000 // Only for Kaleido10
#define KLD_DMA_CALCOBUF     0x00020000 // Only for Kaleido10
#define KLD_DMA_ECDTBUF      0x00020800
#define KLD_DMA_RSV0         0x00020C00
#define KLD_DMA_ARMIMEM      0x00030000
#define KLD_DMA_ARMDMEM      0x00038000

// Size of the area
#define KLD_DMA_MEMCBUF0_SZ  0x00002000
#define KLD_DMA_MEMCBUF1_SZ  0x00002000
#define KLD_DMA_MEMCBUF2_SZ  0x00002000
#define KLD_DMA_MEMCBUF3_SZ  0x00002000
#define KLD_DMA_RSDBUF_SZ    0x00004000
#define KLD_DMA_RECONBUF_SZ  0x00002000
#define KLD_DMA_CALCSBUF_SZ  0x00001000
#define KLD_DMA_CALCMBUF_SZ  0x00001000
#define KLD_DMA_IPRDBUF_SZ   0x00000800
#define KLD_DMA_BSWBUF_SZ    0x00000800
#define KLD_DMA_BSABUF_SZ    0x00001000
#define KLD_DMA_BSBUF_SZ     0x00001000
#define KLD_DMA_MBDTBUF_SZ   0x00000800
#define KLD_DMA_LPFDBUF_SZ   0x00003800
#define KLD_DMA_LPFWBUF0_SZ  0x00001000
#define KLD_DMA_LPFWBUF1_SZ  0x00001000
#define KLD_DMA_ECDABUF_SZ   0x00003000
#define KLD_DMA_STRBUF_SZ    0x00001000
#define KLD_DMA_ECDSBUF_SZ   0x00001800
#define KLD_DMA_ECDWBUF_SZ   0x00000800
#define KLD_DMA_IPORGBUF_SZ  0x00001000 // Only for Kaleido10
#define KLD_DMA_CALCOBUF_SZ  0x00000800 // Only for Kaleido10
#define KLD_DMA_ECDTBUF_SZ   0x00000400
#define KLD_DMA_RSV0_SZ      0x0000F400
#define KLD_DMA_ARMIMEM_SZ   0x00004000
#define KLD_DMA_ARMDMEM_SZ   0x00002000


//=============================================================================
// Communication regsiter memory map (Byte address)
//=============================================================================
#define KLD_CMM_REG_ENBL     0xFFFFF000
#define KLD_CMM_ARM_BARR     0xFFFFF004
#define KLD_CMM_E_M_BARR     0xFFFFF008
#define KLD_CMM_ARM_DONE     0xFFFFF00C

// ARMDMEM_SZ
#define ARMDMEM_SZ  KLD_DTCM_SZ - 0x0400  
#define ARMIMEM_OFFSET  0x1000   
#define ARMIMEM_SZ  KLD_ITCM_SZ - ARMIMEM_OFFSET                 

            
#endif
