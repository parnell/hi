
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
//    const size_t C = pdata->getCols();
    std::vector<Pivot*> onepivot = pdata->pickPivots(1);
//    Stat stat = pdata->calculateStat();

    tstat<dist_type> stat = pdata->calculateStat(onepivot[0]);
//    dprintf(" depth=%d\tvar=%f\tsize=%ld\tratio=%f\tthresh=%f\n", depth, stat.var(), R, stat.ratio(), lshVarThreshold);
    if (stat.ratio() < lshVarThreshold){
//        auto pivots = vecutil::randRowPointers<Dat>(R, C, maxPivots, pdata->getDat());

        auto pivots = pdata->pickPivots(maxPivots-1);
        pivots.emplace_back(onepivot[0]);
        std::vector<DecideResult*> res(pivots.size());
        size_t idx = 0;
        std::vector<DataManager*> cuts;
        cuts.emplace_back(pdata);
        for (auto _pivot : pivots){
//            std::cout << "Pre sort" << *pdata << std::endl;
            res[idx] = new DecideResult(_pivot);
            auto &pdr = res[idx];

            auto& pivot = *pdr->ppivot;
            auto& d = pdr->data;

            pdata->sort(pivot, true);
            pdr->indexGroup = IG_SPATIAL;
            /// Partition
            /// partition pivot index
            auto pivotdat = pdata->sliceData(0,1);
            assert(pivotdat->idxs.size() == 1);
            auto pidx = pivotdat->idxs[0];
            pivot.index = pidx;
            /// split partition regions
            auto *psubdat = pdata->sliceData(1, R / 2 + 1);
            auto *psubdat2 = pdata->sliceData(R / 2 + 1, R);
            pivot.maxl_idx = R / 2;
            pivot.minr_idx= R / 2 + 1;
            d.emplace_back(psubdat);
            d.emplace_back(psubdat2);
            assert(d[0]->getRows() + d[1]->getRows() + 1 == R);
            assert(pdata->idxs.size() == R);
//            assert(pivot.distances.size() == R);
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

#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../utils/testutil.hpp"
#include "../../../dprint.hpp"
#include "../../controllers/EucDataManager.hpp"


TEST(controllers, ID_test_decide)
{
    unsigned int R = 1024;
    unsigned int C = 2;

    auto m = testutil::makeM<float>(R, C);
    EucDataManager<float> edat(m, R,C, false,true, 0);
    IndexDecider id;
    auto drs = id.decide(&edat, 1, 1000000, 0);
    size_t total = 0;
    size_t idxtotal = 0;

    for (auto & pdr : drs){
        for (auto& dat : pdr->data){
            total += dat->getRows();
            idxtotal += dat->idxs.size();
            for (int i = 0; i < dat->idxs.size(); ++i) {
                EXPECT_EQ(dat->idxs[i], (*dynamic_cast<Euc<float>*>(dat->getElement(i)))[0]
                );
            }
//            dcoutl( * dynamic_cast<EucDataManager<float>*>(dat));
        }
    }
    EXPECT_EQ(total+1, R);
    EXPECT_EQ(idxtotal+1, R);
}

#endif
