///////////////////////////////////////////////////////////
//  templateclass TCSWParameter.h
//  Implementation of the Class template<class T>CSWParameter
//  Created on:      28-二月-2013 14:09:47
//  Original author: zy
///////////////////////////////////////////////////////////

#if !defined(EA_DDB8A06A_E403_4af8_B339_EF47910886C5__INCLUDED_)
#define EA_DDB8A06A_E403_4af8_B339_EF47910886C5__INCLUDED_

#include "SWStream.h"
#include "SWParamStore.h"


/**
 * @brief 参数模板类
 */
template<class T>
class CSWParameter : public CSWObject
{
CLASSINFO(CSWParameter,CSWObject)
public:
	/**
	 * @brief 构造函数
	 */
	CSWParameter()
    {
		m_fNeedUpdate = FALSE;
    };
	/**
	 * @brief 析构函数
	 */
	virtual ~CSWParameter()
    {
    };
	/**
	 * @brief 保存参数到非易失设备
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT Commit()
    {

        CSWFileStream objStream; HRESULT result = E_FAIL;

        const CHAR *strSaveFilePath = m_strSaveFilePath;

		// 如果不需要更新，就不重写到存储。
		if( !m_fNeedUpdate )
		{
			printf("<SWParameter>Commit do not update %s.\n", strSaveFilePath);
			return S_OK;
		}

		printf("<SWParameter>commit %s.\n", strSaveFilePath);

        // 写文件
        if( S_OK == objStream.Initialize( (CHAR*)strSaveFilePath, "w" ))
		{
			// 先写入版本信息头
			STREAM_INFO cInfoHeader;
			result = objStream.Write(&cInfoHeader, sizeof(cInfoHeader), NULL);
			if( S_OK == result )
			{
				// 保存 只保存值。
				result =  m_objSWParamStore.SaveToStream(&objStream, 1);
				if( S_OK == result)
				{
					result = objStream.Commit( );
				}
			}
        }

		printf("<SWParameter>commit %s. return 0x%08x\n", strSaveFilePath, result);
		m_fNeedUpdate = FALSE;
        return result;
    };
	/**
	 * @brief 获取参数对象的结构体
	 * @param [out] Type : 参数结构体的引用
	 * @return - S_OK : 成功 - E_FAIL :  失败
	 */
	virtual T& Get( )
	{
	    return m_tValue;
	}
	/**
	 * @brief 参数对象初始化函数
	 * @param [in] szParaFilename : 参数文件全路径
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT Initialize( CSWString strFilePath )
    {
        LoadFromFile( (m_strSaveFilePath = strFilePath) );
        return S_OK;
    };


	/**
	 * @brief 从XML流中加载
	 * @param [in] pXmlData : XML数据
	 * @param [in] nDataSize : 数据大小
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT LoadFromXml( CHAR* pXmlData , INT nDataSize )
    {
        if( pXmlData == NULL || nDataSize <= 0 ) return E_FAIL;


        TiXmlDocument objTiXmlDocument;
        if( NULL != objTiXmlDocument.Parse(  pXmlData ))
		{
			// 从XML加载时必须要更新存储。
			m_fNeedUpdate = TRUE;
            return this->XmlToParam( &objTiXmlDocument );
		}
        return E_FAIL;
    };

 	/**
	 * @brief 清除原有数据
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	 virtual HRESULT Clear( )
	 {
	 		m_objSWParamStore.Clear();
			m_fNeedUpdate = TRUE;
	 		return Commit();
	 }

	/**
	 * @brief  把参数转换成XML数据串
	 * @param [out] pbParamBuf : 输出参数缓存
	 * @param [in] inBufLen : 输出缓存大小
	 * @param [out] outDataLen : 输出数据实际长度
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
    HRESULT ToXml( CSWString& strParamXml )
    {
        return m_objSWParamStore.ToXml( strParamXml );
    }

private:
	typedef struct _STREAM_INFO
	{
		DWORD Tag;
		DWORD Flag;
		DWORD StreamLen;
		DWORD Crc32;

		_STREAM_INFO()
		{
			Tag = 0xFF000008;
			Flag = 0;
			StreamLen = 0;
			Crc32 = 0;
		}
	}STREAM_INFO;
public://protected:
	/**
	 * @brief 从文件中加载
	 * @param [in] strFilePath : 需要加载的文件
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	virtual HRESULT LoadFromFile( CSWString strFilePath )
    {
        const CHAR *szFile = strFilePath;
        CSWFileStream objSWFileStream;  
		HRESULT result = E_FAIL;

        // 恢复数据
		HRESULT hr = objSWFileStream.Initialize((CHAR*)szFile , "r");
		if( hr != S_OK )
		{
			printf("<param>Initialize %s. failed.\n", szFile);
			result = S_OK;
		}
		else
		{
			// 读取参数信息头
			DWORD dwRead = 0;
			STREAM_INFO cInfoHeader;
			if( S_OK == objSWFileStream.Read(&cInfoHeader, sizeof(cInfoHeader), &dwRead) 
				&& cInfoHeader.Tag == 0xFF000008 )
			{
				result = m_objSWParamStore.RestoreFromStream( &objSWFileStream);
				// 从文件加载时默认不需要更新存储。
				m_fNeedUpdate = FALSE; 
			}
			else
			{
				// 从文件加载失败时需要更新存储
				m_fNeedUpdate = TRUE; 
				printf("<param>Initialize %s.check failed, read:%d.delete all param.%d,%d,%d,%d.\n", szFile, dwRead, cInfoHeader.Tag, cInfoHeader.Flag, cInfoHeader.StreamLen, cInfoHeader.Crc32 );
				result = S_OK;
			}
		}

        return result;
    };

	/**
	 * @brief 读取参数文件中类型为枚举的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。 节点值的保存形式定义为&ldquo;
	 * '枚举值':'枚举名'&rdquo;，比如&ldquo;0:H264流&rdquo;； 节点值的取值范围保存形式定义为&ldquo;'枚举值0':'枚举名0';
	 * '枚举值1':'枚举名1';...&rdquo;，比如&ldquo;0:H264流;1: JPEG流;&rdquo;.
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szNode : 参数项存储的节点
	 * @param [out] piValue : 参数项的值
	 * @param [in] iDefaultVal : 参数项的默认值
	 * @param [in] szValueRange : 参数项的取值范围枚举
	 * @param [in] szChName : KEY的中文名
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @param [in] szOnlyRead : 该参数只读，不允许改写
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT GetEnum( const CHAR* szPath, const CHAR* szKeyName, INT* piValue
                 , const INT iDefaultVal, const CHAR* szValueRange, const CHAR* szChName
                 , const CHAR* szRemark, const DWORD dwLevel, const INT IsOnlyRead = false )
    {
		HRESULT hr = m_objSWParamStore.GetEnum( szPath, szKeyName, piValue , iDefaultVal
                                         , szValueRange, szChName
                                         , szRemark, dwLevel , IsOnlyRead);

		// 返回S_FALSE表示有更新
		if( hr == S_FALSE )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };
    	/**
	 * @brief 更新参数文件中类型为枚举的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。 节点值的保存形式定义为&ldquo;
	 * '枚举值':'枚举名'&rdquo;，比如&ldquo;0:H264流&rdquo;； 节点值的取值范围保存形式定义为&ldquo;'枚举值0':'枚举名0';
	 * '枚举值1':'枚举名1';...&rdquo;，比如&ldquo;0:H264流;1: JPEG流;&rdquo;.
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szNode : 参数项存储的节点
	 * @param [out] piValue : 参数项的值
	 * @param [in] iDefaultVal : 参数项的默认值
	 * @param [in] szValueRange : 参数项的取值范围枚举
	 * @param [in] szChName : KEY的中文名
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT UpdateEnum( const CHAR* szPath, const CHAR* szKeyName, const INT iValue )
    {
        HRESULT hr = m_objSWParamStore.UpdateEnum( szPath, szKeyName, iValue );
		if( S_OK == hr )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };
	/**
	 * @brief 读取参数文件中数据类型为float的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szNode : 参数项存储的节点
	 * @param [out] pfltValue : 参数项的值
	 * @param [in] fltDefaultVal : 参数项的默认值
	 * @param [in] fltMaxVal : 参数项的最大值
	 * @param [in] fltMinVal : 参数项的最小值
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @param [in] szOnlyRead : 该参数只读，不允许改写
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 "Section name=[] cname[]"
	 */
	HRESULT GetFloat(const CHAR* szPath, const CHAR* szKeyName, FLOAT* pfltValue
                  , const FLOAT fltDefaultVal, const FLOAT fltMaxVal, const FLOAT fltMinVal
                  , const CHAR* szChName , const CHAR* szRemark, const DWORD dwLevel
                  , const INT IsOnlyRead = false)
    {

        HRESULT hr = m_objSWParamStore.GetFloat( szPath , szKeyName
                                         , pfltValue , fltDefaultVal ,fltMaxVal, fltMinVal
										, szChName , szRemark, dwLevel , IsOnlyRead );

		// 返回S_FALSE表示有更新
		if( hr == S_FALSE )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };
	/**
	 * @brief 更新参数文件中数据类型为float的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szNode : 参数项存储的节点
	 * @param [out] pfltValue : 参数项的值
	 * @param [in] fltDefaultVal : 参数项的默认值
	 * @param [in] fltMaxVal : 参数项的最大值
	 * @param [in] fltMinVal : 参数项的最小值
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 "Section name=[] cname[]"
	 */
	HRESULT UpdateFloat( const CHAR* szPath, const CHAR* szKeyName, const FLOAT fltValue)
	{
		HRESULT hr = m_objSWParamStore.SetFloat( szPath, szKeyName, fltValue );
		if( S_OK == hr )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
	}

