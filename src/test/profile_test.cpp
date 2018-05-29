#include <iostream>
#include "../main/utils/testutil.hpp"
#include "../main/datastructures/controllers/DataManager.hpp"
#include "../main/datastructures/indexes/hi/HITree.hpp"
#include <limits>
#ifdef COMPILE_PROFILING
#include "gperftools/profiler.h"
#endif

int main(int argc, char** argv) {
#ifdef COMPILE_PROFILING
    ProfilerStart("profile_dump.txt");
#endif
    const int R = 1024*64;
    const int C = 20;
    int k = 3;
    size_t qidx = 3;
    auto m = testutil::makeM(R, C);
    auto m2 = testutil::makeM(R, C);
//    auto queryPoint = &m2[qidx*C];

    hi::HITree tree(new DataManager(m, R, C, true,true, 0));
//    dprintca(m, R, C);
    tree.buildParams.lshVarThreshold = std::numeric_limits<float>::infinity();
    tree.buildParams.maxPivots = 1;
    tree.buildParams.maxLeafPoints = 3;
    tree.build();
    delete[] m2;
#ifdef COMPILE_PROFILING
    ProfilerFlush();
    ProfilerStop();
#endif
    return 0;
}
