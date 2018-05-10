
#include <algorithm>
#include "HI.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../../utils/statutil.hpp"
#include "../../../dprint.hpp"
#include "indexwrappers/LSHWrapper.hpp"
//#include <lshbox/basis.h>
//#include <lshbox/metric.h>
//#include <lshbox/topk.h>
//#include <lshbox/eval.h>
//#include <lshbox/matrix.h>
#include <boost/thread.hpp>

namespace hi {


void HI::build(DataManager *pdata, int nnodes, int max_depth) {
    dcoutl("HI::Build");
    unsigned int nthreads = boost::thread::hardware_concurrency();
    HITree* p_tree = new HITree(pdata);
//    trees_.push_back(p_tree);
    p_tree->buildParams.nnodes = nnodes;
    p_tree->buildParams.nthreads= nthreads;
    p_tree->build();
}



}