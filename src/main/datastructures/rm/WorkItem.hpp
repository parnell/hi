
#ifndef HI_WORKITEM_H
#define HI_WORKITEM_H

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <sstream>
#include <list>

class WorkItem;
static const std::list<WorkItem*> NO_WORK(0);

class ReturnResult{

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & workerid;
        ar & remaining;
    }
public:
    int workerid;
    size_t remaining;

    ReturnResult() : workerid(-1), remaining(0){}
    ReturnResult(int id, size_t remaining) : workerid(id), remaining(remaining){}

};

class WorkItem {
    friend class boost::serialization::access;
    static unsigned long nitems;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & parentid;
        ar & jobid;
    }

protected:
    long id;
    int jobid;
    int parentid;

public:
    WorkItem(): id(nitems++), jobid(-1), parentid(0){}
    WorkItem(int jobid) : id(nitems++), jobid(jobid), parentid(0) {}
    WorkItem(int jobid, int parentid) : id(nitems++), jobid(jobid), parentid(parentid) {}

    long getId() const;

    int getJobId() const;

    int getParentId() const;

    virtual std::list<WorkItem*> work();

    virtual std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, WorkItem const& o) {
        return os << o.toString();
    }
};



#endif //HI_WORKITEM_H
