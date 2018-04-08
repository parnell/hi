
#ifndef HI_WORKITEM_H
#define HI_WORKITEM_H

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <sstream>

class WorkItem {
    friend class boost::serialization::access;
    static unsigned long nitems;

protected:
    long id;
public:
    long getId() const;

    int getJobid() const;

protected:
    int jobid;

public:
    WorkItem(): id(nitems++), jobid(-1){
    }

    WorkItem(int jobid) : id(nitems++), jobid(jobid) {
    }

    virtual void work(){
        std::cout << "       work() " << this->toString() << std::endl;
    };

    virtual std::string toString() const{
        std::stringstream str;
        str << "[WorkItem " << id << ":" << jobid << "]";
        return str.str();
    };


    friend std::ostream& operator<<(std::ostream& os, WorkItem const& o) {
        return os << o.toString();
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & jobid;
    }

};


#endif //HI_WORKITEM_H
