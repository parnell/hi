#include "statutil.hpp"

#if COMPILE_TESTS
#include "gtest/gtest.h"

namespace statutil{

TEST(utils, vartest1)
{
    const int R = 10;
    const int C = 2;
    auto m = new int[R * C];

    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = r*(c+1);
//            std::cout << m[r*C+c] << "  ";
        }
//        std::cout << std::endl;
    }
    Stat stat = calculateVariance<int>(m, R, C);
    EXPECT_EQ(stat.count, R);
    EXPECT_EQ(stat.sum[0], 45);
    EXPECT_EQ(stat.sum[1], 90);
    EXPECT_EQ(stat.var[0], 8.25);
    EXPECT_EQ(stat.var[1], 33.00);
    EXPECT_EQ(stat.mean[0], 4.5);
    EXPECT_EQ(stat.mean[1], 9.00);

//    std::cout << "-----------------" << std::endl;
    const int RS = R/2;
    std::vector<int*> randtest(RS);
    for (int r= 0; r< RS; r++) {
        randtest[r] = &m[r*2*C];
//        for (int c = 0; c < C; ++c) {
//            std::cout << randtest[r][c] << "  ";
//        }
//        std::cout << std::endl;
    }
    stat = calculateVariance<int>(randtest, C);
    EXPECT_EQ(stat.count, RS);
    EXPECT_EQ(stat.sum[0], 20);
    EXPECT_EQ(stat.sum[1], 40);
    EXPECT_EQ(stat.var[0], 8);
    EXPECT_EQ(stat.var[1], 32.00);
    EXPECT_EQ(stat.mean[0], 4);
    EXPECT_EQ(stat.mean[1], 8.00);
}

//
//TEST(utils, vartest2)
//{
//    std::vector<int> v(10);
//    std::iota (std::begin(v), std::end(v), 0);
//
//    EXPECT_EQ(variance<int>(&v[0],v.size(), false), 8.25);
//}
#endif


}