	/**
	 * @brief 读取参数文件中类型为INT的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。
	 * @param [in] szPath : 参数项储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szKeyName : 参数项存储的节点
	 * @param [out] psdwValue : 参数项的值
	 * @param [in] sdwDefaultVal : 参数项的默认值
	 * @param [in] sdwMaxVal : 参数项的最大值
	 * @param [in] sdwMinVal : 参数项的最小值
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @param [in] szOnlyRead : 该参数只读，不允许改写
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT GetInt(const CHAR* szPath, const CHAR* szKeyName, INT* psdwValue
                 , const INT sdwDefaultVal, const INT sdwMinVal, const INT sdwMaxVal
                 , const CHAR* szChName , const CHAR* szRemark, const DWORD dwLevel
                 , const INT IsOnlyRead = false)
    {
        HRESULT hr = m_objSWParamStore.GetInt(  szPath, szKeyName, psdwValue,
                 sdwDefaultVal,  sdwMinVal, sdwMaxVal,
                  szChName , szRemark, dwLevel , IsOnlyRead);
		// 返回S_FALSE表示有更新
		if( hr == S_FALSE )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };

 	/**
	 * @brief 跟新参数文件中类型为INT的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。
	 * @param [in] szPath : 参数项储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szKeyName : 参数项存储的节点
	 * @param [out] psdwValue : 参数项的值
	 * @param [in] sdwDefaultVal : 参数项的默认值
	 * @param [in] sdwMaxVal : 参数项的最大值
	 * @param [in] sdwMinVal : 参数项的最小值
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT UpdateInt( const CHAR* szPath, const CHAR* szKeyName, const INT dwValue)
    {
        HRESULT hr = m_objSWParamStore.SetInt(szPath, szKeyName, dwValue );
		if( S_OK == hr )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };

	/**
	 * @brief 读取参数文件中类型为集合的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。 节点值的保存形式定义为"'元素0',
	 * '元素1'..."，比如"'H264流','JPEG流'"； 节点值的取值范围保存形式定义为"'元素0','元素1'..."，比如"'H264流',
	 * 'JPEG流','YUV流'"。
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szKeyName : 参数项存储的节点
	 * @param [out] piValue : 参数项的值
	 * @param [in] iDefaultVal : 参数项的默认值
	 * @param [in] szValueRange : 参数项的取值范围枚举
	 * @param [in] dwMaxLen : 参数项字符串长度最大值
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @param [in] szOnlyRead : 该参数只读，不允许改写
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */

