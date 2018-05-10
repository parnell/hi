
#include "LSHWrapper.hpp"
#include <lshbox/lsh/itqlsh.h>
#include <lshbox/lsh/rbslsh.h>
#include <lshbox/lsh/rhplsh.h>
#include <lshbox/lsh/thlsh.h>
#include <lshbox/lsh/psdlsh.h>
#include <lshbox/lsh/shlsh.h>
#include <lshbox/lsh/dbqlsh.h>
#include <lshbox/lsh/kdbqlsh.h>



LSHWrapper::LSHWrapper(LSHTYPE lshtype, int D, int M, int L, int S, int I, int N) :
M(M), L(L), D(D), S(S), I(I), N(N){
    this->lshtype = lshtype;
    assert (lshtype != RBS); /// not implemented
    assert (lshtype != TH); /// not implemented
    assert (lshtype != RHP); /// not implemented
//    std::cout << "LSHWrapper\tD=" << D << "\tM=" << M << "\tL=" << L << "\tS=" << S << "\tI=" << I << "\tN="<<N<< std::endl;
}

void LSHWrapper::load(const string& filename){
    switch(lshtype){
        case ITQ: itq.load(filename);break;
//            case KDBQ:kdbq.load(filename);break;
//            case DBQ: dbq.load(filename);break;
//            case PSD: psd.load(filename);break;
////            case RBS: rbs.load(filename);break;
//            case SH: sh.load(filename);break;
        default: break;
    }
}

void LSHWrapper::save(const string& filename){
    switch(lshtype){
        case ITQ: itq.save(filename); break;
//            case KDBQ: kdbq.save(filename); break;
//            case DBQ: dbq.save(filename);break;
//            case PSD: psd.save(filename);break;
////            case RBS: rbs.save(filename);break;
//            case SH: sh.save(filename);break;
        default: break;
    }
}

lshbox::Scanner<lshbox::Matrix<Dat>::Accessor> LSHWrapper::query(Dat *point, const int k) {
    auto am = lshbox::Metric<Dat>(D, L2_DIST );
    lshbox::Matrix<Dat>::Accessor accessor(data);
    lshbox::Scanner<lshbox::Matrix<Dat>::Accessor> scanner(accessor,am,k);
    itq.query(point, scanner);
    return scanner;
}

void LSHWrapper::query(lshbox::Benchmark& bench,
           lshbox::Matrix<float>& data,
           lshbox::Scanner<lshbox::Matrix<float>::Accessor>& scanner,
           unsigned int index){
    switch(lshtype){
        case ITQ: itq.query(data[bench.getQuery(index)], scanner);break;
//            case KDBQ: kdbq.query(data[bench.getQuery(index)], scanner);break;
//            case DBQ: dbq.query(data[bench.getQuery(index)], scanner);break;
//            case PSD: psd.query(data[bench.getQuery(index)], scanner);break;
////            case RBS: rbs.query(data[bench.getQuery(index)], scanner);break;
//            case SH: sh.query(data[bench.getQuery(index)], scanner);break;
        default: break;
    }
}

void LSHWrapper::hash(Dat* pdata, const int R, const int C) {
//    lshbox::Matrix<Dat> data;
    data.transfer(pdata, R, C);
    lshbox::Matrix<Dat>::Accessor accessor(data);

    hash();
}

