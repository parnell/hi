
#include "PivotSelector.hpp"

Data *PivotSelector::findPivot(DataManager* pdata, std::vector<Data *> *pivots) {
    unsigned int num_tries = 0;
    const size_t S = pdata->getRows();
    size_t next_idx = S / 2;    // select the point the median distance away
    Data *next_pivot = NULL;
    if (pivots == nullptr)
        return pdata->getElement(next_idx);
    bool found_idx = false;
    do {
        next_pivot = pdata->getElement(next_idx % S);
        for (Data *p : *pivots) {
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
#include "../../controllers/EucDataManager.hpp"
#include "../../../utils/testutil.hpp"

TEST(utils, findpivot_fast)
{
    const int NPIVOTS = 3;
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);;
    PivotSelector ps;
    EucDataManager<float> mdat(m, R, C, true, true, 0);
//    mdat.print();
    Data* pivot = ps.findPivot(&mdat, nullptr);
    EXPECT_EQ((*static_cast<Euc<float>*>(pivot))[0],5.0);
}

#endif
