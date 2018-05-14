
#include "DataManager.hpp"
#include "../../utils/vecutil.hpp"
#include "../../utils/carray_iterator.hpp"
#include "../indexes/controllers/PivotSorter.hpp"
#include <algorithm>    // std::sort
#include "../../dprint.hpp"
#include <numeric>

DataManager::DataManager() : deleteData(true) {};

DataManager::DataManager(Dat *pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData, std::vector<size_t> idxs) :
        deleteData(deleteData), idxs(idxs) {
    if (shouldTransfer){
        m.transfer(pdata, rows, cols,deleteData);
    } else {
        m.load(pdata, rows, cols);
    }

}
DataManager::DataManager(Dat* pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData, size_t startingIdx):
        deleteData(deleteData), idxs(rows){
    if (shouldTransfer){
        m.transfer(pdata, rows, cols, deleteData);
    } else {
        m.load(pdata, rows, cols);
    }
    std::iota(idxs.begin(), idxs.end(), startingIdx);
//    dprintf("   DATAN  ###  %p \n", m[0]);

}
DataManager::~DataManager() {
    if (! deleteData){
        m.setData(nullptr); /// this will prevent the natural deletion of the data
    }

}

DataManager *DataManager::sliceData(size_t _begin, size_t _end) {
//    dprintf("     ### slicing  %ld   %ld    %ld    %p\n", _begin, _end,   _end - _begin, m[_begin]);
    return new DataManager(m[_begin], _end - _begin, getCols(), true, false,
                           std::vector<size_t>(idxs.begin()+_begin, idxs.begin()+_end));
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

void DataManager::sort(Pivot& pivot) {
    PivotSorter sorter(m.getData(),m.getRows(),m.getCols());
    sorter.sort(idxs, pivot);
}

void DataManager::print_rowp() const {
    m.print_rowp();
}

std::ostream &operator<<(std::ostream &os, DataManager &dm) {
    const size_t R = dm.getRows();
    const size_t C = dm.getCols();
    auto a = dm.getDat();
    os << " [";
    for (size_t __r = 0; __r< R; ++__r){
        os << "\t" << __r << "\t" << dm.idxs[__r] << "[ ";\
        for (size_t __c = 0; __c < C; ++__c) {
            os << a[__r*C+__c] << " ";
        }
        os << "]";
        if (__r != R-1)
            os << std::endl << "  ";
    }
    os << "]" << std::endl;
    return os;
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

    DataManager mdat(m, R, C, true, true, 0);
    EXPECT_EQ(mdat.getDat(), m);
    DataManager mdat2(m, R, C, false,true, 0);
    EXPECT_NE(mdat2.getDat(), m);
}

TEST(controllers, DM_test_serialize)
{
    const int R = 10;
    const int C = 2;
    auto m = testutil::makeM(R, C);
    DataManager dat(m, R,C, false,true, 0);

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
        EXPECT_LE(dat.idxs[i], R);
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
    DataManager dat(m, R,C, false,true, 0);
    DataManager *pdat1 = dat.sliceData(0, R / 2);
    DataManager *pdat2 = dat.sliceData(R / 2, R);
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
    auto m2 = testutil::makeM(R, C);
    DataManager dat2(m2, R, C, false,true, 0);
    pdat1 = dat2.sliceData(0, R / 2);
    pdat2 = dat2.sliceData(R / 2, R);
    EXPECT_EQ(pdat1->getRows(), R/2);
    EXPECT_EQ(pdat2->getRows(), R/2 +1);
    for (int i = 0; i < R/2; ++i) {
        EXPECT_LE(pdat1->idxs[i], R);
        EXPECT_LE(pdat2->idxs[i], R);
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


//
//TEST(controllers, DM_test_sort)
//{
//    unsigned int R = 10;
//    unsigned int C = 2;
//    int pidx = 3;
//    auto m = testutil::makeM(R, C);
//    DataManager dat(m, R,C, false,true, 0);
//    dat.sort(&m[pidx*C]);
//    EXPECT_EQ(dat.idxs[0], pidx);
//    pidx = 8;
//    dat.sort(&m[pidx*C]);
//    EXPECT_EQ(dat.idxs[0], pidx);
//}

#endif




