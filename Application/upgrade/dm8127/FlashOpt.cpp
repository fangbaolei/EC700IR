#include "FlashOpt.h"

int CFlashOpt::node_fd = -1;
int CFlashOpt::subpage_size = 2048;
int CFlashOpt::averbose = 0;
GetDataCallBack CFlashOpt::m_spGetDatafun = NULL;
void *CFlashOpt::m_slpContext = NULL;
bool CFlashOpt::m_sfGetDataFromFifo = false;


int CFlashOpt::WriteUbiImage(const char *szMtdName,unsigned char *pData,int iDataSize)
{
	char szPartition[64];
	sprintf(szPartition,"/dev/%s",szMtdName);

	libmtd_t libmtd;
	struct mtd_info mtd_info;
	//struct mtd_dev_info mtd;
	struct mtd_dev_info mtd = {
		0,0,0,0,"","",0,0,0,0,0,0,0,0,0
	};
	int image_seq = 0;
	int manual_subpage = 0;
	libubi_t libubi;
	struct ubigen_info ui;
	struct ubi_scan_info *si;

	//1.libmtd_open
	libmtd = libmtd_open();
	if (!libmtd)
		return errmsg("MTD subsystem is not present");

	int err;
	int verbose;

	//2.mtd_get_info
	err = mtd_get_info(libmtd, &mtd_info);
	if (err) {
		if (errno == ENODEV)
			errmsg("MTD is not present");
		sys_errmsg("cannot get MTD information");
		goto out_close_mtd;
	}

	//3.mtd_get_dev_info
	err = mtd_get_dev_info(libmtd, szPartition, &mtd);
	if (err) {
		sys_errmsg("cannot get information about \"%s\"", szPartition);
		goto out_close_mtd;
	}

	//4.check subpage_size
	/*sub-page-size,minimum input/output unit used for UBI headers,
	 *e.g. sub-page size in case of NAND flash (equivalent to the 
	 *minimum input/output unit size by default)
	 */
	if (!mtd_info.sysfs_supported) {
	/*
		* Linux kernels older than 2.6.30 did not support sysfs
		* interface, and it is impossible to find out sub-page
		* size in these kernels. This is why users should
		* provide -s option.
		*/
		if (subpage_size == 0) {
			warnmsg("your MTD system is old and it is impossible "
				"to detect sub-page size. Use -s to get rid "
				"of this warning");
			normsg("assume sub-page to be %d", mtd.subpage_size);
		} else {
			mtd.subpage_size = subpage_size;
			manual_subpage = 1;
		}

	} else if (subpage_size && subpage_size != mtd.subpage_size) {
		normsg("Different normal subpage<%d> and get form kernel <%d>",subpage_size,mtd.subpage_size);
		mtd.subpage_size = subpage_size;
		manual_subpage = 1;
	}

	//5.if set subpage size manual, check!
	if (manual_subpage) {
		/* Do some sanity check */
		if (subpage_size > mtd.min_io_size) {
			errmsg("sub-page cannot be larger than min. I/O unit");
			goto out_close_mtd;
		}

		if (mtd.min_io_size % subpage_size) {
			errmsg("min. I/O unit size should be multiple of "
				"sub-page size");
			goto out_close_mtd;
		}
	}

	//6.open partition
	node_fd = open(szPartition, O_RDWR);
	if (node_fd == -1) {
		sys_errmsg("cannot open \"%s\"", szPartition);
		goto out_close_mtd;
	}

	//7.check vid-hdr-offset
	/*vid-hdr-offset, offset if the VID header from start of the 
	 *physical eraseblock (default is the next minimum I/O unit or 
	 *sub-page after the EC header)
	 */

	//8.check partition write able
	if (!mtd.writable) {
		errmsg("mtd%d (%s) is a read-only device", mtd.mtd_num, szPartition);
		goto out_close;
	}

	//9.check if the device is attached to ubi
	/* Make sure this MTD device is not attached to UBI */
	libubi = libubi_open();
	if (libubi) {
		int ubi_dev_num;

		err = mtd_num2ubi_dev(libubi, mtd.mtd_num, &ubi_dev_num);
		libubi_close(libubi);
		if (!err) {
			errmsg("please, first detach mtd%d (%s) from ubi%d",
				mtd.mtd_num, szPartition, ubi_dev_num);
			goto out_close;
		}
	}

	if (/*!args.quiet*/1) {
		normsg_cont("mtd%d (%s), size ", mtd.mtd_num, mtd.type_str);
		ubiutils_print_bytes(mtd.size, 1);
		printf(", %d eraseblocks of ", mtd.eb_cnt);
		ubiutils_print_bytes(mtd.eb_size, 1);
		printf(", min. I/O size %d bytes\n", mtd.min_io_size);
	}

	if (/*args.quiet*/0)
		verbose = 0;
	else if (averbose)
		verbose = 2;
	else
		verbose = 1;

	//10.scan ubi
	err = ubi_scan(&mtd, /*args.*/node_fd, &si, 0/*verbose*/);
	if (err) {
		errmsg("failed to scan mtd%d (%s)", mtd.mtd_num, szPartition);
		goto out_close;
	}

	if (si->good_cnt == 0) {
		errmsg("all %d eraseblocks are bad", si->bad_cnt);
		goto out_free;
	}

	if (si->good_cnt < 2 /*&& (!args.novtbl || args.image)*/) {
		errmsg("too few non-bad eraseblocks (%d) on mtd%d",
			si->good_cnt, mtd.mtd_num);
		goto out_free;
	}

	if (/*!args.quiet*/1) {
		if (si->ok_cnt)			
			normsg("%d eraseblocks have valid erase counter, mean value is %lld",
			si->ok_cnt, si->mean_ec);	//@mean_ec: mean erase counter
		if (si->empty_cnt)
			normsg("%d eraseblocks are supposedly empty", si->empty_cnt);
		if (si->corrupted_cnt)
			normsg("%d corrupted erase counters", si->corrupted_cnt);
		print_bad_eraseblocks(&mtd, si);
	}

	if (si->alien_cnt) {
		if (0/*!args.yes */|| /*!args.quiet*/1)
			warnmsg("%d of %d eraseblocks contain non-ubifs data",
			si->alien_cnt, si->good_cnt);
		if (0/*!args.yes && want_exit()*/) {
			if (1/*args.yes && !args.quiet*/)
				printf("yes\n");
			goto out_free;
		}
	}


	/*ubi-ver,UBI version number to put to EC headers (default is 1)*/
	/*image-seq,32-bit UBI image sequence number to use(by default a random number is picked)*/
	image_seq = rand();
	ubigen_info_init(&ui, mtd.eb_size, mtd.min_io_size, mtd.subpage_size,
		0/*args.vid_hdr_offs*/, 1/*args.ubi_ver*/, image_seq);

	if (si->vid_hdr_offs != -1 && ui.vid_hdr_offs != si->vid_hdr_offs) {
		/*
		 * Hmm, what we read from flash and what we calculated using
		 * min. I/O unit size and sub-page size differs.
		 */
		if (0/*!args.yes*/ || 1/*!args.quiet*/) {
			warnmsg("VID header and data offsets on flash are %d and %d, "
				"which is different to requested offsets %d and %d",
				si->vid_hdr_offs, si->data_offs, ui.vid_hdr_offs,
				ui.data_offs);
			normsg_cont("use new offsets %d and %d? (yes/no)  ",
				    ui.vid_hdr_offs, ui.data_offs);
		}
		if (1/*args.yes || answer_is_yes()*/) {
			if (1/*args.yes*/ && 1/*!args.quiet*/)
				printf("yes\n");
		} else
			ubigen_info_init(&ui, mtd.eb_size, mtd.min_io_size, 0,
					 si->vid_hdr_offs, 1/*args.ubi_ver*/,
					 image_seq);
		normsg("use offsets %d and %d",  ui.vid_hdr_offs, ui.data_offs);
	}
	
	err = flash_image(libmtd, &mtd, &ui, si,pData,iDataSize);
	if (err < 0)
		goto out_free;

	err = format(libmtd, &mtd, &ui, si, err, 1);
	if (err)
		goto out_free;


	ubi_scan_free(si);
	close(/*args.*/node_fd);
	libmtd_close(libmtd);
	return 0;

out_free:
	ubi_scan_free(si);
out_close:
	close(/*args.*/node_fd);
out_close_mtd:
	libmtd_close(libmtd);
	return -1;
}
#define PRINT printf
#define S_OK 0
#define E_FAIL -1

