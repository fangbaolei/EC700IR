/*
 *	(C)版权所有 2010 北京信路威科技发展有限公司
 */

/**
* @file		HvString.h
* @version	1.0
* @brief	CHvString类的定义
* @author	Shaorg
* @date		2010-7-26
*/

#ifndef _HVSTRING_H_
#define _HVSTRING_H_

#include <string>
using namespace std;

namespace HiVideo
{
	/*! @class	CHvString
	* @brief	基于STL中的string类实现，类似于MFC中的CString类。
	*/
	class CHvString
	{
	public:
		/**
		* @brief	默认构造函数
		*/
		CHvString();

		/**
		* @brief	构造函数
		* @param	szString	CHvString初始化后的值
		*/
		CHvString(const char* szString);

		/**
		* @brief	析构函数
		*/
		virtual ~CHvString();

	public:
		/**
		* @brief	获取字符缓冲区的指针
		* @return	指向字符缓冲区的常指针
		* @warning	该返回的指针只能用来读取其指向的内容，不可用来改变其指向的内容。
		*/
		const char* GetBuffer();

		/**
		* @brief	判断字符串是否为空
		* @return	为空：true，不为空：false
		*/
		bool IsEmpty();

		/**
		* @brief	附加CHvString字符串到末尾
		* @param	str	待附加的字符串
		* @return	void
		*/
		void Append(const CHvString& str);

		/**
		* @brief	生成格式化字符串
		* @param	szFormat	最终生成的字符串原型，其中可包括格式符。
		* @param	...			具体的用来替换格式符的数据，其个数由szFormat中的格式符个数确定。
		* @return	成功：最终生成的字符串的长度，失败：-1
		*/
		int Format(const char* szFormat, ...);

		/**
		* @brief	插入字符串到指定位置
		* @param	iIndex	字符串插入的位置（注：为0表示插入到最前端）
		* @param	psz		插入的字符串
		* @return	成功：新字符串的长度，失败：原字符串的长度
		*/
		int Insert(int iIndex, const char* psz);

		/**
		* @brief	获取字符串的长度
		* @return	字符串长度。注：该长度不包括结尾的\0字符
		*/
		int GetLength();

		/**
		* @brief	移除所有指定的字符
		* @param	ch	要移除的字符
		* @return	移除的字符个数
		*/
		int Remove(char ch);

		/**
		* @brief	从指定位置开始，搜寻指定字符串
		* @param	pszSub	要搜寻的字符串
		* @param	iStart	搜寻的起始位置
		* @return	找到的字符串的位置索引。注：该索引位置从0开始，未找到时返回-1
		*/
		int Find(const char* pszSub, int iStart = 0);

		/**
		* @brief	获取所有字母小写化后的字符串
		* @return	字母小写化后的字符串
		*/
		CHvString MakeLower();

		/**
		* @brief	获取所有字母大写化后的字符串
		* @return	字母大写化后的字符串
		*/
		CHvString MakeUpper();

		/**
		* @brief	提取左端指定长度的子串
		* @nCount	提取的字符串的长度
		* @return	提取到的字符串
		*/
		CHvString Left(int nCount);

		/**
		* @brief	提取子串
		* @param	iFirst	该子串的起始位置（注：0为最开始的位置）
		* @param	nCount	该子串的长度
		* @return	提取到的字符串
		*/
		CHvString Mid(int iFirst, int nCount = 0);

	public:
		/**
		* @brief	类型转换函数
		* @return	指向字符缓冲区的常指针
		* @see		GetBuffer()
		*/
		operator const char*();

	public:
		/**
		* @brief	赋CHvString值操作
		*/
		CHvString& operator =(const CHvString& str);

		/**
		* @brief	赋字符串值操作
		*/
		CHvString& operator =(const char* pchar);

		/**
		* @brief	附加CHvString字符串操作
		* @see		Append()
		*/
		CHvString& operator +=(const CHvString& str);

		/**
		* @brief	附加字符串操作
		*/
		CHvString& operator +=(const char* pchar);

		/**
		* @brief	附加字符操作
		*/
		CHvString& operator +=(const char ch);

		/**
		* @brief	连接CHvString字符串操作
		*/
		CHvString operator +(const CHvString& str);

		/**
		* @brief	连接字符串操作
		*/
		CHvString operator +(const char* pchar);

		/**
		* @brief	取指定位置字符操作
		* @param	iIndex	位置
		* @return	取到的字符
		*/
		char operator [](int iIndex);

		bool operator !=(const char* pchar);
		bool operator !=(const CHvString& str);
		bool operator ==(char* pchar);
		bool operator ==(const CHvString& str);
		bool operator >=(const CHvString& str);
		bool operator <=(const CHvString& str);

	private:
		string m_str;

		//通过调用Format方法生成的字符串所允许的最大长度。
		//注：若超过该长度则发生溢出！这项安全性由应用层保证。
		static const int FORMAT_BUFMAXLEN = 256;
	};
}

#endif
