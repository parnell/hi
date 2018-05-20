
#include "IndexDecider.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../../utils/statutil.hpp"
#include "../../../dprint.hpp"
#include "Pivot.hpp"

const size_t START_SAMPLE_SIZE = 5000;
const float SAMPLE_SIZE_SLOPE = 0.01f;
const size_t END_SAMPLE_SIZE = 10000;

//
//Stat IndexDecider::calculateVariance(DataManager *pdata) {
//    const size_t S = pdata->getRows();
//    if (S > START_SAMPLE_SIZE) {
//        size_t n = START_SAMPLE_SIZE + static_cast<int>(S * SAMPLE_SIZE_SLOPE);
//        n = std::min(n, END_SAMPLE_SIZE);
//        std::vector<Dat*> rows = vecutil::randRowPointers<Dat>(S, pdata->getCols(), n, pdata->getDat());
//        return statutil::calculateVariance<Dat>(rows, pdata->getCols());
////        MAX_THRESHOLD = 0.9f;
////        v= (MEANOA + (MAX_THRESHOLD*VAROA) );
//
//    } else {
//        return statutil::calculateVariance<Dat>(pdata->getDat(), pdata->getRows(), pdata->getCols());
//    }
//}



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
