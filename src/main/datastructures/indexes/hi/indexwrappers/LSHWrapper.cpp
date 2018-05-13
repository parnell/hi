
#include "LSHWrapper.hpp"
#include <lshbox/lsh/itqlsh.h>
#include <lshbox/lsh/rbslsh.h>
#include <lshbox/lsh/rhplsh.h>
#include <lshbox/lsh/thlsh.h>
#include <lshbox/lsh/psdlsh.h>
#include <lshbox/lsh/shlsh.h>
#include <lshbox/lsh/dbqlsh.h>
#include <lshbox/lsh/kdbqlsh.h>
#include "../../../../dprint.hpp"

class HITree;

const unsigned int _M = 500; ///(521) Hash table size
const unsigned int _L = 5; /// Number of hash tables
const unsigned int _S = 100; /// Size of vectors in train
const unsigned int _I = 5;/// Training iterations
const unsigned int _N = 4; /// Binary code bytes, (has to be less than D)

const int _MIN_ROWS_LSH = 100;


LSHWrapper::LSHWrapper(LSHTYPE lshtype, int D, int M, int L, int S, int I, int N) :
        M(M), L(L), D(D), S(S), I(I), N(N), lshtype(lshtype), pdata(nullptr){
    assert (lshtype != RBS); /// not implemented
    assert (lshtype != TH); /// not implemented
    assert (lshtype != RHP); /// not implemented
//    std::cout << "LSHWrapper\tD=" << D << "\tM=" << M << "\tL=" << L << "\tS=" << S << "\tI=" << I << "\tN="<<N<< std::endl;
}

/**
 * If used, must still set paramaters M,L,D,S,I,N
 * @param lshtype
 */
LSHWrapper::LSHWrapper(LSHTYPE lshtype, const size_t R, const size_t C) :
        M(0), L(0), D(0), S(0), I(0), N(0), lshtype(lshtype), pdata(nullptr){
    assert (lshtype != RBS); /// not implemented
    assert (lshtype != TH); /// not implemented
    assert (lshtype != RHP); /// not implemented
    setDefaultParams(R, C);
//    std::cout << "LSHWrapper\tD=" << D << "\tM=" << M << "\tL=" << L << "\tS=" << S << "\tI=" << I << "\tN="<<N<< std::endl;
}

LSHWrapper::~LSHWrapper() {
//    if (pdata){
//        delete pdata;
//    }
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

lsh_scanner LSHWrapper::query(Dat *point, const int k) {
    auto am = lshbox::Metric<Dat>(D, L2_DIST );
    lshbox::Matrix<Dat>::Accessor accessor(*pdata);
    lsh_scanner scanner(accessor,am,k);
    scanner.reset(point);
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
    this->pdata = new lshbox::Matrix<Dat>();
    this->pdata->transfer(pdata, R, C); /// Careful, potentially doesn't like being part of a tree and transferring
    lshbox::Matrix<Dat>::Accessor accessor(*this->pdata);

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
            itq.train(*pdata);
            itq.hash(*pdata);
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

size_t LSHWrapper::size() const {
    return pdata->getSize();
}

void LSHWrapper::setDefaultParams(const size_t R, const size_t C) {
    const auto r = static_cast<unsigned int>(R);
    assert(R > 2);
    /** int D=2, int M=500, int L=5, int S = 100, int I = 5*/
    M = R > _MIN_ROWS_LSH ? _M : std::max(static_cast<unsigned int>(2), r / 2);
    L = _L;
    D = static_cast<unsigned int>(C);
    S = R > _MIN_ROWS_LSH ? _S : std::max(static_cast<unsigned int>(2), r / 2);
    I = _I;
    N = C > _N ? _N : std::min(static_cast<unsigned int>(C), _N);

    dprintf("%d %d %d %d %d N=%d \n", M, L, D, S, I, N);
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


TEST(hi, LSHWrapper_wrapper_serialization)
{
    const int R = 11;
    const int C = 2;
    int k = 3;
    const auto r = static_cast<unsigned int>(R);
    auto m = testutil::makeM(R, C);
///LSHWrapper::LSHWrapper(LSHTYPE lshtype, int D, int M, int L, int S, int I) :
    LSHWrapper lsh(LSHTYPE::ITQ, R, C);
    lsh.hash(m, R, C);
    EXPECT_EQ(lsh.size(), R);

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
    auto scan1 = lsh.query(&m[pidx * C],k);
    auto scan2 = lsh2.query(&m[pidx * C],k);
    scan1.topk().genTopk();
    scan2.topk().genTopk();
    auto res1 = scan1.topk().getTopk();
    auto res2 = scan2.topk().getTopk();
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
