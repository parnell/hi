
#include "HIBuildItem.hpp"
#include "../HI.hpp"
//#include <lshbox.h>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>

namespace hi {

//template<class Archive>
//void HIBuildItem::serialize(Archive &ar, const unsigned int version) {
//    ar & boost::serialization::base_object<WorkItem>(*this);
////        ar & *pdataset;
//    ar & items;
//}

std::list<WorkItem*> HIBuildItem::work() {
    std::string filename = "/Users/i855892/data/rdata/vec/gaussian/gaussian__d=14_s=10000_nclus=1_var=0.1.hdf5";
//    std::string filename = "/Users/i855892/data/rdata/vec/gaussian/gaussian__d=50_s=1000000_nclus=1_var=0.1.hdf5";

    dataManager.loadData(filename);
    HI hi;

    hi.build(&dataManager, nranks);
//    lshbox::Matrix<Dat> data;
//    data.load(dataset.ptr(), dataset.rows, dataset.cols);
//    std::cout << "built!!!" << data.getSize() << " : " << data.getDim() << std::endl;
//    std::cout << "built!!!"  << std::endl;
    return NO_WORK;
}

std::string HIBuildItem::toString() const {
    std::stringstream str;
    str << "[HIBuildItem " << id << ":" << jobid << "]";
    return str.str();
}


}