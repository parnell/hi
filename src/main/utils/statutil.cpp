
#include "statutil.hpp"

#if COMPILE_TESTS
#include "gtest/gtest.h"
#include <numeric>

namespace varutil{
TEST(utils, vartest)
{
    std::vector<int> v(10);
    std::iota (std::begin(v), std::end(v), 0);

    EXPECT_EQ(variance<int>(&v[0],v.size(), false), 8.25);
}
}
#endif