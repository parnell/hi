
#ifndef SECONDARY_WORKER_H
#define SECONDARY_WORKER_H


#include <boost/mpi.hpp>
#include <list>
#include "WorkItem.hpp"

namespace mpi = boost::mpi;

class Secondary {
    mpi::communicator world;

    volatile bool quit;
    std::list<WorkItem*> queue;
    std::list<mpi::request> sends;
    int rank;
    void resolveSends();
public:
    explicit Secondary(int rank);

    void dowork();

    void recvMessages();

    bool checkQuit();

    void run();
};


#endif //HI_WORKER_H
