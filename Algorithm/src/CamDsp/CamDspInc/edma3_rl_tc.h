/******************************************************************************
**+-------------------------------------------------------------------------+**
**|                            ****                                         |**
**|                            ****                                         |**
**|                            ******o***                                   |**
**|                      ********_///_****                                  |**
**|                      ***** /_//_/ ****                                  |**
**|                       ** ** (__/ ****                                   |**
**|                           *********                                     |**
**|                            ****                                         |**
**|                            ***                                          |**
**|                                                                         |**
**|         Copyright (c) 1998-2006 Texas Instruments Incorporated          |**
**|                        ALL RIGHTS RESERVED                              |**
**|                                                                         |**
**| Permission is hereby granted to licensees of Texas Instruments          |**
**| Incorporated (TI) products to use this computer program for the sole    |**
**| purpose of implementing a licensee product based on TI products.        |**
**| No other rights to reproduce, use, or disseminate this computer         |**
**| program, whether in part or in whole, are granted.                      |**
**|                                                                         |**
**| TI makes no representation or warranties with respect to the            |**
**| performance of this computer program, and specifically disclaims        |**
**| any responsibility for any damages, special or consequential,           |**
**| connected with the use of this program.                                 |**
**|                                                                         |**
**+-------------------------------------------------------------------------+**
******************************************************************************/

/**  \file   edma3_rl_tc.h
       \brief  EDMA3 Transfer Controller Register Desciption.

        This file contains the register layer for the EDMA3 Transfer Controller.

    (C) Copyright 2006, Texas Instruments, Inc

    \version
                1.0     Anuj Aggarwal       - Created

 */

#ifndef _EDMA3_RL_TC_H_
#define _EDMA3_RL_TC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
* Register Overlay Structure for DFIREG
\**************************************************************************/
typedef struct  {
    volatile unsigned int DFOPT;
    volatile unsigned int DFSRC;
    volatile unsigned int DFCNT;
    volatile unsigned int DFDST;
    volatile unsigned int DFBIDX;
    volatile unsigned int DFMPPRXY;
    volatile unsigned char RSVD0[40];
} EDMA3_TCRL_DfiregRegs;

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile unsigned int REV;
    volatile unsigned int TCCFG;
    volatile unsigned char RSVD0[248];
    volatile unsigned int TCSTAT;
    volatile unsigned int INTSTAT;
    volatile unsigned int INTEN;
    volatile unsigned int INTCLR;
    volatile unsigned int INTCMD;
    volatile unsigned char RSVD1[12];
    volatile unsigned int ERRSTAT;
    volatile unsigned int ERREN;
    volatile unsigned int ERRCLR;
    volatile unsigned int ERRDET;
    volatile unsigned int ERRCMD;
    volatile unsigned char RSVD2[12];
    volatile unsigned int RDRATE;
    volatile unsigned char RSVD3[188];
    volatile unsigned int POPT;
    volatile unsigned int PSRC;
    volatile unsigned int PCNT;
    volatile unsigned int PDST;
    volatile unsigned int PBIDX;
    volatile unsigned int PMPPRXY;
    volatile unsigned char RSVD4[40];
    volatile unsigned int SAOPT;
    volatile unsigned int SASRC;
    volatile unsigned int SACNT;
    volatile unsigned int SADST;
    volatile unsigned int SABIDX;
    volatile unsigned int SAMPPRXY;
    volatile unsigned int SACNTRLD;
    volatile unsigned int SASRCBREF;
    volatile unsigned int SADSTBREF;
    volatile unsigned char RSVD5[28];
    volatile unsigned int DFCNTRLD;
    volatile unsigned int DFSRCBREF;
    volatile unsigned int DFDSTBREF;
    volatile unsigned char RSVD6[116];
    EDMA3_TCRL_DfiregRegs DFIREG[4];
} EDMA3_TCRL_Regs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* REV */

#define EDMA3_TCRL_REV_TYPE_MASK         (0x00FF0000u)
#define EDMA3_TCRL_REV_TYPE_SHIFT        (0x00000010u)
#define EDMA3_TCRL_REV_TYPE_RESETVAL     (0x00000006u)

#define EDMA3_TCRL_REV_CLASS_MASK        (0x0000FF00u)
#define EDMA3_TCRL_REV_CLASS_SHIFT       (0x00000008u)
#define EDMA3_TCRL_REV_CLASS_RESETVAL    (0x00000004u)

#define EDMA3_TCRL_REV_REV_MASK          (0x000000FFu)
#define EDMA3_TCRL_REV_REV_SHIFT         (0x00000000u)
#define EDMA3_TCRL_REV_REV_RESETVAL      (0x00000001u)

#define EDMA3_TCRL_REV_RESETVAL          (0x00060401u)

/* TCCFG */

#define EDMA3_TCRL_TCCFG_DREGDEPTH_MASK  (0x00000300u)
#define EDMA3_TCRL_TCCFG_DREGDEPTH_SHIFT (0x00000008u)
#define EDMA3_TCRL_TCCFG_DREGDEPTH_RESETVAL (0x00000000u)

/*----DREGDEPTH Tokens----*/
#define EDMA3_TCRL_TCCFG_DREGDEPTH_1ENTRY (0x00000000u)
#define EDMA3_TCRL_TCCFG_DREGDEPTH_2ENTRY (0x00000001u)
#define EDMA3_TCRL_TCCFG_DREGDEPTH_4ENTRY (0x00000002u)

#define EDMA3_TCRL_TCCFG_BUSWIDTH_MASK   (0x00000030u)
#define EDMA3_TCRL_TCCFG_BUSWIDTH_SHIFT  (0x00000004u)
#define EDMA3_TCRL_TCCFG_BUSWIDTH_RESETVAL (0x00000000u)

