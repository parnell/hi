
#ifndef HI_LSHWRAPPER_HPP
#define HI_LSHWRAPPER_HPP
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
#include <lshbox/lsh/itqlsh.h>

#include "../../../../globals.hpp"
#include "../../../Stat.hpp"
#include <atomic>
//using namespace lshbox;
using std::string;



const unsigned int _M = 521; ///(521) Hash table size
const unsigned int _L = 5; /// Number of hash tables
const unsigned int _S = 100; /// Size of vectors in train
const unsigned int _I = 50;/// Training iterations
const unsigned int _N = 4; /// Binary code bytes, (has to be less than D)

const int _MIN_ROWS_LSH = 100;


enum LSHTYPE {
    KDBQ, /// Zhu, H. (2014) K-means based double-bit quantization for hashing.
    ITQ, /// Gong et al (2013).  Iterative quantization: A procrustean approach to learning binary codes
    DBQ, /// Gong et al (2012) Double-Bit Quantization for Hashing.
    PSD, // Datar, Indyk (2004) Locality-sensitive hashing scheme based on p-stable distributions
    RBS, /// Indyk Motwani (1999) Approximate Nearest Neighbor - Towards Removing the Curse of Dimensionality
    RHP, /// Charikar (2002) Similarity estimation techniques from rounding algorithms
    SH, /// Weiss (2008), Spectral Hashing.
    TH /// Wang et al (2007). Image Similarity Search with Compact Data Structures
};

class HIQueryResults;

struct LSHStat{
    friend std::ostream& operator<<(std::ostream&, LSHStat&);
    class tstat<float> precision;
    class tstat<float> recall;
    class tstat<float> cost;
    class tstat<size_t> ndistlsh;
    class tstat<size_t> ndistcalcs;

    std::ostream& print(const char* p, std::ostream &out);
};
template <typename T>
class LSHWrapper {
//    lshbox::kdbqLsh<Dat> kdbq;
//    lshbox::dbqLsh<Dat> dbq;
//    lshbox::psdLsh<Dat> psd;
////    lshbox::rbsLsh<Dat> rbs;
//    lshbox::shLsh<Dat> sh;
    LSHTYPE lshtype = ITQ;
    lshbox::Matrix<T>* pdata;
    typedef lshbox::Scanner<typename lshbox::Matrix<T>::Accessor> lsh_scanner;

public:
    std::vector<size_t> idxs;
    lshbox::itqLsh<T> itq;
    friend class boost::serialization::access;

