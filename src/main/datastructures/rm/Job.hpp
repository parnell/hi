
#ifndef HI_JOB_HPP
#define HI_JOB_HPP


#include <queue>
#include <list>
#include "WorkItem.hpp"

class Job {
    static unsigned long njobs;
    friend class JobHandler;
protected:
    std::list<WorkItem*> unfinished;

public:
    const long id;

    virtual ~Job(){
        for (auto& pwi: unfinished){
            if (pwi){
                delete pwi;
            }
        }
        unfinished.clear();
    }

    Job() : id(njobs++){}

    virtual std::list<WorkItem*>& getWorkItems();

    virtual void addWorkItem(WorkItem& workItem);
    virtual void addWorkItem(WorkItem* pWorkItem);


    virtual std::string toString() const{
        std::stringstream str;
        str << "[Job " << id << "]";
        return str.str();
    };

    friend std::ostream& operator<<(std::ostream& os, Job const& o) {
        return os << o.toString();
    }

};


#endif //HI_JOB_HPP