/*----BUSWIDTH Tokens----*/
#define EDMA3_TCRL_TCCFG_BUSWIDTH_32BIT  (0x00000000u)
#define EDMA3_TCRL_TCCFG_BUSWIDTH_64BIY  (0x00000001u)
#define EDMA3_TCRL_TCCFG_BUSWIDTH_128BIT (0x00000002u)

#define EDMA3_TCRL_TCCFG_FIFOSIZE_MASK   (0x00000007u)
#define EDMA3_TCRL_TCCFG_FIFOSIZE_SHIFT  (0x00000000u)
#define EDMA3_TCRL_TCCFG_FIFOSIZE_RESETVAL (0x00000000u)

/*----FIFOSIZE Tokens----*/
#define EDMA3_TCRL_TCCFG_FIFOSIZE_32BYTE (0x00000000u)
#define EDMA3_TCRL_TCCFG_FIFOSIZE_64BYTE (0x00000001u)
#define EDMA3_TCRL_TCCFG_FIFOSIZE_128BYTE (0x00000002u)
#define EDMA3_TCRL_TCCFG_FIFOSIZE_256BYTE (0x00000003u)
#define EDMA3_TCRL_TCCFG_FIFOSIZE_512BYTE (0x00000004u)

#define EDMA3_TCRL_TCCFG_RESETVAL        (0x00000000u)

/* TCSTAT */

#define EDMA3_TCRL_TCSTAT_DFSTRT_MASK  (0x00003000u)
#define EDMA3_TCRL_TCSTAT_DFSTRT_SHIFT (0x0000000Cu)
#define EDMA3_TCRL_TCSTAT_DFSTRT_RESETVAL (0x00000000u)

#define EDMA3_TCRL_TCSTAT_ATCV_MASK    (0x00000100u)
#define EDMA3_TCRL_TCSTAT_ATCV_SHIFT   (0x00000008u)
#define EDMA3_TCRL_TCSTAT_ATCV_RESETVAL (0x00000000u)

/*----ATCV Tokens----*/
#define EDMA3_TCRL_TCSTAT_ATCV_IDLE    (0x00000000u)
#define EDMA3_TCRL_TCSTAT_ATCV_BUSY    (0x00000001u)

#define EDMA3_TCRL_TCSTAT_DSTACT_MASK  (0x00000070u)
#define EDMA3_TCRL_TCSTAT_DSTACT_SHIFT (0x00000004u)
#define EDMA3_TCRL_TCSTAT_DSTACT_RESETVAL (0x00000000u)

/*----DSTACT Tokens----*/
#define EDMA3_TCRL_TCSTAT_DSTACT_EMPTY (0x00000000u)
#define EDMA3_TCRL_TCSTAT_DSTACT_1TR   (0x00000001u)
#define EDMA3_TCRL_TCSTAT_DSTACT_2TR   (0x00000002u)
#define EDMA3_TCRL_TCSTAT_DSTACT_3TR   (0x00000003u)
#define EDMA3_TCRL_TCSTAT_DSTACT_4TR   (0x00000004u)

#define EDMA3_TCRL_TCSTAT_WSACTV_MASK  (0x00000004u)
#define EDMA3_TCRL_TCSTAT_WSACTV_SHIFT (0x00000002u)
#define EDMA3_TCRL_TCSTAT_WSACTV_RESETVAL (0x00000000u)

/*----WSACTV Tokens----*/
#define EDMA3_TCRL_TCSTAT_WSACTV_NONE  (0x00000000u)
#define EDMA3_TCRL_TCSTAT_WSACTV_PEND  (0x00000001u)

#define EDMA3_TCRL_TCSTAT_SRCACTV_MASK (0x00000002u)
#define EDMA3_TCRL_TCSTAT_SRCACTV_SHIFT (0x00000001u)
#define EDMA3_TCRL_TCSTAT_SRCACTV_RESETVAL (0x00000000u)

/*----SRCACTV Tokens----*/
#define EDMA3_TCRL_TCSTAT_SRCACTV_IDLE (0x00000000u)
#define EDMA3_TCRL_TCSTAT_SRCACTV_BUSY (0x00000001u)

#define EDMA3_TCRL_TCSTAT_PROGBUSY_MASK (0x00000001u)
#define EDMA3_TCRL_TCSTAT_PROGBUSY_SHIFT (0x00000000u)
#define EDMA3_TCRL_TCSTAT_PROGBUSY_RESETVAL (0x00000000u)

/*----PROGBUSY Tokens----*/
#define EDMA3_TCRL_TCSTAT_PROGBUSY_IDLE (0x00000000u)
#define EDMA3_TCRL_TCSTAT_PROGBUSY_BUSY (0x00000001u)

#define EDMA3_TCRL_TCSTAT_RESETVAL     (0x00000000u)

/* INTSTAT */

#define EDMA3_TCRL_INTSTAT_TRDONE_MASK   (0x00000002u)
#define EDMA3_TCRL_INTSTAT_TRDONE_SHIFT  (0x00000001u)
#define EDMA3_TCRL_INTSTAT_TRDONE_RESETVAL (0x00000000u)

/*----TRDONE Tokens----*/
#define EDMA3_TCRL_INTSTAT_TRDONE_NONE   (0x00000000u)
#define EDMA3_TCRL_INTSTAT_TRDONE_DONE   (0x00000001u)

