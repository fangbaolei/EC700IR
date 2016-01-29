

#include "SWFC.h"
#include "SWLog.h"
#include "drv_device.h"
#include "Tester.h"
#include "SWUtils.h"

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "libmtd.h"
#include "linux/types.h"
#include "mtd/mtd-user.h"


#define FPGA_ID_ADR   0x900     //
#define FPGA_VERSION  0x56425F02      //VT_02



static BOOL _fForceStop = FALSE;


CTester::CTester()
{}

CTester::~CTester()
{}


HRESULT CTester::SetForceStopFlag(const BOOL fFlag)
{
	SW_TRACE_NORMAL("Info: SetForceStopFlag to %s \n", fFlag ? "TRUE" : "FALSE");
	_fForceStop = fFlag;
}

BOOL CTester::GetForceStopFlag()
{
	//SW_TRACE_NORMAL("Info: ForceStopFlag = %s \n", _fForceStop ? "TRUE" : "FALSE");
	return _fForceStop;
}
	



/**
* @brief NAND FLASH的读写测试，把DDR中一段数据写到NAND FLASH中，接着
		  读到DDR的另一空间，进行比较。每个block单独测试。
		
* @retval S_OK表示SPI FLASH测试成功.
* @retval E_FAIL表示硬件底层操作出错.
*/
HRESULT CTester::TestFlash()
{
	/*	
	读写NAND FLASH 的block，进行测试:
	① 待测的NAND FLASH容量为512K字节（FLASH总容量为512MB，测试保留区域），
		测试时将测试区域划分为4个block，每个block为64K字（128KB）；
	② 先对被测block进行擦除操作，
		然后往block中的每个单元（16位，每个block有64K个16位单元）写入其对应的地址偏移量。
		写完一个block后，把block内的每个单元读出来跟写入的数据进行比较。
	*/
	
	struct mtd_dev_info mtd = {
		0,0,0,0,"","",0,0,0,0,0,0,0,0,0
	};
	libmtd_t mtd_desc;
    static const char	*mtd_device = "/dev/mtd11";	//测试专用分区
	int fd = -1;
	int ret = S_OK;
	unsigned char write_mode = MTD_OPS_RAW;
	
	//const DWORD dwTotalSize = 256*1024*1024;
	const int pagelen = 2048;	//?定值？
	const DWORD blockSize = pagelen * 64;
	//const DWORD dwTestblockstart = 0xFD80000;	//硬件地址？
	//const DWORD dwTestAddr = 0xFD80000;
	const DWORD dwTestblockstart = 0x0;	//分区开始
	const DWORD dwTestAddr = 0x0;
	
	const DWORD dwTestSize = 512*1024;//512KByte
	//DWORD dwLen = dwTestSize;
	DWORD dwblockCount = dwTestSize/blockSize;
	DWORD mtdoffset = 0;
	
	
	//unsigned char writebuf[128 *1024];
	//unsigned char readbuf[128 *1024];
	unsigned char *writebuf = (unsigned char *)swpa_mem_alloc(128 *1024);
	if (NULL == writebuf)
	{
		SW_TRACE_NORMAL("Err: malloc write buf failed!");
		return E_OUTOFMEMORY;
	}
	unsigned char *readbuf = (unsigned char *)swpa_mem_alloc(128 *1024);
	if (NULL == readbuf)
	{
		SW_TRACE_NORMAL("Err: malloc read buf failed!");
		SAFE_MEM_FREE(writebuf);
		return E_OUTOFMEMORY;
	}
	unsigned short *wr_ptr16 = (unsigned short* )(writebuf);
	//unsigned short *rd_ptr16 = (unsigned short* )(readbuf);
	unsigned int i = 0,j=0;
	
	//create test data
	for(i = 0; i < ( 64 * 1024); i++)
	{
		wr_ptr16[i] = i; 
	}
	
	
	SW_TRACE_NORMAL("Info: Testing NAND Flash...............\n");
	
	/* Open the device */
	if ((fd = open(mtd_device, O_RDWR)) < 0)
	{
		SW_TRACE_NORMAL("%s\n", mtd_device);
		//close(fd);	
		SAFE_MEM_FREE(readbuf);
		SAFE_MEM_FREE(writebuf);
		return E_FAIL;
	}
	
	mtd_desc = libmtd_open();
	if (!mtd_desc)
	{
		SW_TRACE_NORMAL("can't initialize libmtd\n");
		//libmtd_close(mtd_desc);
		close(fd);	
		SAFE_MEM_FREE(readbuf);
		SAFE_MEM_FREE(writebuf);
		return E_FAIL;
	}
	
	/* Fill in MTD device capability structure */
	if (mtd_get_dev_info(mtd_desc, mtd_device, &mtd) < 0)
	{
		SW_TRACE_NORMAL("mtd_get_dev_info failed");
		libmtd_close(mtd_desc);
		close(fd);
		SAFE_MEM_FREE(readbuf);
		SAFE_MEM_FREE(writebuf);
		return E_FAIL;
	}
	
	SW_TRACE_NORMAL("Info: mtd oobsize %d,subpagesize %d,ebsize %d,miniosize %d\n",
		mtd.oob_size,mtd.subpage_size,mtd.eb_size,mtd.min_io_size);
	
	//erase block	
	//@eb_size: eraseblock size
	
	for (i = dwTestblockstart; i < dwTestblockstart + dwTestSize; i += mtd.eb_size) {
		//skip bad?
	
		if (mtd_erase(mtd_desc, &mtd, fd, i / mtd.eb_size)) {
			SW_TRACE_NORMAL("%s: MTD Erase failure\n", mtd_device);
			ret = E_FAIL;
			goto closeall;		
		}
	}
	
	// begin test NAND FLASH write and read.
	for(i = 0 ; i < dwblockCount; i++)
	{

		if (GetForceStopFlag())
		{
			return S_OK;
		}
		
		/*  write test data to a block*/
		j = 0;
		mtdoffset = dwTestAddr + blockSize * i;
		while ( j < blockSize)
		{
			//@min_io_size: minimum input/output unit size
			if( mtd_write(mtd_desc, &mtd, fd, mtdoffset / mtd.eb_size,
				mtdoffset % mtd.eb_size,&writebuf[j],mtd.min_io_size,NULL,0,write_mode) )
			{	
				SW_TRACE_NORMAL("%s: MTD write failure\n", mtd_device);
				ret = E_FAIL;	
				goto closeall;
			}
								
			mtdoffset += mtd.min_io_size;
			j += pagelen;						
		}
		
		/* read test data from a block*/
		memset(readbuf, 0x00, blockSize);	
		j = 0;
		mtdoffset = dwTestAddr + blockSize * i;
		while ( j < blockSize)
		{
			if( mtd_read(&mtd, fd, mtdoffset / mtd.eb_size, mtdoffset % mtd.eb_size, &readbuf[j], mtd.min_io_size))
			{
				SW_TRACE_NORMAL("MTD read failure.\n");
				ret = E_FAIL;
				goto closeall;
			}

			mtdoffset += mtd.min_io_size;
			j += pagelen;
		}

		if (GetForceStopFlag())
		{
			return S_OK;
		}
		
		/* cheeck the data */
		for(j = 0 ; j < blockSize ; j++)
		{
			if( readbuf[j] != writebuf[j] )
			{
				SW_TRACE_NORMAL("data check failure.\n");
				ret = E_FAIL;
				goto closeall;
			}
		}	
	
	}
	
closeall:
	libmtd_close(mtd_desc);
	close(fd);	
	
	SAFE_MEM_FREE(readbuf);
	SAFE_MEM_FREE(writebuf);

    swpa_utils_shell("flash_eraseall /dev/mtd11", NULL);

	if(ret == E_FAIL)
	{	
		SW_TRACE_NORMAL("Info: NAND Flash Test -- NG!\n");
	}else{
		SW_TRACE_NORMAL("Info: NAND Flash Test -- OK!\n");
	}
			
	return ret;
}


