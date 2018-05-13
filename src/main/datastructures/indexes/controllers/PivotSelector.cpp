
#include "PivotSelector.hpp"

Dat *PivotSelector::findPivot(DataManager* pdata, std::vector<Dat *> *pivots) {
    unsigned int num_tries = 0;
    const size_t S = pdata->getRows();
    size_t next_idx = S / 2;    // select the point the median distance away
    Dat *next_pivot = NULL;
    if (pivots == nullptr)
        return pdata->getRow(next_idx);
    bool found_idx = false;
    do {
        next_pivot = pdata->getRow(next_idx % S);
        for (Dat *p : *pivots) {
            if (next_pivot == p) {
                found_idx = true;
                break;
            }
        }
        ++next_idx;
        ++num_tries;
    } while (found_idx && num_tries < S);
    if (!found_idx)
        return next_pivot;
    return nullptr;
}


#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../dprint.hpp"

TEST(utils, findpivot_fast)
{
    const int NPIVOTS = 3;
    const int R = 10;
    const int C = 2;
    auto m = new Dat[R * C];

    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = r*(c+1); }
    }
    PivotSelector ps;
    DataManager mdat(m, R, C, true, true, 0);
//    mdat.print();
    Dat* pivot = ps.findPivot(&mdat, nullptr);
    EXPECT_EQ(pivot[0],5.0);
}

#endif
