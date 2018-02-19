
#ifndef HI_WORKITEM_H
#define HI_WORKITEM_H

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>

class WorkItem {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & name;
    }
    std::string name;

    friend std::ostream& operator<<(std::ostream& os, WorkItem const& o) {
        return os << "name=" << o.name << std::endl;
    }
public:
};


#endif //HI_WORKITEM_H
