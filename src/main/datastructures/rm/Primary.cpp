
#include "Primary.hpp"
#include "Secondary.hpp"
#include "../../utils/vecutil.hpp"

#include <vector>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>
#include <boost/thread.hpp>

#include <numeric>
#include <map>
#include <list>
#include <unordered_map>

#include <queue>
#include <boost/mpi/request.hpp>
#include "../../utils/vecutil.hpp"

#include "WorkItem.hpp"
#include "../../dtypes.hpp"
#include "../../utils/Timer.hpp"
#include "Job.hpp"



void Primary::resolveSends(){
    if (!sends.empty()) {
        sends.remove_if([](mpi::request req) { return req.test(); });
    }
}
Primary::Primary() : jh(static_cast<unsigned int>(world.size())){}

void Primary::runWorker(){
    Secondary worker(0);
    worker.run();
}

void Primary::runJobs(){
    std::vector<int> workerIds(world.size());
    std::iota(workerIds.begin(), workerIds.end(), 0);
    auto nworkers = static_cast<int>(workerIds.size());

    boost::thread workerThread{runWorker};
    std::list<WorkItem*> allitems = jh.getAllItems();
    int sendto;
    while (true) {
        /// Send all of our items
        if (!allitems.empty()){
            Timer t("msend");
            std::vector<WorkItem*> v{ std::begin(allitems), std::end(allitems) };
            for (int i=0, sendto=1; i< nworkers;++i, ++sendto){
                std::vector<WorkItem *> send = vecutil::split(v, nworkers, i);
                std::cout << " >>> " << workerIds[sendto % nworkers] << " " << send.size() << " " << allitems.size() << std::endl;
                sends.push_back(world.isend(workerIds[sendto % nworkers], TagType::WORK, send));
                jh.sendingWorkTo(sendto, send.size());
            }
        }
        resolveSends();

        boost::this_thread::yield();

        std::for_each(allitems.begin(), allitems.end(), vecutil::DeleteVector<WorkItem*>());
        allitems.clear();

        /// Check for done items
        for (auto& i : workerIds) {
            while (world.iprobe(i, TagType::WORK_STATUS)) {
                size_t r;
                mpi::request req = world.irecv(i, TagType::WORK_STATUS, r);
                jh.workItemComplete(ReturnResult(i,r));
                std::cout << jh << "                 <<< Secondary " <<i << "  remaining=" << r <<
                          "  complete=" << jh.isComplete() << std::endl;
            }
        }
        /// Check for complete
        if (jh.isComplete()){
            std::cout << "====\n >>> complete " << std::endl;
            break;
        }
        boost::this_thread::yield();
    }

    std::cout << "======================  sending quit " << std::endl;
    mpi::request reqs[nworkers];
    int n=0;
    for (auto& i : workerIds) {
        reqs[n++] = world.isend(i, TagType::QUIT);
    }
    mpi::wait_all(reqs, reqs + nworkers);
    std::cout << "======================  gathering " << std::endl;
    workerThread.join();
    std::cout << "======================  quitting " << std::endl;
}



void Primary::addJob(Job* pjob) {
    jh.addJob(pjob);
}