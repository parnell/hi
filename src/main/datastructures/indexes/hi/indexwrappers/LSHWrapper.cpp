
#include "LSHWrapper.hpp"
//#include <lshbox/lsh/itqlsh.h>
//#include <lshbox/lsh/rbslsh.h>
//#include <lshbox/lsh/rhplsh.h>
//#include <lshbox/lsh/thlsh.h>
//#include <lshbox/lsh/psdlsh.h>
//#include <lshbox/lsh/shlsh.h>
//#include <lshbox/lsh/dbqlsh.h>
//#include <lshbox/lsh/kdbqlsh.h>
#include "../../../../dprint.hpp"

class HITree;


std::ostream& operator<< (std::ostream& os, LSHStat& stat) {
    stat.print("", os);
    return os;
}

std::ostream& LSHStat::print(const char* p, std::ostream &out){
    std::string pr(p); pr+= "_p";
    std::string r(p); r+= "_r";
    std::string c(p); c+= "_c";
    std::string ndist(p); ndist+= "_lsh";
    std::string ncalcs(p); ncalcs+= "_calcs";
    out << "[LSHStat\tsize=" << precision.count() << "\t";
    unsigned print_flags = tstat<float>::ALL - tstat<float>::SUM - tstat<float>::COUNT - tstat<float>::VAR;
//    precision.print(pr.c_str(),out, print_flags);
//    out << "\t";
    recall.print(r.c_str(),out, print_flags);
    out <<"\t";
    cost.print(c.c_str(),out, print_flags);
    out << "]";
    ndistlsh.print(ndist.c_str(),out, print_flags);
    out <<"\t";
    ndistcalcs.print(ncalcs.c_str(),out, print_flags);
    out << "]";
    return out;
}

#if COMPILE_TESTS
#include "gtest/gtest.h"
#include "../../../../utils/testutil.hpp"
#include "../../../../utils/stringutils.hpp"
#include "../../../../dprint.hpp"
#include "../../../Stat.hpp"
#include "../../../controllers/FDNADataManager.hpp"
#include <fstream>
#include <cstdlib>

