//
//#include <numeric>
//#include "PivotSorter.hpp"
//#include "../../Stat.hpp"
//#include "../../../dprint.hpp"
//#include <utility>    // std::swap
//
//PivotSorter::PivotSorter(Dat* pdata, const size_t R, const size_t C):
//        pdata(pdata), C(C), R(R) {
//}
//
//void PivotSorter::reorder(Dat* pdata, std::vector<Tup>& tups, size_t R, size_t C)
//{
//    for (size_t i=0; i<R; ++i) {
//        while (tups[i].sort_index != i) {
//            std::swap_ranges(
//                    &pdata[tups[i].sort_index*C],
//                    &pdata[tups[i].sort_index*C+C],
//                    &pdata[tups[tups[i].sort_index].sort_index*C]
////                    &pdata[std::get<0>(idxdist[std::get<0>(idxdist[i])])*C]
//            );
//            std::swap(tups[i].sort_index, tups[tups[i].sort_index].sort_index);
//        }
//    }
//}
//
//void PivotSorter::sort(std::vector<size_t>& idxs, Pivot& pivot) {
//    auto& distances = pivot.distances;
//    if (pivot.ppivot->size() != C){
//        dprintf(" wtf   %zu \n", pivot.pivot.size());
//    }
//    assert(pivot.pivot.size() == C);
//    /// Calculate distance to the pivot for all points
//    for (size_t i = 0; i < R; ++i) {
//        Dat dist_ = 0;
//        for (unsigned j = 0; j != C; ++j) {
//            dist_ += sqr(pivot.pivot[j] - pdata[i*C+j]);
//        }
//        const float d = std::sqrt(dist_);
//        distances[i] = Tup(idxs[i], d, i);
//    }
//
//    std::sort(std::begin(distances), std::end(distances), [](auto &left, auto &right) {
//        return left.distance < right.distance;
//    });
//    /// copy over new index order
//    for (size_t i=0; i<R; ++i){
//        idxs[i] = distances[i].oindex;
//    }
//    /// copy new locations of Dat* array
//    reorder(pdata, distances, R, C);
////    for (int k = 0; k < distances.size() ; ++k) {
////        dprintf("%d   %zu   %f   %zu   %f\n", k, std::get<0>(distances[k]), std::get<1>(distances[k]),std::get<2>(distances[k]), pdata[k*C]) ;
////    }
//
////    return pr;
//}
//
//PivotSorter::~PivotSorter() {
//
//}
//
//void PivotSorter::setData(Dat *pdata, size_t R, const size_t C) {
//    this->pdata = pdata;
//    this->R = R;
//    this->C = C;
//}
//
//Dat *PivotSorter::getData() const {
//    return pdata;
//}
//
//size_t PivotSorter::getR() const {
//    return R;
//}
//
//size_t PivotSorter::getC() const {
//    return C;
//}
//
//
//#if COMPILE_TESTS
//#include "gtest/gtest.h"
//#include "../../../dprint.hpp"
//#include "../../../utils/testutil.hpp"
////
////TEST(utils, pivot_test)
////{
////    const int R = 10;
////    const int C = 2;
////    size_t pidx = 9;
////    auto m = testutil::makeM(R, C);
////    auto m2 = testutil::makeM(R, C);
////
////    auto ppivot = new Dat[C];
////    std::copy(&m[pidx*C],&m[pidx*C+C], ppivot);
////
////    EXPECT_EQ(ppivot[0], m[pidx*C]);
////    EXPECT_EQ(ppivot[1], m[pidx*C+1]);
////    PivotSorter ps(m, R, C);
////    std::vector<std::pair<size_t, Dat >> distances(R);
////    std::vector<size_t> idxs(R);
////    std::iota(idxs.begin(), idxs.end(), 0);
////    Pivot piv(ppivot, R, C);
////
////    ps.sort(idxs, piv);
////    for (int i = 0; i < R; ++i) {
////        EXPECT_EQ(9 - i, idxs[i]);
////    }
////    EXPECT_EQ(m[0], ppivot[0]);
////    EXPECT_EQ(m[1], ppivot[1]);
////    delete[] ppivot;
////    pidx = 4;
////    ppivot = new Dat[C];
////    std::copy(&m[pidx*C],&m[pidx*C+C], ppivot);
////    PivotSorter ps2(m, R, C);
////    std::vector<size_t> idx2(R);
////    std::iota(idx2.begin(), idx2.end(), 0);
////
////    Pivot piv2(ppivot, R, C);
////
////    ps2.sort(idx2, piv2);
////    EXPECT_EQ(idx2[0], pidx);
////    EXPECT_EQ(idx2[R-1], R-1);
////    EXPECT_EQ(m[0], ppivot[0]);
////    EXPECT_EQ(m[1], ppivot[1]);
////
////    int idx_start = 100;
////    for (size_t pid=0; pid < R ; ++pid){
////        std::vector<size_t> idx3(R);
////        std::iota(idx3.begin(), idx3.end(),idx_start  );
////        auto m3 = testutil::makeM(R, C);
////        PivotSorter ps3(m3, R, C);
////        Pivot piv3(&m2[pid*C], R, C);
////        ps3.sort(idx3, piv3);
////        EXPECT_EQ(idx3[0]-idx_start , pid);
////        for (int i = 0; i < R; ++i) {
//////            dprintf(" %d  %lu     %lu:%lu      \n",pid,  idx3[i], size_t(m3[i * C]), size_t(m3[i * C + 1]));
////            EXPECT_EQ(size_t(m3[i*C]), idx3[i] - idx_start );
////        }
////        delete[] m3;
////    }
////}
//
//#endif
