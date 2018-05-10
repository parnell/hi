
#include "HITree.hpp"

std::mutex mylock;

namespace hi {

HITree::HITree() :
        pdata(nullptr), pdecider(nullptr), proot(nullptr) {
}


HITree::HITree(DataManager *pdata) :
        pdata(pdata), pdecider(new IndexDecider()), proot(nullptr) {
}


IndexDecider *HITree::getDecider() const {
    return pdecider;
}

void HITree::setDecider(IndexDecider *pdecider) {
    delete pdecider;
    this->pdecider = pdecider;
}

DataManager *HITree::getDataManager() const {
    return pdata;
}

void HITree::setDataManager(DataManager *pdata) {
    this->pdata = pdata;
}

HINode *HITree::getRoot() const {
    return proot;
}

void HITree::setRoot(HINode *proot) {
    this->proot = proot;
}

void HITree::query(int depth/*=0*/) {
    proot->query(queryParams, depth);
}


void HITree::build(int depth/*=0*/) {
    buildResults.npoints = pdata->getRows();
    if (depth==0) {
        proot = new HINode(this);
        proot->firstsplit(buildParams, pdata);
    } else {
        proot->build(buildParams, pdata, depth);
    }
}



}

#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../dprint.hpp"
#include "../../../utils/testutil.hpp"
#include <fstream>

TEST(hi, HITree_test_build_query_LSH)
{
    const int R = 101;
    const int C = 2;
    auto m = testutil::makeM(R, C);

    DataManager mdat(m, R, C, true, true);
    hi::HITree tree(&mdat);
    tree.buildParams.lshVarThreshold = 0;

    tree.build();
    std::ofstream ofs("lsh_test_build.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }
    hi::HITree newtree;
    {
        std::ifstream ifs("lsh_test_build.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newtree;
    }
    EXPECT_EQ(tree.getRoot()->isLeaf(),newtree.getRoot()->isLeaf());
    EXPECT_EQ(tree.getRoot()->getIndexType(),newtree.getRoot()->getIndexType());
    EXPECT_EQ(tree.getRoot()->getLSH()->D,tree.getRoot()->getLSH()->D);
//    dcoutl(newtree.buildResults);
    EXPECT_EQ(R, newtree.buildResults.npointsInLSH);
    newtree.query();
}


TEST(hi, HITree_test_build_query_spatial)
{
    const int R = 101;
    const int C = 2;
    auto m = testutil::makeM(R, C);

    DataManager mdat(m, R, C, true,true);
    hi::HITree tree(&mdat);
    tree.buildParams.lshVarThreshold = 10000000;
//    tree.setDecider(new SpatialIndexDecider());
    EXPECT_EQ(1,1);
    tree.build();
    std::ofstream ofs("spatial_test_build.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }
    hi::HITree newtree;
    {
        std::ifstream ifs("spatial_test_build.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newtree;
    }
    EXPECT_EQ(tree.getRoot()->isLeaf(),newtree.getRoot()->isLeaf());
    EXPECT_EQ(tree.getRoot()->getIndexType(),newtree.getRoot()->getIndexType());
//    dcoutl(" build results=");
    dcoutl(newtree.buildResults);
    EXPECT_EQ(R,newtree.buildResults.npoints);
    EXPECT_EQ(R,newtree.buildResults.npointsInInternal + newtree.buildResults.npointsInLeaf);
}


#endif
