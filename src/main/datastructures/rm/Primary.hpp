
#ifndef HI_MASTER_HPP
#define HI_MASTER_HPP


#include <boost/mpi.hpp>
#include "JobHandler.hpp"

namespace mpi = boost::mpi;

class Primary {
    mpi::communicator world;
    JobHandler jh;
    std::list<mpi::request> sends;

    void resolveSends();
public:
    Primary();

    static void runWorker();

    void runJobs();

    void addJob(Job* pjob);
};


#endif //HI_MASTER_HPP
