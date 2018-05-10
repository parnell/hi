
#ifndef HI_DATAMANAGER_HPP
#define HI_DATAMANAGER_HPP


#include <string>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
//#include <lshbox.h>
#include "../../globals.hpp"
#include "../rm/M.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>



class DataManager {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & m;
    }
    bool deleteData;
public:
    rm::M<Dat> m;

public:
    ~DataManager();

    DataManager();

    DataManager(Dat* pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData);

    Dat *getDat() const;

    size_t getCols() const;

    size_t getRows() const;

    lshbox::Matrix<Dat> getLSHMatrix();

    inline Dat get(size_t r, size_t c);
    inline Dat* getRow(size_t r) {return m[r];}

    void print() const;

    void print_rowp() const;

    void sort(Dat *pivot);
public:

    Dat* loadData(std::string filename);


    DataManager *sliceData(size_t _begin, size_t _end);
};


#endif //HI_DATAMANAGER_HPP