	// 目前此类型还未支持。
	// todo.
	HRESULT GetSet(const CHAR* szPath, const CHAR* szKeyName, INT* piValue
                , const INT iDefaultVal, const CHAR* szValueRange, const DWORD dwMaxLen
                , const CHAR* szChName , const CHAR* szRemark, const DWORD dwLevel
                , const INT IsOnlyRead = false)
    {
        HRESULT hr = m_objSWParamStore.GetSet(szPath, szKeyName, piValue, iDefaultVal
                                , szValueRange , dwMaxLen,  szChName
                                , szRemark, dwLevel
                                , IsOnlyRead);
		// 返回S_FALSE表示有更新
		if( hr == S_FALSE )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };

	/**
	 * @brief 更新参数文件中类型为集合的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。 节点值的保存形式定义为"'元素0',
	 * '元素1'..."，比如"'H264流','JPEG流'"； 节点值的取值范围保存形式定义为"'元素0','元素1'..."，比如"'H264流',
	 * 'JPEG流','YUV流'"。
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szKeyName : 参数项存储的节点
	 * @param [out] piValue : 参数项的值
	 * @param [in] iDefaultVal : 参数项的默认值
	 * @param [in] szValueRange : 参数项的取值范围枚举
	 * @param [in] dwMaxLen : 参数项字符串长度最大值
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */

