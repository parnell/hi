
#include "HIBuildItem.hpp"
//#include <lshbox.h>


namespace hi {

//template<class Archive>
//void HIBuildItem::serialize(Archive &ar, const unsigned int version) {
//    ar & boost::serialization::base_object<WorkItem>(*this);
////        ar & *pdataset;
//    ar & items;
//}

std::list<WorkItem*> HIBuildItem::work() {
//        pindex = new flann::HI<flann::L2<Dat>>(*pdataset, flann::HIIndexParams(1));
//    pindex = new flann::KDTreeIndex<flann::L2<float>>(*pdataset, flann::KDTreeIndexParams(1));
    // /usr/local/include/flann/ -> /usr/local/Cellar/flann/1.8.4_3/include/flann
    std::string filename = "/Users/i855892/data/rdata/vec/gaussian/gaussian__d=14_s=10000_nclus=1_var=0.1.hdf5";
    std::string indexName = "data";
    flann::Matrix<Dat> dataset;
//        pindex->buildIndex();
    load_from_file(dataset, filename, indexName);


//        lshbox::Matrix<Dat> data(filename);
    lshbox::Matrix<Dat> data;
    data.load(dataset.ptr(), dataset.rows, dataset.cols);
    std::cout << "built!!!" << data.getSize() << " : " << data.getDim() << std::endl;
//    std::cout << "built!!!"  << std::endl;
    return NO_WORK;
}

}