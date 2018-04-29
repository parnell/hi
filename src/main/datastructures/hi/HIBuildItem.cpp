
#include "HIBuildItem.hpp"
#include "HI.hpp"
#include <lshbox.h>


namespace hi {

//template<class Archive>
//void HIBuildItem::serialize(Archive &ar, const unsigned int version) {
//    ar & boost::serialization::base_object<WorkItem>(*this);
////        ar & *pdataset;
//    ar & items;
//}

std::list<WorkItem*> HIBuildItem::work() {
    std::string filename = "/Users/i855892/data/rdata/vec/gaussian/gaussian__d=14_s=10000_nclus=1_var=0.1.hdf5";
    dataManager.loadData(filename);
    HI hi;
    hi.build(&dataManager);
//    lshbox::Matrix<Dat> data;
//    data.load(dataset.ptr(), dataset.rows, dataset.cols);
//    std::cout << "built!!!" << data.getSize() << " : " << data.getDim() << std::endl;
//    std::cout << "built!!!"  << std::endl;
    return NO_WORK;
}

}