#define EDMA3_TCRL_INTSTAT_PROGEMPTY_MASK (0x00000001u)
#define EDMA3_TCRL_INTSTAT_PROGEMPTY_SHIFT (0x00000000u)
#define EDMA3_TCRL_INTSTAT_PROGEMPTY_RESETVAL (0x00000000u)

/*----PROGEMPTY Tokens----*/
#define EDMA3_TCRL_INTSTAT_PROGEMPTY_NONE (0x00000000u)
#define EDMA3_TCRL_INTSTAT_PROGEMPTY_EMPTY (0x00000001u)

#define EDMA3_TCRL_INTSTAT_RESETVAL      (0x00000000u)

/* INTEN */

#define EDMA3_TCRL_INTEN_TRDONE_MASK     (0x00000002u)
#define EDMA3_TCRL_INTEN_TRDONE_SHIFT    (0x00000001u)
#define EDMA3_TCRL_INTEN_TRDONE_RESETVAL (0x00000000u)

/*----TRDONE Tokens----*/
#define EDMA3_TCRL_INTEN_TRDONE_DISABLE  (0x00000000u)
#define EDMA3_TCRL_INTEN_TRDONE_ENABLE   (0x00000001u)

#define EDMA3_TCRL_INTEN_PROGEMPTY_MASK  (0x00000001u)
#define EDMA3_TCRL_INTEN_PROGEMPTY_SHIFT (0x00000000u)
#define EDMA3_TCRL_INTEN_PROGEMPTY_RESETVAL (0x00000000u)

/*----PROGEMPTY Tokens----*/
#define EDMA3_TCRL_INTEN_PROGEMPTY_DISABLE (0x00000000u)
#define EDMA3_TCRL_INTEN_PROGEMPTY_ENABLE (0x00000001u)

#define EDMA3_TCRL_INTEN_RESETVAL        (0x00000000u)

/* INTCLR */

#define EDMA3_TCRL_INTCLR_TRDONE_MASK    (0x00000002u)
#define EDMA3_TCRL_INTCLR_TRDONE_SHIFT   (0x00000001u)
#define EDMA3_TCRL_INTCLR_TRDONE_RESETVAL (0x00000000u)

/*----TRDONE Tokens----*/
#define EDMA3_TCRL_INTCLR_TRDONE_CLEAR   (0x00000001u)

#define EDMA3_TCRL_INTCLR_PROGEMPTY_MASK (0x00000001u)
#define EDMA3_TCRL_INTCLR_PROGEMPTY_SHIFT (0x00000000u)
#define EDMA3_TCRL_INTCLR_PROGEMPTY_RESETVAL (0x00000000u)

/*----PROGEMPTY Tokens----*/
#define EDMA3_TCRL_INTCLR_PROGEMPTY_CLEAR (0x00000001u)

#define EDMA3_TCRL_INTCLR_RESETVAL       (0x00000000u)

/* INTCMD */

#define EDMA3_TCRL_INTCMD_SET_MASK       (0x00000002u)
#define EDMA3_TCRL_INTCMD_SET_SHIFT      (0x00000001u)
#define EDMA3_TCRL_INTCMD_SET_RESETVAL   (0x00000000u)

/*----SET Tokens----*/
#define EDMA3_TCRL_INTCMD_SET_SET        (0x00000001u)

#define EDMA3_TCRL_INTCMD_EVAL_MASK      (0x00000001u)
#define EDMA3_TCRL_INTCMD_EVAL_SHIFT     (0x00000000u)
#define EDMA3_TCRL_INTCMD_EVAL_RESETVAL  (0x00000000u)

/*----EVAL Tokens----*/
#define EDMA3_TCRL_INTCMD_EVAL_EVAL      (0x00000001u)

#define EDMA3_TCRL_INTCMD_RESETVAL       (0x00000000u)

/* ERRSTAT */

#define EDMA3_TCRL_ERRSTAT_MMRAERR_MASK  (0x00000008u)
#define EDMA3_TCRL_ERRSTAT_MMRAERR_SHIFT (0x00000003u)
#define EDMA3_TCRL_ERRSTAT_MMRAERR_RESETVAL (0x00000000u)

/*----MMRAERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_MMRAERR_NONE  (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_MMRAERR_ERROR (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_TRERR_MASK    (0x00000004u)
#define EDMA3_TCRL_ERRSTAT_TRERR_SHIFT   (0x00000002u)
#define EDMA3_TCRL_ERRSTAT_TRERR_RESETVAL (0x00000000u)

/*----TRERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_TRERR_NONE    (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_TRERR_ERROR   (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_BUSERR_MASK   (0x00000001u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_SHIFT  (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_RESETVAL (0x00000000u)

/*----BUSERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_BUSERR_NONE   (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_ERROR  (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_RESETVAL      (0x00000000u)

/* ERREN */

#define EDMA3_TCRL_ERREN_MMRAERR_MASK    (0x00000008u)
#define EDMA3_TCRL_ERREN_MMRAERR_SHIFT   (0x00000003u)
#define EDMA3_TCRL_ERREN_MMRAERR_RESETVAL (0x00000000u)

/*----MMRAERR Tokens----*/
#define EDMA3_TCRL_ERREN_MMRAERR_ENABLE  (0x00000001u)
#define EDMA3_TCRL_ERREN_MMRAERR_DISABLE (0x00000000u)

#define EDMA3_TCRL_ERREN_TRERR_MASK      (0x00000004u)
#define EDMA3_TCRL_ERREN_TRERR_SHIFT     (0x00000002u)
#define EDMA3_TCRL_ERREN_TRERR_RESETVAL  (0x00000000u)

/*----TRERR Tokens----*/
#define EDMA3_TCRL_ERREN_TRERR_ENABLE    (0x00000001u)
#define EDMA3_TCRL_ERREN_TRERR_DISABLE   (0x00000000u)

