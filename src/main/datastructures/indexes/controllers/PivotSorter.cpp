
#include <numeric>
#include "PivotSorter.hpp"
#include "../../Stat.hpp"
#include "../../../dprint.hpp"
#include <utility>    // std::swap

PivotSorter::PivotSorter(Dat* pdata, const size_t R, const size_t C):
        pdata(pdata), C(C), R(R), distances(R) {
}

void PivotSorter::reorder(Dat* pdata, std::vector<std::tuple<size_t, dist_type, size_t >>& idxdist, size_t R, size_t C)
{
    for (size_t i=0; i<R; ++i) {
        while (std::get<0>(idxdist[i]) != i) {
            std::swap_ranges(
                    &pdata[std::get<0>(idxdist[i])*C],
                    &pdata[std::get<0>(idxdist[i])*C+C],
                    &pdata[std::get<0>(idxdist[std::get<0>(idxdist[i])])*C]
            );
            std::swap(std::get<0>(idxdist[i]), std::get<0>(idxdist[std::get<0>(idxdist[i])]));
        }
    }
}

Stat PivotSorter::sort(std::vector<size_t>& idxs, Dat* _ppivot) {
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
        distances[i] = std::make_tuple(i, d, idxs[i]);
    }

    std::sort(std::begin(distances), std::end(distances), [](auto &left, auto &right) {
        return std::get<1>(left) < std::get<1>(right);
    });
    /// copy over new index order
    for (size_t i=0; i<R; ++i){
        idxs[i] = std::get<2>(distances[i]);
    }
    /// copy new locations of Dat* array
    reorder(pdata, distances, R, C);
    return stat;
}

PivotSorter::~PivotSorter() {

}

void PivotSorter::setData(Dat *pdata, size_t R, const size_t C) {
    this->pdata = pdata;
    this->R = R;
    this->C = C;
}

Dat *PivotSorter::getData() const {
    return pdata;
}

size_t PivotSorter::getR() const {
    return R;
}

size_t PivotSorter::getC() const {
    return C;
}


#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../dprint.hpp"
#include "../../../utils/testutil.hpp"

TEST(utils, pivot_test)
{
    const int R = 10;
    const int C = 2;
    size_t pidx = 9;
    auto m = testutil::makeM(R, C);
    auto m2 = testutil::makeM(R, C);

    auto ppivot = new Dat[C];
    std::copy(&m[pidx*C],&m[pidx*C+C], ppivot);

    EXPECT_EQ(ppivot[0], m[pidx*C]);
    EXPECT_EQ(ppivot[1], m[pidx*C+1]);
    PivotSorter ps(m, R, C);
    std::vector<std::pair<size_t, Dat >> distances(R);
    std::vector<size_t> idxs(R);
    std::iota(idxs.begin(), idxs.end(), 0);
    ps.sort(idxs, &m[pidx*C]);
    for (int i = 0; i < R; ++i) {
        EXPECT_EQ(9 - i, idxs[i]);
    }
    EXPECT_EQ(m[0], ppivot[0]);
    EXPECT_EQ(m[1], ppivot[1]);
    delete[] ppivot;
    pidx = 4;
    ppivot = new Dat[C];
    std::copy(&m[pidx*C],&m[pidx*C+C], ppivot);
    PivotSorter ps2(m, R, C);
    std::vector<size_t> idx2(R);
    std::iota(idx2.begin(), idx2.end(), 0);

    ps2.sort(idx2, ppivot);
    EXPECT_EQ(idx2[0], pidx);
    EXPECT_EQ(idx2[R-1], R-1);
    EXPECT_EQ(m[0], ppivot[0]);
    EXPECT_EQ(m[1], ppivot[1]);

    int idx_start = 100;
    for (size_t pid=0; pid < R ; ++pid){
        std::vector<size_t> idx3(R);
        std::iota(idx3.begin(), idx3.end(),idx_start  );
        auto m3 = testutil::makeM(R, C);
        PivotSorter ps3(m3, R, C);
        ps3.sort(idx3, &m2[pid*C]);
        EXPECT_EQ(idx3[0]-idx_start , pid);
        for (int i = 0; i < R; ++i) {
//            dprintf(" %d  %lu     %lu:%lu      \n",pid,  idx3[i], size_t(m3[i * C]), size_t(m3[i * C + 1]));
            EXPECT_EQ(size_t(m3[i*C]), idx3[i] - idx_start );
        }
    }
}

#endif