/**
* @brief ：I2C EEPROM读写测试，读写EEPROM的每一页，一共测试8页

* @retval S_OK表示SPI FLASH的ID检测成功.
* @retval E_FAIL表示硬件底层操作出错.
*/
HRESULT CTester::TestEEPROM()
{
	/*
	待测的I2C EEPROM容量为2KB（总容量为128KB），
	测试时划分为8页，每页为256字节；
	对每页进行读写测试，往每页的每个单元写入该单元对应的地址偏移量，
	写完一页后，一一读出该页的每个单元，看是否正确。
	*/
	HRESULT hRet = S_OK;
	const DWORD dwTotalSize = 128*1024;
	const DWORD dwTestSize = 1*1024;
	//DWORD dwLen = dwTestSize;
	DWORD dwAddr = dwTotalSize - dwTestSize;  //the start address 
	const DWORD dwPageSize = 256; 
	DWORD dwPageCount = dwTestSize/dwPageSize;

	PBYTE pbWriteBuf = (PBYTE) swpa_mem_alloc(dwPageSize);
	if (NULL == pbWriteBuf)
	{
		SW_TRACE_NORMAL("Err: no memory for pbWriteBuf\n");
		return E_OUTOFMEMORY;
	}
	for (DWORD i = 0; i < dwPageSize; i++)
	{
		pbWriteBuf[i] = i%256;
	}
	PBYTE pbReadBuf = (PBYTE) swpa_mem_alloc(dwPageSize);
	if (NULL == pbReadBuf)
	{
		SAFE_MEM_FREE(pbWriteBuf);
		SW_TRACE_NORMAL("Err: no memory for pbReadBuf\n");
		return E_OUTOFMEMORY;
	}	

	
	SW_TRACE_NORMAL("Info: Testing I2C EEPROM...\n");
	
	for (DWORD dwPage = 0; dwPage < dwPageCount; dwPage++)
	{
		UINT uLen = dwPageSize;
		DWORD dwPageStart = dwAddr + dwPage*dwPageSize;
		DWORD dwPageEnd = dwPageStart + dwPageSize - 1;

		if (GetForceStopFlag())
		{
			return S_OK;
		}

		//写数据
		SW_TRACE_NORMAL("Info: writing EEPROM (%#x~%#x)\n", dwPageStart, dwPageEnd);
		if (0 != drv_eeprom_write(DEVID_IIC_EEPROM, dwPageStart, pbWriteBuf, &uLen)
			|| dwPageSize != uLen)
		{
			SW_TRACE_NORMAL("Err: failed to write EEPROM (%#x ~ %#x)\n", dwPageStart, dwPageEnd);
			hRet = E_FAIL;
			goto FreeMem;
		}

		if (GetForceStopFlag())
		{
			return S_OK;
		}
		
		//读数据
		SW_TRACE_NORMAL("Info: reading EEPROM (%#x~%#x)\n", dwPageStart, dwPageEnd);
		swpa_memset(pbReadBuf, 0, dwPageSize);
		uLen = dwPageSize;
		if (0 != drv_eeprom_read(DEVID_IIC_EEPROM, dwPageStart, pbReadBuf, &uLen)
			|| dwPageSize != uLen)
		{
			SW_TRACE_NORMAL("Err: failed to read EEPROM (%#x ~ %#x)\n", dwPageStart, dwPageEnd);
			hRet = E_FAIL;
			goto FreeMem;
		}

		if (0 != swpa_memcmp(pbReadBuf, pbWriteBuf, dwPageSize))
		{
			SW_TRACE_NORMAL("Err: EEPROM (%#x ~ %#x) test failed!\n", dwPageStart, dwPageEnd);
			hRet = E_FAIL;
			goto FreeMem;
		}
		SW_TRACE_NORMAL("Info: EEPROM (%#x ~ %#x) OK!\n", dwPageStart, dwPageEnd);
	}
	
	SW_TRACE_NORMAL("Info: I2C EEPROM Test -- OK!\n");	

FreeMem:

	SAFE_MEM_FREE(pbReadBuf);
	SAFE_MEM_FREE(pbWriteBuf);

	return hRet;
}


