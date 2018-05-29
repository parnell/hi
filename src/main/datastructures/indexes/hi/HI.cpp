
#include <algorithm>
#include "HI.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../../utils/statutil.hpp"
#include "../../../dprint.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "../../controllers/EucDataManager.hpp"
//#include <lshbox/basis.h>
//#include <lshbox/metric.h>
//#include <lshbox/topk.h>
//#include <lshbox/eval.h>
//#include <lshbox/matrix.h>
#include <boost/thread.hpp>

namespace hi {


void HI::_build(DataManager *pdata, int max_depth) {
    HITree* p_tree = new HITree(pdata, i);
    trees_.emplace_back(p_tree);
    p_tree->buildParams.nnodes = nnodes;
    p_tree->buildParams.nthreads= nthreads;
    p_tree->build(0);

}


void HI::build(DataManager *pdata, size_t nnodes, int max_depth) {
//    dcoutl("HI::Build");
    unsigned int nthreads = boost::thread::hardware_concurrency();
    HITree* p_tree = new HITree(pdata);
//    trees_.push_back(p_tree);
    p_tree->buildParams.nnodes = nnodes;
    p_tree->buildParams.nthreads= nthreads;
    p_tree->build(0);
    trees_.emplace_back(p_tree);
}

void HI::build(const std::string& filename, size_t nnodes, int max_depth) {
//    dcoutl("HI::Build");
    unsigned int nthreads = boost::thread::hardware_concurrency();
    for (int i = 0; i < nnodes; ++i) {
        DataManager* pdat = EucDataManager<itype>::loadData(filename, i, nnodes);
        HITree* p_tree = new HITree(pdat, i);
        trees_.emplace_back(p_tree);
        p_tree->buildParams.nnodes = nnodes;
        p_tree->buildParams.nthreads= nthreads;
        p_tree->build(0);
    }
}

size_t HI::size() const {
    size_t s = 0;
    for (auto it = trees_.begin(); it < trees_.end(); ++it) {
        s += (*it)->size();
    }
    return s;
}


}



#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../dprint.hpp"
#include "../../../utils/testutil.hpp"
#include "../../../utils/stringutils.hpp"
#include "../../../utils/Timer.hpp"
#include <fstream>
#include <limits>

TEST(hi, HI_test_load)
{
    std::string filename = sutil::sformat("%s/../data/tests/gaussian__d=20_s=10000_nclus=1_var=0.1.bin", CMAKE_CURRENT_BINARY_DIR);
    {
        hi::HI tree;
        Timer("Split Time");
        tree.build(filename, 6);
        EXPECT_EQ(tree.size(), 10000);
    }
    {
        hi::HI tree;
        Timer("single Time");
        tree.build(filename, 1);
        EXPECT_EQ(tree.size(), 10000);
    }
}

#endif
