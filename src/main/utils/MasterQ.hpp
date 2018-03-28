#ifndef HI_MASTERQ_H
#define HI_MASTERQ_H
#include <queue>
#include "../datastructures/rm/WorkItem.hpp"
#include "gtest/gtest.h"

TEST(Dummy, foobar)
{
    EXPECT_EQ(1, 1);
}


class MasterQ {
public:
    std::queue<WorkItem> q;
    MasterQ(){

    }
};


#endif //HI_MASTERQ_H
