
#ifndef HI_SPATIAL_HPP
#define HI_SPATIAL_HPP


//#include <flann/flann.h>
#include "../../../../globals.hpp"
#include "../HITree.hpp"
#include "../params.hpp"
#include "../../../controllers/DataManager.hpp"
namespace hi {

class SpatialWrapper {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
//        ar & pkd;
    }

//    flann::KDTreeIndex<flann::L2<Dat>> *pkd;
public:
//    SpatialWrapper();
    std::vector<DataManager*> build(const HIBuildParams &params, DataManager *pManager, int depth);
};

}

#endif //HI_SPATIAL_HPP
