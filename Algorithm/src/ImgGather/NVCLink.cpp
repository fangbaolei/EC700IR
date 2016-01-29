// 该文件编码格式必须为WINDOWS-936格式

#include "ImgGatherer.h"

inline WORD16 BYTE8_WORD16_Ex(BYTE8* pbData)
{
    WORD16 wHigh = pbData[0];
    return (wHigh << 8) + (WORD16)pbData[1];
}

const double JPEG_CHROMA[] =
{
    0,
    3.250,
    3.125,
    3.000,
    2.875,
    2.750,
    2.625,
    2.500,
    2.375,
    2.250,
    2.125,
    2.000,
    1.875,
    1.750,
    1.625,
    1.500,
    1.375,
    1.250,
    1.125,
    1.000,
    0.825,
    0.750,
    0.625,
    0.500,
    0.375,
    0.250
};

void CalculateHeader(BYTE8* dest, int ImageColumns, int ImageRows, int RestartInterval, float QuantizationLuma, float QuantizationChroma)
{
    BYTE8* header_buffer = dest;
    int bcnt,i,tmp;
    BYTE8 quan_tmp;
    int iIndex(0);
    //之字表
    const int jpeg_natural_order[64] =
    {
        0,  1,  8, 16,  9,  2,  3, 10,
        17, 24, 32, 25, 18, 11,  4,  5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13,  6,  7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63,
    };
    static int std_qtables[128] =
    {
        // chrominance quantization table 色度
        8,8,12,22,48,48,48,48,
        8,10,12,32,48,48,48,48,
        12,12,28,48,48,48,48,48,
        22,32,48,48,48,48,48,48,
        48,48,48,48,48,48,48,48,
        48,48,48,48,48,48,48,48,
        48,48,48,48,48,48,48,48,
        48,48,48,48,48,48,48,52,
        // luminance quantization table 亮度
        8,4,4,8,12,20,24,30,
        6,6,6,8,12,28,30,26,
        6,6,8,12,20,28,34,28,
        6,8,10,14,24,42,40,30,
        8,10,18,28,34,54,50,38,
        12,16,26,32,40,52,56,46,
        24,32,38,42,50,60,60,50,
        36,46,46,48,56,50,50,52
    };
    //直流亮度位表
    static const unsigned char bits_dc_luminance[17] = { /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
    //直流亮度值表
    static const unsigned char val_dc_luminance[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    //直流色度位表
    static const unsigned char bits_dc_chrominance[17] = { /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
    //直流色度值表
    static const unsigned char val_dc_chrominance[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    static const unsigned char bits_ac_luminance[17] = { /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
    static const unsigned char val_ac_luminance[] =
        { 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
          0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
          0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
          0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
          0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
          0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
          0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
          0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
          0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
          0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
          0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
          0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
          0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
          0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
          0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
          0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
          0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
          0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
          0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
          0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
          0xf9, 0xfa
        };

    static const unsigned char bits_ac_chrominance[17] = { /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
    static const unsigned char val_ac_chrominance[] =
        { 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
          0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
          0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
          0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
          0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
          0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
          0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
          0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
          0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
          0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
          0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
          0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
          0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
          0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
          0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
          0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
          0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
          0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
          0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
          0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
          0xf9, 0xfa
        };


    //output start of image marker FFD8
    header_buffer[iIndex++] = (0xff);
    header_buffer[iIndex++] = (0xd8);

    //output quantization tables marker

    header_buffer[iIndex++] = (0xff);
    header_buffer[iIndex++] = (0xdb);

    //output DQT segment length (132 = 2*64 + 4 that is x84)
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = (0x84);

    //specify beginning of luminance table with 8-bit precision
    header_buffer[iIndex++] = (0x00);

    //output luminance table
    for (bcnt = 0; bcnt < 64; bcnt++)
    {
        quan_tmp = (BYTE8)(std_qtables[jpeg_natural_order[bcnt] + 64] * QuantizationLuma + 0.5);
        //header_buffer[bytes_in_header_buffer] = (GByte) std_qtables[jpeg_natural_order[bcnt] + 64];
        header_buffer[iIndex++] = quan_tmp;
    }

    //specify beginning of chrominance table with 8-bit precision
    header_buffer[iIndex++] = (0x01);

    //output chrominance table
    for (bcnt = 0; bcnt < 64; bcnt++)
    {
        quan_tmp = (BYTE8)(std_qtables[jpeg_natural_order[bcnt]] * QuantizationChroma + 0.5);
        header_buffer[iIndex++] = quan_tmp;
        //header_buffer[bytes_in_header_buffer] = (GByte) std_qtables[jpeg_natural_order[bcnt]];
    }

    //**Sometimes I comment out the restart interval, since we can save some reg and logic and file space.
    //output DRI marker
    header_buffer[iIndex++] = (0xff);
    header_buffer[iIndex++] = (0xdd);

    //output DRI segment length in bytes
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = (0x04);

    //restart interval, only one intra image encoded
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = RestartInterval;

    //output frame header marker
    header_buffer[iIndex++] = (0xff);
    header_buffer[iIndex++] = (0xc0);

    //output frame header length
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = (0x11);

    //sample precision
    header_buffer[iIndex++] = (0x08);

    //number of lines (480 = 0x01e0) //try 472 = 0x01d8

    //  header_buffer[bytes_in_header_buffer] = 0x01; bytes_in_header_buffer++;
    //  header_buffer[bytes_in_header_buffer] = 0xd8; bytes_in_header_buffer++;

    header_buffer[iIndex++] = ImageRows/256;
    tmp = ImageRows;
    while (tmp>256) tmp=tmp-256;

    header_buffer[iIndex++] = tmp;

    //number of columns (640 = 0x0280) //try 632 = 0x278

    //header_buffer[bytes_in_header_buffer] = 0x02; bytes_in_header_buffer++;
    //header_buffer[bytes_in_header_buffer] = 0x80; bytes_in_header_buffer++;

    header_buffer[iIndex++] = ImageColumns/256;
    tmp = ImageColumns;
    while (tmp>256) tmp=tmp-256;

    header_buffer[iIndex++] = tmp;

    //number of components in frame
    header_buffer[iIndex++] = (0x03);

    //first component description - luminance
    header_buffer[iIndex++] = (0x01);
    header_buffer[iIndex++] = (0x21);
    header_buffer[iIndex++] = (0x00);

    //second component description
    header_buffer[iIndex++] = (0x02);
    header_buffer[iIndex++] = (0x11);
    header_buffer[iIndex++] = (0x01);

    //third component description
    header_buffer[iIndex++] = (0x03);
    header_buffer[iIndex++] = (0x11);
    header_buffer[iIndex++] = (0x01);

    //output huffman table marker
    header_buffer[iIndex++] = (0xff);
    header_buffer[iIndex++] = (0xc4);

    //length of DHT segment
    header_buffer[iIndex++] = (0x01);
    header_buffer[iIndex++] = (0xa2);

    //luminance dc table descriptor (class 0; identifier 0)
    header_buffer[iIndex++] = (0x00);

    //number of huffman codes of length i
    for (i = 1; i < 17; i++)
        header_buffer[iIndex++] = (bits_dc_luminance[i]);

    //associated values
    for (i = 0; i < 12; i++)
        header_buffer[iIndex++] = (val_dc_luminance[i]);

    //luminance ac table descriptor (class 1; identifier 0)
    header_buffer[iIndex++] = (0x10);

    //number of huffman codes of length i
    for (i = 1; i < 17; i++)
        header_buffer[iIndex++] = (bits_ac_luminance[i]);

    //associated values
    for (i = 0; i < 162; i++)
        header_buffer[iIndex++] = (val_ac_luminance[i]);

    //chrominance dc table descriptor (class 0; identifier 1)
    header_buffer[iIndex++] = (0x01);

    //number of huffman codes of length i
    for (i = 1; i < 17; i++)
        header_buffer[iIndex++] = (bits_dc_chrominance[i]);

    //associated values
    for (i = 0; i < 12; i++)
        header_buffer[iIndex++] = (val_dc_chrominance[i]);

    //chrominance ac table descriptor (class 1; identifier 1)
    header_buffer[iIndex++] = (0x11);

    //number of huffman codes of length i
    for (i = 1; i < 17; i++)
        header_buffer[iIndex++] = (bits_ac_chrominance[i]);

    //associated values
    for (i = 0; i < 162; i++)
        header_buffer[iIndex++] = (val_ac_chrominance[i]);

    //output SOS  marker
    header_buffer[iIndex++] = (0xff);
    header_buffer[iIndex++] = (0xda);

    //length of SOS header segment
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = (0x0c);

    //number of components in scan
    header_buffer[iIndex++] = (0x03);

    //descriptors of components in scan
    header_buffer[iIndex++] = (0x01);
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = (0x02);
    header_buffer[iIndex++] = (0x11);
    header_buffer[iIndex++] = (0x03);
    header_buffer[iIndex++] = (0x11);
    header_buffer[iIndex++] = (0x00);
    header_buffer[iIndex++] = (0x3f);
    header_buffer[iIndex++] = (0x00);

    //this completes frame header: output it in file
}


CNVCLink::CNVCLink()
        : m_iQuality(11)
        , m_wWidth(2048)
        , m_wHeight(1536)
{
}

CNVCLink::~CNVCLink()
{
}

HRESULT CNVCLink::ReadRegValue(BYTE8 bAddr, WORD16& wValue)
{
    HRESULT hr(S_OK);
    BYTE8 rgbRecvBuf[MAX_RECV_SIZE_NVC];
    char szSendBuf[256];

    // 1.客户机向服务器发送读请求报文RRQ
    sprintf(szSendBuf, "getreg?page=3;reg=%d;", bAddr);

    hr = m_tftp_ptl.SendRRQ(szSendBuf, "netascii", "blksize", "1450", RCV_BEGIN_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:读寄存器第1步失败！\n");
        return hr;
    }

    // 2.服务器向客户机发送选项报文OACK
    int iTimes = 0;
    while (1)
    {
        iTimes++;
        memset(rgbRecvBuf, 0, MAX_RECV_SIZE_NVC);

        hr = m_tftp_ptl.RecvData((char*)rgbRecvBuf, MAX_RECV_SIZE_NVC, NULL, RCV_TIMEOUT_NVC);

        if (rgbRecvBuf[1] == TFTP_OACK)
        {
            break;
        }
        else if (FAILED(hr) || (iTimes > 3))
        {
            HV_Trace(3, "NVCLink.cpp:读寄存器第2步失败！\n");
            return E_FAIL;
        }
    }

    // 3.客户机向服务器发送确认报文ACK
    hr = m_tftp_ptl.SendACK(0x0000, RCV_BEGIN_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:读寄存器第3步失败！\n");
        return hr;
    }

    // 4.服务器向客户机发送数据报文DATA
    iTimes = 0;
    while (1)
    {
        iTimes++;
        memset(rgbRecvBuf, 0, MAX_RECV_SIZE_NVC);
        hr = m_tftp_ptl.RecvData((char*)rgbRecvBuf, MAX_RECV_SIZE_NVC, NULL, RCV_TIMEOUT_NVC);
        if (rgbRecvBuf[1] == TFTP_DATA)
        {
            wValue = BYTE8_WORD16_Ex(&rgbRecvBuf[4]);
            break;
        }
        else if ((hr != S_OK) || (iTimes > 3))
        {
            HV_Trace(3, "NVCLink.cpp:读寄存器第4步失败！\n");
            return E_FAIL;
        }
    }

    // 4.客户机向服务器发送确认报文ACK
    hr = m_tftp_ptl.SendACK(0x0001, RCV_BEGIN_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:读寄存器第5步失败！\n");
        return hr;
    }

    return S_OK;
}

HRESULT CNVCLink::WriteRegValue(BYTE8 bAddr, WORD16 wValue)
{
    HRESULT hr(S_OK);
    BYTE8 rgbRecvBuf[MAX_RECV_SIZE_NVC] = {0};

    // 1、客户机向服务器发送写请求报文WRQ
    hr = m_tftp_ptl.SendWRQ("reg_set", "netascii", "blksize", "1450", RCV_BEGIN_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:写寄存器第1步失败！\n");
        return hr;
    }

    // 2、服务器向客户机发送选项报文OACK
    int iTimes = 0;
    while (1)
    {
        iTimes++;
        memset(rgbRecvBuf, 0, MAX_RECV_SIZE_NVC);
        hr = m_tftp_ptl.RecvData((char*)rgbRecvBuf, MAX_RECV_SIZE_NVC, NULL, RCV_TIMEOUT_NVC);
        if (rgbRecvBuf[1] == TFTP_OACK)
        {
            break;
        }
        else if  ((hr != S_OK) || (iTimes > 3))
        {
            HV_Trace(3, "NVCLink.cpp:写寄存器第2步失败！\n");
            return E_FAIL;
        }
    }

    // 3、客户机向服务器发送数据报文DATA
    BYTE8 rgbDATA[10] = {0};
    rgbDATA[0] = bAddr;
    rgbDATA[1] = BYTE8((wValue >> 8) & 0x00FF);
    rgbDATA[2] = BYTE8(wValue & 0x00FF);
    hr = m_tftp_ptl.SendDATA(0x0001, (char*)rgbDATA, 3, RCV_BEGIN_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:写寄存器第3步失败！\n");
        return hr;
    }

    return S_OK;
}

HRESULT CNVCLink::Connect(const char* pszIP)
{
    HRESULT hr(S_OK);
    if (S_OK != m_tftp_ptl.Connect(pszIP))
    {
        return E_FAIL;
    }

    WORD16 wWidth(2048);
    WORD16 wHeight(1536);

    hr = GetWidthFromCamera(wWidth);
    if (hr != S_OK) return hr;
    hr = GetHeightFromCamera(wHeight);
    if (hr != S_OK) return hr;
    return SetImgInfo(wWidth, wHeight);
}

HRESULT CNVCLink::DisConnect()
{
    return m_tftp_ptl.DisConnect();
}

HRESULT CNVCLink::GetHeightFromCamera(WORD16& wHeight)
{
    return ReadRegValue(0x49, wHeight);
}

HRESULT CNVCLink::GetWidthFromCamera(WORD16& wWidth)
{
    return ReadRegValue(0x48, wWidth);
}

HRESULT CNVCLink::GetDevVer(WORD16& wDevVer)
{
    return ReadRegValue(123, wDevVer);
}

HRESULT CNVCLink::GetDevType(WORD16& wDevType)
{
    return ReadRegValue(122, wDevType);
}

HRESULT CNVCLink::SaveConfigForever()
{
    HRESULT hr(S_OK);
    hr = WriteRegValue(112, 123);
    if (S_OK == hr)
    {
        hr = WriteRegValue(113, 0);
        if (hr != S_OK)
        {
            HV_Trace(3, "NVCLink.cpp:永久保存设置第2步失败！\n");
        }
    }
    else
    {
        HV_Trace(3, "NVCLink.cpp:永久保存设置第1步失败！\n");
    }
    return hr;
}

HRESULT CNVCLink::RestoreDefaultConfig()
{
    HRESULT hr(S_OK);
    hr = WriteRegValue(0x02, 0x0069);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x01, 0x000F);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x10, 0x0002);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x0F, 0x0005);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x45, 0x0080);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x44, 0x0080);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x26, 0x0000);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x04, 0x011D);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x02, 0x0055);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x02, 0x0069);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x46, 0x0000);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x47, 0x0000);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x48, 0x0640);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x49, 0x04B0);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x46, 0x0000);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x47, 0x0000);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x48, 0x0640);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x49, 0x04B0);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x07, 0x0008);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x0B, 0x0002);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x0D, 0x0014);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x02, 0x0055);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x02, 0x0069);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x11, 0x0000);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x51, 0x0007);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x54, 0x0040);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x53, 0x00A0);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x51, 0x0007);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x60, 0x0008);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x65, 0x0048);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x54, 0x0019);
    if (hr != S_OK) return hr;
    hr = WriteRegValue(0x60, 0x0040);
    if (hr != S_OK) return hr;

    return S_OK;
}

