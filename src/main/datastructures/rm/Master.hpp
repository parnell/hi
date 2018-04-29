
#ifndef HI_MASTER_HPP
#define HI_MASTER_HPP


#include <vector>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/export.hpp>
#include <numeric>
#include <chrono> // sleeping
#include <thread> // sleeping
#include <map>
#include <list>
#include <unordered_map>

#include <queue>
#include "../../utils/vecutil.hpp"

#include "WorkItem.hpp"
#include "../../dtypes.hpp"
#include "../Timer.hpp"
#include "Job.hpp"
#include "JobHandler.hpp"
#include "Worker.hpp"


namespace mpi = boost::mpi;


template <typename T>
class Master {
    std::vector<T>* pdata;
    mpi::communicator world;

    JobHandler jh;
//    std::mutex mtx;
    std::list<mpi::request> sends;
    std::list<mpi::request> recvs;
    void resolveSends(){
        if (!sends.empty()) {
            sends.remove_if([](mpi::request req) { return req.test(); });
//            std::cout << "   @@@@ Master " << world.rank() << "  resolving sends (" << sends.size() << " ) " << std::endl;
        }
    }
public:
    Master() : jh(world.size()){
        this->pdata = NULL;
    }

    Master(std::vector<T>* pdata) : Master(), pdata(pdata){

    }
    static void runWorker(){
        Worker worker(0);
        worker.run();
    }

    void runjobs(){
        std::vector<int> workerIds(world.size());
        std::iota(workerIds.begin(), workerIds.end(), 0);
        const int nworkers = static_cast<int>(workerIds.size());

//        std::unique_lock<std::mutex> lck(mtx);
        std::thread workerThread = std::thread(runWorker);
        std::list<WorkItem*> allitems = jh.getAllItems();
        Timer marker("marker");
        int sendto;
        while (true) {
//            std::cout << "Master LOOPING " << jh <<  std::endl;
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

            std::this_thread::yield();
            std::for_each(allitems.begin(), allitems.end(), vecutil::DeleteVector<WorkItem*>());
            allitems.clear();

            /// Check for Rebalancing workloads
            if (marker.ellapsed_and_mark(std::chrono::nanoseconds(REBALANCE_TIME_NS)) && jh.shouldRebalance()){
                std::cout << " >>> ---------- REBALANCING ---------- " << std::endl;
            }

            /// Check for done items
            for (auto& i : workerIds) {
                while (world.iprobe(i, TagType::WORK_STATUS)) {
//                    std::cout << "               <<< Slave " << i << "  ircv complete=" << std::endl;
                    int r;
                    mpi::request req = world.irecv(i, TagType::WORK_STATUS, r);
                    jh.workItemComplete(ReturnResult(i,r));
                    std::cout << jh << "                 <<< Slave " <<i << "  remaining=" << r <<
                              "  complete=" << jh.isComplete() << std::endl;
//                    std::cout << "               !!!!!!!!!!!!!<<< Slave " << i << "  done with workitem " << completedWorkItem <<
//                              "  complete=" << jh.isComplete() << std::endl;
                }
            }

            if (jh.isComplete()){
                std::cout << "====\n >>> complete " << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::this_thread::yield();
            resolveSends();
        }

        std::cout << "======================  sending quit " << std::endl;
        Timer t("quittime");
        std::vector< std::vector<T> > all_numbers;
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



    void addJob(Job* pjob) {
        jh.addJob(pjob);
    }
};


#endif //HI_MASTER_HPP
