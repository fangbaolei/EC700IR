#include "stdafx.h"
#include "ToolsFunction.h"

HRESULT GetPlateNumByXmlExtInfo(const char* szXmlExtInfo, char* szPlateNum, DWORD32* pdwTimeLow, DWORD32* pdwTimeHigh, DWORD32* pdwCarID, char* IllegalInfo)
{
	BOOL fHadTimeInfo = FALSE;
	TiXmlDocument cXmlDoc;
	const TiXmlElement* pElementname;
	if ( cXmlDoc.Parse(szXmlExtInfo) )
	{
		const TiXmlElement* pRootElement = cXmlDoc.RootElement();
		if ( pRootElement )
		{
			const TiXmlElement* pElementResultSet = pRootElement->FirstChildElement("ResultSet");
			if ( pElementResultSet )
			{
				const TiXmlElement* pElementResult = pElementResultSet->FirstChildElement("Result");
				if ( pElementResult )
				{
					const TiXmlElement* pElementPlateName = pElementResult->FirstChildElement("PlateName");
					if ( pElementPlateName )
					{
						char szPlateColor[3] = {0};
						memcpy(szPlateColor, pElementPlateName->GetText(), 2);
						if (strcmp(szPlateColor, "蓝") == 0
							|| strcmp(szPlateColor, "绿") == 0
							|| strcmp(szPlateColor, "黄") == 0
							|| strcmp(szPlateColor, "白") == 0
							|| strcmp(szPlateColor, "黑") == 0)
						{
							strcpy(szPlateNum, pElementPlateName->GetText());
						}
						else
						{
							// 非有牌车前需要增加两个空格
							strcpy(szPlateNum, "  ");
							strcat(szPlateNum, pElementPlateName->GetText());
						}

						// 识别结果时间
						pElementname = pElementResult->FirstChildElement("TimeLow");
						if (pElementname)
						{
							fHadTimeInfo = TRUE;
							sscanf(pElementname->Attribute("value"), "%u", pdwTimeLow);
						}

						pElementname = pElementResult->FirstChildElement("TimeHigh");
						if (pElementname)
						{
							sscanf(pElementname->Attribute("value"), "%u", pdwTimeHigh);
						}

						pElementname = pElementResult->FirstChildElement("CarID");
						if (pElementname)
						{
							sscanf(pElementname->Attribute("value"), "%u", pdwCarID);
						}


						// 新XML附加信息中加入了"chnname"中文节点，方便以后拓展
						if (fHadTimeInfo)
						{
							const char *pszChnName;
							const char *pszValue;
							pElementname = pElementResult->FirstChildElement();
							while (pElementname)
							{
								pszChnName = pElementname->Attribute("chnname");
								pszValue = pElementname->Attribute("value");
								if (pszChnName && pszValue)
								{
									// 事件检测附加信息直接取value
									if (strstr(pszChnName, "事件检测") != NULL)
									{
										strcat(szPlateNum, "\n");
										strcat(szPlateNum, pszValue);
										const char *temp1 = NULL, *temp2 = NULL, *temp3 = NULL, *temp4 = NULL;
										temp1 = strstr((const char*)pszValue, "违章:");
										if ( temp1 != NULL )
										{
											temp2 = strchr(temp1, '\n');
											int IllInfoLen = 0;
											int illHeadLen = strlen("违章:是");
											if(temp2 != NULL)
											{
												IllInfoLen = strlen(temp1) - strlen(temp2);
												if(IllInfoLen > illHeadLen)
												{
													temp3 = strchr(temp1,'<');
													strncpy(IllegalInfo, temp3, IllInfoLen - illHeadLen);
													IllegalInfo[IllInfoLen - illHeadLen] = '\0';
												}
											}
											else
											{
												IllInfoLen = strlen(temp1);
												if(IllInfoLen > illHeadLen)
												{
													temp3 = strchr(temp1,'<');
													strncpy(IllegalInfo, temp3, IllInfoLen - illHeadLen);
													IllegalInfo[IllInfoLen - illHeadLen] = '\0';
												}
											}
										}

									}
									else
									{
										strcat(szPlateNum, "\n");
										strcat(szPlateNum, pszChnName);
										strcat(szPlateNum, ":");
										strcat(szPlateNum, pszValue);
									}
								}
								pElementname = pElementname->NextSiblingElement();
							}
						}
						else // 不包含chnname中文节点则需要逐个进行解析
						{
							pElementname = pElementResult->FirstChildElement("ReverseRun");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车辆逆向行驶:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("VideoScaleSpeed");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "视频测速:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("SpeedLimit");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "限速值:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("ScaleSpeedOfDistance");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "距离测量误差比例:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("ObservedFrames");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "有效帧数:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("Confidence");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "平均可信度:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("FirstCharConf");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "首字可信度:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("CarArriveTime");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车辆检测时间:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("CarType");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车辆类型:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("CarColor");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车身颜色:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("RoadNumber");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车道:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("BeginRoadNumber");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "起始车道号:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("StreetName");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "路口名称:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("StreetDirection");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "路口方向:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("EventCheck");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("FrameName");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "视频帧名:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("PlateLightType");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "摄相机亮度级别:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("AmbientLight");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "环境亮度:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("PlateLight");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车牌亮度:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}

							pElementname = pElementResult->FirstChildElement("PlateVariance");
							if (pElementname)
							{
								strcat(szPlateNum, "\n");
								strcat(szPlateNum, "车牌对比度:");
								strcat(szPlateNum, pElementname->Attribute("value"));
							}
						}
						return S_OK;
					}
				}
			}
		}
	}

	return E_FAIL;
}

