
#include "Pivot.hpp"


Pivot::Pivot(Data* _ppivot, size_t index, size_t R): ppivot(_ppivot), index(index), distances(R){
//    distances(R)
//        assert(!pivot.empty());
//        assert(pivot.size() == C);
}

dist_type Pivot::getMaxLeft() const {
    return distances[maxl_idx].distance;
//    return 0;
}

dist_type Pivot::getMinRight() const {
    return distances[minr_idx].distance;
//    return 0;
}





#if COMPILE_TESTS
#include "gtest/gtest.h"

TEST(data, Pivot_test_serialization)
{
    Pivot pivot;
    {
        std::ofstream ofs("HINode_test_serialization.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << pivot;
    }
    Pivot newpivot;
    {
        std::ifstream ifs("HINode_test_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newpivot;
    }
    EXPECT_EQ(newpivot.ppivot, pivot.ppivot);
}

#endif