/**
* @brief ：RTC测试，随时间变化读取RTC时间值

* @retval S_OK:  成功进行测试	
* @retval E_FAIL: 测试失败
*/
HRESULT CTester::TestRTC()
{
	/*
	(1)设置RTC的初始时间；
	(2)每隔一秒读取一次RTC时间，看时间是否正确递增。
	*/
	
	SW_TRACE_NORMAL("Info: Testing DM8127 RTC...\n");

	HRESULT hr = S_OK;
	SWPA_TIME sTestTime = {0x1, 0x1};
	SWPA_TIME sCurTime;
	DWORD dwPrevTick=0, dwTick=0;

	if (SWPAR_OK != swpa_datetime_gettime(&sCurTime))
	{
		SW_TRACE_DEBUG("Err: get time failed!\n");
		SW_TRACE_NORMAL("Info: DM8127 RTC Test ..... NG!\n");		
		return E_FAIL;
	}

	if (SWPAR_OK != swpa_datetime_settime(&sTestTime))
	{
		SW_TRACE_DEBUG("Err: set time failed!\n");
		SW_TRACE_NORMAL("Info: DM8127 RTC Test ............ NG!\n");		
		return E_FAIL;
	}
	
	dwPrevTick = swpa_datetime_gettick();

	for (DWORD i=0; i < 5; i++)
	{		
		if (GetForceStopFlag())
		{
			SW_TRACE_DEBUG("Info: Test is forced to stop!\n");
			break;
		}

		SWPA_TIME sPrevTime = sTestTime;
		
		swpa_thread_sleep_ms(1000);

		if (SWPAR_OK != swpa_datetime_gettime(&sTestTime))
		{
			SW_TRACE_DEBUG("Err: get time failed!\n");
			hr = E_FAIL;
			break;
		}

		if (1000 > (sTestTime.sec*1000+sTestTime.msec)-(sPrevTime.sec*1000+sPrevTime.msec))
		{
			hr = E_FAIL;
			break;
		}
	}

	dwTick = swpa_datetime_gettick();

	sCurTime.sec += (sCurTime.msec + dwTick - dwPrevTick) / 1000;
	sCurTime.msec = (sCurTime.msec + dwTick - dwPrevTick) % 1000;

	if (SWPAR_OK != swpa_datetime_settime(&sCurTime))
	{
		SW_TRACE_DEBUG("Err: set time failed!\n");
		SW_TRACE_NORMAL("Info: DM8127 RTC Test .............................. NG!\n");		
		return E_FAIL;
	}
	
	SW_TRACE_NORMAL("Info: DM8127 RTC Test ............................................................................ %s!\n",
		FAILED(hr) ? "NG" : "OK");	
	
	return hr;
}



/**
* @brief ：LM75测试，读取当前温度

* @retval S_OK:  成功进行测试	
* @retval E_FAIL: 测试失败
*/
HRESULT CTester::TestLM75()
{
	/*
	读取LM75的温度值，看是否与其周围的温度相符。
	*/
	
	SW_TRACE_NORMAL("Info: Testing LM75...\n");
	
	INT iTemperature = 0;
	if (0 != drv_device_get_temperature(DEVID_IIC_LM75, &iTemperature))
	{
		SW_TRACE_NORMAL("Info: LM75 Test -- NG!\n");
		return E_FAIL;
	}
	
	SW_TRACE_NORMAL("Info: The temperature is %03d C\n", iTemperature);	
	
	SW_TRACE_NORMAL("Info: LM75 Test -- OK!\n");

	return S_OK;
}


