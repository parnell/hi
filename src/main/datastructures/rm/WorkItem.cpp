
#include "WorkItem.hpp"

unsigned long WorkItem::nitems = 0;

long WorkItem::getId() const {
    return id;
}

int WorkItem::getJobid() const {
    return jobid;
}