//#define oobblock writesize
#define MAX_OOB_SIZE	64
#define MAX_PAGE_SIZE	2048

unsigned char oobbuf[MAX_OOB_SIZE];
unsigned char oobreadbuf[MAX_OOB_SIZE];
unsigned char writebuf[MAX_PAGE_SIZE];
int CFlashOpt::WriteBinImage(const char *szBlockName,unsigned char *pbData,int iDataSize)
{
	if (false == m_sfGetDataFromFifo && NULL == pbData)
	{
		printf("write bin image args err\n");
		return -1;
	}
	bool fGetDataFromFifo = m_sfGetDataFromFifo;
	m_sfGetDataFromFifo = false;

	char szPartitionName[32] = {0};
	sprintf(szPartitionName,"/dev/%s",szBlockName);


	int fd, imglen = 0, pagelen, baderaseblock, blockstart = -1;
	struct mtd_info_user meminfo;
	loff_t offs;
	int ret = S_OK;
	int readlen = 0;
	unsigned long offset = 0;
	int iRet = 0;

	unsigned char *pbDataRead = pbData;	//读取数据指针
	int iDataFree = iDataSize;		//剩余数据

	int	blockalign = 1; /*default to using 16K block size */

	//int	noecc = 0;
	int	writeoob = 0;
	int	pad = 1;
	imglen = iDataSize;

	if (0 != EraseBlock(szPartitionName))
	{
		PRINT("Err: Erase Block Failed!\n");
		return E_FAIL;
	}

	memset(oobbuf, 0xff, sizeof(oobbuf));
	/* Open the device */
	if ((fd = open(szPartitionName, O_RDWR)) == -1) {
		PRINT("Err: Open Flash %s Failed!\n",szPartitionName);
		ret = E_FAIL;
		goto EXIT_FLASHNAND;
	}
	/* Fill in MTD device capability structure */
	if (ioctl(fd, MEMGETINFO, &meminfo) != 0) {
		PRINT("Err: Iotcl MEMGETINFO Failed,%s!\n",strerror(errno));
		close(fd);
		ret = E_FAIL;
		goto EXIT_FLASHNAND;
	}
	/* Set erasesize to specified number of blocks - to match jffs2 (virtual) block size */
	meminfo.erasesize *= blockalign;

	/* Make sure device page sizes are valid */
    if (!(meminfo.oobsize == 16 && meminfo.writesize == 512) &&
        !(meminfo.oobsize == 8 && meminfo.writesize == 256) &&
        !(meminfo.oobsize == 64 && meminfo.writesize == 2048)) {
			PRINT("Err: Unknown flash (not normal NAND)\n");
			close(fd);
			ret = E_FAIL;
			goto EXIT_FLASHNAND;
	}
    PRINT("Info: Flash oobsize %d,oobblock %d\n",meminfo.oobsize,meminfo.writesize);

	
    pagelen = meminfo.writesize + ((writeoob == 1) ? meminfo.oobsize : 0);
	// Check, if file is pagealigned
	if ((!pad) && ((imglen % pagelen) != 0)) {
		PRINT("Err: Input file is not page aligned\n");
		ret = E_FAIL;
		goto restoreoob;
	}
	// Check, if length fits into device
    if (((imglen / pagelen) * meminfo.writesize) > (meminfo.size - offset)) {
		PRINT("Err: Image %d bytes, NAND page %d bytes, OOB area %u bytes, device size %u bytes\n",
            imglen, pagelen, meminfo.writesize, meminfo.size);
		PRINT("Err: Input file does not fit into device error\n");
		ret = E_FAIL;
		goto restoreoob;
	}
	/* Get data from input and write to the device */
	while (imglen && (offset < meminfo.size)) {
		// new eraseblock , check for bad block(s)
		// Stay in the loop to be sure if the mtdoffset changes because
		// of a bad block, that the next block that will be written to
		// is also checked. Thus avoiding errors if the block(s) after the
		// skipped block(s) is also bad (number of blocks depending on
		// the blockalign
		while (blockstart != (offset & (~meminfo.erasesize + 1))) {
			blockstart = offset & (~meminfo.erasesize + 1);
			offs = blockstart;
			baderaseblock = 0;
			//PRINT("\tWriting data to block %x\n", blockstart);

			/* Check all the blocks in an erase block for bad blocks */
			do {
				if ((iRet = ioctl(fd, MEMGETBADBLOCK, &offs)) < 0) {
					PRINT("Err: Ioctl(MEMGETBADBLOCK) Failed %s\n",strerror(errno));
					ret = E_FAIL;
					goto restoreoob;
				}
				if (iRet == 1) {
					baderaseblock = 1;
					PRINT("Bad block at %x, %u block(s) from %x will be skipped\n", (int) offs, blockalign, blockstart);
				}

				if (baderaseblock) {
					offset = blockstart + meminfo.erasesize;
				}
				offs +=  meminfo.erasesize / blockalign ;
			} while ( offs < blockstart + meminfo.erasesize );

		}



        readlen = meminfo.writesize;
		if (pad && (imglen < readlen))
		{
			readlen = imglen;
            memset(writebuf + readlen, 0xff, meminfo.writesize - readlen);
		}

		int cpylen = readlen > iDataFree ? iDataFree : readlen;

		if (true == fGetDataFromFifo)
		{
			if (m_spGetDatafun(writebuf,cpylen,m_slpContext) != cpylen)
			{
				PRINT("Err: Get Data From Fifo Failed!\n");
				ret = E_FAIL;
				goto restoreoob;
			}
		}
		else
		{
			memcpy(writebuf,pbDataRead,cpylen);
			pbDataRead += cpylen;
		}

		iDataFree -= cpylen;

		if(readlen != cpylen)
		{
			if(0 == cpylen)	//已经没有数据可读
			{
				break;
			}
			PRINT("File I/O error on input file\n");
			ret = E_FAIL;
			goto restoreoob;
		}
        if (1/*!args.quiet*/ && !averbose)
        {
			printf("\rflashing offset %d -- %2lld %% complete  ",
                offset, (long long)(offset + meminfo.writesize) * 100 / iDataSize);
			fflush(stdout);
		}
		/* Write out the Page data */
		int iRet = pwrite(fd, writebuf, meminfo.writesize, offset);
		if (iRet != meminfo.writesize) 
		{
			PRINT("Err: pwrite Failed,%s\n",strerror(errno));
			ret = E_FAIL;
			goto restoreoob;
		}

		imglen -= readlen;
        offset += meminfo.writesize;
	}

restoreoob:
	close(fd);
	printf("\n");

	if (imglen > 0) {
		PRINT ("Err: Data did not fit into device, due to bad blocks\n");
		ret = E_FAIL;
	}
	PRINT("Info: Write Mtd %s Finish!\n",szBlockName);

EXIT_FLASHNAND:
	return ret;
}