/**
* @brief ：AT88SC测试区的基本测试用来检验基本I2C通讯

* @param [in] : TestByte : 传入的测试数据
* @retval S_OK:  成功进行测试	
* @retval E_FAIL: 测试失败
*/
HRESULT CTester::TestAT88SCOp(const BYTE bTestData)
{
	const UINT uAddr = 0x0A; //	先用标准写接口，往0x0a地址写一个字节的数据，再用标准读接口，读出0x0a地址，看是否是之前写入的值
	BYTE bData = bTestData;
	DWORD dwLen = sizeof(bData);

	swpa_thread_sleep_ms(200);
	
	//SW_TRACE_NORMAL("info: dwLen = %u\n", dwLen);
	if (0 != drv_sec_std_write(DEVID_IIC_AT88SC, uAddr, &bData, &dwLen)
		|| sizeof(bData) != dwLen)
	{
		//SW_TRACE_NORMAL("info: dwLen = %u\n", dwLen);
		SW_TRACE_NORMAL("Err: failed to write IIC AT88SC @ %#X\n", uAddr);
		return E_FAIL;
	}
	//SW_TRACE_NORMAL("info: dwLen = %u\n", dwLen);

	swpa_thread_sleep_ms(100);
	
	bData = ~bData;
	if (0 != drv_sec_std_read(DEVID_IIC_AT88SC, uAddr, &bData, &dwLen)
		|| sizeof(bData) != dwLen
		|| bTestData != bData
		)
	{
		SW_TRACE_NORMAL("Err: failed to read IIC AT88SC @ %#x\n", uAddr);
		return E_FAIL;
	}

	return S_OK;
}


/**
* @brief ：AT88SC非加密读写测试

* @retval S_OK:  成功进行测试	
* @retval E_FAIL: 测试失败
*/
HRESULT CTester::TestAT88SC()
{
	/*
	往AT88SC的通用寄存器写入测试数据，然后读出来比较。
	*/
	
	SW_TRACE_NORMAL("Info: Testing AT88SC...\n");
	//if (0 != drv_sec_crypt_init(DEVID_IIC_AT88SC))
	//{
	//	SW_TRACE_NORMAL("Err: failed to init IIC AT88SC\n");
	//	return E_FAIL;
	//}

	if (FAILED(TestAT88SCOp(0x00))
		||FAILED(TestAT88SCOp(0xFF))
		||FAILED(TestAT88SCOp(0xAA))
		||FAILED(TestAT88SCOp(0x55))
		)
	{
		SW_TRACE_NORMAL("Info: AT88SC Test -- NG!\n");
		return E_FAIL;
	}	
	
	SW_TRACE_NORMAL("Info: AT88SC Test -- OK!\n");

	return S_OK;
}




/**
* @brief : 打开一个串口。

* @param [in] szDevice : The serial port device name, eg, /dev/ttyS0，/dev/ttyUSB0

* @retval Return fd if success, otherwise will return -1 with some msg.
*/
INT CTester::OpenUARTPort(const CHAR* szDevice)
{
    INT fd = -1; /* File descriptor for the port, we return it. */
    INT ret;

    fd = open(szDevice, O_RDWR | O_NOCTTY | O_NDELAY);    
    if (fd == -1)
	{
        SW_TRACE_NORMAL("Err: Unable to open port %s\n", szDevice);
	}

    /* block */
    ret = fcntl(fd, F_SETFL, 0);
    if (ret < 0)
	{
        //My_Txtout("fcntl\n");
        SW_TRACE_NORMAL("Err: fcntl ret %d\n", ret);
	}
    ret = isatty(STDIN_FILENO);
    if (ret == 0)
	{
        //My_Txtout("Standard input is not a terminal device.\n");
        SW_TRACE_NORMAL("Err: Standard input is not a terminal device.\n");
	}        
  
    return fd;
}


