
#ifndef HI_DATA_H
#define HI_DATA_H

#include <string>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/export.hpp>

#include "../dtypes.hpp"


class Data {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< Data >();
//        ar & serial_number_;
//        ar & dist_;
    }

protected:
//    size_t serial_number_;
//    dtype dist_;

public:
    Data() = default;
//    Data(size_t s) : serial_number_(s) {}
    virtual ~Data() = default;
//    size_t getSerialNumber() const {return serial_number_; }

    virtual std::string toString() const {return std::string("Data");};

//    virtual void setDist(dtype t) { dist_ = t; }
//
//    virtual dtype getDist() const { return dist_; }

    virtual size_t size() const {return 0;}

    virtual float d(const Data* other) const  {return 0;};

//    virtual bool operator ==(const Data& other) const {
//        return serial_number_ == other.serial_number_;
//    }
//
//    virtual bool operator <(const Data& other) const {
//        return dist_ < other.dist_;
//    }
//    virtual bool operator <(const Data* other) const {
//        return dist_ < other->dist_;
//    }
};

//BOOST_SERIALIZATION_ASSUME_ABSTRACT( Data );
BOOST_CLASS_EXPORT_KEY(Data);

#endif //HI_DATA_H
