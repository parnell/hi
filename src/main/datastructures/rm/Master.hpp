
#ifndef HI_MASTER_HPP
#define HI_MASTER_HPP


#include <vector>
#include <iostream>
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <numeric>

#include <queue>
#include "../../utils/vecutil.hpp"
#include "../../dtypes.hpp"
#include "../hi/HIBuildItem.hpp"
#include "Job.hpp"
#include "JobHandler.hpp"
#include "Worker.hpp"
#include "../Timer.hpp"
#include "../../utils/stringutils.hpp"

#include <chrono> // sleeping
#include <thread> // sleeping
#include <map>
#include <list>
#include <unordered_map>

namespace mpi = boost::mpi;


template <typename T>
class Master {
    std::vector<T>* pdata;
    mpi::communicator world;

    JobHandler jh;
    std::mutex mtx;

public:
    Master(){
        this->pdata = NULL;
    }

    Master(std::vector<T>* pdata) : Master(), pdata(pdata){

    }
    static void runWorker(){
        Worker worker;
        worker.run();
    }

    void runjobs(){
        std::vector<size_t> workerIds(world.size());
        std::iota(workerIds.begin(), workerIds.end(), 0);
        const size_t nworkers = workerIds.size();

//        std::unique_lock<std::mutex> lck(mtx);
        std::thread workerThread = std::thread(runWorker);
        std::list<WorkItem*> allitems = jh.getAllItems();
        std::list<mpi::request> sends;
        int sendto = 0;
        long completedWorkItem;
        while (true) {
//            std::cout << "Master LOOPING " << jh <<  std::endl;
            if (!allitems.empty()){
                Timer t("msend");
                for (auto iter = allitems.begin(); iter != allitems.end(); ++iter){
                    ++sendto;
                    std::cout << " >>> " << workerIds[sendto % nworkers] << " " << **iter << " " << allitems.size() << std::endl;
                    sends.push_back(world.isend(workerIds[sendto % nworkers], TagType::WORK, *iter));
                }
            }
            std::this_thread::yield();
            for_each(allitems.begin(), allitems.end(), vecutil::DeleteVector<WorkItem*>());
            allitems.clear();
            sends.remove_if([](mpi::request req){ return req.test(); });

            /// Check for done items
            for (auto& i : workerIds) {
                while (boost::optional<mpi::status> status = world.iprobe(i, TagType::WORK_STATUS)) {
                    std::cout << "               <<< Slave " << i << "  ircv complete=" << std::endl;

                    mpi::request req = world.irecv(i, TagType::WORK_STATUS, completedWorkItem);
                    std::cout << "                 <<< Slave " << i << "  done with workitem " << completedWorkItem <<
                              "  complete=" << jh.isComplete() << std::endl;
                    std::cout << "               !!!!!!!!!!!!!<<< Slave " << i << "  done with workitem " << completedWorkItem <<
                              "  complete=" << jh.isComplete() << std::endl;
                    jh.workItemComplete(completedWorkItem);
                }
            }

            if (jh.isComplete()){
                std::cout << "====\n >>> complete " << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::this_thread::yield();
        }

        std::cout << "======================  sending quit " << std::endl;
        Timer t("quittime");
        std::vector< std::vector<T> > all_numbers;
        mpi::request reqs[nworkers];
        int n=0;
        for (auto& i : workerIds) {
            reqs[n++] = world.isend(i, TagType::QUIT, 1);
        }
        mpi::wait_all(reqs, reqs + nworkers);
        std::cout << "======================  gathering " << std::endl;
        workerThread.join();
        std::cout << "======================  quitting " << std::endl;
    }



    void addJob(Job& job) {
        jh.addJob(job);
    }
};


#endif //HI_MASTER_HPP