/**
* @brief 关闭串口

* @param [in] fd: The file description returned by open_port().
* @retval S_OK : 表示关闭成功.
* @retval E_FAIL : 表示硬件底层操作出错.
*/
HRESULT CTester::CloseUARTPort(INT fd)
{
    if (close(fd) < 0)
    {    		
        //My_Txtout("Unable to close the port.\n"); 
        SW_TRACE_NORMAL("Err: Unable to close the port.\n");
        return E_FAIL;
	}
	
    return S_OK;
}

 
/**
* @brief 对串口进行配置

* @param [in] fd: The file description returned by open_port().
* @param [in] speed    : The baud rate
* @param [in] data_bits : The data bits
* @param [in] parity    : The parity bits
* @param [in] stop_bits : The stop bits
* @retval S_OK : 表示串口配置成功.
* @retval E_FAIL : 表示硬件底层操作出错. 				
*/
HRESULT CTester::SetupUARTPort(INT fd, INT speed, INT data_bits, INT parity, INT stop_bits)
{
    INT speed_arr[] = {B115200, B9600, B38400, B19200, B4800};
    INT name_arr[] = {115200, 9600, 38400, 19200, 4800};
    struct termios opt;
    INT ret= E_FAIL;
    INT i=0;
    INT len=0;

    ret = tcgetattr(fd, &opt);		/* get the port attr */
    if (ret < 0)
    {
        SW_TRACE_NORMAL("Err: Unable to get UART Port attribute\n");
        return E_FAIL;
	}

    opt.c_cflag |= (CLOCAL | CREAD); /* enable the receiver, set local mode */
    opt.c_cflag &= ~CSIZE;			/* mask the character size bits*/

    /* baud rate */
    len = sizeof(speed_arr) / sizeof(INT);
    for (i = 0; i < len; i++)
    {
        if (speed == name_arr[i])
        {
            cfsetispeed(&opt, speed_arr[i]);
            cfsetospeed(&opt, speed_arr[i]);
        }
        if (i == len)
        {
            SW_TRACE_NORMAL("ERr: Unsupported baud rate.\n");
            return E_FAIL;
        }
    }
    
    /* data bits */
    switch (data_bits)
    {
    	case 8:
      	opt.c_cflag |= CS8;
        break;
		
    	case 7:
      	opt.c_cflag |= CS7;
        break;

		default:
        SW_TRACE_NORMAL("Err: Unsupported data bits.\n");
		return E_FAIL;
    }

    /* parity bits */
    switch (parity)
    {
    	case 'N':
    	case 'n':
        opt.c_iflag &= ~INPCK;
        opt.c_cflag &= ~PARENB;
        break;
		
    	case 'O':
    	case 'o':
        opt.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
        opt.c_cflag |= (PARODD | PARENB);
        break;
		
    	case 'E':
    	case 'e':
        opt.c_iflag |= (INPCK|ISTRIP); /*enable parity check, strip parity bits*/
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
        break;
		
    	default:
        SW_TRACE_NORMAL("Err: Unsupported parity bits.");
        return E_FAIL;
    }

    /* stop bits */
    switch (stop_bits)
    {
	    case 1:
        opt.c_cflag &= ~CSTOPB;
        break;
		
	    case 2:
        opt.c_cflag |= CSTOPB;
        break;
		
	    default:
        SW_TRACE_NORMAL("Err: Unsupported stop bits.");
        return E_FAIL;
    }

    /* raw input */
    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* raw ouput */
    opt.c_oflag &= ~OPOST;

    //设置流控
    //RTS/CTS (硬件) 流控制
    opt.c_cflag &= ~CRTSCTS; //无流控
    //输入的 XON/XOFF 流控制
    opt.c_iflag &= ~IXOFF;//不启用
    //输出的 XON/XOFF 流控制
    opt.c_iflag &= ~IXON ;//不启用

    // 防止0x0D变0x0A
    opt.c_iflag &= ~(ICRNL|IGNCR);

    tcflush(fd, TCIFLUSH);
    opt.c_cc[VTIME] = 1; /* time out */
    opt.c_cc[VMIN] = 0; /* minimum number of characters to read */

    ret = tcsetattr(fd, TCSANOW, &opt); /* update it now */
    if (ret < 0)
    {
        SW_TRACE_NORMAL("Err: Unable to setup the port.");
        return E_FAIL;
	}
	
    return S_OK; /* everything is OK! */
}


	/**
	* @brief ：串口测试。
					
	* @retval S_OK表示串口测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
HRESULT CTester::TestUART()
{
	/*
	功能串口回环后检测特征数据以判断
	*/
	
	SW_TRACE_NORMAL("Info: Testing UART...\n");

	DWORD Count_temp;
	INT fd0,fd1,ret;
		
	BYTE Tx_Buffer[35]= 
		{0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
		 0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,0x90,0xA0,0xB0,0xC0,0xD0,0xE0,0xF0,
		 0x00,0xFF,0xAA,0x55,'\n'};
		 
	BYTE Rx_Buffer[512];
	
	volatile BYTE *Tx_ptr,*Rx_ptr;

	
	SW_TRACE_NORMAL("Info: UART Closed Loop Testing...\n");
	
	 //open uart1
	fd1 = OpenUARTPort("/dev/ttyS1");   
	if( fd1 < 0 ) return E_FAIL;
	//setup the port
	if( SetupUARTPort(fd1, 115200, 8, 'N', 1) != S_OK) return E_FAIL;

	
	
	//---------UART1 send, UART1 received-----------------------------------
	
	//drv_device_set_rs485_direction(DEVID_RS485_1, 1);     //UART1 send dir

	swpa_thread_sleep_ms(10);//mdelay(10);
	

	ret = write(fd1, Tx_Buffer, 34);                              //UART1 send data
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: UART1 Send Data Fail!!\n");

		CloseUARTPort(fd1);
    	return E_FAIL;
	}

	swpa_thread_sleep_ms(100);//mdelay(100); 
	
	ret = read(fd1, Rx_Buffer, 512);                               //UART1 received data 
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: UART1 Read Data Fail!!\n");

		CloseUARTPort(fd1);
    	return E_FAIL;
	}   
    
	Tx_ptr = Tx_Buffer;
	Rx_ptr = Rx_Buffer;

    Count_temp = 34;
    
    while(Count_temp)
    {	
		if(*Tx_ptr != *Rx_ptr)
        {
    		SW_TRACE_NORMAL("Err: UART Test -- NG!\n");

			CloseUARTPort(fd1);
			return E_FAIL;
		}
		
		Count_temp--;
		Tx_ptr++;
		Rx_ptr++;
	}
	
	
	CloseUARTPort(fd1);
	
	SW_TRACE_NORMAL("Info: UART Test -- OK!\n");
	
}
	
