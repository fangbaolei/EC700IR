/*
 * Serial.h
 *	串口收发类.
 *		说明：
 *			使用本类对串口进行独占操作。
 *			通过打开和关闭串口对串口进行加锁与解锁。加锁后即可收发数据。
 *		注意：
 *			本类的不同对象对串口操作时亦会因已加锁而进行等待，
 *			即不同的串口操作不需要同一对象。
 *      Author: ganzz
 */

#ifndef SERIALBASE_H_
#define SERIALBASE_H_

#include <pthread.h>
#include <vector>

class SerialPort
{
public:
    char szDev[20];
    volatile int iOpenCount;
    volatile int iLockCount;
    pthread_mutex_t mutex;
    int fd;
};

class CSerialBase
{
public:
	CSerialBase();
	virtual ~CSerialBase();

	// 打开串口
	// 参数：
	//          szDev 		要打开的设备，如/dev/ttyS0
	// 返回：
	//          0		成功
	//			-1		打开设备失败
	int Open(const char* szDev);

	// 关闭串口，解除锁定
	int Close();

    // 锁定串口.若处于串口加锁状态，则等待
	// 参数：
    // 			nTimeOMs    超时毫秒数
	// 返回：
	//          0		成功
	//			-1		打开设备失败
    int Lock(unsigned long nTimeOMs = 1000);

    // 判断串口是否处于加锁状态
	// 返回：
	//          true	已加锁
	//			false	未加锁
    bool IsLock();

    // 解锁串口
	// 返回：
	//          0		成功
	//			-1		打开设备失败
    int UnLock();

	// 设置串口属性。
	// 参数：
	// 			nDatasize 		数据位
	// 			nBaudrate 		波特率
	//			nParity			奇偶校验位，0－无，1－奇，2－偶
	// 			iStopBit 	停止位
	// 说明：
	int SetAttr(int nBaudrate, int nDatasize, int nParity, int iStopBit);

	// 发送指定长度的数据，直到发完或无法发送
	// 参数：
	//          msg 		要发送的信息数据，不以\0结尾
	// 			nLen		数据长度
	// 返回值：	已经发送的数据长度，若发送失败，返回值必不等于要发送的数据长度
	int Send(unsigned char* szBuf, int nLen);

	// 接收指定长度的数据，直到接收完成、超时或无法接收
	// 参数：
	//          msg 		要接收的数据缓冲指针，得到的数据不以\0结尾
	// 			nLen		要接收的数据长度
	//			nTimeOMs	超时,默认nTimeOMs毫秒
	// 返回值：	已经接收的数据长度，若接收失败，返回值必不等于要接收的数据长度
	int Recv(unsigned char* szBuf, int nLen, unsigned long nTimeOMs = 1000);

	// 清空接收与写入缓存
	void Flush();

private:
	SerialPort* m_pSerialPort;

	static pthread_mutex_t s_SerialMutex;
	static std::vector<SerialPort*> s_SerialPtrVct;
};

#endif /* SERIALBASE_H_ */
