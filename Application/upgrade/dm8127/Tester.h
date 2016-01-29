
#ifndef _TESTER_IN_AUTOTEST_H_
#define _TESTER_IN_AUTOTEST_H_

#include "SWBaseType.h"
#include "drv_device.h"

class CTester
{
public:
	
	CTester();

	virtual ~CTester();

	static HRESULT SetForceStopFlag(const BOOL fFlag);

	static BOOL GetForceStopFlag();
		
	/**
	* @brief NAND FLASH的读写测试，把DDR中一段数据写到NAND FLASH中，接着
			  读到DDR的另一空间，进行比较。每个block单独测试。
			
	* @retval S_OK表示SPI FLASH测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestFlash();
	

	/**
	* @brief ：I2C EEPROM读写测试，读写EEPROM的每一页，一共测试8页

	* @retval S_OK表示SPI FLASH的ID检测成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestEEPROM();
	

	/**
	* @brief ：RTC测试，随时间变化读取RTC时间值
	
	* @retval S_OK:  成功进行测试	
	* @retval E_FAIL: 测试失败
	*/
	static HRESULT TestRTC();

	
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
	* @brief ：串口测试。
					
	* @retval S_OK表示串口测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestUART();




	/**
	* @brief ：对SD卡进行数据读写
					
	* @retval S_OK表示测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestSDcard();



	/**
	* @brief ：DM8127 to FPGA IO TEST
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT TestDM8127FPGAIO();





	/**
	* @brief ：生产测试切换器控制测试

	* @param [in] TestNum: 切换器测试次数。
	* @retval 无。
	*/
	static void Product_SwitcherTest(BYTE TestNum);



	/**
	* @brief ：生产测试DC光圈输出控制测试

	* @param [in] TestNum: 切换器测试次数。
	* @retval 无。
	*/
	static void Product_DcCtl_TEST(BYTE TestNum);


	/**
	* @brief ：球机进入生产测试模式。
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT Ball_IntoTestMode();


	/**
	* @brief ：球机退出生产测试模式。
	
	* @retval S_OK表示IO测试成功.
	* @retval E_FAIL表示硬件底层操作出错.
	*/
	static HRESULT Ball_StopTestMode();



private:
	
	
	/**
* @brief ：对指定目录下的进行指定大小的数据读写
				
* @retval S_OK表示测试成功.
* @retval E_FAIL表示硬件底层操作出错.
*/
	static HRESULT CheckSDmmcReadWrite(const CHAR* szDir, const DWORD dwSize);
	
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
	* @brief ：AT88SC测试区的基本测试用来检验基本I2C通讯

	* @param [in] : TestByte : 传入的测试数据
	* @retval S_OK:  成功进行测试	
	* @retval E_FAIL: 测试失败
	*/
	static HRESULT TestAT88SCOp(const BYTE bTestData);

	/**
	* @brief ：生产测试切换器输出控制

	* @param [in] ctl: 切换器.开关控制。
	*				0： OFF， 1：ON 。
	* @retval 无。
	*/
	static void Product_SwitcherCtl(BYTE ctl);

	/**
	* @brief ：生产测试DC光圈输出控制

	* @param [in] ctl: DC光圈.开关控制。
	*				0： OFF， 1：ON 。
	* @retval 无。
	*/
	static void Product_DcCtl(BYTE ctl);
};


#endif //_TESTER_IN_AUTOTEST_H_

