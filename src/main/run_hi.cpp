#include <iostream>
#include <boost/thread.hpp>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>  /// for gatherv
#include <numeric>
#include <boost/serialization/export.hpp>

#include "datastructures/rm/WorkItem.hpp"
#include "datastructures/rm/Secondary.hpp"
#include "datastructures/data.hpp"
#include "datastructures/datatypes/Euc.hpp"
#include "datastructures/rm/Primary.hpp"

#include "dtypes.hpp"
#include "globals.hpp"


#include "datastructures/min/MinJob.hpp"
#include "utils/Timer.hpp"
#include "utils/vecutil.hpp"
#include "utils/stringutils.hpp"
#include "datastructures/indexes/hi/job/HIBuildItem.hpp"
#include "datastructures/indexes/hi/job/HIJob.hpp"


namespace mpi = boost::mpi;

BOOST_CLASS_EXPORT_GUID(WorkItem, "WorkItem");
BOOST_CLASS_EXPORT_GUID(hi::HIBuildItem, "HIBuildItem");
BOOST_CLASS_EXPORT_GUID(MinItem, "MinItem");
BOOST_CLASS_EXPORT_GUID(ReturnResult, "ReturnResult");


int main(int argc, char** argv) {
    unsigned int nthreads = boost::thread::hardware_concurrency();

    std::cout << "Starting argc=" << argc << std::endl;
    for (int k = 0; k < argc; ++k) {
        std::cout << argv[k] << " ";
    }
    std::cout << std::endl;
    std::string filename;
    std::string queryFilename;

    float radius = 0.4;
    std::string indexName = "data";
    int c;
    int knn = -1;
    bool build = true;
    bool query = true;

    int dims=-1;
    int nclusters = -1;
    int size = -1;

    while((c =  getopt(argc, const_cast<char * const * >(argv), "bk:y:c:s:d:r:i:q:n:")) != EOF){
        switch (c){
            case 'b': build=true; break;
            case 'k': knn=atoi(optarg); break;
            case 'y': {
//                int t = atoi(optarg);
//                if (t < 0 || t > IndexType::LAST_TYPE) {
//                    fprintf(stderr, "Index Type %d is not defined\n", t);
//                }
//                index_type = static_cast<IndexType>(t);
            }
                break;
            case 'i': filename = optarg; break;
            case 'q': queryFilename = optarg; break;
            case 'n': indexName= optarg; break;
            case 'c': nclusters=atoi(optarg); break;
            case 's': size=atoi(optarg); break;
            case 'd': dims=atoi(optarg); break;
            case 'r': radius = (float) atof(optarg); break;
            case ':': /// No operand
                fprintf(stderr,"Option -%c requires an operand\n", optopt);
                return -1;
            default: break;
        }
    }
    if (build){
        /// Verify build options
    }

    if (query){
        /// Verify query options
    }
    mpi::environment env(argc, argv, mpi::threading::multiple);
    mpi::communicator world;
    const int wsize = world.size();
    std::cout << "rank=" << world.rank() << "\tsize=" << wsize << "  nthreads=" << nthreads << std::endl;
    /// Build Job
//    std::string n = sutil::sformat("%d", world.rank());
    Timer t(sutil::sformat("%d", world.rank()));

    if (world.rank() == 0) {
        auto pj = new hi::HIJob();
//        MinJob j(10000000, 4);

        if (build && query) {
            auto bi = new hi::HIBuildItem(pj->id);
            pj->addWorkItem(*bi);
        } else if (build){
//            m.build();
        } else if (query){
//            m.query();
        }
        /// create Primary
        Primary m;
        m.addJob(pj);
        m.runJobs();
    } else {
        /// Create Secondary
        Secondary m(world.rank());
        m.run();
    }

    return 0;
}
