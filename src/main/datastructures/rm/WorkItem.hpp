
#ifndef HI_WORKITEM_H
#define HI_WORKITEM_H

#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <sstream>
#include <list>
#include <boost/mpi/communicator.hpp>

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
    boost::mpi::communicator world;

    static unsigned long nitems;

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & id;
        ar & parentid;
        ar & jobid;
        ar & nranks;
        ar & node;
    }

protected:
    long id;
    size_t jobid;
    int parentid;
    int nranks;
    int node = -1;

public:
    WorkItem(): id(nitems++), jobid(0), parentid(0), nranks(world.size()){}
    WorkItem(size_t jobid) : id(nitems++), jobid(jobid), parentid(0),nranks(world.size()) {}
    WorkItem(size_t jobid, int parentid, int nranks) : id(nitems++), jobid(jobid), parentid(parentid), nranks(world.size()) {}

    long getId() const;

    int getJobId() const;

    int getParentId() const;

    virtual std::list<WorkItem*> work();

    virtual std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, WorkItem const& o) {
        return os << o.toString();
    }

    int getNode() const {
        return node;
    }

    void setNode(int node) {
        this->node = node;
    }

};



#endif //HI_WORKITEM_H
