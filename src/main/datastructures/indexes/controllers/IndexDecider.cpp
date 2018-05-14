
#include "IndexDecider.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../../utils/statutil.hpp"
#include "../../../dprint.hpp"
#include "Pivot.hpp"

const size_t START_SAMPLE_SIZE = 5000;
const float SAMPLE_SIZE_SLOPE = 0.01f;
const size_t END_SAMPLE_SIZE = 10000;

std::vector<DecideResult*> IndexDecider::decide(DataManager *pdata, int maxPivots, float lshVarThreshold, int depth) {
    const size_t R = pdata->getRows();
    const size_t C = pdata->getCols();

    Stat stat = calculateVariance(pdata);
//    dprintf(" depth=%d\tvar=%f\tsize=%ld   thresh=%f\n", depth, stat.var[0], R, lshVarThreshold);
    if (stat.var[0] < lshVarThreshold){
        auto pivots = vecutil::randRowPointers<Dat>(R, C, maxPivots, pdata->getDat());
        std::vector<DecideResult*> res(pivots.size());
        size_t idx = 0;
        for (auto _pivot : pivots){
//            std::cout << "Pre sort" << *pdata << std::endl;
            res[idx] = new DecideResult(new Pivot(_pivot, R, C));
            auto &pdr = res[idx];

            auto& pivot = *pdr->ppivot;
            auto& d = pdr->data;

            pdata->sort(pivot);
            pdr->indexGroup = IG_SPATIAL;
            /// Partition
            /// partition pivot index
            DataManager* pivotdat = pdata->sliceData(0,1);
            assert(pivotdat->idxs.size() == 1);
            auto pidx = pivotdat->idxs[0];
            pivot.index = pidx;
            /// split partition regions
            DataManager *psubdat = pdata->sliceData(1, R / 2 + 1);
            DataManager *psubdat2 = pdata->sliceData(R / 2 + 1, R);
            pivot.maxl_idx = R / 2;
            pivot.minr_idx= R / 2 + 1;
            d.emplace_back(psubdat);
            d.emplace_back(psubdat2);
            assert(d[0]->getRows() + d[1]->getRows() + 1 == R);
            assert(pdata->idxs.size() == R);
            assert(pivot.distances.size() == R);
//            assert(pdr->idxpivot.second.size() > 0);
            ++idx;
            delete pivotdat;
        }
        return res;
    } else {
        std::vector<DecideResult*> res(1);
        res[0] = new DecideResult();
        size_t idx = res.size()-1;
        res[idx]->indexGroup = IG_LSH;
        res[idx]->data.emplace_back(pdata);
        return res;
    }
}

Stat IndexDecider::calculateVariance(DataManager *pdata) {
    const size_t S = pdata->getRows();
    if (S > START_SAMPLE_SIZE) {
        size_t n = START_SAMPLE_SIZE + static_cast<int>(S * SAMPLE_SIZE_SLOPE);
        n = std::min(n, END_SAMPLE_SIZE);
        std::vector<Dat*> rows = vecutil::randRowPointers<Dat>(S, pdata->getCols(), n, pdata->getDat());
        return statutil::calculateVariance<Dat>(rows, pdata->getCols());
//        MAX_THRESHOLD = 0.9f;
//        v= (MEANOA + (MAX_THRESHOLD*VAROA) );

    } else {
        return statutil::calculateVariance<Dat>(pdata->getDat(), pdata->getRows(), pdata->getCols());
    }
}



#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../utils/testutil.hpp"
#include "../../../dprint.hpp"


TEST(controllers, ID_test_decide)
{
    unsigned int R = 1024;
    unsigned int C = 2;

    auto m = testutil::makeM(R, C);
    DataManager dat(m, R,C, false,true, 0);
    IndexDecider id;
    auto drs = id.decide(&dat, 1, 1000000, 0);
    size_t total = 0;
    size_t idxtotal = 0;

    for (auto & pdr : drs){
        for (auto& dat : pdr->data){
            total += dat->getRows();
            idxtotal += dat->idxs.size();
            for (int i = 0; i < dat->idxs.size(); ++i) {
                EXPECT_EQ(dat->idxs[i], (size_t) dat->getDat()[i*C]);
            }
        }
    }
    EXPECT_EQ(total+1, R);
    EXPECT_EQ(idxtotal+1, R);
}

#endif
