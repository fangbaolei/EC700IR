
#ifndef _TESTER_IN_AUTOTEST_H_
#define _TESTER_IN_AUTOTEST_H_

#include "drv_device.h"

class CTester
{
public:
	
	CTester();

	virtual ~CTester();
	
	/*
	static HRESULT TestDM6467DDR();
	*/

	
	static HRESULT TestFlash(const DRV_DEVICE_TYPE dwFlashID, const DWORD dwAddr, const DWORD dwSectorCount, const DWORD dwSectorSize);
	
	/**
	* @brief SPI FLASH的读写测试，把DDR中一段数据写到SPI FLASH中，接着
			  读到DDR的另一空间，进行比较。每个扇区单独测试。
			
	* @retval S_OK表示SPI FLASH测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM6467Flash();
	

	/**
	* @brief ：I2C EEPROM读写测试，读写EEPROM的每一页，一共测试8页

	* @retval S_OK表示SPI FLASH的ID检测成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM6467EEPROM();
	

	/**
	* @brief ：RTC测试，随时间变化读取RTC时间值
	
	* @retval S_OK:  成功进行测试	
	* @retval E_FAIL: 测试失败
	*/
	static HRESULT TestDM6467RTC();

	
	/**
	* @brief ：LM75测试，读取当前温度
	
	* @retval S_OK:  成功进行测试	
	* @retval E_FAIL: 测试失败
	*/
	static HRESULT TestLM75();
	

	
	/**
	* @brief ：AT88SC非加密读写测试
	
	* @retval S_OK:  成功进行测试	
	* @retval E_FAIL: 测试失败
	*/
	static HRESULT TestAT88SC();
	

	/**
	* @brief ：DM6467端的VPIF测试

	* @retval S_OK表示VPIF测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestVPIF();
	
	
	/**
	* @brief ：对USB存储设备，如U盘、USB to Sata硬盘进行数据读写
					
	* @retval S_OK表示测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestUSB();
	

	/**
	* @brief ：串口0及串口1交叉回环测试。
					
	* @retval S_OK表示串口回环测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestUART();
	

	
	/**
	* @brief SPI FLASH的读写测试，把DDR中一段数据写到SPI FLASH中，接着
			  读到DDR的另一空间，进行比较。每个扇区单独测试。
			
	* @retval S_OK表示SPI FLASH测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM368Flash();
	

	/**
	* @brief ：DM6467T to FPGA IO TEST
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM6467FPGAIO();
	

	/**
	* @brief ：DM6467T to DM368 IO TEST
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM6467DM368IO();
	

	/**
	* @brief ：DM368 to FPGA IO TEST,DM6467 通过USB网络命令告知DM368进行测试，并返回结果
	
	* @retval S_OK表示测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM368FPGAIO();
	
	/**
	* @brief ：生产测试切换器输出控制
	@param [in] ctl: 切换器.开关控制。
						0： OFF， 1：ON 。
	* @retval 无。

	*/
   static void ProductSwitcherCtl(BYTE ctl);


	/**
	* @brief ：生产测试切换器控制测试
	@param [in] TestNum: 切换器测试次数。

	* @retval 无。
	*/
	static void ProductSwitcherTest(BYTE TestNum);


	/**
	* @brief ：生产测试DC光圈输出控制
	@param [in] ctl: DC光圈.开关控制。
						0： OFF， 1：ON 。
	* @retval 无。

	*/
	static void ProductDcCtl(BYTE ctl);

	/**
	* @brief ：生产测试DC光圈输出控制测试
	@param [in] TestNum: 切换器测试次数。

	* @retval 无。

	*/
	static void ProductDcCtlTEST(BYTE TestNum);

	/**
	* @brief ：硬盘电源控制
	@param [in] ctl: 硬盘电源开关控制。
						0： OFF， 1：ON 。
	* @retval 无。

	*/
	static void HdPowerCtl(BYTE ctl);

	
	static HRESULT DeinitVPIF();

	static HRESULT InitVPIF();

	static HRESULT SetForceStopFlag(const BOOL fFlag);

	static HRESULT GetForceStopFlag();


private:
	
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
	static HRESULT SetupUARTPort(INT iFD, INT iSpeed, INT iDataBits, INT iParity, INT iStopBits);

	/**
	* @brief 关闭串口

	* @param [in] fd: The file description returned by open_port().
	* @retval S_OK : 表示关闭成功.
	* @retval E_FAIL : 表示硬件底层操作出错.
	*/
	static HRESULT CloseUARTPort(INT iFD);

	/**
	* @brief : 打开一个串口。

	* @param [in] szDevice : The serial port device name, eg, /dev/ttyS0，/dev/ttyUSB0

	* @retval Return fd if success, otherwise will return -1 with some msg.
	*/
	static INT OpenUARTPort(const CHAR* szDevice);

	/**
	* @brief DM6467T to FPGA IO CHECK

	* @param [in] Testdata: 输入的测试数据,只有低3位有效。
	* @retval S_OK : 表示IO测试成功.
	* @retval E_FAIL : 表示硬件底层操作出错.
	*/
	static HRESULT CheckIODM6467ToFPGA(BYTE bTestdata);

	/**
	* @brief DM6467T to DM368 IO CHECK

	* @param [in] Testdata: 输入的测试数据,只有低2位有效。
	* @retval S_OK : 表示IO测试成功.
	* @retval E_FAIL : 表示硬件底层操作出错.
	*/
	static HRESULT CheckIODM6467ToDM368(BYTE bTestdata);

	 
	/**
	* @brief ：对指定目录下的进行指定大小的数据读写
					
	* @retval S_OK表示测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT CheckUSBReadWrite(const CHAR* szDir, const DWORD dwSize);


	/**
	* @brief ：AT88SC测试区的基本测试用来检验基本I2C通讯

	* @param [in] : TestByte : 传入的测试数据
	* @retval S_OK:  成功进行测试	
	* @retval E_FAIL: 测试失败
	*/
	static HRESULT TestAT88SCOp(const BYTE bTestData);

	
	static HRESULT SetDM368ToIOTestMode();




private:

};


#endif //_TESTER_IN_AUTOTEST_H_

