
#include "Pivot.hpp"


Pivot::Pivot():  ppivot(nullptr), pdistances(new std::vector<Tup>()){};

Pivot::Pivot(Data* _ppivot, size_t index, size_t R): ppivot(_ppivot), index(index), pdistances(new std::vector<Tup>(R)) {

}

dist_type Pivot::getMaxLeft() const {
    return (*pdistances)[maxl_idx].distance;
}

dist_type Pivot::getMinRight() const {
    return (*pdistances)[minr_idx].distance;
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
    EXPECT_TRUE(pivot.ppivot == nullptr);
    EXPECT_TRUE(newpivot.ppivot == nullptr);
}

#endif