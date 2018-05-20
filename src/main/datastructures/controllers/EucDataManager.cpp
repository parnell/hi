
#include "EucDataManager.hpp"

#include "../../utils/vecutil.hpp"
#include "../indexes/controllers/PivotSorter.hpp"
#include <algorithm>    // std::sort
#include "../../dprint.hpp"
#include <numeric>
#include <boost/serialization/export.hpp>


//BOOST_CLASS_EXPORT_IMPLEMENT(EucDataManager<float>);


#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../dprint.hpp"
#include "../../utils/testutil.hpp"
#include "../../utils/stringutils.hpp"

TEST(controllers, DM_test_save_load)
{
    std::string filename = sutil::sformat("%s/../data/tests/dm_test.bin",
                                          CMAKE_CURRENT_BINARY_DIR);
    const int R = 1024;
    const int C = 18;
    auto m = testutil::makeM<float>(R, C);
    {
        EucDataManager<float> pmdat(m, R, C, true, true,0);
        pmdat.m.save(filename);
    }
    {
        EucDataManager<float> *pmdat = EucDataManager<float>::loadData(filename);
        EXPECT_EQ(pmdat->getRows(), R);
        EXPECT_EQ(pmdat->getCols(), C);
        delete pmdat;
    }
}

TEST(controllers, DM_test_load)
{
    std::string filename = sutil::sformat("%s/../data/tests/gaussian__d=20_s=10000_nclus=1_var=0.1.bin",
                                          CMAKE_CURRENT_BINARY_DIR);
    auto pmdat = EucDataManager<float>::loadData(filename);
    EXPECT_EQ(pmdat->getRows(), 10000);
}

TEST(controllers, DM_construction_Memory)
{
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);

    EucDataManager<float> mdat(m, R, C, true, true, 0);
    EXPECT_EQ(mdat.getDat(), m);
    EucDataManager<float> mdat2(m, R, C, false,true, 0);
    EXPECT_NE(mdat2.getDat(), m);
}

TEST(controllers, DM_test_serialize)
{
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    EucDataManager<float> dat(m, R,C, false,true, 0);

    {
        std::ofstream ofs("dm_test_serialize.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << dat;
    }

    EucDataManager<float> newdat;
    {
        std::ifstream ifs("dm_test_serialize.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newdat;
    }
    for (int i = 0; i < R; ++i) {
        EXPECT_LE(dat.idxs[i], R);
        for (int j = 0; j <C; ++j) {
            EXPECT_EQ(dat.m[i][j], newdat.m[i][j]);
        }
    }
}

TEST(controllers, DM_test_serialize_dm)
{
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);
    DataManager* pdat = new EucDataManager<float>(m, R,C, false,true, 0);

    {
        std::ofstream ofs("DM_test_serialize_dm.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << pdat;
    }

    DataManager* pnewdat;
    {
        std::ifstream ifs("DM_test_serialize_dm.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> pnewdat;
    }
    for (int i = 0; i < R; ++i) {
        EXPECT_LE(pdat->idxs[i], R);
        for (int j = 0; j <C; ++j) {
            EXPECT_EQ(static_cast<EucDataManager<float>*>(pdat)->m[i][j],
                      static_cast<EucDataManager<float>*>(pnewdat)->m[i][j]);
        }
    }
}


TEST(controllers, DM_test_slice)
{
    unsigned int R = 10;
    unsigned int C = 2;
    auto m = testutil::makeM<float>(R, C);
    EucDataManager<float> dat(m, R,C, false,true, 0);
    auto *pdat1 = static_cast<EucDataManager<float>*>(dat.sliceData(0, R / 2));
    auto *pdat2 = static_cast<EucDataManager<float>*>(dat.sliceData(R / 2, R));
    EXPECT_EQ(pdat1->getRows(), R/2);
    EXPECT_EQ(pdat2->getRows(), R/2);
    for (int i = 0; i < R/2; ++i) {
        EXPECT_LE(pdat1->idxs[i], R);
        EXPECT_LE(pdat2->idxs[i], R);
        EXPECT_EQ(pdat1->m[i], dat.m[i]);
        EXPECT_EQ(pdat2->m[i], dat.m[R/2 + i]);
        EXPECT_EQ(pdat1->idxs[i], i);
        EXPECT_EQ(pdat2->idxs[i], static_cast<int>(i+R/2));
    }
    R = 11;
    const int R2 = R/2;
    auto m2 = testutil::makeM<float>(R, C);
    EucDataManager<float> dat2(m2, R, C, false,true, 0);
    pdat1 = static_cast<EucDataManager<float>*>(dat2.sliceData(0, R / 2));
    pdat2 = static_cast<EucDataManager<float>*>(dat2.sliceData(R / 2, R));
    EXPECT_EQ(pdat1->getRows(), R/2);
    EXPECT_EQ(pdat2->getRows(), R/2 +1);
    for (int i = 0; i < R/2; ++i) {
        EXPECT_LE(pdat1->idxs[i], R);
        EXPECT_LE(pdat2->idxs[i], R);
        EXPECT_EQ(pdat1->m[i], dat2.m[i]);
        EXPECT_EQ(pdat2->m[i], dat2.m[R/2 + i]);
    }
    EXPECT_EQ(pdat2->m[R/2], dat2.m[R-1]);

    auto *pdat3 = static_cast<EucDataManager<float>*>(pdat2->sliceData(R2 - 3, R2));
    EXPECT_EQ(pdat3->getRows(), 3);
    for (int i = 0; i < R2-3; ++i) {
        EXPECT_EQ(pdat2->m[R2-3 + i], pdat3->m[i]);
    }
}



TEST(controllers, DM_test_sort)
{
    unsigned int R = 10;
    unsigned int C = 2;
    auto m = testutil::makeM<float>(R, C);
    EucDataManager<float> dat(m, R,C, false,true, 0);
    for (size_t pidx = 0; pidx < R; ++pidx) {
        auto pe = new Euc<float>(&m[pidx * C], C);
        Pivot p(pe, pidx, R);
        dat.sort(p);
        EXPECT_EQ(dat.idxs[0], pidx);
    }
}

#endif



