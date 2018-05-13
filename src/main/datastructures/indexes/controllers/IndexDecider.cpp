
#include "IndexDecider.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../../utils/statutil.hpp"
#include "../../../dprint.hpp"

const size_t START_SAMPLE_SIZE = 5000;
const float SAMPLE_SIZE_SLOPE = 0.01f;
const size_t END_SAMPLE_SIZE = 10000;

std::vector<DecideResult> IndexDecider::decide(DataManager *pdata, int maxPivots, float lshVarThreshold, int depth) {
    std::vector<DecideResult> res;
    const size_t R = pdata->getRows();
    const size_t C = pdata->getCols();

    Stat stat = calculateVariance(pdata);
//    dprintf(" depth=%d\tvar=%f\tsize=%ld   thresh=%f\n", depth, stat.var[0], R, lshVarThreshold);
    if (stat.var[0] < lshVarThreshold){
        auto pivots = vecutil::randRowPointers<Dat>(R, C, maxPivots, pdata->getDat());

        for (auto pivot : pivots){
//            std::cout << "Pre sort" << *pdata << std::endl;
            res.emplace_back(DecideResult());
            const size_t idx = res.size()-1;
            auto& d = res[idx].data;
            std::vector<Dat> pp(&pivot[0], &pivot[C]);
//            std::copy(&pivot[0], &pivot[C], pp.begin());
            auto pidx = pdata->idxs[0];
            pdata->sort(pivot);
//            std::cout << "Post sort" << *pdata << std::endl;
            res[idx].indexGroup = IG_SPATIAL;
            d.emplace_back(pdata->sliceData(1, R / 2 + 1));
            d.emplace_back(pdata->sliceData(R/2 + 1, R));
            assert(d[0]->getRows() + d[1]->getRows() + 1 == R);
            assert(pdata->idxs.size() == R);
//            std::cout << "     1111" << *pdata->sliceData(1, R / 2) << std::endl;
//            std::cout << "      22222" << *pdata->sliceData(1, R / 2) << std::endl;

//            const size_t mys = res[idx].data.size();
//            assert(res[idx].data[mys]->idxs[0] == idx);
            for (size_t i = 0; i < R; ++i) {
                dprintf(" %lu %f:%f -- pidx=%lu    %lu <> %lu    max=%lu\n", i, (*pdata)[i][0], (*pdata)[i][1], pidx, pdata->idxs[i], pdata->getRows());

            }
//            dprintf("  %f:%f -- pidx=%lu    %lu     max=%lu\n", (*pdata)[pidx][0], (*pdata)[pidx][1], pidx, pdata->getRows());
//            assert(pidx < pdata->getRows());
//            assert(pdata->idxs[pidx] < pdata->getRows());

            res[idx].idxpivot = std::make_pair(pidx, pp);
        }
    } else {
        res.emplace_back(DecideResult());
        size_t idx = res.size()-1;
        res[idx].indexGroup = IG_LSH;
        res[idx].data.emplace_back(pdata);
    }
    return res;
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
    unsigned int R = 10;
    unsigned int C = 2;
    int pidx = 3;
    auto m = testutil::makeM(R, C);
    DataManager dat(m, R,C, false,true, 0);
    IndexDecider id;
    id.decide(&dat, 1, 1000000, 0);
//    dcoutl(dat);
//    EXPECT_EQ(dat.idxs[0], pidx);
//    pidx = 8;
//    dat.sort(&m[pidx*C]);
//    EXPECT_EQ(dat.idxs[0], pidx);
}

#endif
