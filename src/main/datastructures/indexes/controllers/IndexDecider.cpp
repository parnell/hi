
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
        auto pivots = vecutil::randRowPointers<Dat>(R, C, 1, pdata->getDat());
//        dprintf("  spatial  pivots=%ld\n", pivots.size());
        for (auto pivot : pivots){
            res.emplace_back(DecideResult());
            auto *pp = new Dat[C];
            size_t idx = res.size()-1;
            std::copy(&pivot[0], &pivot[C], pp);
            pdata->sort(pivot);
            res[idx].indexGroup = IG_SPATIAL;
            res[idx].data.emplace_back(pdata->sliceData(1, R / 2));
            res[idx].data.emplace_back(pdata->sliceData(R/2, R));
            res[idx].pivot = pp;
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