int CFlashOpt::EraseBlock(const char *szBlockName)
{	
	unsigned long long start = 0;
	int isNAND = 1;
    static int noskipbad = 1;		/* do not skip bad blocks */
	off_t offset = 0;
	unsigned int eb, eb_start, eb_cnt;
	struct mtd_dev_info mtd = {
		0,0,0,0,"","",0,0,0,0,0,0,0,0,0
	};

	libmtd_t mtd_desc;
	char mtd_device[64];
	strcpy(mtd_device,szBlockName);

	int fd = -1;
	int hRet = S_OK;
	//char write_mode = MTD_OPS_RAW;

	mtd_desc = libmtd_open();
	if (!mtd_desc)
	{
		PRINT("Err: can't initialize libmtd\n");
		return E_FAIL;
	}

	/* Open the device */
	if ((fd = open(mtd_device, O_RDWR)) < 0)
	{
		PRINT("Err: open %s failed\n", mtd_device);
		libmtd_close(mtd_desc);
		return E_FAIL;
	}

	/* Fill in MTD device capability structure */
	if (mtd_get_dev_info(mtd_desc, mtd_device, &mtd) < 0)
	{
		PRINT("Err: mtd_get_dev_info failed\n");
		hRet = E_FAIL;
		goto closeall;
	}

	PRINT("Info: mtd oobsize %d,subpagesize %d,ebsize %d,miniosize %d\n",
		mtd.oob_size,mtd.subpage_size,mtd.eb_size,mtd.min_io_size);

	isNAND = mtd.type == MTD_NANDFLASH ? 1 : 0;
	eb_start = start / mtd.eb_size;
	eb_cnt = (mtd.size / mtd.eb_size) - eb_start;

	for (eb = eb_start; eb < eb_start + eb_cnt; eb++) {
		offset = (off_t)eb * mtd.eb_size;
		//skip bad
		if (!noskipbad) {
			int ret = mtd_is_bad(&mtd, fd, eb);
			if (ret > 0) {
				//verbose(!quiet, "Skipping bad block at %08"PRIxoff_t, offset);
				continue;
			} else if (ret < 0) {
				if (errno == EOPNOTSUPP) {
					noskipbad = 1;
					if (isNAND)
					{
						PRINT("Err: %s: Bad block check not available", mtd_device);
						hRet = E_FAIL;
						goto closeall;
					}
				} 
				else{
					PRINT("Err: %s: MTD get bad block failed", mtd_device);
					hRet = E_FAIL;
					goto closeall;
				}
			}
		}

		if (mtd_erase(mtd_desc, &mtd, fd, eb) != 0) {
			PRINT("Err: %s: MTD Erase failure\n", mtd_device);
			continue;
		}
	}

closeall:
	close(fd);
	libmtd_close(mtd_desc);


	return hRet;
}
int CFlashOpt::format(libmtd_t libmtd, const struct mtd_dev_info *mtd,
	const struct ubigen_info *ui, struct ubi_scan_info *si,
	int start_eb, int novtbl)
{
	int eb, err, write_size;
	struct ubi_ec_hdr *hdr;
	struct ubi_vtbl_record *vtbl;
	int eb1 = -1, eb2 = -1;
	long long ec1 = -1, ec2 = -1;

	write_size = UBI_EC_HDR_SIZE + mtd->subpage_size - 1;
	write_size /= mtd->subpage_size;
	write_size *= mtd->subpage_size;
	hdr = (ubi_ec_hdr*)malloc(write_size);
	if (!hdr)
		return sys_errmsg("cannot allocate %d bytes of memory", write_size);
	memset(hdr, 0xFF, write_size);

	for (eb = start_eb; eb < mtd->eb_cnt; eb++) {
		long long ec;

		if (1/*!args.quiet*/ && !averbose) {
			printf("\rformatting eraseblock %d -- %2lld %% complete  ",
				eb, (long long)(eb + 1 - start_eb) * 100 / (mtd->eb_cnt - start_eb));
			fflush(stdout);
		}

		if (si->ec[eb] == EB_BAD)
			continue;

		if (0/*args.override_ec*/)
			/*ec = args.ec*/;
		else if (si->ec[eb] <= EC_MAX)
			ec = si->ec[eb] + 1;
		else
			ec = si->mean_ec;
		ubigen_init_ec_hdr(ui, hdr, ec);

		if (averbose) {
			normsg_cont("eraseblock %d: erase", eb);
			fflush(stdout);
		}

		err = mtd_erase(libmtd, mtd, /*args.*/node_fd, eb);
		if (err) {
			if (1/*!args.quiet*/)
				printf("\n");

			sys_errmsg("failed to erase eraseblock %d", eb);
			if (errno != EIO)
				goto out_free;

			if (mark_bad(mtd, si, eb))
				goto out_free;
			continue;
		}

		if ((eb1 == -1 || eb2 == -1) && !novtbl) {
			if (eb1 == -1) {
				eb1 = eb;
				ec1 = ec;
			} else if (eb2 == -1) {
				eb2 = eb;
				ec2 = ec;
			}
			if (averbose)
				printf(", do not write EC, leave for vtbl\n");
			continue;
		}

		if (averbose) {
			printf(", write EC %lld\n", ec);
			fflush(stdout);
		}
		err = mtd_write(libmtd, mtd, /*args.*/node_fd, eb, 0, hdr,
			write_size, NULL, 0, 0);
		if (err) {
			if (1/*!args.quiet */&& !averbose)
				printf("\n");
			sys_errmsg("cannot write EC header (%d bytes buffer) to eraseblock %d",
				write_size, eb);

			if (errno != EIO) {
				if (!/*args.*/subpage_size != mtd->min_io_size)
					normsg("may be sub-page size is "
					"incorrect?");
				goto out_free;
			}

			err = mtd_torture(libmtd, mtd, /*args.*/node_fd, eb);
			if (err) {
				if (mark_bad(mtd, si, eb))
					goto out_free;
			}
			continue;

		}
	}

	if (1/*!args.quiet */&& !averbose)
		printf("\n");

	if (!novtbl) {
		if (eb1 == -1 || eb2 == -1) {
			errmsg("no eraseblocks for volume table");
			goto out_free;
		}

		verbose(averbose, "write volume table to eraseblocks %d and %d", eb1, eb2);
		vtbl = ubigen_create_empty_vtbl(ui);
		if (!vtbl)
			goto out_free;

		err = ubigen_write_layout_vol(ui, eb1, eb2, ec1,  ec2, vtbl,
			/*args.*/node_fd);
		free(vtbl);
		if (err) {
			errmsg("cannot write layout volume");
			goto out_free;
		}
	}

	free(hdr);
	return 0;

out_free:
	free(hdr);
	return -1;
}



