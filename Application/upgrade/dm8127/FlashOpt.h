#ifndef __FLASH_OPT_H__
#define __FLASH_OPT_H__

#define PROGRAM_NAME "UpgradeUbi"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <errno.h>

#include <sys/ioctl.h>
extern "C"
{
#include "libubi.h"
#include "libmtd.h"
#include "libscan.h"
#include "libubigen.h"

#include "mtd_swab.h"
#include "common.h"
#include "ubiutils-common.h"
#include "crc32.h"

#include "mtd/mtd-abi.h"
};

#define MAX_CONSECUTIVE_BAD_BLOCKS 4

typedef int (*GetDataCallBack)(void *buf,unsigned int len,void *lp);

class CFlashOpt
{
public:

	//写UBI镜像到FLASH
	static int WriteUbiImage(const char *szMtdName,unsigned char *pData,int iDataSize);

	//写二进制镜像到FLASH,比如U-Boot、Kernel
	static int WriteBinImage(const char *szMtdName,unsigned char *pData,int iDataSize);
	
	//擦除分区
	static int EraseBlock(const char *szBlockName);

	//注册回调函数，获取镜像数据
	static void RegCallBackFunc(GetDataCallBack fun,void *lp)
	{
		m_spGetDatafun = fun;
		m_slpContext = lp;
		m_sfGetDataFromFifo = true;
	};
	
private:
	static int format(libmtd_t libmtd, const struct mtd_dev_info *mtd,
		const struct ubigen_info *ui, struct ubi_scan_info *si,
		int start_eb, int novtbl);

	static int flash_image(libmtd_t libmtd, const struct mtd_dev_info *mtd,
		const struct ubigen_info *ui, struct ubi_scan_info *si, 
		unsigned char *pData,int iLen);

	static int change_ech(struct ubi_ec_hdr *hdr, uint32_t image_seq,
		long long ec);

	static int mark_bad(const struct mtd_dev_info *mtd, struct ubi_scan_info *si, int eb);

	static int consecutive_bad_check(int eb);

	static int drop_ffs(const struct mtd_dev_info *mtd, const void *buf, int len);

	static void print_bad_eraseblocks(const struct mtd_dev_info *mtd,
		const struct ubi_scan_info *si);

	static void ubiutils_print_bytes(long long bytes, int bracket);

private:

	static int node_fd;

	static int subpage_size;	//页大小
	static int averbose;		//是否输出详细日志

	//获取数据回调函数
	static GetDataCallBack m_spGetDatafun;	

	//获取数据上下文参数
	static void *m_slpContext;

	static bool m_sfGetDataFromFifo;
};

#endif