HRESULT CNVCLink::GetImageHeight(DWORD32& dwHeight)
{
    dwHeight = (DWORD32)m_wHeight;
    return S_OK;
}

HRESULT CNVCLink::GetImageWidth(DWORD32& dwWidth)
{
    dwWidth = (DWORD32)m_wWidth;
    return S_OK;
}

HRESULT CNVCLink::SetImgInfo(WORD16 wWidth /*= 2048*/, WORD16 wHeight/* = 1536*/)
{
    HRESULT hr(S_OK);

    m_wWidth = wWidth;
    m_wHeight = wHeight;

    CalculateHeader(m_rgbJpegHeader, wWidth, wHeight, 16, (float)JPEG_CHROMA[m_iQuality], (float)JPEG_CHROMA[m_iQuality]);

    return hr;
}

HRESULT CNVCLink::SetQuality(WORD16 wQuality/* = 11*/)
{
    HRESULT hr(S_OK);
    if ((wQuality < 1) || (wQuality > 25))
    {
        wQuality = 11;
        hr = S_FALSE;
    }
    m_iQuality = wQuality;
    return hr;
}

//#if RUN_PLATFORM == PLATFORM_DSP_BIOS
//#pragma DATA_ALIGN(128)
//#endif
near char g_rgRecvBuf[2][MAX_RECV_SIZE_NVC];

