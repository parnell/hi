
#ifndef HI_DATAMANAGER_HPP
#define HI_DATAMANAGER_HPP


#include <string>
#include <flann/io/hdf5.h>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
#include <lshbox/lsh/itqlsh.h>
//#include <lshbox.h>
#include "../globals.hpp"
#include "rm/M.hpp"


class DataManager {
public:
    rm::M<Dat> m;

public:
    Dat *getDat() const;

    size_t getCols() const;

    size_t getRows() const;

    lshbox::Matrix<Dat> getLSHMatrix();
public:

    Dat* loadData(std::string filename);

    void calculateVariance();

    void calculateVariance(std::vector<Dat *> points);

};


#endif //HI_DATAMANAGER_HPP
