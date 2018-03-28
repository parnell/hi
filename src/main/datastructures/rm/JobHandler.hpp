
#ifndef HI_JOBHANDLER_HPP
#define HI_JOBHANDLER_HPP


#include <unordered_map>
#include <map>
#include "Job.hpp"

class JobHandler {
public:
    std::map<int,Job*> jobs;
    std::unordered_map<int,bool> workItemCompleted;

    std::list<WorkItem*> getAllItems(){
        std::list<WorkItem*> allitems;
        for ( auto const & kv : jobs ) {
            allitems.splice(allitems.end(), kv.second->getWorkItems());
        }
        return allitems;
    }

    void workItemComplete(int workId) {
        workItemCompleted[workId] = true;
    }

    void addJob(Job &job) {
        jobs[job.id] = &job;
        for ( auto const & i : job.getWorkItems() ) {
            workItemCompleted[i->getId()] = false;
        }
    }

    virtual std::string toString() const{
        std::stringstream str;
        str << "[JH ";
        for (auto& kv : workItemCompleted){
            str << kv.first << ":" << kv.second;
        }
        str << "]";
        return str.str();
    };


    friend std::ostream& operator<<(std::ostream& os, JobHandler const& o) {
        return os << o.toString();
    }

    bool isComplete() {
        for (auto& kv : workItemCompleted){
            if (!kv.second){
                return false; }
        }
        return true;
    }
};


#endif //HI_JOBHANDLER_HPP
