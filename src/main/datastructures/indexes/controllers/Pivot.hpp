
#ifndef HI_PIVOTRESULT_HPP
#define HI_PIVOTRESULT_HPP

#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "../../Stat.hpp"
#include "../../../globals.hpp"
#include "../../../dprint.hpp"

struct Tup{
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< Tup >();
        ar & oindex;
        ar & distance;
        ar & sort_index;
    }

    size_t oindex; /// original index (from point load)
    dist_type distance;
    size_t sort_index;
    Tup() = default;
    Tup(size_t oindex, dist_type distance, size_t sort_index):
            oindex(oindex), distance(distance), sort_index(sort_index){}
};

struct Pivot{
    Pivot();
    Pivot(Data* _ppivot, size_t index, size_t R);

    ~Pivot(){
        if (ppivot)
            delete ppivot;
        if (pdistances)
            delete pdistances;
    }
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< Pivot >();
        ar & ppivot;
        ar & pdistances;
        ar & index;
        ar & maxl_idx;
        ar & minr_idx;
//        ar & stat; /// Currently not saving stat information since it's usually good for building only
    }

    Data* ppivot;
    std::vector<Tup>* pdistances;
    Stat stat;
    size_t index;
    size_t maxl_idx; /// max_index of left
    size_t minr_idx; /// min_index of right

    dist_type getMaxLeft() const;
    dist_type getMinRight() const;

};


#endif //HI_PIVOTRESULT_HPP