HRESULT CNVCLink::GetJpgImge(char* pcBuf, DWORD32* pdwSize,  DWORD32* pdwTime, WORD16 wRes/* = 1*/)
{
    if (NULL == pcBuf) return E_POINTER;

    char *pRecvFastBuf = g_rgRecvBuf[0];
    char *pRecvFastBufLast = g_rgRecvBuf[1];
    char* pcDes(pcBuf);

    memcpy(pcDes, m_rgbJpegHeader, JPEG_HEAD_SIZE);
    pcDes += JPEG_HEAD_SIZE;

    HRESULT hr(S_OK);

    char szFileName[256];
    sprintf(szFileName, "image?res=full;x0=0;y0=0;x1=%d;y1=%d;quality=%d;doublescan=0;", m_wWidth, m_wHeight, m_iQuality);
    hr = m_tftp_ptl.SendRRQ(szFileName, "netascii", "blksize", "1450", RCV_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:获取图片发送请求失败！\n");
        return hr;
    }

    // 服务器向客户机发送选项报文OACK
    int iTimes = 0;
    while (1)
    {
        iTimes ++;
        hr = m_tftp_ptl.RecvData((char*)pRecvFastBuf, MAX_RECV_SIZE_NVC, NULL, RCV_TIMEOUT_NVC);
        if (pRecvFastBuf[1] == TFTP_OACK)
        {
            break;
        }
        else if (hr != S_OK)
        {
            HV_Trace(3, "NVCLink.cpp:获取图片接收OACK失败！\n");
            return hr;
        }
        else if (iTimes > 2)
        {
            HV_Trace(3, "NVCLink.cpp:获取图片接收OACK超时！\n");
            return S_FALSE;
        }
    }

    DWORD32 dwBlockSize = 1450;
    DWORD32 dwRecvSize = 0;
    DWORD32 dwCountSize = 0;
    DWORD32 dwDataSize = 0;
    DWORD32 dwOffSet = 0;

    // 客户机向服务器发送确认报文ACK
    hr = m_tftp_ptl.SendACK(0, RCV_TIMEOUT_NVC);
    if (hr != S_OK)
    {
        HV_Trace(3, "NVCLink.cpp:获取图片发送ACK0失败！\n");
        return hr;
    }

    iTimes = 0;
    while (1)
    {
        static DWORD32 dwCurBlockNo = 0;
        // 得到图片数据
        if (iTimes >= 10)
        {
            HV_Trace(3, "NVCLink.cpp:获取图片尝试次数过多！\n");
            hr = S_FALSE;
            break;
        }
        hr = m_tftp_ptl.RecvData((char*)pRecvFastBuf, MAX_RECV_SIZE_NVC, (int*)&dwRecvSize, RCV_TIMEOUT_NVC);
        if (hr != S_OK)
        {
            HV_Trace(3, "NVCLink.cpp:获取图片接收DATA包失败! 当前包号：%d\n", dwCurBlockNo);
            break;
        }
        if (pRecvFastBuf[1] != TFTP_DATA)
        {
            iTimes++;
            continue;
        }

        // 客户机向服务器发送确认报文ACK
        dwCurBlockNo = BYTE8_WORD16_Ex((BYTE8*)&pRecvFastBuf[2]);
        hr = m_tftp_ptl.SendACK((WORD16)(dwCurBlockNo), RCV_TIMEOUT_NVC);
        if (hr != S_OK)
        {
            HV_Trace(3, "NVCLink.cpp:获取图片发送ACKN失败！\n");
            break;
        }

        // 当接收到第一个数据包时，得到当前时间
        if ((dwCurBlockNo == 1) && (pdwTime))
        {
            *pdwTime = GetSystemTick();
        }
        dwDataSize = dwRecvSize - 4;

        if (dwDataSize > (*pdwSize) - dwCountSize - JPEG_HEAD_SIZE)
        {
            HV_Trace(3, "NVCLink.cpp:获取图片内存不足！\n");
            hr = E_OUTOFMEMORY;
            break;
        }

        char *pDst = pcDes + (dwCurBlockNo - 1) * dwBlockSize - dwOffSet;
        if (((BYTE8)pRecvFastBuf[4] == 0xFF) && ((BYTE8)pRecvFastBuf[5] == 0xD8))
        {
            memcpy(pDst, &pRecvFastBuf[6], dwDataSize - 2);
            dwOffSet += 2;
            dwCountSize += (dwDataSize - 2);
        }
        else
        {
            memcpy(pDst, &pRecvFastBuf[4], dwDataSize);
            dwCountSize += dwDataSize;
        }
        char *pTemp = pRecvFastBufLast;
        pRecvFastBufLast = pRecvFastBuf;
        pRecvFastBuf = pTemp;

        if (dwDataSize < dwBlockSize)
        {
            hr = S_OK;
            break;
        }
    }

    *pdwSize = JPEG_HEAD_SIZE + dwCountSize;

    return hr;
}
