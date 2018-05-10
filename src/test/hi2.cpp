#include <iostream>
#include <boost/mpi.hpp>
#include <numeric>
#include <thread>
#include <set>
#include <list>
#include <boost/serialization/export.hpp>
#include "../main/utils/Timer.hpp"
#include "../main/utils/stringutils.hpp"
#include "../main/datastructures/min/MinJob.hpp"
#include "../main/datastructures/rm/Secondary.hpp"
#include "../main/datastructures/rm/Primary.hpp"

namespace mpi = boost::mpi;

BOOST_CLASS_EXPORT_GUID(WorkItem, "WorkItem");
BOOST_CLASS_EXPORT_GUID(MinItem, "MinItem");


//typedef std::unique_ptr<Euc<int>> Dat;

int main(int argc, char** argv) {
    unsigned int nthreads = std::thread::hardware_concurrency();
    std::cout << "Starting argc=" << argc << std::endl;
    for (int k = 0; k < argc; ++k) {
        std::cout << argv[k] << " ";
    }
    std::cout << std::endl;
    std::string filename;
    std::string queryFilename;


    mpi::environment env;
    mpi::communicator world;
    const int wsize = world.size();
    std::cout << "rank=" << world.rank() << "\tsize=" << wsize << "  nthreads=" << nthreads << std::endl;
    /// Build Job
    Timer t(sutil::sformat("%d", world.rank()));

    if (world.rank() == 0) {
        MinJob* pj = new MinJob(10000000, 4);
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