HRESULT SaveStreamFile(IStream* pStmData, int iSize, const char* pszFilePath)
{
	if (NULL == pStmData || NULL == pszFilePath)
	{
		return E_POINTER;
	}

	LARGE_INTEGER liTemp = {0};
	pStmData->Seek(liTemp,STREAM_SEEK_SET,NULL);
	ULONG ulSize = 0;
	int iSizeNew;
	iSizeNew = (iSize > 1024 * 300) ? iSize:1024 * 300;

	BYTE* pDataByte = new BYTE[iSizeNew * 2];

	HRESULT hr = pStmData->Read(pDataByte, iSizeNew, &ulSize);

	if (hr != S_OK)
	{
		delete [] pDataByte;
		pDataByte = NULL;

		return hr;
	}


	int iRealSize = 0;
	FILE* pFile = fopen(pszFilePath, "wb");
	if (pFile)
	{
		iRealSize = (int)fwrite(pDataByte, ulSize, 1, pFile);
		fclose(pFile);
	}
	delete [] pDataByte;
	pDataByte = NULL;
	return S_OK;
}

////////////////////////////////////////////////////////////////////
//
//  函数名称 : VPR_BinImage2BMP(int *piByteBinImagLen,BYTE* pByteBinImage,char * pchBMPFileName)
//  函数功能 : 二值化车牌转换为BMP图片
//  参数说明 : iByteBinImageLen----车辆二值图的大小
//             pByteBinImage----车辆二值图
//			   pchBMPFileName----BMP牌照图片文件的完整目录路径
//  返回值   : TRUE----转换成功,FALSE----转换失败
//  备注     : 无
//
////////////////////////////////////////////////////////////////////
BOOL BinImage2BMP(int iByteBinImageLen, BYTE* pByteBinImage, const char * pchBMPFileName)
{	
	BOOL bSuccess=TRUE;	
	/************** 车牌识别厂家填入代码开始 ******************/
	FILE *pfile = fopen( pchBMPFileName, "wb" );
	if ( pfile == NULL )
	{
		bSuccess = FALSE;
	}
	else
	{
		DWORD32 dwOffSet = 54;
		DWORD32 dwRBGStride = ( ( BIN_WIDTH * 3 + 2 ) >> 2 ) << 2;
		DWORD32 dwRgbSize = dwRBGStride * BIN_HEIGHT;
		DWORD32 dwBmpSize = dwOffSet + dwRgbSize;
		BYTE *pbBmp = new BYTE[ dwBmpSize ];
		BYTE *pbDest = pbBmp;
		*( pbDest++ ) = 'B';									//BMP文件标志.
		*( pbDest++ ) = 'M';
		pbDest += SetInt32ToByteArray( pbDest, dwBmpSize );		//BMP文件大小.
		pbDest += SetInt32ToByteArray( pbDest, 0 );				//reserved
		pbDest += SetInt32ToByteArray( pbDest, dwOffSet);		//文件头偏移量.
		pbDest += SetInt32ToByteArray( pbDest, 0x28 );			//biSize.
		pbDest += SetInt32ToByteArray( pbDest, BIN_WIDTH );		//biWidth.
		pbDest += SetInt32ToByteArray( pbDest, BIN_HEIGHT );	//biHeight.
		pbDest += SetInt16ToByteArray( pbDest, 1 );				//biPlanes.
		pbDest += SetInt16ToByteArray( pbDest, 24 );			//biBitCount.
		pbDest += SetInt32ToByteArray( pbDest, 0 );				//biCompression.
		pbDest += SetInt32ToByteArray( pbDest, dwRgbSize );		//biSizeImage.
		pbDest += SetInt32ToByteArray( pbDest, 0 );				//biXPerMeter.
		pbDest += SetInt32ToByteArray( pbDest, 0 );				//biYPerMeter.
		pbDest += SetInt32ToByteArray( pbDest, 0 );				//biClrUsed.
		pbDest += SetInt32ToByteArray( pbDest, 0 );				//biClrImportant.
		for ( int y = 0; y < BIN_HEIGHT; y++ )
		{
			for ( int x = 0; x < BIN_WIDTH; x++ )
			{
				if ( pByteBinImage[ ( y * BIN_WIDTH + x ) / BIN_BIT_COUNT ] &
					( 1 << ( ( y * BIN_WIDTH + x ) % BIN_BIT_COUNT ) ) )
				{
					*( pbDest + ( BIN_HEIGHT - y - 1 ) * dwRBGStride + x * 3 + 0 ) = 255;
					*( pbDest + ( BIN_HEIGHT - y - 1 ) * dwRBGStride + x * 3 + 1 ) = 255;
					*( pbDest + ( BIN_HEIGHT - y - 1 ) * dwRBGStride + x * 3 + 2 ) = 255;
				}
				else
				{
					*( pbDest + ( BIN_HEIGHT - y - 1 ) * dwRBGStride + x * 3 + 0 ) = 0;
					*( pbDest + ( BIN_HEIGHT - y - 1 ) * dwRBGStride + x * 3 + 1 ) = 0;
					*( pbDest + ( BIN_HEIGHT - y - 1 ) * dwRBGStride + x * 3 + 2 ) = 0;
				}
			}
		}
		fwrite( pbBmp, 1, dwBmpSize, pfile );
		fclose( pfile );
		delete[] pbBmp;
	}
	/************** 车牌识别厂家填入代码结束 ******************/	
	return bSuccess;
}

//获取磁盘状态
//参数：
//filename: 包含路径的文件名，也可以是路径
//返回：
//true : 有磁盘空间，可以保存数据
//false : 磁盘空间小于200M，停止保存数据
BOOL DiskStatus(LPCTSTR filename)
{
	char disk[3];
	disk[0] = filename[0];
	disk[1] = filename[1];
	disk[2] = '\0';

	if ( disk[0] == '\\' && disk[0] == '\\')  //网络路径，直接返回为真
	{
		return true;
	}

	if(disk[1] != ':')
	{
		return false;
	}
	ULARGE_INTEGER nFreeBytesAvailable;
	nFreeBytesAvailable.QuadPart = 0;
	ULARGE_INTEGER nTotalNumberOfBytes;
	ULARGE_INTEGER nTotalNumberOfFreeBytes;
	::GetDiskFreeSpaceEx(disk , &nFreeBytesAvailable , &nTotalNumberOfBytes , &nTotalNumberOfFreeBytes);
	if(((double)nFreeBytesAvailable.QuadPart)/1024/1024 <= 200)
	{
		return false;
	}

	return true;
}