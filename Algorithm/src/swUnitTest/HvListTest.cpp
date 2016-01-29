#include <gtest/gtest.h>

#include "HvList.h"
using namespace HiVideo;

//基本功能测试
TEST(HvListTest, Test1)
{
    CHvList<int> cList;

    ASSERT_TRUE(cList.IsEmpty());
    ASSERT_FALSE(cList.IsFull());

    ASSERT_EQ(0, cList.GetSize());

    ASSERT_EQ(-1, cList.GetHeadPosition());
    ASSERT_EQ(-1, cList.GetTailPosition());

    ASSERT_EQ(0, cList.AddHead(1));
    ASSERT_EQ(0, cList.AddHead(2));
    ASSERT_EQ(0, cList.AddHead(3));

    ASSERT_FALSE(cList.IsEmpty());

    ASSERT_EQ(3, cList.GetSize());

    ASSERT_EQ(0, cList.GetHeadPosition());
    ASSERT_EQ(2, cList.GetTailPosition());

    ASSERT_EQ(3, cList.AddTail(89));
    ASSERT_EQ(4, cList.AddTail(101));

    ASSERT_EQ(5, cList.GetSize());

    ASSERT_EQ(0, cList.GetHeadPosition());
    ASSERT_EQ(4, cList.GetTailPosition());

    HVPOSITION pos;
    pos = 0;
    ASSERT_TRUE(cList.IsValidPos(pos));
    pos = 2;
    ASSERT_TRUE(cList.IsValidPos(pos));
    pos = 4;
    ASSERT_TRUE(cList.IsValidPos(pos));
    pos = -1;
    ASSERT_FALSE(cList.IsValidPos(pos));
    pos = 5;
    ASSERT_FALSE(cList.IsValidPos(pos));
    pos = 6;
    ASSERT_FALSE(cList.IsValidPos(pos));

    cList.RemoveAll();
    ASSERT_TRUE(cList.IsEmpty());
    ASSERT_EQ(0, cList.GetSize());
}

//元素获取测试
TEST(HvListTest, Test2)
{
    CHvList<int> cList;

    cList.AddTail(3);
    cList.AddTail(2);
    cList.AddTail(1);
    cList.AddTail(89);
    cList.AddTail(101);
    //链表的当前内容：3,2,1,89,101

    HVPOSITION p1;
    int iData;

    p1 = -5;
    iData = cList.GetNext(p1);
    ASSERT_EQ(3, iData);
    ASSERT_EQ(-1, p1);

    p1 = 0;
    iData = cList.GetNext(p1);
    ASSERT_EQ(3, iData);
    ASSERT_EQ(1, p1);

    p1 = 2;
    iData = cList.GetNext(p1);
    ASSERT_EQ(1, iData);
    ASSERT_EQ(3, p1);

    p1 = 3;
    iData = cList.GetNext(p1);
    ASSERT_EQ(89, iData);
    ASSERT_EQ(4, p1);

    p1 = 4;
    iData = cList.GetNext(p1);
    ASSERT_EQ(101, iData);
    ASSERT_EQ(-1, p1);

    p1 = 1000;
    iData = cList.GetNext(p1);
    ASSERT_EQ(101, iData);
    ASSERT_EQ(-1, p1);

    p1 = -20;
    iData = cList.GetPrev(p1);
    ASSERT_EQ(3, iData);
    ASSERT_EQ(-1, p1);

    p1 = 0;
    iData = cList.GetPrev(p1);
    ASSERT_EQ(3, iData);
    ASSERT_EQ(-1, p1);

    p1 = 2;
    iData = cList.GetPrev(p1);
    ASSERT_EQ(1, iData);
    ASSERT_EQ(1, p1);

    p1 = 3;
    iData = cList.GetPrev(p1);
    ASSERT_EQ(89, iData);
    ASSERT_EQ(2, p1);

    p1 = 4;
    iData = cList.GetPrev(p1);
    ASSERT_EQ(101, iData);
    ASSERT_EQ(3, p1);

    p1 = 990;
    iData = cList.GetPrev(p1);
    ASSERT_EQ(101, iData);
    ASSERT_EQ(-1, p1);
}

//元素移除测试
TEST(HvListTest, Test3)
{
    CHvList<int> cList;

    cList.AddTail(3);
    cList.AddTail(2);
    cList.AddTail(1);
    cList.AddTail(89);
    cList.AddTail(101);
    //链表的当前内容：3,2,1,89,101
    int iData;

    iData = cList.RemoveHead();
    ASSERT_EQ(3, iData);

    iData = cList.RemoveTail();
    ASSERT_EQ(101, iData);

    iData = cList.RemoveHead();
    ASSERT_EQ(2, iData);

    iData = cList.RemoveTail();
    ASSERT_EQ(89, iData);

    ASSERT_EQ(1, cList.GetSize());

    cList.AddTail(2);
    cList.AddTail(3);
    cList.AddTail(4);
    cList.AddTail(5);
    cList.AddTail(6);
    //链表的当前内容：1,2,3,4,5,6
    ASSERT_EQ(6, cList.GetSize());

    HVPOSITION pos;

    pos = 1;
    cList.RemoveAt(pos);
    ASSERT_EQ(5, cList.GetSize());

    pos = 0;
    cList.RemoveAt(pos);

    iData = cList.RemoveHead();
    ASSERT_EQ(3, iData);
}
