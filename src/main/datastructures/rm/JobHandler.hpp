
#ifndef HI_JOBHANDLER_HPP
#define HI_JOBHANDLER_HPP


#include <unordered_map>
#include <map>
#include <vector>
#include "Job.hpp"
#include "WorkItem.hpp"
#include "LoadBalancer.hpp"

class JobHandler {
    LoadBalancer loadBalancer;

public:
    explicit JobHandler(unsigned int nworkers){
        workItemCompleted.resize(nworkers);
        for (int i = 0; i < nworkers; ++i){
            workItemCompleted[i] = 0;
        }
    }

    ~JobHandler(){
        for ( auto const & kv : jobs ) {
            delete kv.second;
        }
    }

    std::map<size_t,Job*> jobs;
    std::vector<size_t> workItemCompleted;

    std::list<WorkItem*> getAllItems(){
        std::list<WorkItem*> allitems;
        for ( auto const & kv : jobs ) {
            allitems.splice(allitems.end(), kv.second->getWorkItems());
        }
        return allitems;
    }

    void workItemComplete(ReturnResult result) {
        workItemCompleted[result.workerid] = result.remaining;
    }

    void sendingWorkTo(int workerid, size_t amountOfWork=1) {
        workItemCompleted[workerid] += amountOfWork;
    }

    void addJob(Job* pjob) {
        jobs[pjob->id] = pjob;
    }

    virtual std::string toString() const{
        std::stringstream str;
        str << "[JH ";
        for (unsigned int i=0;i< workItemCompleted.size(); ++i){
            str << "("<<i << ":" << workItemCompleted[i]<<")";
        }
        str << "]";
        return str.str();
    };


    friend std::ostream& operator<<(std::ostream& os, JobHandler const& o) {
        return os << o.toString();
    }

    bool isComplete() {
        for (auto& i : workItemCompleted){
            if (i > 0){
                return false; }
        }
        return true;
    }

    bool shouldRebalance() {
        return loadBalancer.shouldRebalance(workItemCompleted);
    }
};


#endif //HI_JOBHANDLER_HPP