/**
* @brief ：对指定目录下的进行指定大小的数据读写
				
* @retval S_OK表示测试成功.
* @retval E_FAIL表示硬件底层操作出错.
*/
HRESULT CTester::CheckSDmmcReadWrite(const CHAR* szDir, const DWORD dwSize)
{
	if (NULL == szDir)
	{
		SW_TRACE_NORMAL("Err: NULL == szDir!\n");
		return E_INVALIDARG;
	}

	CHAR szFileName[256] = {0};
	swpa_strncpy(szFileName, szDir, sizeof(szFileName));
	swpa_strcat(szFileName, "testsdmmc");

	if (SWPAR_OK != swpa_utils_file_exist(szFileName))
	{
		if (SWPAR_OK != swpa_utils_file_create(szFileName))
		{
			SW_TRACE_NORMAL("Err: failed to create %s under %s\n", szFileName, szDir);
			return E_FAIL;
		}
	}

	
	INT fd = swpa_file_open(szFileName, "w");
	if (0 > fd)
	{
		SW_TRACE_NORMAL("Err: failed to open file %s\n", szFileName);
		swpa_utils_file_delete(szFileName);
		return E_FAIL;
	}

	
	PBYTE pbReadBuf = (PBYTE)swpa_mem_alloc(dwSize);
	PBYTE pbWriteBuf = (PBYTE)swpa_mem_alloc(dwSize);
	if (NULL == pbReadBuf || NULL == pbWriteBuf)
	{
		SW_TRACE_NORMAL("Err: no memory for pbReadBuf and pbWriteBuf\n");
		swpa_file_close(fd);
		swpa_utils_file_delete(szFileName);
		return E_FAIL;
	}

	swpa_memset(pbReadBuf, 0, dwSize);
	swpa_memset(pbWriteBuf, 0, dwSize);

	for (DWORD i=0; i<dwSize; i++)
	{
		pbWriteBuf[i] = i%256;
	}

	swpa_file_seek(fd, 0, SWPA_SEEK_SET);
	INT iRetSize = 0;
	if (SWPAR_OK != swpa_file_write(fd, pbWriteBuf, (INT)dwSize, &iRetSize)
		|| (INT)dwSize != iRetSize)
	{
		SW_TRACE_NORMAL("Err: failed to write file %s\n", szFileName);
		swpa_file_close(fd);
		swpa_mem_free(pbReadBuf);
		swpa_mem_free(pbWriteBuf);
		swpa_utils_file_delete(szFileName);
		return E_FAIL;
	}

	swpa_file_close(fd);

	fd = swpa_file_open(szFileName, "r");
	if (0 > fd)
	{
		SW_TRACE_NORMAL("Err: failed to open file %s\n", szFileName);
		swpa_mem_free(pbReadBuf);
		swpa_mem_free(pbWriteBuf);
		swpa_utils_file_delete(szFileName);
		return E_FAIL;
	}

	
	if (SWPAR_OK != swpa_file_read(fd, pbReadBuf, (INT)dwSize, &iRetSize)
		|| (INT)dwSize != iRetSize)
	{
		SW_TRACE_NORMAL("Err: failed to read file %s\n", szFileName);
		swpa_file_close(fd);
		swpa_mem_free(pbReadBuf);
		swpa_mem_free(pbWriteBuf);
		swpa_utils_file_delete(szFileName);
		return E_FAIL;
	}

	if (0 != swpa_memcmp(pbReadBuf, pbWriteBuf, dwSize))
	{
		SW_TRACE_NORMAL("Err: pbReadBuf != pbWriteBuf\n");
		swpa_file_close(fd);
		swpa_mem_free(pbReadBuf);
		swpa_mem_free(pbWriteBuf);
		swpa_utils_file_delete(szFileName);
		return E_FAIL;
	}

	swpa_file_close(fd);
	swpa_mem_free(pbReadBuf);
	swpa_mem_free(pbWriteBuf);
	swpa_utils_file_delete(szFileName);

	return S_OK;
}




	/**
	* @brief ：对SD卡进行数据读写
					
	* @retval S_OK表示测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
HRESULT CTester::TestSDcard()
{
	
	/*
	对SD卡储存设备进行数据读写
	插SD卡后，内核自动检测出磁盘，但需要在程序中挂载磁盘，然后就能用fwrite之类的函数进行读写了。
	*/
	
	SW_TRACE_NORMAL("Info: Testing SD card...\n");
	
	const CHAR* szPartition = "/dev/mmcblk0p1";
	const CHAR* szDir = "/mnt/swtestsd/";

	INT iStatus = -1;
	if (SWPAR_OK != swpa_device_get_sdcard_status(&iStatus))
	{
		SW_TRACE_NORMAL("Err: failed to get SD card status\n");
		return E_FAIL;
	}

	if (0 != iStatus
		|| (!swpa_utils_file_exist("/dev/mmcblk0")))
	{
		SW_TRACE_NORMAL("Err: no SD card plugged in!\n");
		return E_FAIL;
	}
	else if (!swpa_utils_file_exist("/dev/mmcblk0p1"))
	{
		/*if (SWPAR_OK != swpa_hdd_fdisk("/dev/mmcblk0", 8, 0))
		{
			SW_TRACE_NORMAL("Err: failed to fdisk SD card!\n");
			return E_FAIL;
		}*/
		swpa_hdd_fdisk("/dev/mmcblk0", 8, 0);
		swpa_thread_sleep_ms(10*1000);
		
		swpa_utils_shell("mkfs.ext3 -T largefile /dev/mmcblk0p1", 0);
		SW_TRACE_NORMAL("Info: Formatting SD card...\n");
		swpa_thread_sleep_ms(5*60*1000);
	}
	
	if (SWPAR_OK != swpa_utils_dir_exist(szDir))
	{
		if (SWPAR_OK != swpa_utils_dir_create(szDir))
		{
			SW_TRACE_NORMAL("Err: failed to create %s\n", szDir);
			return E_FAIL;
		}
	}	

	if (SWPAR_OK != swpa_hdd_mount(szPartition, szDir, NULL))//if (SWPAR_OK != swpa_sdmmc_mount(szPartition, szDir, NULL))
	{

		SW_TRACE_NORMAL("Err: failed to mount %s to %s\n", szPartition, szDir);
		return E_FAIL;
	}

	LONGLONG llAvailSize = 0;
	if (SWPAR_OK != swpa_hdd_get_availsize(szPartition, &llAvailSize))//if (SWPAR_OK != swpa_sdmmc_get_availsize(szPartition, &llAvailSize))
	{
		SW_TRACE_NORMAL("Err: failed to get available size of %s \n", szPartition);
		return E_FAIL;
	}

	DWORD dwSize = 0;
	if (llAvailSize > 1*(LONGLONG)1024*1024*1024)//larger than 1GB available, then test 10MB
	{
		dwSize = 10*1024*1024;
	}
	else if (llAvailSize > 1*(LONGLONG)1024*1024)//larger than 1MB available, then test 1MB
	{
		dwSize = 1*1024*1024;
	}
	else if (llAvailSize > 1*(LONGLONG)1024)//larger than 1KB available, then test 1KB
	{
		dwSize = 1*1024;
	}
	else
	{
		SW_TRACE_NORMAL("Err: no enough space under %s \n", szPartition);
		swpa_hdd_umount(szPartition);//swpa_sdmmc_umount(szPartition);
		swpa_utils_dir_remove(szDir);
		return E_FAIL;
	}

	if (FAILED(CheckSDmmcReadWrite(szDir, dwSize)))
	{
		SW_TRACE_NORMAL("Err: SD card Test -- NG!\n");
		swpa_hdd_umount(szPartition);//swpa_sdmmc_umount(szPartition);
		swpa_utils_dir_remove(szDir);
		return E_FAIL;
	}

	if (SWPAR_OK != swpa_hdd_umount(szPartition))//if (SWPAR_OK != swpa_sdmmc_umount(szPartition))//todo: always failed!!
	{
		SW_TRACE_NORMAL("Err: failed to umount %s \n", szPartition);
		return E_FAIL;
	}

	if (SWPAR_OK != swpa_utils_dir_remove(szDir))
	{
		SW_TRACE_NORMAL("Err: failed to remove %s\n", szDir);
		return E_FAIL;
	}
	
	SW_TRACE_NORMAL("Info: SD card Test -- OK!\n");

	return S_OK;
	
}



	/**
	* @brief ：DM8127 to FPGA IO TEST
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
HRESULT CTester::TestDM8127FPGAIO()
{	
	int temp = 0;
	unsigned int temp32 = 0,addr = 0;
	
	SW_TRACE_NORMAL("Info: Testing DM8127 to FPGA IO ........\n");
	
	addr = FPGA_ID_ADR | ( 1 << 12)|(1 << 15);
	drv_fpga_read_single(DEVID_SPI_FPGA, addr, &temp32);

	if (FPGA_VERSION != temp32)
	{
        SW_TRACE_NORMAL("Err: Failed to Get FPGA Version,Current:0x%x !\n",temp32);    //FPGA Version
		return E_FAIL;
	}

	//----------------------------------------------------------
		drv_gpio_set_output(DEVID_GPIO3,15,0);        //SOFT_TRIG = 0;
		swpa_thread_sleep_ms(10);//mdelay(10);
		drv_gpio_get_input(DEVID_GPIO3, 22, &temp);   
		
		if(0x01 != temp)
		{
			SW_TRACE_NORMAL("Info: DM8127 to FPGA IO TEST --NG!\n");
			return E_FAIL;
			
		}
 //-------------------------------------------------------------
	
		drv_gpio_set_output(DEVID_GPIO3,15,1);        //SOFT_TRIG = 0;
		swpa_thread_sleep_ms(10);//mdelay(10);
		drv_gpio_get_input(DEVID_GPIO3, 22, &temp);   
		
		if(0x00 != temp)
		{
			SW_TRACE_NORMAL("Info: DM8127 to FPGA IO TEST --NG!\n");
			return E_FAIL;
			
		}
 //-------------------------------------------------------------
	
		SW_TRACE_NORMAL("Info: DM8127 to FPGA IO TEST --OK!\n");
		return S_OK;	
}



/**
* @brief ：生产测试切换器输出控制

* @param [in] ctl: 切换器.开关控制。
					0： OFF， 1：ON 。
* @retval 无。
*/
void CTester::Product_SwitcherCtl(BYTE ctl)
{

	switch(ctl)
	{
		case 0:
			drv_gpio_set_output(DEVID_GPIO2,25,0);
			drv_gpio_set_output(DEVID_GPIO2,27,1);
			swpa_thread_sleep_ms(1500);
			drv_gpio_set_output(DEVID_GPIO2,25,0);
			drv_gpio_set_output(DEVID_GPIO2,27,0);
		break;
		
		case 1:
			drv_gpio_set_output(DEVID_GPIO2,25,1);
			drv_gpio_set_output(DEVID_GPIO2,27,0);	
			swpa_thread_sleep_ms(1500);
			drv_gpio_set_output(DEVID_GPIO2,25,0);
			drv_gpio_set_output(DEVID_GPIO2,27,0);	
		break;
		
		default:
		break;	
	}	
}