TEST(utils, LSHWrapper_bool_test)
{
    const int R = 101;
    const int C = 100;
    int Q = 10; /// benchmark size
    auto m = new bool[R * C];
    int k = 5;
    srand(0); /// keep the same random seed
    for (int r= 0; r< R; ++r){
        for (int c = 0; c < C; ++c) {
            m[r*C+c] = static_cast<bool>(rand() % 2); }
    }

    lshbox::Matrix<bool> mat;
    mat.load(m, R, C);
    lshbox::itqLsh<bool> itq;
    lshbox::itqLsh<bool>::Parameter itqp;
    itqp.M = _M; ///(521) Hash table size
    itqp.L = _L; /// Number of hash tables
    itqp.D = C;
    itqp.N = _N; /// Binary code bytes, (has to be less than D)
    assert (itqp.N <= itqp.D);
    itqp.S = _S; /// Size of vectors in train
    itqp.I = _I*2; /// Training iterations

    itq.reset(itqp);
    itq.train(mat);
    itq.hash(mat);

    std::ofstream ofs("LSHWrapper_bool_test.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << itq;
    }
    {
        lshbox::itqLsh<bool> itq2;
        std::ifstream ifs("LSHWrapper_bool_test.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> itq2;
    }
    class tstat<float> sr("recall");
    class tstat<float> sp("cost");
    for (int i = 0; i < R; ++i) {
        std::atomic<size_t> distcalcs{0};
        std::atomic<size_t> ndistlsh{0};
        auto am = lshbox::Metric<bool>(C, L2_DIST, &distcalcs, &ndistlsh);
        lshbox::Matrix<bool>::Accessor accessor(mat);
        lshbox::Scanner<lshbox::Matrix<bool>::Accessor> scanner(accessor, am, k);
        bool *qp = &m[i * C];
        scanner.reset(qp);
        itq.query(qp, scanner); /// automatically does gentopk

        auto &tk = scanner.topk().getTopk();
        EXPECT_EQ(0, tk[0].first);
    }

    LSHStat stat;
    /// check aginst scan for precision recall, just for visual verifying
//    calculatePR(mat, itq, Q, k, stat);
//    dcoutl(stat);
}

TEST(utils, LSHWrapper_matrix_serialization)
{
    const int R = 101;
    const int C = 2;
    auto m = testutil::makeM<float>(R, C);

    lshbox::Matrix<float> lsh;
    lsh.load(m, R, C);

    std::ofstream ofs("lshw_test_build.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << lsh;
    }
    lshbox::Matrix<float> lsh2;
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
    const int R = 128;
    const int C = 10;
    int k = 3;
    auto m = testutil::makeMZC<float>(R, C);

///LSHWrapper::LSHWrapper(LSHTYPE lshtype, int D, int M, int L, int S, int I) :
    LSHWrapper<float> lsh(LSHTYPE::ITQ, R, C);
    lsh.hash(m, R, C);
    EXPECT_EQ(lsh.size(), R);

    std::ofstream ofs("lshw_wrapper_serialization.idx");
    {
        boost::archive::text_oarchive oa(ofs);
        oa << lsh;
    }
    LSHWrapper<float> lsh2;
    {
        std::ifstream ifs("lshw_wrapper_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> lsh2;
    }
    EXPECT_EQ(lsh.D, lsh2.D);

    auto omegas = lsh.itq.omegasAll;
    auto omegas2 = lsh2.itq.omegasAll;
    for (size_t i = 0; i <omegas.size(); ++i) {
        for (size_t j = 0; j <omegas[i].size(); ++j) {
            EXPECT_EQ(omegas[i][j], omegas2[i][j]);
        }
    }
    auto pcsAll = lsh.itq.pcsAll;
    auto pcsAll2 = lsh2.itq.pcsAll;
    for (size_t i = 0; i <pcsAll.size(); ++i) {
        for (size_t j = 0; j <pcsAll[i].size(); ++j) {
            EXPECT_EQ(pcsAll[i][j], pcsAll2[i][j]);
        }
    }

    int pidx = 3;
    std::atomic<size_t> distcalcs{0};
    std::atomic<size_t> distcalcs2{0};
    std::atomic<size_t> distcalcslsh{0};
    std::atomic<size_t> distcalcslsh2{0};
    auto scan1 = lsh.query(&m[pidx * C],k, distcalcs,distcalcslsh);
    auto scan2 = lsh2.query(&m[pidx * C],k, distcalcs2,distcalcslsh2);
    scan1.topk().genTopk();
    scan2.topk().genTopk();
    auto res1 = scan1.topk().getTopk();
    auto res2 = scan2.topk().getTopk();
    auto it1 = res1.begin();
    auto it2 = res2.begin();
    EXPECT_EQ((size_t) distcalcs, (size_t) distcalcs2);
//    dprintf("  # distcalcs =%zu   # distcalcs2=%zu\n", (size_t)distcalcs, (size_t)distcalcs2);
    for (; it1 != res1.end(); ++it1, ++it2 ){
//        std::cout << it1->second << ": " << it1->first << "\t" <<
//                  it2->second << ": " << it2->first <<std::endl;
        EXPECT_EQ(it1->second, it2->second);
        EXPECT_EQ(it1->first, it2->first);
    }
}

TEST(hi, LSHWrapper_toeuc_test)
{
    int k = 3;
    std::string filename = sutil::sformat("%s/../data/tests/1028.dna", CMAKE_CURRENT_BINARY_DIR);
    FDNADataManager *pdat = FDNADataManager::loadData(filename, 0, 0);
    EucDataManager<bool>* peuc = pdat->toEuclidean<bool>();
    const size_t R = peuc->getRows();
    const size_t C = peuc->getCols();
    EXPECT_EQ(C, 5*4);
    rm::M<bool>* pm = new rm::M<bool>();
    pm->load(peuc->getDat(), R, C);
//    dprintca(peuc->getDat(), R, C);

    LSHWrapper<bool> lsh(LSHTYPE::ITQ, R, C);
    std::atomic<size_t> distcalcs{0};
    std::atomic<size_t> distcalcslsh{0};
    for (size_t j = 0; j < 2; ++j) {
        bool* qp = new bool[C];
//        dprintca(qp, 1, C);
        peuc->m.load(pm->getData(), R,C);
        lsh.hash(peuc->getDat(), R, C);

        std::copy(peuc->getDat(), peuc->getDat() + C, qp);
        auto& tk = lsh.query(qp, k, distcalcs, distcalcslsh).topk().getTopk();
        EXPECT_FLOAT_EQ(0, tk[0].first);
    }

//    dprintca(peuc->getDat(), R, C);

}

#endif