#define EDMA3_TCRL_ERREN_BUSERR_MASK     (0x00000001u)
#define EDMA3_TCRL_ERREN_BUSERR_SHIFT    (0x00000000u)
#define EDMA3_TCRL_ERREN_BUSERR_RESETVAL (0x00000000u)

/*----BUSERR Tokens----*/
#define EDMA3_TCRL_ERREN_BUSERR_ENABLE   (0x00000001u)
#define EDMA3_TCRL_ERREN_BUSERR_DISABLE  (0x00000000u)

#define EDMA3_TCRL_ERREN_RESETVAL        (0x00000000u)

/* ERRCLR */

#define EDMA3_TCRL_ERRCLR_MMRAERR_MASK   (0x00000008u)
#define EDMA3_TCRL_ERRCLR_MMRAERR_SHIFT  (0x00000003u)
#define EDMA3_TCRL_ERRCLR_MMRAERR_RESETVAL (0x00000000u)

/*----MMRAERR Tokens----*/
#define EDMA3_TCRL_ERRCLR_MMRAERR_CLEAR  (0x00000001u)

#define EDMA3_TCRL_ERRCLR_TRERR_MASK     (0x00000004u)
#define EDMA3_TCRL_ERRCLR_TRERR_SHIFT    (0x00000002u)
#define EDMA3_TCRL_ERRCLR_TRERR_RESETVAL (0x00000000u)

/*----TRERR Tokens----*/
#define EDMA3_TCRL_ERRCLR_TRERR_CLEAR    (0x00000001u)

#define EDMA3_TCRL_ERRCLR_BUSERR_MASK    (0x00000001u)
#define EDMA3_TCRL_ERRCLR_BUSERR_SHIFT   (0x00000000u)
#define EDMA3_TCRL_ERRCLR_BUSERR_RESETVAL (0x00000000u)

/*----BUSERR Tokens----*/
#define EDMA3_TCRL_ERRCLR_BUSERR_CLEAR   (0x00000001u)

#define EDMA3_TCRL_ERRCLR_RESETVAL       (0x00000000u)

/* ERRDET */

#define EDMA3_TCRL_ERRDET_TCCHEN_MASK    (0x00020000u)
#define EDMA3_TCRL_ERRDET_TCCHEN_SHIFT   (0x00000011u)
#define EDMA3_TCRL_ERRDET_TCCHEN_RESETVAL (0x00000000u)

#define EDMA3_TCRL_ERRDET_TCINTEN_MASK   (0x00010000u)
#define EDMA3_TCRL_ERRDET_TCINTEN_SHIFT  (0x00000010u)
#define EDMA3_TCRL_ERRDET_TCINTEN_RESETVAL (0x00000000u)

#define EDMA3_TCRL_ERRDET_TCC_MASK       (0x00003F00u)
#define EDMA3_TCRL_ERRDET_TCC_SHIFT      (0x00000008u)
#define EDMA3_TCRL_ERRDET_TCC_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_ERRDET_STAT_MASK      (0x0000000Fu)
#define EDMA3_TCRL_ERRDET_STAT_SHIFT     (0x00000000u)
#define EDMA3_TCRL_ERRDET_STAT_RESETVAL  (0x00000000u)

/*----STAT Tokens----*/
#define EDMA3_TCRL_ERRDET_STAT_NONE      (0x00000000u)
#define EDMA3_TCRL_ERRDET_STAT_READ_ADDRESS (0x00000001u)
#define EDMA3_TCRL_ERRDET_STAT_READ_PRIVILEGE (0x00000002u)
#define EDMA3_TCRL_ERRDET_STAT_READ_TIMEOUT (0x00000003u)
#define EDMA3_TCRL_ERRDET_STAT_READ_DATA (0x00000004u)
#define EDMA3_TCRL_ERRDET_STAT_READ_EXCLUSIVE (0x00000007u)
#define EDMA3_TCRL_ERRDET_STAT_WRITE_ADDRESS (0x00000009u)
#define EDMA3_TCRL_ERRDET_STAT_WRITE_PRIVILEGE (0x0000000Au)
#define EDMA3_TCRL_ERRDET_STAT_WRITE_TIMEOUT (0x0000000Bu)
#define EDMA3_TCRL_ERRDET_STAT_WRITE_DATA (0x0000000Cu)
#define EDMA3_TCRL_ERRDET_STAT_WRITE_EXCLUSIVE (0x0000000Fu)

#define EDMA3_TCRL_ERRDET_RESETVAL       (0x00000000u)

/* ERRCMD */

#define EDMA3_TCRL_ERRCMD_SET_MASK       (0x00000002u)
#define EDMA3_TCRL_ERRCMD_SET_SHIFT      (0x00000001u)
#define EDMA3_TCRL_ERRCMD_SET_RESETVAL   (0x00000000u)

/*----SET Tokens----*/
#define EDMA3_TCRL_ERRCMD_SET_SET        (0x00000001u)

#define EDMA3_TCRL_ERRCMD_EVAL_MASK      (0x00000001u)
#define EDMA3_TCRL_ERRCMD_EVAL_SHIFT     (0x00000000u)
#define EDMA3_TCRL_ERRCMD_EVAL_RESETVAL  (0x00000000u)

/*----EVAL Tokens----*/
#define EDMA3_TCRL_ERRCMD_EVAL_EVAL      (0x00000001u)

#define EDMA3_TCRL_ERRCMD_RESETVAL       (0x00000000u)

/* RDRATE */

