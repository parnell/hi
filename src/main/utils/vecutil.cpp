#include "vecutil.hpp"


#include <random>

namespace vecutil {
std::unordered_set<size_t> pickSet(size_t N, int k, std::mt19937 &gen) {
    std::unordered_set<size_t> elems;
    for (size_t r = N - k; r < N; ++r) {
        int v = std::uniform_int_distribution<>(0, r)(gen);
        if (!elems.insert(v).second) {
            elems.insert(r);
        }
    }
    return elems;
}

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
    auto r= pickPointers<int>(R, C,  k, m);
    EXPECT_EQ(r.size(), k);
}

}
#endif

