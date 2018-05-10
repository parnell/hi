
#include "DataManager.hpp"
#include "../../utils/vecutil.hpp"
#include "../../utils/carray_iterator.hpp"
#include "../indexes/controllers/PivotSorter.hpp"
#include <algorithm>    // std::sort
//#include <flann/flann.h>
//#include <flann/io/hdf5.h>
#include "../../dprint.hpp"

DataManager::DataManager() : deleteData(true) {};

DataManager::DataManager(Dat *pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData) : deleteData(deleteData) {
    if (shouldTransfer){
        m.transfer(pdata, rows, cols);
    } else {
        m.load(pdata, rows, cols);
    }
//    dprintf("   DATAN  ###  %p \n", m[0]);
}
DataManager::~DataManager() {
    if (! deleteData){
        m.setData(nullptr); /// this will prevent the natural deletion of the data
    }
}

DataManager *DataManager::sliceData(size_t _begin, size_t _end) {
//    dprintf("     ### slicing  %ld   %ld    %ld    %p\n", _begin, _end,   _end - _begin, m[_begin]);
    return new DataManager(m[_begin], _end - _begin, getCols(), true, false);
}

Dat* DataManager::loadData(std::string filename){
    m.load(filename);
    return m.getData();
}

size_t DataManager::getRows() const {
    return m.getSize();
}

Dat *DataManager::getDat() const {
    return m.getData();
}

size_t DataManager::getCols() const {
    return m.getDim();
}


lshbox::Matrix<Dat> DataManager::getLSHMatrix(){
    lshbox::Matrix<Dat> data;
//    data.transfer(m.getData(), m.getSize(), m.getDim());
    data.load(m.getData(), m.getSize(), m.getDim());
    return data;
}

Dat DataManager::get(size_t r, size_t c) {
    return m[r][c];
}


void DataManager::print() const {
    m.print();
}


struct MyComp {
    MyComp(Dat* pivot, size_t C): pivot(pivot), C(C) { }
    bool operator () (Dat* i, Dat* j) { return (i<j); }

    Dat* pivot;
    size_t C;
};

void DataManager::sort(Dat *pivot) {
    PivotSorter sorter(m.getData(),m.getRows(),m.getCols());
    sorter.sort(pivot);
//    const size_t R = m.getRows();
//    const size_t C = m.getCols();
//    Dat* pd = m.getData();
//    std::vector<Dat *> rows(R);
//    for (size_t i = 0; i < R; ++i) {
//        rows[i] = &pd[i*C]; }
//    std::sort(std::begin(rows), std::end(rows), MyComp(pivot));

}

void DataManager::print_rowp() const {
    m.print_rowp();
}



#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../dprint.hpp"
#include "../../utils/testutil.hpp"
#include "../../utils/stringutils.hpp"

TEST(controllers, DM_test_load)
{
    std::string filename = sutil::sformat("%s/../data/tests/gaussian__d=14_s=10000_nclus=1_var=0.1.bin",
                                          CMAKE_CURRENT_BINARY_DIR);
    DataManager mdat;
    mdat.loadData(filename);
    EXPECT_EQ(mdat.getRows(), 10000);
}

TEST(controllers, DM_construction_Memory)
{
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM(R, C);

    DataManager mdat(m, R, C, true, true);
    Dat* pbeg = mdat.getDat();
    EXPECT_EQ(mdat.getDat(), m);
    DataManager mdat2(m, R, C, false,true);
    EXPECT_NE(mdat2.getDat(), m);
}

TEST(controllers, DM_test_serialize)
{
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM(R, C);
    DataManager dat(m, R,C, false,true);

    std::ofstream ofs("dm_test_serialize.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << dat;
    }

    DataManager newdat;
    {
        std::ifstream ifs("dm_test_serialize.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newdat;
    }
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j <C; ++j) {
            EXPECT_EQ(dat.m[i][j], newdat.m[i][j]);
        }
    }
}

TEST(controllers, DM_test_slice)
{
    unsigned int R = 10;
    unsigned int C = 2;
    auto m = testutil::makeM(R, C);
    DataManager dat(m, R,C, false,true);
    DataManager *pdat1 = dat.sliceData(0, R / 2);
    DataManager *pdat2 = dat.sliceData(R / 2, R);
    EXPECT_EQ(pdat1->getRows(), R/2);
    EXPECT_EQ(pdat2->getRows(), R/2);
    for (int i = 0; i < R/2; ++i) {
        EXPECT_EQ(pdat1->m[i], dat.m[i]);
        EXPECT_EQ(pdat2->m[i], dat.m[R/2 + i]);
    }

    R = 11;
    const int R2 = R/2;
    auto m2 = testutil::makeM(R, C);
    DataManager dat2(m2, R, C, false,true);
    pdat1 = dat2.sliceData(0, R / 2);
    pdat2 = dat2.sliceData(R / 2, R);
    EXPECT_EQ(pdat1->getRows(), R/2);
    EXPECT_EQ(pdat2->getRows(), R/2 +1);
    for (int i = 0; i < R/2; ++i) {
        EXPECT_EQ(pdat1->m[i], dat2.m[i]);
        EXPECT_EQ(pdat2->m[i], dat2.m[R/2 + i]);
    }
    EXPECT_EQ(pdat2->m[R/2], dat2.m[R-1]);

    DataManager *pdat3 = pdat2->sliceData(R2 - 3, R2);
    EXPECT_EQ(pdat3->getRows(), 3);
    for (int i = 0; i < R2-3; ++i) {
        EXPECT_EQ(pdat2->m[R2-3 + i], pdat3->m[i]);
    }

}

#endif




