#include <gtest/gtest.h>

#include "HvString.h"
using namespace HiVideo;

//基本功能测试
TEST(HvStringTest, Test1)
{
    CHvString str;
    ASSERT_STREQ("", str.GetBuffer());
    ASSERT_TRUE(str.IsEmpty());
    ASSERT_EQ(0, str.GetLength());

    str = "hello";
    ASSERT_STREQ("hello", str.GetBuffer());
    ASSERT_FALSE(str.IsEmpty());
    ASSERT_EQ(5, str.GetLength());

    str.Append(",world");
    ASSERT_STREQ("hello,world", str.GetBuffer());
    ASSERT_EQ(11, str.GetLength());

    int iRet = str.Format("110 %s = %d.", "warning",120);
    ASSERT_STREQ("110 warning = 120.", str.GetBuffer());
    ASSERT_EQ(18, iRet);
}

//大小写转换以及提取子串功能测试
TEST(HvStringTest, Test2)
{
    CHvString str("hi,aBcD,123");
    ASSERT_STREQ("hi,aBcD,123", str.GetBuffer());

    CHvString strUp;
    strUp = str.MakeUpper();
    ASSERT_STREQ("hi,aBcD,123", str.GetBuffer());
    ASSERT_STREQ("HI,ABCD,123", strUp.GetBuffer());

    CHvString strLow;
    strLow = str.MakeLower();
    ASSERT_STREQ("hi,aBcD,123", str.GetBuffer());
    ASSERT_STREQ("hi,abcd,123", strLow.GetBuffer());

    str = "hi,you are good!";
    CHvString strL2 = str.Left(2);
    ASSERT_STREQ("hi,you are good!", str.GetBuffer());
    ASSERT_STREQ("hi", strL2.GetBuffer());

    CHvString strMid = str.Mid(0, 6);
    ASSERT_STREQ("hi,you are good!", str.GetBuffer());
    ASSERT_STREQ("hi,you", strMid.GetBuffer());

    strMid = str.Mid(3,3);
    ASSERT_STREQ("you", strMid.GetBuffer());
}

//插入，移除，寻找测试
TEST(HvStringTest, Test3)
{
    CHvString str;
    str = "a";
    ASSERT_EQ(2, str.Insert(0, "c"));
    ASSERT_STREQ("ca", (const char*)(str));

    ASSERT_EQ(7, str.Insert(1, "hello"));
    ASSERT_STREQ("chelloa", str.GetBuffer());

    ASSERT_EQ(10, str.Insert(7, "end"));
    ASSERT_STREQ("chelloaend", str.GetBuffer());

    ASSERT_EQ(2, str.Remove('e'));
    ASSERT_STREQ("chlloand", str.GetBuffer());

    ASSERT_EQ(2, str.Remove('l'));
    ASSERT_STREQ("choand", str.GetBuffer());

    ASSERT_EQ(1, str.Remove('a'));
    ASSERT_STREQ("chond", str.GetBuffer());

    ASSERT_EQ(1, str.Remove('d'));
    ASSERT_STREQ("chon", str.GetBuffer());

    ASSERT_EQ(1, str.Remove('c'));
    ASSERT_STREQ("hon", str.GetBuffer());

    str = "abcdefgaxe";
    ASSERT_EQ(1, str.Find("bc"));
    ASSERT_EQ(6, str.Find("g"));
    ASSERT_EQ(0, str.Find("a"));

    ASSERT_EQ(1, str.Find("bc", 1));
    ASSERT_EQ(-1, str.Find("bc", 2));

    ASSERT_EQ(7, str.Find("a", 6));
}

//操作符重载测试
TEST(HvStringTest, Test4)
{
    CHvString str;
    str = "hi";
    ASSERT_STREQ("hi", str.GetBuffer());

    CHvString s;
    s = str;
    ASSERT_STREQ("hi", s.GetBuffer());

    s += str;
    ASSERT_STREQ("hihi", s.GetBuffer());

    s += "girl";
    ASSERT_STREQ("hihigirl", s.GetBuffer());

    s += '.';
    ASSERT_STREQ("hihigirl.", s.GetBuffer());

    CHvString s2;
    s2 = s + str;
    ASSERT_STREQ("hihigirl.hi", s2.GetBuffer());

    CHvString s3;
    s3 = str + ",xyz";
    ASSERT_STREQ("hi,xyz", s3.GetBuffer());

    ASSERT_EQ('i', s3[1]);
    ASSERT_EQ('h', s3[0]);
    ASSERT_EQ('y', s3[4]);

    CHvString ss1;
    CHvString ss2;

    ss1 = "hi";
    ss2 = "h";

    ASSERT_TRUE(ss1 != ss2);
    ASSERT_TRUE(ss1 != "kkk");

    ss2 = "hi";
    ASSERT_TRUE(ss1 == ss2);
    ASSERT_TRUE(ss1 == "hi");

    ss1 = "a";
    ss2 = "b";
    ASSERT_TRUE(ss1 <= ss2);

    ss2 = "a";
    ASSERT_TRUE(ss1 <= ss2);

    ss1 = "6";
    ss2 = "2";
    ASSERT_TRUE(ss1 >= ss2);

    ss2 = "6";
    ASSERT_TRUE(ss1 >= ss2);
}