int CFlashOpt::flash_image(libmtd_t libmtd, const struct mtd_dev_info *mtd,
		       const struct ubigen_info *ui, struct ubi_scan_info *si, 
			   unsigned char *pData,int iLen)
{
	if (false == m_sfGetDataFromFifo && NULL == pData)
	{
		printf("flash image args err\n");
		return -1;
	}
	bool fGetDataFromFifo = m_sfGetDataFromFifo;
	m_sfGetDataFromFifo = false;

	int /*fd, */img_ebs, eb, written_ebs = 0, divisor, skip_data_read = 0;
	off_t st_size = iLen;
	img_ebs = st_size / mtd->eb_size;
	
	unsigned char *pRead = pData;
	char *buf = NULL;
	int iReadCnt = 0;

	if (img_ebs > si->good_cnt) {
		sys_errmsg("file  is too large (%lld bytes)",
			    (long long)st_size);
		goto out_close;
	}

	if (st_size % mtd->eb_size) {
		return sys_errmsg("file  (size %lld bytes) is not multiple of ""eraseblock size (%d bytes)",
				  (long long)st_size, mtd->eb_size);
		goto out_close;
	}

	verbose(averbose, "will write %d eraseblocks", img_ebs);

	divisor = img_ebs;
	buf = (char *)malloc(mtd->eb_size);
	if (NULL == buf)
	{
		printf("malloc buf failed\n");
		goto out_close;
	}
	for (eb = 0; eb < mtd->eb_cnt; eb++) {
		int err, new_len;
		//char buf[mtd->eb_size];
		memset(buf,0x00,mtd->eb_size);

		long long ec;

		if (1/*!args.quiet*/ && !averbose) {
			printf("\rflashing eraseblock %d -- %2lld %% complete  ",
			       eb, (long long)(eb + 1) * 100 / divisor);
			fflush(stdout);
		}

		if (si->ec[eb] == EB_BAD) {
			divisor += 1;
			continue;
		}

		if (averbose) {
			normsg_cont("eraseblock %d: erase", eb);
			fflush(stdout);
		}

		err = mtd_erase(libmtd, mtd, /*args.*/node_fd, eb);
		if (err) {
			if (1/*!args.quiet*/)
				printf("\n");
			sys_errmsg("failed to erase eraseblock %d", eb);

			if (errno != EIO)
				goto out_close;

			if (mark_bad(mtd, si, eb))
				goto out_close;

			continue;
		}

		if (!skip_data_read) {

			int iFreeLen = iLen - iReadCnt;
			if (iFreeLen < 0)
			{
				sys_errmsg("read to the buf end!!!");
				goto out_close;
			}
			int iReadLen = mtd->eb_size < iFreeLen?mtd->eb_size:iFreeLen;

			if (true == fGetDataFromFifo)
			{
				if (m_spGetDatafun(buf,iReadLen,m_slpContext) != iReadLen)
				{
					sys_errmsg("get data from fifo failed!!");
					goto out_close;
				}
				iReadCnt += iReadLen;
			}
			else
			{
				memcpy(buf,pRead,iReadLen);
				pRead += iReadLen;
				iReadCnt += iReadLen;
			}
		}
		skip_data_read = 0;

		if (0/*args.override_ec*/)
			/*ec = args.ec*/;
		else if (si->ec[eb] <= EC_MAX)
			ec = si->ec[eb] + 1;
		else
			ec = si->mean_ec;

		if (averbose) {
			printf(", change EC to %lld", ec);
			fflush(stdout);
		}
		err = change_ech((struct ubi_ec_hdr *)buf, ui->image_seq, ec);
		if (err) {
			errmsg("bad EC header at eraseblock %d of file",
			       written_ebs);
			goto out_close;
		}

		if (averbose) {
			printf(", write data\n");
			fflush(stdout);
		}
		new_len = drop_ffs(mtd, buf, mtd->eb_size);

		err = mtd_write(libmtd, mtd, /*args.*/node_fd, eb, 0, buf, new_len,
				NULL, 0, 0);
		if (err) {
			sys_errmsg("cannot write eraseblock %d", eb);

			if (errno != EIO)
				goto out_close;

			err = mtd_torture(libmtd, mtd, /*args.*/node_fd, eb);
			if (err) {
				if (mark_bad(mtd, si, eb))
					goto out_close;
			}
			/*
			 * We have to make sure that we do not read next block
			 * of data from the input image or stdin - we have to
			 * write buf first instead.
			 */
			skip_data_read = 1;
			continue;
		}
		if (++written_ebs >= img_ebs)
			break;
	}

	if (1/*!args.quiet*/ && !averbose)
		printf("\n");
	//close(fd);
	free(buf);
	return eb + 1;

out_close:
	//close(fd);
	free(buf);
	return -1;
}

