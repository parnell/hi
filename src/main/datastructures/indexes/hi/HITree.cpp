
#include "HITree.hpp"

namespace hi {


void HITree::setDataManager(DataManager *pdata) {
    this->pdata = pdata;
}

HINode *HITree::getRoot() const {
    return proot;
}

void HITree::setRoot(HINode *proot) {
    this->proot = proot;
}

void HITree::knnquery(Data* queryPoint, int depth/*=0*/) {
    assert(queryPoint != nullptr);
    queryParams.querypoint = queryPoint;
    assert(queryParams.k > 0);
//    queryParams.C = pdata->getCols();
//    assert(queryParams.C > 0);
    queryResults.reset(queryParams.k);
    proot->knnquery(queryPoint, depth);
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

void HITree::printQueryResults() {
    std::cout << queryResults << std::endl;
    auto tk = queryResults.getTopK();
//    for (size_t i = 0; i < tk.size() ;++i){
//        std::cout << " idx=" << tk[i].second << "\tdist=" << tk[i].first << "\tpoint=(" <<
//                  (*pdata)[tk[i].second][0] <<"," <<(*pdata)[tk[i].second][1] << ")"<< std::endl;
//    }
}

HITree::~HITree() {
    if (pdata)
        delete pdata;
    if (pdecider)
        delete pdecider;
    if (proot)
        delete proot;
}

size_t HITree::size() const {
    return pdata->getRows();
}


}

#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../dprint.hpp"
#include "../../../utils/testutil.hpp"
#include "../../../utils/stringutils.hpp"
#include "../../controllers/EucDataManager.hpp"
#include <fstream>
#include <limits>


TEST(hi, HITree_test_serialization_blank)
{
    const int R = 64;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    hi::HITree tree(new EucDataManager<float>(m, R, C, true, true, 0));
    std::string filename = sutil::sformat("%s/HINode_test_serialization_leaf.idx", CMAKE_CURRENT_BINARY_DIR);
    {
        std::ofstream ofs(filename);
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }

    hi::HITree newtree;
    {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ia(ifs);
        ia >> newtree;
    }
    EXPECT_EQ(tree.size(), newtree.size() );
}


TEST(hi, HITree_test_serialization_leaf)
{
    const int R = 64;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    hi::HITree tree(new EucDataManager<float>(m, R, C, true, true, 0));
    tree.buildParams.lshVarThreshold = std::numeric_limits<float>::infinity();
    tree.buildParams.maxLeafPoints = R*2;
    tree.buildParams.maxPivots = 1;
    tree.build(0);
    std::string filename = sutil::sformat("%s/HINode_test_serialization_leaf.idx", CMAKE_CURRENT_BINARY_DIR);
    {
        std::ofstream ofs(filename);
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }

    hi::HITree newtree;
    {
        std::ifstream ifs(filename);
        boost::archive::text_iarchive ia(ifs);
        ia >> newtree;
    }
    auto root = tree.getRoot();
    EXPECT_EQ(root->isLeaf(), true);
}

TEST(hi, HITree_test_serialization_save)
{
    const int R = 129;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    auto m2 = testutil::makeM<float>(R, C);

    hi::HITree tree(new EucDataManager<float>(m, R, C, true, true, 0));
    tree.buildParams.maxDepth=2;
    tree.buildParams.lshVarThreshold = std::numeric_limits<float>::infinity();;
    tree.buildParams.maxLeafPoints = 75;
    tree.buildParams.maxPivots = 1;
    tree.build(0);
    auto root = tree.getRoot();
    EXPECT_EQ(root->isLeaf(), false);
    EXPECT_EQ(root->getpChildren()->size(), 2);
    EXPECT_TRUE(root->getPivots() != nullptr);
    EXPECT_EQ(root->getPivots()->size(), 1);
/// Should have built a tree with 64 points in each leaf. with 1 root split
    auto child1 =(*root->getpChildren())[0];
    auto child2 =(*root->getpChildren())[1];
    EXPECT_EQ(child1->isLeaf(), true);
    EXPECT_EQ(child2->isLeaf(), true);
    EXPECT_TRUE(child1->getpChildren() == nullptr);
    EXPECT_TRUE(child1->getLeafPoints() != nullptr);
    EXPECT_EQ(child1->getLeafPoints()->getRows(), R/2);
    EXPECT_EQ(child2->getLeafPoints()->getRows(), R/2);
    {
        std::ofstream ofs("HINode_test_serialization.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }

}


TEST(hi, HITree_test_serialization_load)
{
    const int R = 129;

    hi::HITree newtree;
    {
        std::ifstream ifs("HINode_test_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newtree;
    }
    auto root = newtree.getRoot();
    EXPECT_EQ(root->isLeaf(), false);
    EXPECT_TRUE(root->getPivots() != nullptr);
    EXPECT_EQ(root->getPivots()->size(), 1);

    auto child1 =(*newtree.getRoot()->getpChildren())[0];
    auto child2 =(*newtree.getRoot()->getpChildren())[1];
    EXPECT_EQ(child1->isLeaf(), true);
    EXPECT_EQ(child2->isLeaf(), true);
    EXPECT_TRUE(child1->getpChildren() == nullptr);
    EXPECT_TRUE(child1->getLeafPoints() != nullptr);
    EXPECT_EQ(child1->getLeafPoints()->getRows(), R/2);
    EXPECT_EQ(child2->getLeafPoints()->getRows(), R/2);
}

TEST(hi, HITree_test_build_serialization)
{
    const int R = 17;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    auto m2 = testutil::makeM<float>(R, C);
    const size_t qidx = 3;
    float* queryPoint = &m2[qidx*C];
    Euc<float> qp(queryPoint, C);

    hi::HITree tree(new EucDataManager<float>(m, R, C, true, true, 0));
    tree.buildParams.maxDepth=2;
    tree.buildParams.lshVarThreshold = std::numeric_limits<float>::infinity();;
    tree.buildParams.maxLeafPoints = 10;
    tree.buildParams.maxPivots = 1;
    tree.build(0);
    auto root = tree.getRoot();
    EXPECT_EQ(root->isLeaf(), false);
    EXPECT_EQ(root->getpChildren()->size(), 2);
    EXPECT_TRUE(root->getPivots() != nullptr);
    EXPECT_EQ(root->getPivots()->size(), 1);
/// Should have built a tree with 64 points in each leaf. with 1 root split
    auto child1 =(*root->getpChildren())[0];
    auto child2 =(*root->getpChildren())[1];
    EXPECT_EQ(child1->isLeaf(), true);
    EXPECT_EQ(child2->isLeaf(), true);
    EXPECT_TRUE(child1->getpChildren() == nullptr);
    EXPECT_TRUE(child1->getLeafPoints() != nullptr);
    EXPECT_EQ(child1->getLeafPoints()->getRows(), R/2);
    EXPECT_EQ(child2->getLeafPoints()->getRows(), R/2);
    {
        std::ofstream ofs("HINode_test_build_serialization.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << tree;
    }
    hi::HITree newtree;
    {
        std::ifstream ifs("HINode_test_build_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newtree;
    }

    root = newtree.getRoot();
    EXPECT_EQ(root->isLeaf(), false);
    EXPECT_TRUE(root->getPivots() != nullptr);
    EXPECT_EQ(root->getPivots()->size(), 1);

    child1 =(*newtree.getRoot()->getpChildren())[0];
    child2 =(*newtree.getRoot()->getpChildren())[1];
    EXPECT_EQ(child1->isLeaf(), true);
    EXPECT_EQ(child2->isLeaf(), true);
    EXPECT_TRUE(child1->getpChildren() == nullptr);
    EXPECT_TRUE(child1->getLeafPoints() != nullptr);
    EXPECT_EQ(child1->getLeafPoints()->getRows(), R/2);
    EXPECT_EQ(child2->getLeafPoints()->getRows(), R/2);

    tree.knnquery(&qp,0);
//    tree.printQueryResults();
    auto res = tree.queryResults.getTopK();
    EXPECT_EQ(res[0].second, qidx);

//    newtree.queryParams.setQueryPoint(queryPoint, C);
    newtree.knnquery(&qp,0);
    res = newtree.queryResults.getTopK();
    EXPECT_EQ(res[0].second, qidx);
    delete[] m2;
}
#if __DATA_TYPE__ == __VEC_TYPE__

TEST(hi, HITree_test_build_query_spatial)
{
    const int R = 4096;
    const int C = 20;
    int k = 3;
    size_t qidx = 3;
    auto m = testutil::makeM<float>(R, C);
    auto m2 = testutil::makeM<float>(R, C);
    float* queryPoint = &m2[qidx*C];

    hi::HITree tree(new EucDataManager<float> (m, R, C, true,true, 0));
//    dprintca(m, R, C);
    tree.buildParams.lshVarThreshold = std::numeric_limits<float>::infinity();;
    tree.buildParams.maxPivots = 1;
    tree.buildParams.maxLeafPoints = 3;
    tree.build(0);

    hi::HITree lsh(new EucDataManager<float>(m2, R, C, true,true, 0));
//    dprintca(m, R, C);
    lsh.buildParams.lshVarThreshold = 0;
    lsh.build(0);

    EXPECT_EQ(tree.getRoot()->isLeaf(), false);
    {
        std::ofstream ofs("spatial_test_build.idx");
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
    EXPECT_EQ(tree.buildResults.npointsInLSH,newtree.buildResults.npointsInLSH);
    EXPECT_EQ(tree.buildResults.npointsInLeaf,newtree.buildResults.npointsInLeaf);
    EXPECT_EQ(tree.buildResults.ninternal,newtree.buildResults.ninternal);
    EXPECT_EQ(tree.buildResults.nspatial,newtree.buildResults.nspatial);
    EXPECT_EQ(tree.buildResults.nnodes,newtree.buildResults.nnodes);
    EXPECT_EQ(R,tree.buildResults.npointsInInternal + tree.buildResults.npointsInLeaf);

    EXPECT_EQ(R,newtree.buildResults.npoints);
    EXPECT_EQ(R,newtree.buildResults.npointsInInternal + newtree.buildResults.npointsInLeaf);
    EXPECT_EQ(newtree.buildResults.nnodes, newtree.buildResults.ninternal + newtree.buildResults.nleaf);

    /// Query

    Euc<float> qp(queryPoint, C);
//    tree.queryParams.setQueryPoint(queryPoint, C);
    tree.knnquery(&qp,0);

//    newtree.queryParams.setQueryPoint(queryPoint, C);
    newtree.knnquery(&qp,0);

//    lsh.queryParams.setQueryPoint(queryPoint, C);
    lsh.knnquery(&qp,0);
    auto ttk = tree.queryResults.getTopK();
    auto nttk = newtree.queryResults.getTopK();
    auto ltk = lsh.queryResults.getTopK();

    EXPECT_EQ(k, ttk.size());
    EXPECT_EQ(k, nttk.size());

    auto it1 = ttk.begin();
    auto it2 = nttk.begin();

    EXPECT_EQ(it1->second, qidx);
    for (; it1 < ttk.end(); ++it1, ++it2 ){
        EXPECT_EQ(it1->second, it2->second);
        EXPECT_EQ(it1->first, it2->first);
    }

//    tree.printQueryResults();
}


TEST(hi, HITree_test_load_from_file)
{
    std::string filename = sutil::sformat("%s/../data/tests/gaussian__d=20_s=10000_nclus=1_var=0.1.bin",
                                          CMAKE_CURRENT_BINARY_DIR);
    EucDataManager<float> *pmdat = EucDataManager<float>::loadData(filename);
    EXPECT_EQ(pmdat->getRows(), 10000);
    hi::HITree tree(pmdat);
//    tree.build();
}

TEST(hi, HITree_test_build_query_LSH)
{
    const int R = 4096;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    int k = 3;
    size_t qidx = 3;
    const size_t sidx = 10000;
    auto pmdat = new EucDataManager<float>(m, R, C, true, true, sidx);
    hi::HITree tree(pmdat);
    tree.buildParams.lshVarThreshold = 0;

    tree.build(0);
    {
        std::ofstream ofs("lsh_test_build.idx");
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

    EXPECT_EQ(R, newtree.buildResults.npointsInLSH);
    /// Query

    Euc<float> qp((*pmdat)[qidx], C);
//    tree.queryParams.setQueryPoint((*pmdat)[qidx], C);
    tree.knnquery(&qp,0);
//    newtree.queryParams.setQueryPoint((*pmdat)[qidx], C);
    newtree.knnquery(&qp,0);

    EXPECT_EQ(k, tree.queryResults.getTopK().size());
    EXPECT_EQ(k, newtree.queryResults.getTopK().size());
    EXPECT_EQ(sidx + qidx, tree.queryResults.getTopK()[0].second);

    auto it1 = tree.queryResults.getTopK().begin();
    auto it2 = newtree.queryResults.getTopK().begin();

//    for (; it1 != tree.queryResults.getTopK().end(); ++it1, ++it2 ){
//        EXPECT_EQ(it1->second, it2->second);
//        EXPECT_EQ(it1->first, it2->first);
//    }
    EXPECT_EQ(sidx + 3, tree.queryResults.getTopK()[0].second);
    EXPECT_EQ(sidx + 2, tree.queryResults.getTopK()[1].second);
    EXPECT_EQ(sidx + 4, tree.queryResults.getTopK()[2].second);
//    tree.printQueryResults();

}


TEST(hi, HITree_test_LSH_sliced_idxs)
{
    const int R = 101;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    auto pmdat = new EucDataManager<float>(m, R, C, true, true, 100);

    hi::HITree tree(pmdat);
    tree.buildParams.lshVarThreshold = 0;
    tree.build(0);

//    tree.queryParams.setQueryPoint((*pmdat)[3], C);
    Euc<float> qp((*pmdat)[3], C);
    tree.knnquery(&qp,0);

    auto r = tree.queryResults.getTopK();
    EXPECT_EQ(r[0].second, 103);
    EXPECT_EQ(r[1].second, 102);
    EXPECT_EQ(r[2].second, 104);
}

#endif

#endif