/**
* @brief ：生产测试切换器控制测试

* @param [in] TestNum: 切换器测试次数。
* @retval 无。
*/
void CTester::Product_SwitcherTest(BYTE TestNum)
{

	unsigned int i =0 ;
	SW_TRACE_NORMAL("Info: Testing SwitcherCtl...\n");
	for( i = 0 ; i < TestNum ; i++)
	{
		Product_SwitcherCtl(1);   //  ON
		if (GetForceStopFlag())
		{
			return;
		}
		swpa_thread_sleep_ms(1000);//mdelay(1000);
		Product_SwitcherCtl(0);  //  OFF
		if (GetForceStopFlag())
		{
			return;
		}
		swpa_thread_sleep_ms(1000);//mdelay(1000);
	}
		
}


/**
* @brief ：生产测试DC光圈输出控制

* @param [in] ctl: DC光圈.开关控制。
					0： OFF， 1：ON 。
* @retval 无。
*/
void CTester::Product_DcCtl(BYTE ctl)
{

	//unsigned int temp32 =0 ;
	switch(ctl)
	{
		case 0:
			//drv_gpio_set_output(DEVID_GPIO1,22,0);   //IR0
			//drv_gpio_set_output(DEVID_GPIO1,21,1);   //IR1
			
			drv_autoiris_pwm(DEVID_PWM, 0);
		break;
		
		case 1:
			//drv_gpio_set_output(DEVID_GPIO1,22,1);    //IR0
			//drv_gpio_set_output(DEVID_GPIO1,21,0);    //IR1
			
			drv_autoiris_pwm(DEVID_PWM, 1000);
		break;
		
		default:
		break;	
		
	}	

}