    unsigned int M; ///(521) Hash table size
    unsigned int L; /// Number of hash tables
    unsigned int D; /// dimension
    unsigned int S; /// Size of vectors in train
    unsigned int I; /// Training iterations
    unsigned int N; /// Binary code bytes, (has to be less than D)
    unsigned int __T = 2; /// Index mode, you can choose 1(CAUCHY) or 2(GAUSSIAN)
    float W = 0.5; /// Window size

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< LSHWrapper >();
        ar & lshtype;
        ar & M;
        ar & L;
        ar & D;
        ar & S;
        ar & I;
        ar & N;
        ar & __T;
        ar & pdata;
        ar & itq;

    }


    LSHWrapper() : pdata(nullptr){}
    LSHWrapper(LSHTYPE lshtype, int D, int M, int L, int S, int I, int N) :
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
    LSHWrapper(LSHTYPE lshtype, const size_t R, const size_t C) :
            M(0), L(0), D(0), S(0), I(0), N(0), lshtype(lshtype), pdata(nullptr){
        assert (lshtype != RBS); /// not implemented
        assert (lshtype != TH); /// not implemented
        assert (lshtype != RHP); /// not implemented
        setDefaultParams(R, C);
//    std::cout << "LSHWrapper\tD=" << D << "\tM=" << M << "\tL=" << L << "\tS=" << S << "\tI=" << I << "\tN="<<N<< std::endl;
    }

    ~LSHWrapper() {
//    if (pdata){
//        delete pdata;
//    }
    }

    size_t size() const {
        return pdata->getSize();
    }

    void setDefaultParams(const size_t R, const size_t C) {
        const auto r = static_cast<unsigned int>(R);
        assert(R > 2);
        /** int D=2, int M=500, int L=5, int S = 100, int I = 5*/
        M = R > _MIN_ROWS_LSH ? _M : std::max(static_cast<unsigned int>(2), r / 2);
        L = _L;
        D = static_cast<unsigned int>(C);
        S = R > _MIN_ROWS_LSH ? _S : std::max(static_cast<unsigned int>(2), r / 2);
        I = _I;
        N = C > _N ? _N : std::min(static_cast<unsigned int>(C), _N);

//    dprintf("%d %d %d %d %d N=%d \n", M, L, D, S, I, N);
    }

    void load(const string& filename){
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

    void save(const string& filename){
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

    void query(lshbox::Benchmark& bench,
               lshbox::Matrix<T>& data,
               typename lshbox::Scanner<typename lshbox::Matrix<T>::Accessor>& scanner,
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
    lsh_scanner query(Euc<T>* point, const int k, std::atomic<size_t>& distcalcs, std::atomic<size_t>& ndistlsh){
        return query( &point->getCoords()[0], k, distcalcs, ndistlsh);
    }

    lsh_scanner query(T* point, const int k, std::atomic<size_t>& distcalcs, std::atomic<size_t>& ndistlsh){
        auto am = lshbox::Metric<T>(D, L2_DIST, &distcalcs, &ndistlsh );
        typename lshbox::Matrix<T>::Accessor accessor(*pdata);
        lsh_scanner scanner(accessor,am,k);
        scanner.reset(point);
        itq.query(point, scanner);
        return scanner;
    }

    void hash(){
        switch(lshtype) {
            case ITQ:
                assert (N > 0);
                typename lshbox::itqLsh<T>::Parameter itqp;
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
    void hash(T* pdata, const int R, const int C){
        this->pdata = new lshbox::Matrix<T>();
        this->pdata->transfer(pdata, R, C); /// Careful, potentially doesn't like being part of a tree and transferring

        hash();
    }

    void calculatePR(lshbox::Matrix<T>& matrix, lshbox::itqLsh<T>& lsh, int Q, int k, LSHStat& stat){
        std::atomic<size_t> distcalcs{0};
        std::atomic<size_t> ndistlsh{0};
        const size_t C = matrix.getDim();
        const size_t R = matrix.getSize();

        lshbox::Benchmark bench;
        bench.init(Q, k, R, 1);
        lshbox::Metric<T> metric(C, L2_DIST);

        /// fill in benchmark
        for (unsigned ii = 0; ii != Q; ++ii)
        {
            size_t q = bench.getQuery(ii);
            lshbox::Topk &topk = bench.getAnswer(ii);
            for (unsigned j = 0; j != R; ++j) {
                topk.push(j, metric.dist(matrix[q], matrix[j])); }
            topk.genTopk();
//            std::cout << q << " \t   query=" << i<< "\t time=" << timer2.elapsed() << std::endl;
        }
        lshbox::Stat cost, recall;

        auto am = lshbox::Metric<T>(C, L2_DIST, &distcalcs, &ndistlsh);
        typename lshbox::Matrix<T>::Accessor accessor(matrix);
        lsh_scanner scanner(accessor, am, k);
        /// scan
        for (unsigned ii = 0; ii != bench.getQ(); ++ii)
        {
            lsh.query(matrix[bench.getQuery(ii)], scanner);
            stat.recall.addStat(bench.getAnswer(ii).recall(scanner.topk()));
            stat.precision.addStat(bench.getAnswer(ii).precision(scanner.topk()));
            stat.cost.addStat(float(scanner.cnt()) / float(matrix.getSize()));
            stat.ndistcalcs.addStat(distcalcs);
            stat.ndistlsh.addStat(ndistlsh);
        }
    }



};

#endif //HI_LSHWRAPPER_HPP
