
#ifndef HI_METRICDATAMANAGER_HPP
#define HI_METRICDATAMANAGER_HPP


#include "DataManager.hpp"
#include "EucDataManager.hpp"

class MetricDataManager : public DataManager {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(DataManager);
    }

public:
    bool isEuclidean() override {
        return false;
    }
};
BOOST_CLASS_EXPORT_KEY(MetricDataManager);

#endif //HI_METRICDATAMANAGER_HPP