/**
* @brief ：生产测试DC光圈输出控制测试

* @param [in] TestNum: 切换器测试次数。?
* @retval 无。
*/
void CTester::Product_DcCtl_TEST(BYTE TestNum)
{
	unsigned int i = 0 ;
	
	for( i = 0 ; i < TestNum ; i++)
	{
		Product_DcCtl(1);   //  ON
		if (GetForceStopFlag())
		{
			return;
		}
		swpa_thread_sleep_ms(500);//mdelay(500);
		Product_DcCtl(0);  //  OFF
		if (GetForceStopFlag())
		{
			return;
		}
		swpa_thread_sleep_ms(500);//mdelay(500);

	}

}



	/**
	* @brief ：球机进入生产测试模式。
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
HRESULT CTester::Ball_IntoTestMode()
{
	
	int temp = 0;
	unsigned int temp32 = 0,addr = 0,i=0,j=0;
	INT fd0,fd1,ret;
	
	SW_TRACE_NORMAL("Info: Hight speed Ball into TestMode...\n");

	ret = swpa_camera_ball_set_wiper();    //开启雨刷
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: set wiper Fail!!\n");
        return E_FAIL;
	}
	
	
	ret = swpa_camera_ball_set_defog_fan(1); //启动风扇及加热
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: set defog fan fail!!\n");
    	return E_FAIL;
	}
		
    //红外LED测试
    ret = swpa_camera_ball_set_led_mode(0x2, 0x2);  //强制打开红外LED；
    if( ret < 0)
    {
        SW_TRACE_NORMAL("Err: set infrared led Fail!!\n");
        return E_FAIL;
    }
		
	for(i =0; i < 3;i++)
	{

		ret = swpa_camera_ball_set_led_power(0,0,10);  // 打开远距灯组；
		if( ret < 0)
		{
			SW_TRACE_NORMAL("Err: set infrared power Fail!!\n");
            return E_FAIL;
		}
		
		swpa_thread_sleep_ms(2000);
			
		ret = swpa_camera_ball_set_led_power(10,0,0);  // 打开近距灯组；
		if( ret < 0)
		{
			SW_TRACE_NORMAL("Err: set infrared power Fail!!\n");
            return E_FAIL;
		}
		
		swpa_thread_sleep_ms(2000);
		
	}
	
	ret = swpa_camera_ball_set_led_mode(0x3, 0x2);  //强制关闭红外LED；
	if( ret < 0)
	{
        SW_TRACE_NORMAL("Err: set infrared led Fail!!\n");
    	return E_FAIL;
	}
		
	ret = swpa_camera_ball_set_defog_fan(0); //关闭风扇及加热
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: set defog fan fail!!\n");
    	return E_FAIL;
	}

#if 0
    ret = swpa_camera_ball_restore_factory_settings();//恢复出厂默认
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: restore_factory_settings fail!!\n");
    	return E_FAIL;
	}
    swpa_thread_sleep_ms(2000);
#endif

//    SW_TRACE_NORMAL("!!!!!!!!!!!!!!!!!!!!!!!!WANGNC: JUST DEBUG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	ret = swpa_camera_ball_factorytest_start();   //进入老化测试模式
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: swpa_camera_ball_factorytest_start Fail!!\n");
        return E_FAIL;
	}
	
	return S_OK;
		
}



	/**
	* @brief ：球机退出生产测试模式。
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
HRESULT CTester::Ball_StopTestMode()
{
	
	int temp = 0,ret;
	unsigned int temp32 = 0,addr = 0;
	
	

	
	SW_TRACE_NORMAL("Info: Hight speed Ball Stop TestMode...\n");
	
	
	ret = swpa_camera_ball_factorytest_stop( );   //退出老化模式
	
	if( ret < 0)
	{
		SW_TRACE_NORMAL("Err: swpa_camera_ball_factorytest_stop Fail!!\n");

    	return E_FAIL;
	}
	
	
	

	return S_OK;
		
}




