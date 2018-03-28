
#include "Job.hpp"

unsigned long Job::njobs = 0;

std::list<WorkItem*>& Job::getWorkItems() {
    return unfinished;
}

void Job::addWorkItem(WorkItem& workItem){
    unfinished.push_back(&workItem);
}