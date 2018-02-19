
#ifndef HI_DATA_H
#define HI_DATA_H

#include <string>
#include <vector>

#include "../global_includes.h"

class Data {
protected:
    size_t serial_number_;
    dist_type dist_;

public:
    Data(size_t s) : serial_number_(s) {}
    size_t getSerialNumber() const {return serial_number_; }

    virtual std::string toString() const = 0;

    void setDist(dist_type t) { dist_ = t; }
    dist_type getDist() const { return dist_; }

    virtual size_t size() const = 0;

    virtual dist_type d(const Data* other) const = 0;

    virtual bool operator ==(const Data& other) const {
        if (serial_number_ == other.serial_number_)
            return true;
        return false;
    }

    virtual bool operator <(const Data& other) const {
        return dist_ < other.dist_;
    }
    virtual bool operator <(const Data* other) const {
        return dist_ < other->dist_;
    }
};

#endif //HI_DATA_H
