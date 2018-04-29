
#include "DataManager.hpp"
#include "../utils/vecutil.hpp"

//#include <lshbox/basis.h>
//#include <lshbox/metric.h>
//#include <lshbox/topk.h>
//#include <lshbox/eval.h>
//#include <lshbox/matrix.h>
//#include <lshbox/lsh/itqlsh.h>

Dat* DataManager::loadData(std::string filename){
//        std::string filename = "/Users/i855892/data/rdata/vec/gaussian/gaussian__d=14_s=10000_nclus=1_var=0.1.hdf5";
    std::string indexName = "data";
    flann::Matrix<Dat> dataset;
//        pindex->buildIndex();
    load_from_file(dataset, filename, indexName);

//        lshbox::Matrix<Dat> data(filename);
    Dat* pdat = dataset.ptr();
    size_t rows = dataset.rows;
    size_t cols = dataset.cols;
    m.transfer(pdat, rows, cols);

//    lshbox::Matrix<Dat> data;
//    data.load(dataset.ptr(), dataset.rows, dataset.cols);
    std::cout << "Loading Data !!!" << rows << " : " << cols << std::endl;
    return pdat;
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
    data.transfer(m.getData(), m.getSize(), m.getDim());
    return data;
}

//void DataManager::calculateVariance() {
//    Dat* pd;
//    if (rows > 1000){
//        int n = static_cast<int>(rows * 0.01);
//        std::vector<size_t> idxs = vecutil::pick(rows, n);
//        Dat* pivot = m[idxs[0]];
////
//////        std::vector<Dat*> d(n);
////        float variance = 0;
////        for (size_t i =0; i< n;++i){
////            variance += sqr(m[i] - mean);
////        }
////
////        for (int x = 0; x < (v.size() - 1); x++) {
////            variance += (v.at(x) - mean) * (v.at(x) - mean);
////        }
//        variance /= v.size();
//    } else {
//
//    }
//}


void DataManager::calculateVariance()
{
    const size_t size = m.getSize();
    const size_t D = m.getDim();
    var.clear();
    mean.clear();

    var.resize(D);
    mean.resize(D);

    float avg[D];
    const Dat* points = m.getData();
    /// i = 0, i_n = 1
//    std::copy_n(m[0], m.getDim(), avg);
    for (size_t i = 0; i < D; ++i) {
        avg[i] = points[i]; }
    /// i = 1, i_n = 0.5
    if (size >= 2) {
        //assert(points[1].dim() == D);
        for (int d = D - 1; d >= 0; --d) {
            float const delta = points[1 * D + d] - avg[d];
            avg[d] += delta * 0.5f;
            var[d] = delta * (points[1 * D + d] - avg[d]);
        }
    } else {
//        std::fill_n(var, D, 0.0f);
        for (size_t i = 0; i < D; ++i) {
            var[i] = 0.0f; }
    }


    // i = 2, ...
    for (size_t i = 2; i < size; ) {
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (size_t d = 0; d < D; ++d) {
                float const delta = points[i * D + d] - avg[d];
                avg[d] += delta * i_n;
                var[d] += delta * (points[i * D + d] - avg[d]);
            }
        }
        ++i;

        if (i >= size) break;
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (long int d = D - 1; d >= 0; --d) {
                float const delta = points[i * D + d] - avg[d];
                avg[d] += delta * i_n;
                var[d] += delta * (points[i * D + d] - avg[d]);
            }
        }
        ++i;
    }

    for (int j = 0; j < D; ++j) {
        var[j] /= size;
        mean[j] = avg[j];
    }
}

void DataManager::calculateVariance(std::vector<Dat*> points)
{
    const size_t size = points.size();
    const size_t D = m.getDim();
    var.clear();
    mean.clear();
    var.resize(D);
    mean.resize(D);


    float avg[D];
//    const Dat* points = m.getData();
    /// i = 0, i_n = 1
//    std::copy_n(m[0], m.getDim(), avg);
    for (size_t i = 0; i < D; ++i) {
        avg[i] = points[0][i]; }

    Dat* row;
    /// i = 1, i_n = 0.5
    if (size >= 2) {
        //assert(points[1].dim() == D);
        for (int d = D - 1; d >= 0; --d) {
            float const delta = points[1][d] - avg[d];
            avg[d] += delta * 0.5f;
            var[d] = delta * (points[1][d] - avg[d]);
        }
    } else {
//        std::fill_n(var, D, 0.0f);
        for (size_t i = 0; i < D; ++i) {
            var[i] = 0.0f; }
    }


    // i = 2, ...
    for (size_t i = 2; i < size; ) {
        row = points[i];
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (size_t d = 0; d < D; ++d) {
                float const delta = row[d] - avg[d];
                avg[d] += delta * i_n;
                var[d] += delta * (row[d] - avg[d]);
            }
        }
        ++i;

        row = points[i];
        if (i >= size) break;
        {
            const float i_n = 1.0f / (1.0f + i);
            //assert(points[i].dim() == D);
            for (long int d = D - 1; d >= 0; --d) {
                float const delta = row[d] - avg[d];
                avg[d] += delta * i_n;
                var[d] += delta * (row[d] - avg[d]);
            }
        }
        ++i;
    }

    for (int j = 0; j < D; ++j) {
        var[j] /= size;
        mean[j] = avg[j];
    }
}


#if COMPILE_TESTS
#include "gtest/gtest.h"

TEST(utils, testvar)
{
    const int R = 10;
    const int C = 2;
    DataManager d;
    d.m.reset(C, R);
    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            d.m[r][c] = r*(c+1);
//            std::cout << d.m[r][c] << "  ";
        }
//        std::cout << std::endl;
    }
    d.calculateVariance();
    EXPECT_EQ(d.var[0], 8.25);
    EXPECT_EQ(d.var[1], 33.00);
    EXPECT_EQ(d.mean[0], 4.5);
    EXPECT_EQ(d.mean[1], 9.00);
//    std::cout << "-----------------" << std::endl;
    const int RS = R/2;
    std::vector<Dat*> randtest(RS);
    for (int r= 0; r< RS; r++) {
//        std::cout <<"  " << r << "  " << r*C << "  ";
        randtest[r] = d.m[r*2];
//        for (int c = 0; c < C; ++c) {
//            std::cout << randtest[r][c] << "  ";
//        }
//        std::cout << std::endl;
    }
    d.calculateVariance(randtest);
    EXPECT_EQ(d.var[0], 8);
    EXPECT_EQ(d.var[1], 32.00);
    EXPECT_EQ(d.mean[0], 4);
    EXPECT_EQ(d.mean[1], 8.00);
}

#endif

