
#ifndef HI_INDEX_HPP
#define HI_INDEX_HPP

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/access.hpp>

namespace plib {

class Index {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
    }
public:
    Index() = default;

};

}


#endif //HI_INDEX_HPP
