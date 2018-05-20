
#include "Euc.hpp"
BOOST_CLASS_EXPORT_IMPLEMENT(Euc<int>)
BOOST_CLASS_EXPORT_IMPLEMENT(Euc<float>)

#if COMPILE_TESTS
#include "gtest/gtest.h"

TEST(data, Euc_test_serialization)
{
    std::vector<int> p = {0,1,2};
    Euc<int> point(p);
    {
        std::ofstream ofs("Euc_test_serialization.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << point;
    }
    Euc<int> newpoint;
    {
        std::ifstream ifs("Euc_test_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newpoint;
    }
    EXPECT_EQ(point.size(), newpoint.size());
    for (size_t i = 0; i < point.size(); ++i) {
        EXPECT_EQ(point[i], newpoint[i]);
    }
}

#endif