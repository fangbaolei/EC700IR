/*
 * HvSerialLink.h
 *	串口协议连接类
 *      by ganzz
 */

#ifndef HVSERIALLINK_H_
#define HVSERIALLINK_H_

#include "SerialBase.h"

class CHvSerialLink : public CSerialBase
{
public:
	CHvSerialLink();
	CHvSerialLink(int nDevPort);
	virtual ~CHvSerialLink();

    int Open(const char* szDev);

    //
    //
    bool ExecuteCommand(char bCmdId,
            unsigned char* pbSendData,
            unsigned int iSendLen,
            unsigned char* pReceiveData,
            unsigned int & iReceiveLen,
            unsigned int dwRelayMs = 1000);

    // 使用串口协议发送数据
    // 返回: -1 失败; 0 成功
    // 注意:     此前必先设置通道，若使用没有设置通道的Open()函数打开串口，
	//          首次发送数据必须使用本函数或Send()函数设置通道
	int SendCmdData(const unsigned char bCmdId,
            unsigned char* pbData,
            unsigned int dwSendLen);

    // 使用串口协议接收数据
    // 返回: -1 失败; 0 成功
	int RecvCmdData(unsigned char* pbCmdId,
			unsigned char* pbData,
			unsigned int* pdwRecvLen,
			unsigned int dwRelayMs);

    // 从协议数据包里获取数据
	// 参数：
	//          recvbuf 		接收到的协议数据包缓冲
	// 			dwRecvLen		接收到的协议数据包长度
	//          pbCmdId         存放解析到的ID，可为空
	//          pbSN            存放解析到的协议序列号，可为空
	//          pbData          存放解析到的数据，可为空
	//          pdwDataLen      输入输出，输入时存放缓冲长度，输出解析到的数据长度
	//                          可为空，但必需确保缓冲足够长
    // 返回: -1 解析失败 -2 给定的数据缓冲不足
	static int PickupDataFromPacket(
            const unsigned char* recvbuf,
			const unsigned int dwRecvLen,
			unsigned char* pbCmdId,
			unsigned char* pbSN,
			unsigned char* pbData,
			unsigned int* pdwDataLen);

    // 组包
    // 参数：
    //          bCmdId          协议ID
    //          bSN             协议序列号
    //          pbData          要打包的数据
    //          dwDataLen       数据长度
    //          pbPacketData    打包后的数据缓冲，必须够长
    //          pdwDataLen      接收打包后的数据长度
    static void MakePacket(
            const unsigned char bCmdId,
            const unsigned char bSN,
            const unsigned char* pbData,
			const unsigned int dwDataLen,
			unsigned char* pbPacketData,
			unsigned int* pdwDataLen);

    //  接收一个完整的串口协议数据包
    //          pbData          要接收的数据包缓冲指针
    //          pdwLen          要接收的数据长度
    //          dwRelayMs	    超时毫秒数
    int RecvPacket(unsigned char* pbData,
			unsigned int* pdwLen,
			unsigned int dwRelayMs);

private:

    // 切换通道
    // 返回值: 0 成功；-1 发送失败； -2 解析接收数据包失败
	int SendSetChannel(unsigned char bChannel, unsigned int nTimeOMs);

	static const int MAX_PACK_LEN       = 16*4*8 + 128; //最大数据包长度
	static const int FALG_NUMBER        = 0x00; //序号
	static const int FALG_BEGIN         = 0xFF; //开始标志
	static const int FALG_END           = 0xFE; //结束标志
	static const int FALG_EXCHANGE      = 0xFD; //转义标志
	static const int FALG_NONE          = 0xF0; //无事件标志
};

#endif /* HVSERIAL_H_ */