#define EDMA3_TCRL_RDRATE_RDRATE_MASK    (0x00000007u)
#define EDMA3_TCRL_RDRATE_RDRATE_SHIFT   (0x00000000u)
#define EDMA3_TCRL_RDRATE_RDRATE_RESETVAL (0x00000000u)

/*----RDRATE Tokens----*/
#define EDMA3_TCRL_RDRATE_RDRATE_AFAP    (0x00000000u)
#define EDMA3_TCRL_RDRATE_RDRATE_4CYCLE  (0x00000001u)
#define EDMA3_TCRL_RDRATE_RDRATE_8CYCLE  (0x00000002u)
#define EDMA3_TCRL_RDRATE_RDRATE_16CYCLE (0x00000003u)
#define EDMA3_TCRL_RDRATE_RDRATE_32CYCLE (0x00000004u)

#define EDMA3_TCRL_RDRATE_RESETVAL       (0x00000000u)

/* POPT */

#define EDMA3_TCRL_POPT_TCCHEN_MASK      (0x00400000u)
#define EDMA3_TCRL_POPT_TCCHEN_SHIFT     (0x00000016u)
#define EDMA3_TCRL_POPT_TCCHEN_RESETVAL  (0x00000000u)

/*----TCCHEN Tokens----*/
#define EDMA3_TCRL_POPT_TCCHEN_DISABLE   (0x00000000u)
#define EDMA3_TCRL_POPT_TCCHEN_ENABLE    (0x00000001u)

#define EDMA3_TCRL_POPT_TCINTEN_MASK     (0x00100000u)
#define EDMA3_TCRL_POPT_TCINTEN_SHIFT    (0x00000014u)
#define EDMA3_TCRL_POPT_TCINTEN_RESETVAL (0x00000000u)

/*----TCINTEN Tokens----*/
#define EDMA3_TCRL_POPT_TCINTEN_DISABLE  (0x00000000u)
#define EDMA3_TCRL_POPT_TCINTEN_ENABLE   (0x00000001u)

#define EDMA3_TCRL_POPT_TCC_MASK         (0x0003F000u)
#define EDMA3_TCRL_POPT_TCC_SHIFT        (0x0000000Cu)
#define EDMA3_TCRL_POPT_TCC_RESETVAL     (0x00000000u)

#define EDMA3_TCRL_POPT_FWID_MASK        (0x00000700u)
#define EDMA3_TCRL_POPT_FWID_SHIFT       (0x00000008u)
#define EDMA3_TCRL_POPT_FWID_RESETVAL    (0x00000000u)

/*----FWID Tokens----*/
#define EDMA3_TCRL_POPT_FWID_8BIT        (0x00000000u)
#define EDMA3_TCRL_POPT_FWID_16BIT       (0x00000001u)
#define EDMA3_TCRL_POPT_FWID_32BIT       (0x00000002u)
#define EDMA3_TCRL_POPT_FWID_64BIT       (0x00000003u)
#define EDMA3_TCRL_POPT_FWID_128BIT      (0x00000004u)
#define EDMA3_TCRL_POPT_FWID_256BIT      (0x00000005u)

#define EDMA3_TCRL_POPT_PRI_MASK         (0x00000070u)
#define EDMA3_TCRL_POPT_PRI_SHIFT        (0x00000004u)
#define EDMA3_TCRL_POPT_PRI_RESETVAL     (0x00000000u)

#define EDMA3_TCRL_POPT_DAM_MASK         (0x00000002u)
#define EDMA3_TCRL_POPT_DAM_SHIFT        (0x00000001u)
#define EDMA3_TCRL_POPT_DAM_RESETVAL     (0x00000000u)

/*----DAM Tokens----*/
#define EDMA3_TCRL_POPT_DAM_INCR         (0x00000000u)
#define EDMA3_TCRL_POPT_DAM_FIFO         (0x00000001u)

#define EDMA3_TCRL_POPT_SAM_MASK         (0x00000001u)
#define EDMA3_TCRL_POPT_SAM_SHIFT        (0x00000000u)
#define EDMA3_TCRL_POPT_SAM_RESETVAL     (0x00000000u)

/*----SAM Tokens----*/
#define EDMA3_TCRL_POPT_SAM_INCR         (0x00000000u)
#define EDMA3_TCRL_POPT_SAM_FIFO         (0x00000001u)

#define EDMA3_TCRL_POPT_RESETVAL         (0x00000000u)

/* PSRC */

#define EDMA3_TCRL_PSRC_SADDR_MASK       (0xFFFFFFFFu)
#define EDMA3_TCRL_PSRC_SADDR_SHIFT      (0x00000000u)
#define EDMA3_TCRL_PSRC_SADDR_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_PSRC_RESETVAL         (0x00000000u)

/* PCNT */

#define EDMA3_TCRL_PCNT_BCNT_MASK        (0xFFFF0000u)
#define EDMA3_TCRL_PCNT_BCNT_SHIFT       (0x00000010u)
#define EDMA3_TCRL_PCNT_BCNT_RESETVAL    (0x00000000u)

#define EDMA3_TCRL_PCNT_ACNT_MASK        (0x0000FFFFu)
#define EDMA3_TCRL_PCNT_ACNT_SHIFT       (0x00000000u)
#define EDMA3_TCRL_PCNT_ACNT_RESETVAL    (0x00000000u)

#define EDMA3_TCRL_PCNT_RESETVAL         (0x00000000u)

/* PDST */

#define EDMA3_TCRL_PDST_DADDR_MASK       (0xFFFFFFFFu)
#define EDMA3_TCRL_PDST_DADDR_SHIFT      (0x00000000u)
#define EDMA3_TCRL_PDST_DADDR_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_PDST_RESETVAL         (0x00000000u)

