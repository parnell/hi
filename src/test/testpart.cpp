
#include <boost/serialization/export.hpp>

#include <algorithm>
#include "HI.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../../utils/statutil.hpp"
#include "../../../dprint.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "../../controllers/EucDataManager.hpp"
#include <boost/thread.hpp>


int main(int argc, char** argv) {
    std::string filename = sutil::sformat("%s/../data/tests/gaussian__d=20_s=10000_nclus=1_var=0.1.bin", CMAKE_CURRENT_BINARY_DIR);
    hi::HI<EucDataManager<float>> hit;
    Timer("single Time");
    hit.build(filename, 1, 1000);
    EXPECT_EQ(hit.size(), 10000);
}
