
#include "SpatialWrapper.hpp"
#include "../../controllers/Partitioner.hpp"

namespace hi {


std::vector<DataManager*> SpatialWrapper::build(const HIBuildParams &params, DataManager *pdata, int depth) {

//    flann::Matrix<Dat> dataset(pdata->getDat(), pdata->getRows(), pdata->getCols());
//    pkd = new flann::KDTreeIndex<flann::L2<Dat>>(dataset, flann::KDTreeIndexParams(1));
//    pkd->buildIndex();
//    Partitioner p;
//    auto res = std::vector<DataManager*>(0);
//    return res;
//    return p.partition(params, pdata, depth);
}

//SpatialWrapper::SpatialWrapper() : pkd(nullptr) {
//
//}

}