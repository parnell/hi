
#ifndef HI_BUILDITEM_HPP
#define HI_BUILDITEM_HPP


#include "../../globals.hpp"
#include "../rm/WorkItem.hpp"

class HIBuildItem : public WorkItem{
    std::vector<Dat> items;
    friend class boost::serialization::access;
public:

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object< WorkItem >( *this );
        ar & items;
    }

public:
    HIBuildItem(): WorkItem(){}
    HIBuildItem(int jobid) : WorkItem(jobid){}
    HIBuildItem(int jobid, std::vector<Dat> data) : WorkItem(jobid), items(std::move(data)){}

    virtual void work(){

    }

    virtual std::string toString() const{
        std::stringstream str;
        str << "[HIBuildItem " << id << ":" << jobid << "]";
        return str.str();
    };


    friend std::ostream& operator<<(std::ostream& os, HIBuildItem const& o) {
        return os << o.toString();
    }
};


#endif //HI_BUILDITEM_HPP