	HRESULT UpdateSet(const CHAR* szPath, const CHAR* szKeyName, const INT iValue)
    {
        HRESULT hr = m_objSWParamStore.UpdateSet(szPath, szKeyName, iValue);
			if( S_OK == hr )
			{
				m_fNeedUpdate = TRUE;
			}
			return hr;
    };

	/**
	 * @brief 读取参数文件中类型为char*的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szKeyName : 参数项存储的节点
	 * @param [out] szValue : 参数项的值
	 * @param [in] szDefaultVal : 参数项的默认值
	 * @param [in] dwMaxLen : 参数项的最大长度
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @param [in] szOnlyRead : 该参数只读，不允许改写
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT GetString(const CHAR* szPath, const CHAR* szKeyName, CHAR* szValue
                   , const CHAR* szDefaultVal
                   , const DWORD dwMaxLen, const CHAR* szChName, const CHAR* szRemark
                   , const DWORD dwLevel
                   , const INT IsOnlyRead = false)
    {
        HRESULT hr = m_objSWParamStore.GetString( szPath, szKeyName, szValue, szDefaultVal
                   , dwMaxLen, szChName , szRemark, dwLevel,IsOnlyRead);
		// 返回S_FALSE表示有更新
		if( hr == S_FALSE )
		{
			m_fNeedUpdate = TRUE;
		}
		if(!swpa_strcmp(szValue, "NULL"))
		{
			swpa_strcpy(szValue, "");
		}
		return hr;
    };
	/**
	 * @brief 更新参数文件中类型为char*的设定项到参数结构体中，如果节点不存在则先在参数文件中生成节点，再返回节点默认值。
	 * @param [in] szPath : 参数项存储的路径,例如 \\System[中文名]\\CamLan[中文名]
	 * @param [in] szKeyName : 参数项存储的节点
	 * @param [out] szValue : 参数项的值
	 * @param [in] szDefaultVal : 参数项的默认值
	 * @param [in] dwMaxLen : 参数项的最大长度
	 * @param [in] szRemark : 对参数的注释说明
	 * @param [in] dwLevel : 参数的等级
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
	HRESULT UpdateString(const CHAR* szPath, const CHAR* szKeyName, const CHAR* szValue)
    {
        HRESULT hr = m_objSWParamStore.SetString( szPath, szKeyName, szValue);
		if( S_OK == hr )
		{
			m_fNeedUpdate = TRUE;
		}
		return hr;
    };
 	/**
	 * @brief  把XML数据转换为ParamNode数据存储，
	 * 调用该函数将会清除原有数据信息
	 * @param [in] pXmlDoc : 输入的是XML文件打开对象
	 * @param [in] szPath : XML中的SECTION 名
	 * @param [out] szNode : XML中的KEY名
	 * @return - S_OK : 成功 - E_FAIL : 失败
	 */
    HRESULT XmlToParam( TiXmlDocument *pXmlDoc )
    {
        return m_objSWParamStore.RestoreFromXml( pXmlDoc );
    }
private:
    T m_tValue;

    CSWParamStore m_objSWParamStore;

    CSWString m_strSaveFilePath;

	BOOL m_fNeedUpdate;
};
#endif // !defined(EA_DDB8A06A_E403_4af8_B339_EF47910886C5__INCLUDED_)