void LSHWrapper::hash(){
    switch(lshtype) {
        case ITQ:
            assert (N > 0);
            lshbox::itqLsh<Dat>::Parameter itqp;
            itqp.M = M; ///(521) Hash table size
            itqp.L = L; /// Number of hash tables
            itqp.D = D;
            itqp.N = N; /// Binary code bytes, (has to be less than D)
            assert (itqp.N <= itqp.D);
            itqp.S = S; /// Size of vectors in train
            itqp.I = I; /// Training iterations

            itq.reset(itqp);
            itq.train(data);
            itq.hash(data);
            break;

//            case KDBQ:
//                lshbox::kdbqLsh<Dat>::Parameter kdbqp;
//                kdbqp.M = M; ///(521) Hash table size
//                kdbqp.L = L; /// Number of hash tables
//                kdbqp.D = D;
//                kdbqp.N = N; /// /// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
//                kdbqp.I = I; /// Training iterations
//                kdbq.reset(kdbqp);
//                kdbq.train(data);
////                kdbq.hash(data);// no hashing
//                break;
//            case DBQ:
//                lshbox::dbqLsh<Dat>::Parameter dbqp;
//                dbqp.M = M; ///(521) Hash table size
//                dbqp.L = L; /// Number of hash tables
//                dbqp.D = D;
//                dbqp.N = N; /// /// Number of projection dimensions,corresponding to 2*N binary code bytes for each vector
//                dbqp.I = I; /// Training iterations
//                dbq.reset(dbqp);
//                dbq.train(data);
//                dbq.train(data);
//                break;
//            case PSD:
//                lshbox::psdLsh<Dat>::Parameter psdp;
//                psdp.M = M; ///(521) Hash table size
//                psdp.L = L; /// Number of hash tables
//                psdp.D = D;
//                psdp.T = T; /// Index mode, you can choose 1(CAUCHY) or 2(GAUSSIAN)
//                psdp.W = W; /// Window Size
//                psd.reset(psdp);
////                psd.train() /// no training
//                psd.hash(data);
//                break;
//            case SH:
//                lshbox::shLsh<Dat>::Parameter shp;
//                shp.M = M; ///(521) Hash table size
//                shp.L = L; /// Number of hash tables
//                shp.D = D;
//                shp.N = N; /// Binary code bytes
//                shp.S = S; /// Size of vectors in train
//                sh.reset(shp);
//                sh.train(data);
//                sh.hash(data);
//                break;
        default: break;
    }
}


#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../../utils/testutil.hpp"
#include "../../../../dprint.hpp"
#include <fstream>

TEST(utils, LSHWrapper_matrix_serialization)
{
    const int R = 101;
    const int C = 2;
    auto m = testutil::makeM(R, C);

    lshbox::Matrix<Dat> lsh;
    lsh.load(m, R, C);

    std::ofstream ofs("lshw_test_build.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << lsh;
    }
    lshbox::Matrix<Dat> lsh2;
    {
        std::ifstream ifs("lshw_test_build.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> lsh2;
    }
    EXPECT_EQ(lsh.getSize(), lsh2.getSize());
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            EXPECT_EQ(lsh[i][j],lsh2[i][j]);
        }
    }
}

const unsigned int _M = 500; ///(521) Hash table size
const unsigned int _L = 5; /// Number of hash tables
const unsigned int _S = 100; /// Size of vectors in train
const unsigned int _I = 5;/// Training iterations
const unsigned int _N = 4; /// Binary code bytes, (has to be less than D)

const int _MIN_ROWS_LSH = 100;


TEST(hi, LSHWrapper_wrapper_serialization)
{
    const int R = 11;
    const int C = 2;
    int k = 3;
    const auto r = static_cast<unsigned int>(R);
    auto m = testutil::makeM(R, C);
    unsigned int M = R > _MIN_ROWS_LSH ? _M : std::min(r, r/2);
    unsigned int L = _L;
    unsigned int S = R > _MIN_ROWS_LSH ? _S : std::min(r, r/2);
    unsigned int I = _I;
    unsigned int N = C > _N ? _N : std::min(static_cast<unsigned int>(C), _N);
///LSHWrapper::LSHWrapper(LSHTYPE lshtype, int D, int M, int L, int S, int I) :
    LSHWrapper lsh(LSHTYPE::ITQ, C, M, L, S, I, N);
    lsh.hash(m, R, C);

    std::ofstream ofs("lshw_wrapper_serialization.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << lsh;
    }
    LSHWrapper lsh2;
    {
        std::ifstream ifs("lshw_wrapper_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> lsh2;
    }
    EXPECT_EQ(lsh.D, lsh2.D);
    int pidx = 3;
    auto scanner = lsh.query(&m[pidx * C],k);
    auto scanner2 = lsh2.query(&m[pidx * C],k);
    EXPECT_EQ(scanner.cnt(), scanner2.cnt());
    scanner.topk().genTopk();
    scanner2.topk().genTopk();
    auto res1 = scanner.topk().getTopk();
    auto res2 = scanner2.topk().getTopk();
    auto it1 = res1.begin();
    auto it2 = res2.begin();
    for (; it1 != res1.end(); ++it1, ++it2 ){
//        std::cout << it1->second << ": " << it1->first << "\t" <<
//                  it2->second << ": " << it2->first <<std::endl;
        EXPECT_EQ(it1->second, it2->second);
        EXPECT_EQ(it1->first, it2->first);
    }
}

#endif
