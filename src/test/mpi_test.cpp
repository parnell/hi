#include <iostream>
#include <boost/thread.hpp>
#include <boost/mpi.hpp>
#include <boost/serialization/export.hpp>
#include <numeric>
#include <set>
#include <list>
#include "../main/utils/Timer.hpp"
#include "../main/utils/stringutils.hpp"
#include "../main/datastructures/min/TestJob.hpp"
#include "../main/datastructures/rm/Secondary.hpp"
#include "../main/datastructures/rm/Primary.hpp"

namespace mpi = boost::mpi;

BOOST_CLASS_EXPORT_GUID(WorkItem, "WorkItem");
BOOST_CLASS_EXPORT_GUID(TestWorkItem, "TestWorkItem");

//typedef std::unique_ptr<Euc<int>> Dat;

int main(int argc, char** argv) {

    unsigned int nthreads = boost::thread::hardware_concurrency();
    std::cout << "mpi_test Starting argc=" << argc << std::endl;
    for (int k = 0; k < argc; ++k) {
        std::cout << argv[k] << " ";
    }
    mpi::environment env(argc, argv, mpi::threading::multiple);
    mpi::communicator world;
    const int wsize = world.size();
    std::cout << "rank=" << world.rank() << "\tsize=" << wsize << "  nthreads=" << nthreads << std::endl;

    /// Build Job
    Timer t(sutil::sformat("%d", world.rank()));

    if (world.rank() == 0) {
        TestJob* pj = new TestJob(100000, 4);
        /// create master
        Primary m;
        m.addJob(pj);
        m.runJobs();
    } else {
        /// Create worker
        Secondary m(world.rank());
        m.run();
    }
    return 0;
}
