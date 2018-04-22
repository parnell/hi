
#include "WorkItem.hpp"

unsigned long WorkItem::nitems = 0;

long WorkItem::getId() const {
    return id;
}

int WorkItem::getParentId() const {
    return parentid;
}

int WorkItem::getJobId() const {
    return jobid;
}

std::list<WorkItem*> WorkItem::work(){
    std::cout << "       work() " << this->toString() << std::endl;
    return NO_WORK;
};

std::string WorkItem::toString() const{
    std::stringstream str;
    str << "[WorkItem " << id << ":" << parentid<<":"<<jobid << "]";
    return str.str();
};
