
#ifndef HI_LSHWRAPPER_HPP
#define HI_LSHWRAPPER_HPP
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
#include <lshbox/lsh/itqlsh.h>

#include "../../../../globals.hpp"
//using namespace lshbox;
using std::string;

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

class LSHWrapper {
//    lshbox::kdbqLsh<Dat> kdbq;
//    lshbox::dbqLsh<Dat> dbq;
//    lshbox::psdLsh<Dat> psd;
////    lshbox::rbsLsh<Dat> rbs;
//    lshbox::shLsh<Dat> sh;
    LSHTYPE lshtype = ITQ;
    lshbox::Matrix<Dat> data;
    lshbox::itqLsh<Dat> itq;
public:
    friend class boost::serialization::access;

    unsigned int M; ///(521) Hash table size
    unsigned int L; /// Number of hash tables
    unsigned int D; /// dimension
    unsigned int S; /// Size of vectors in train
    unsigned int I; /// Training iterations
    unsigned int N; /// Binary code bytes, (has to be less than D)
    unsigned int T = 2; /// Index mode, you can choose 1(CAUCHY) or 2(GAUSSIAN)
    float W = 0.5; /// Window size

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & lshtype;
        ar & M;
        ar & L;
        ar & D;
        ar & S;
        ar & I;
        ar & N;
        ar & T;
        ar & data;
        ar & itq;

    }

    LSHWrapper(LSHTYPE lshtype, int D=2, int M=500, int L=5, int S = 100, int I = 5, int N=4);
    LSHWrapper() = default;

    void load(const string& filename);

    void save(const string& filename);

    void query(lshbox::Benchmark& bench,
               lshbox::Matrix<float>& data,
               lshbox::Scanner<lshbox::Matrix<float>::Accessor>& scanner,
               unsigned int index);
    lshbox::Scanner<lshbox::Matrix<Dat>::Accessor> query(Dat* point, const int k);


    void hash();
    void hash(Dat* pdata, const int R, const int C);

};

#endif //HI_LSHWRAPPER_HPP