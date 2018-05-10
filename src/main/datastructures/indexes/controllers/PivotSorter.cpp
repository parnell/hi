
#include "PivotSorter.hpp"
#include "../../Stat.hpp"


PivotSorter::PivotSorter(Dat* pdata, const size_t R, const size_t C):
pdata(pdata), C(C), R(R), distances(R) {
}

Stat PivotSorter::sort(Dat* _ppivot) {
    Stat stat;
    Dat pivot[C];
    std::copy(&_ppivot[0],&_ppivot[C], &pivot[0]);
    /// Calculate distance to the pivot for all points
    for (size_t i = 0; i < R; ++i) {
        Dat dist_ = 0;
        for (unsigned j = 0; j != C; ++j) {
            dist_ += sqr(pivot[j] - pdata[i*C+j]);
        }
        const float d = std::sqrt(dist_);
        distances[i] = std::make_pair(i, d);
//        stat.add(d);
    }
    std::sort(std::begin(distances), std::end(distances), [](auto &left, auto &right) {
        return left.second < right.second;
    });
    for (size_t k = 0; k < R; ++k) {
        if (distances[k].first < k)
            continue;
        std::swap_ranges( &pdata[k*C], &pdata[k*C+C], &pdata[distances[k].first*C] );
    }
    return stat;
}

PivotSorter::~PivotSorter() {

}


#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../dprint.hpp"

TEST(utils, pivot_test)
{
    const int R = 10;
    const int C = 2;
    int pidx = 9;
    auto m = new Dat[R * C];

    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = r*(c+1); }
    }

    auto ppivot = new Dat[C];
    std::copy(&m[pidx*C],&m[pidx*C+C], ppivot);

    EXPECT_EQ(ppivot[0], m[pidx*C]);
    EXPECT_EQ(ppivot[1], m[pidx*C+1]);
    PivotSorter mdat(m, R, C);
    std::vector<std::pair<size_t, Dat >> distances(R);
    mdat.sort(&m[pidx*C]);
    EXPECT_EQ(m[0], ppivot[0]);
    EXPECT_EQ(m[1], ppivot[1]);
    delete[] ppivot;
    pidx = 4;
    ppivot = new Dat[C];
    std::copy(&m[pidx*C],&m[pidx*C+C], ppivot);
    PivotSorter mdat2(m, R, C);
    mdat2.sort(ppivot);
    EXPECT_EQ(m[0], ppivot[0]);
    EXPECT_EQ(m[1], ppivot[1]);
}

#endif