int CFlashOpt::change_ech(struct ubi_ec_hdr *hdr, uint32_t image_seq,
	long long ec)
{
	uint32_t crc;

	/* Check the EC header */
	if (be32_to_cpu(hdr->magic) != UBI_EC_HDR_MAGIC)
		return errmsg("bad UBI magic %#08x, should be %#08x",
		be32_to_cpu(hdr->magic), UBI_EC_HDR_MAGIC);

	crc = mtd_crc32(UBI_CRC32_INIT, hdr, UBI_EC_HDR_SIZE_CRC);
	if (be32_to_cpu(hdr->hdr_crc) != crc)
		return errmsg("bad CRC %#08x, should be %#08x\n",
		crc, be32_to_cpu(hdr->hdr_crc));

	hdr->image_seq = cpu_to_be32(image_seq);
	hdr->ec = cpu_to_be64(ec);
	crc = mtd_crc32(UBI_CRC32_INIT, hdr, UBI_EC_HDR_SIZE_CRC);
	hdr->hdr_crc = cpu_to_be32(crc);

	return 0;
}


/* TODO: we should actually torture the PEB before marking it as bad */
int CFlashOpt::mark_bad(const struct mtd_dev_info *mtd, struct ubi_scan_info *si, int eb)
{
	int err;

	if (0/*!args.yes*/) {
		normsg_cont("mark it as bad? Continue (yes/no) ");
		//if (!answer_is_yes())
			return -1;
	}

	if (1/*!args.quiet*/)
		normsg_cont("marking block %d bad", eb);

	if (1/*!args.quiet*/)
		printf("\n");

	if (!mtd->bb_allowed) {
		if (1/*!args.quiet*/)
			printf("\n");
		return errmsg("bad blocks not supported by this flash");
	}

	err = mtd_mark_bad(mtd,/* args.*/node_fd, eb);
	if (err)
		return err;

	si->bad_cnt += 1;
	si->ec[eb] = EB_BAD;

	return consecutive_bad_check(eb);
}