/* PBIDX */

#define EDMA3_TCRL_PBIDX_DBIDX_MASK      (0xFFFF0000u)
#define EDMA3_TCRL_PBIDX_DBIDX_SHIFT     (0x00000010u)
#define EDMA3_TCRL_PBIDX_DBIDX_RESETVAL  (0x00000000u)

#define EDMA3_TCRL_PBIDX_SBIDX_MASK      (0x0000FFFFu)
#define EDMA3_TCRL_PBIDX_SBIDX_SHIFT     (0x00000000u)
#define EDMA3_TCRL_PBIDX_SBIDX_RESETVAL  (0x00000000u)

#define EDMA3_TCRL_PBIDX_RESETVAL        (0x00000000u)

/* PMPPRXY */

#define EDMA3_TCRL_PMPPRXY_PRIV_MASK     (0x00000100u)
#define EDMA3_TCRL_PMPPRXY_PRIV_SHIFT    (0x00000008u)
#define EDMA3_TCRL_PMPPRXY_PRIV_RESETVAL (0x00000000u)

/*----PRIV Tokens----*/
#define EDMA3_TCRL_PMPPRXY_PRIV_USER     (0x00000000u)
#define EDMA3_TCRL_PMPPRXY_PRIV_SUPERVISOR (0x00000001u)

#define EDMA3_TCRL_PMPPRXY_PRIVID_MASK   (0x0000000Fu)
#define EDMA3_TCRL_PMPPRXY_PRIVID_SHIFT  (0x00000000u)
#define EDMA3_TCRL_PMPPRXY_PRIVID_RESETVAL (0x00000000u)

#define EDMA3_TCRL_PMPPRXY_RESETVAL      (0x00000000u)

/* SAOPT */

#define EDMA3_TCRL_SAOPT_TCCHEN_MASK     (0x00400000u)
#define EDMA3_TCRL_SAOPT_TCCHEN_SHIFT    (0x00000016u)
#define EDMA3_TCRL_SAOPT_TCCHEN_RESETVAL (0x00000000u)

/*----TCCHEN Tokens----*/
#define EDMA3_TCRL_SAOPT_TCCHEN_DISABLE  (0x00000000u)
#define EDMA3_TCRL_SAOPT_TCCHEN_ENABLE   (0x00000001u)

#define EDMA3_TCRL_SAOPT_TCINTEN_MASK    (0x00100000u)
#define EDMA3_TCRL_SAOPT_TCINTEN_SHIFT   (0x00000014u)
#define EDMA3_TCRL_SAOPT_TCINTEN_RESETVAL (0x00000000u)

/*----TCINTEN Tokens----*/
#define EDMA3_TCRL_SAOPT_TCINTEN_DISABLE (0x00000000u)
#define EDMA3_TCRL_SAOPT_TCINTEN_ENABLE  (0x00000001u)

#define EDMA3_TCRL_SAOPT_TCC_MASK        (0x0003F000u)
#define EDMA3_TCRL_SAOPT_TCC_SHIFT       (0x0000000Cu)
#define EDMA3_TCRL_SAOPT_TCC_RESETVAL    (0x00000000u)

#define EDMA3_TCRL_SAOPT_FWID_MASK       (0x00000700u)
#define EDMA3_TCRL_SAOPT_FWID_SHIFT      (0x00000008u)
#define EDMA3_TCRL_SAOPT_FWID_RESETVAL   (0x00000000u)

/*----FWID Tokens----*/
#define EDMA3_TCRL_SAOPT_FWID_8BIT       (0x00000000u)
#define EDMA3_TCRL_SAOPT_FWID_16BIT      (0x00000001u)
#define EDMA3_TCRL_SAOPT_FWID_32BIT      (0x00000002u)
#define EDMA3_TCRL_SAOPT_FWID_64BIT      (0x00000003u)
#define EDMA3_TCRL_SAOPT_FWID_128BIT     (0x00000004u)
#define EDMA3_TCRL_SAOPT_FWID_256BIT     (0x00000005u)

#define EDMA3_TCRL_SAOPT_PRI_MASK        (0x00000070u)
#define EDMA3_TCRL_SAOPT_PRI_SHIFT       (0x00000004u)
#define EDMA3_TCRL_SAOPT_PRI_RESETVAL    (0x00000000u)

#define EDMA3_TCRL_SAOPT_DAM_MASK        (0x00000002u)
#define EDMA3_TCRL_SAOPT_DAM_SHIFT       (0x00000001u)
#define EDMA3_TCRL_SAOPT_DAM_RESETVAL    (0x00000000u)

/*----DAM Tokens----*/
#define EDMA3_TCRL_SAOPT_DAM_INCR        (0x00000000u)
#define EDMA3_TCRL_SAOPT_DAM_FIFO        (0x00000001u)

#define EDMA3_TCRL_SAOPT_SAM_MASK        (0x00000001u)
#define EDMA3_TCRL_SAOPT_SAM_SHIFT       (0x00000000u)
#define EDMA3_TCRL_SAOPT_SAM_RESETVAL    (0x00000000u)

/*----SAM Tokens----*/
#define EDMA3_TCRL_SAOPT_SAM_INCR        (0x00000000u)
#define EDMA3_TCRL_SAOPT_SAM_FIFO        (0x00000001u)

#define EDMA3_TCRL_SAOPT_RESETVAL        (0x00000000u)

/* SASRC */

