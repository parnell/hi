#include "vecutil.hpp"


#include <random>

namespace vecutil {

std::vector<size_t> pick(size_t N, int k) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::unordered_set<size_t> elems = pickSet(N, k, gen);

    std::vector<size_t> result(elems.begin(), elems.end());
    std::shuffle(result.begin(), result.end(), gen);
    return result;
}
}

#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "testutil.hpp"
#include "../datastructures/rm/M.hpp"
#include "../dprint.hpp"
#include "Timer.hpp"

namespace vecutil{
TEST(utils, splitvec)
{
    std::vector<int> v(101) ;
    int nparts = 2;
    for (int i =0; i<nparts;i++){
        std::vector<int> ret = vecutil::split<int>(v, nparts, i);
        if (i==0) EXPECT_EQ(ret.size(), 51);
        else if (i==1) EXPECT_EQ(ret.size(), 50);
    }
    nparts = 3;
    for (int i =0; i<nparts;i++){
        std::vector<int> ret = vecutil::split<int>(v, nparts, i);
        if (i==0) EXPECT_EQ(ret.size(), 34);
        else if (i==1) EXPECT_EQ(ret.size(), 34);
        else if (i==2) EXPECT_EQ(ret.size(), 33);
    }
    nparts = 4;
    for (int i =0; i<nparts;i++){
        std::vector<int> ret = vecutil::split<int>(v, nparts, i);
        if (i==0) EXPECT_EQ(ret.size(), 26);
        else if (i==1) EXPECT_EQ(ret.size(), 25);
        else if (i==2) EXPECT_EQ(ret.size(), 25);
        else if (i==3) EXPECT_EQ(ret.size(), 25);
    }

}

TEST(utils, randpick) {
    EXPECT_EQ(pick(25,3).size(), 3);
}

TEST(utils, randpickptrs) {
    const int R = 10;
    const int C = 2;
    int k = 4;
    auto m = new int[R*C];

    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = (r+1)*C + c;
        }
    }
    auto r= randRowPointers<int>(R, C, k, m);
    EXPECT_EQ(r.size(), k);
}


TEST(utils, fftpicks) {
    const int R = 10;
    const int C = 2;
    int k = 4;
    auto m = testutil::makeM<float>(R,C);

    auto res= fft<float, dist_type>(R, C, k, m, rm::M<float>::dist);
    EXPECT_EQ(res.size(), k);
    EXPECT_EQ(*res[0], 0);
    EXPECT_EQ(*res[1], 9);
    EXPECT_EQ(*res[2], 1);
    EXPECT_EQ(*res[3], 8);
}

TEST(utils, speed_fft_vs_random) {
    /// (1k,5k), 100, 14.  (6.48ms, 27ms)
    /// (1k,5k), 100, 5.  (2.61ms, 8ms) linear with k (as expected)
    const int R = 5000;
    const int C = 100;
    int k = 5;
    auto m = testutil::makeM<float>(R,C);
    {
//        Timer t("fft");
        auto res = fft<float, dist_type>(R, C, k, m, rm::M<float>::dist);
        EXPECT_EQ(res.size(), k);
    }
    {
//        Timer t("random");
        auto res = randRowPointers<float>(R, C, k, m);
        EXPECT_EQ(res.size(), k);
    }
}

}
#endif