/*
 * Returns %-1 if consecutive bad blocks exceeds the
 * MAX_CONSECUTIVE_BAD_BLOCKS and returns %0 otherwise.
 */
int CFlashOpt::consecutive_bad_check(int eb)
{
	static int consecutive_bad_blocks = 1;
	static int prev_bb = -1;

	if (prev_bb == -1)
		prev_bb = eb;

	if (eb == prev_bb + 1)
		consecutive_bad_blocks += 1;
	else
		consecutive_bad_blocks = 1;

	prev_bb = eb;

	if (consecutive_bad_blocks >= MAX_CONSECUTIVE_BAD_BLOCKS) {
		if (1/*!args.quiet*/)
			printf("\n");
		return errmsg("consecutive bad blocks exceed limit: %d, bad flash?",
		              MAX_CONSECUTIVE_BAD_BLOCKS);
	}

	return 0;
}

int CFlashOpt::drop_ffs(const struct mtd_dev_info *mtd, const void *buf, int len)
{
	int i;

	for (i = len - 1; i >= 0; i--)
		if (((const uint8_t *)buf)[i] != 0xFF)
			break;

	/* The resulting length must be aligned to the minimum flash I/O size */
	len = i + 1;
	len = (len + mtd->min_io_size - 1) / mtd->min_io_size;
	len *=  mtd->min_io_size;
	return len;
}

void CFlashOpt::print_bad_eraseblocks(const struct mtd_dev_info *mtd,
	const struct ubi_scan_info *si)
{
	int first = 1, eb;

	if (si->bad_cnt == 0)
		return;

	normsg_cont("%d bad eraseblocks found, numbers: ", si->bad_cnt);
	for (eb = 0; eb < mtd->eb_cnt; eb++) {
		if (si->ec[eb] != EB_BAD)
			continue;
		if (first) {
			printf("%d", eb);
			first = 0;
		} else
			printf(", %d", eb);
	}
	printf("\n");
}


void CFlashOpt::ubiutils_print_bytes(long long bytes, int bracket)
{
	const char *p;

	if (bracket)
		p = " (";
	else
		p = ", ";

	printf("%lld bytes", bytes);

	if (bytes > 1024 * 1024 * 1024)
		printf("%s%.1f GiB", p, (double)bytes / (1024 * 1024 * 1024));
	else if (bytes > 1024 * 1024)
		printf("%s%.1f MiB", p, (double)bytes / (1024 * 1024));
	else if (bytes > 1024 && bytes != 0)
		printf("%s%.1f KiB", p, (double)bytes / 1024);
	else
		return;

	if (bracket)
		printf(")");
}