#define EDMA3_TCRL_SASRC_SADDR_MASK      (0xFFFFFFFFu)
#define EDMA3_TCRL_SASRC_SADDR_SHIFT     (0x00000000u)
#define EDMA3_TCRL_SASRC_SADDR_RESETVAL  (0x00000000u)

#define EDMA3_TCRL_SASRC_RESETVAL        (0x00000000u)

/* SACNT */

#define EDMA3_TCRL_SACNT_BCNT_MASK       (0xFFFF0000u)
#define EDMA3_TCRL_SACNT_BCNT_SHIFT      (0x00000010u)
#define EDMA3_TCRL_SACNT_BCNT_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_SACNT_ACNT_MASK       (0x0000FFFFu)
#define EDMA3_TCRL_SACNT_ACNT_SHIFT      (0x00000000u)
#define EDMA3_TCRL_SACNT_ACNT_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_SACNT_RESETVAL        (0x00000000u)

/* SADST */

#define EDMA3_TCRL_SADST_RESETVAL        (0x00000000u)

/* SABIDX */

#define EDMA3_TCRL_SABIDX_DBIDX_MASK     (0xFFFF0000u)
#define EDMA3_TCRL_SABIDX_DBIDX_SHIFT    (0x00000010u)
#define EDMA3_TCRL_SABIDX_DBIDX_RESETVAL (0x00000000u)

#define EDMA3_TCRL_SABIDX_SBIDX_MASK     (0x0000FFFFu)
#define EDMA3_TCRL_SABIDX_SBIDX_SHIFT    (0x00000000u)
#define EDMA3_TCRL_SABIDX_SBIDX_RESETVAL (0x00000000u)

#define EDMA3_TCRL_SABIDX_RESETVAL       (0x00000000u)

/* SAMPPRXY */

#define EDMA3_TCRL_SAMPPRXY_PRIV_MASK    (0x00000100u)
#define EDMA3_TCRL_SAMPPRXY_PRIV_SHIFT   (0x00000008u)
#define EDMA3_TCRL_SAMPPRXY_PRIV_RESETVAL (0x00000000u)

/*----PRIV Tokens----*/
#define EDMA3_TCRL_SAMPPRXY_PRIV_USER    (0x00000000u)
#define EDMA3_TCRL_SAMPPRXY_PRIV_SUPERVISOR (0x00000001u)

#define EDMA3_TCRL_SAMPPRXY_PRIVID_MASK  (0x0000000Fu)
#define EDMA3_TCRL_SAMPPRXY_PRIVID_SHIFT (0x00000000u)
#define EDMA3_TCRL_SAMPPRXY_PRIVID_RESETVAL (0x00000000u)

#define EDMA3_TCRL_SAMPPRXY_RESETVAL     (0x00000000u)

/* SACNTRLD */

#define EDMA3_TCRL_SACNTRLD_ACNTRLD_MASK (0x0000FFFFu)
#define EDMA3_TCRL_SACNTRLD_ACNTRLD_SHIFT (0x00000000u)
#define EDMA3_TCRL_SACNTRLD_ACNTRLD_RESETVAL (0x00000000u)

#define EDMA3_TCRL_SACNTRLD_RESETVAL     (0x00000000u)

/* SASRCBREF */

#define EDMA3_TCRL_SASRCBREF_SADDRBREFG_MASK (0xFFFFFFFFu)
#define EDMA3_TCRL_SASRCBREF_SADDRBREFG_SHIFT (0x00000000u)
#define EDMA3_TCRL_SASRCBREF_SADDRBREFG_RESETVAL (0x00000000u)

#define EDMA3_TCRL_SASRCBREF_RESETVAL    (0x00000000u)

/* SADSTBREF */

#define EDMA3_TCRL_SADSTBREF_RESETVAL    (0x00000000u)

/* DFCNTRLD */

#define EDMA3_TCRL_DFCNTRLD_ACNTRLD_MASK (0x0000FFFFu)
#define EDMA3_TCRL_DFCNTRLD_ACNTRLD_SHIFT (0x00000000u)
#define EDMA3_TCRL_DFCNTRLD_ACNTRLD_RESETVAL (0x00000000u)

#define EDMA3_TCRL_DFCNTRLD_RESETVAL     (0x00000000u)

/* DFSRCBREF */

#define EDMA3_TCRL_DFSRCBREF_RESETVAL    (0x00000000u)

/* DFDSTBREF */

#define EDMA3_TCRL_DFDSTBREF_DADDRBREF_MASK (0xFFFFFFFFu)
#define EDMA3_TCRL_DFDSTBREF_DADDRBREF_SHIFT (0x00000000u)
#define EDMA3_TCRL_DFDSTBREF_DADDRBREF_RESETVAL (0x00000000u)

#define EDMA3_TCRL_DFDSTBREF_RESETVAL    (0x00000000u)

/* DFOPT */

#define EDMA3_TCRL_DFOPT_TCCHEN_MASK     (0x00400000u)
#define EDMA3_TCRL_DFOPT_TCCHEN_SHIFT    (0x00000016u)
#define EDMA3_TCRL_DFOPT_TCCHEN_RESETVAL (0x00000000u)

/*----TCCHEN Tokens----*/
#define EDMA3_TCRL_DFOPT_TCCHEN_DISABLE  (0x00000000u)
#define EDMA3_TCRL_DFOPT_TCCHEN_ENABLE   (0x00000001u)

#define EDMA3_TCRL_DFOPT_TCINTEN_MASK    (0x00100000u)
#define EDMA3_TCRL_DFOPT_TCINTEN_SHIFT   (0x00000014u)
#define EDMA3_TCRL_DFOPT_TCINTEN_RESETVAL (0x00000000u)

