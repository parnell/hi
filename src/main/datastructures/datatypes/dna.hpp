
#ifndef HI_DNA_H
#define HI_DNA_H

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/array.hpp>

#include "../data.hpp"
#include "../../globals.hpp"

//#define HAMMING_DIST
#define NW_DIST

#define kMismatch	1
#define kGap		2

class DNA : public Data {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< DNA >();
        ar & boost::serialization::base_object<Data>(*this);

        ar & seq_len_;
        if (Archive::is_loading::value) {
            seq_ = new char[seq_len_];
        }
        ar & boost::serialization::make_array(seq_,seq_len_);
    }

public:
    DNA() :Data(){};
    DNA(const std::string& sequence, const size_t& s);
    virtual ~DNA();

    virtual std::string toString() const;

    virtual size_t size() const { return seq_len_; }

    virtual float d(const Data* other) const;

    static size_t LoadData(const char* file, std::vector<Data*>* data_arr);

    static dist_type dist(const DNA *v1, const DNA *v2, const size_t C){
        return 0;
    }

    char* getSeq() const { return seq_;}
protected:
    char* seq_;
    unsigned int seq_len_;

    dist_type hammingDist(const DNA* other) const;
    dist_type nwDist(const DNA* other) const;

    void ensureSize(const DNA* self, const DNA* other);
};


BOOST_CLASS_EXPORT_KEY(DNA);



#endif //HI_DNA_H