/*----TCINTEN Tokens----*/
#define EDMA3_TCRL_DFOPT_TCINTEN_DISABLE (0x00000000u)
#define EDMA3_TCRL_DFOPT_TCINTEN_ENABLE  (0x00000001u)

#define EDMA3_TCRL_DFOPT_TCC_MASK        (0x0003F000u)
#define EDMA3_TCRL_DFOPT_TCC_SHIFT       (0x0000000Cu)
#define EDMA3_TCRL_DFOPT_TCC_RESETVAL    (0x00000000u)

#define EDMA3_TCRL_DFOPT_FWID_MASK       (0x00000700u)
#define EDMA3_TCRL_DFOPT_FWID_SHIFT      (0x00000008u)
#define EDMA3_TCRL_DFOPT_FWID_RESETVAL   (0x00000000u)

/*----FWID Tokens----*/
#define EDMA3_TCRL_DFOPT_FWID_8BIT       (0x00000000u)
#define EDMA3_TCRL_DFOPT_FWID_16BIT      (0x00000001u)
#define EDMA3_TCRL_DFOPT_FWID_32BIT      (0x00000002u)
#define EDMA3_TCRL_DFOPT_FWID_64BIT      (0x00000003u)
#define EDMA3_TCRL_DFOPT_FWID_128BIT     (0x00000004u)
#define EDMA3_TCRL_DFOPT_FWID_256BIT     (0x00000005u)

#define EDMA3_TCRL_DFOPT_PRI_MASK        (0x00000070u)
#define EDMA3_TCRL_DFOPT_PRI_SHIFT       (0x00000004u)
#define EDMA3_TCRL_DFOPT_PRI_RESETVAL    (0x00000000u)

#define EDMA3_TCRL_DFOPT_DAM_MASK        (0x00000002u)
#define EDMA3_TCRL_DFOPT_DAM_SHIFT       (0x00000001u)
#define EDMA3_TCRL_DFOPT_DAM_RESETVAL    (0x00000000u)

/*----DAM Tokens----*/
#define EDMA3_TCRL_DFOPT_DAM_INCR        (0x00000000u)
#define EDMA3_TCRL_DFOPT_DAM_FIFO        (0x00000001u)

#define EDMA3_TCRL_DFOPT_SAM_MASK        (0x00000001u)
#define EDMA3_TCRL_DFOPT_SAM_SHIFT       (0x00000000u)
#define EDMA3_TCRL_DFOPT_SAM_RESETVAL    (0x00000000u)

/*----SAM Tokens----*/
#define EDMA3_TCRL_DFOPT_SAM_INCR        (0x00000000u)
#define EDMA3_TCRL_DFOPT_SAM_FIFO        (0x00000001u)

#define EDMA3_TCRL_DFOPT_RESETVAL        (0x00000000u)

/* DFSRC */

#define EDMA3_TCRL_DFSRC_RESETVAL        (0x00000000u)

/* DFCNT */

#define EDMA3_TCRL_DFCNT_BCNT_MASK       (0xFFFF0000u)
#define EDMA3_TCRL_DFCNT_BCNT_SHIFT      (0x00000010u)
#define EDMA3_TCRL_DFCNT_BCNT_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_DFCNT_ACNT_MASK       (0x0000FFFFu)
#define EDMA3_TCRL_DFCNT_ACNT_SHIFT      (0x00000000u)
#define EDMA3_TCRL_DFCNT_ACNT_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_DFCNT_RESETVAL        (0x00000000u)

/* DFDST */

#define EDMA3_TCRL_DFDST_DADDR_MASK      (0xFFFFFFFFu)
#define EDMA3_TCRL_DFDST_DADDR_SHIFT     (0x00000000u)
#define EDMA3_TCRL_DFDST_DADDR_RESETVAL  (0x00000000u)

#define EDMA3_TCRL_DFDST_RESETVAL        (0x00000000u)

/* DFBIDX */

#define EDMA3_TCRL_DFBIDX_DBIDX_MASK     (0xFFFF0000u)
#define EDMA3_TCRL_DFBIDX_DBIDX_SHIFT    (0x00000010u)
#define EDMA3_TCRL_DFBIDX_DBIDX_RESETVAL (0x00000000u)

#define EDMA3_TCRL_DFBIDX_SBIDX_MASK     (0x0000FFFFu)
#define EDMA3_TCRL_DFBIDX_SBIDX_SHIFT    (0x00000000u)
#define EDMA3_TCRL_DFBIDX_SBIDX_RESETVAL (0x00000000u)

#define EDMA3_TCRL_DFBIDX_RESETVAL       (0x00000000u)

/* DFMPPRXY */

#define EDMA3_TCRL_DFMPPRXY_PRIV_MASK    (0x00000100u)
#define EDMA3_TCRL_DFMPPRXY_PRIV_SHIFT   (0x00000008u)
#define EDMA3_TCRL_DFMPPRXY_PRIV_RESETVAL (0x00000000u)

/*----PRIV Tokens----*/
#define EDMA3_TCRL_DFMPPRXY_PRIV_USER    (0x00000000u)
#define EDMA3_TCRL_DFMPPRXY_PRIV_SUPERVISOR (0x00000001u)

#define EDMA3_TCRL_DFMPPRXY_PRIVID_MASK  (0x0000000Fu)
#define EDMA3_TCRL_DFMPPRXY_PRIVID_SHIFT (0x00000000u)
#define EDMA3_TCRL_DFMPPRXY_PRIVID_RESETVAL (0x00000000u)

#define EDMA3_TCRL_DFMPPRXY_RESETVAL     (0x00000000u)

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif  /* _EDMA3_RL_TC_H_ */
