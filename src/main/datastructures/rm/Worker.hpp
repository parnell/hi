
#ifndef HI_WORKER_H
#define HI_WORKER_H

#include <vector>
#include <boost/mpi.hpp>
#include <chrono>
#include <thread>
#include <list>
#include "../../globals.hpp"
#include "../rm/WorkItem.hpp"

namespace mpi = boost::mpi;

class Worker {
    mpi::communicator world;

    volatile bool quit;
    std::list<WorkItem*> queue;
    std::list<mpi::request> sends;
    int rank;
    void resolveSends(bool force=false){
        if (!sends.empty()){
            sends.remove_if([](mpi::request req){ return req.test(); });
//            std::this_thread::sleep_for(std::chrono::milliseconds(10));
//            std::cout << "   ### Slave " << world.rank() << "  resolving. remaining=" << sends.size() << " " << std::endl;
        }
    }
public:
    Worker(int rank) : rank(rank) {
        quit = false;
    }


    ~Worker() {
    }

    void dowork(){
        if (!queue.empty()){ /// Do work
            WorkItem* pwi = queue.front();
            queue.pop_front();
            std::list<WorkItem*> l = pwi->work();
            for (auto& i : l){
                queue.push_back(i);
            }
//            queue.insert(l.begin(), l.end());
            ReturnResult rr = ReturnResult(rank, queue.size());
            std::cout << "    Slave " << rank << "  workleft=" << queue.size()
                      << "  isend (" << pwi->getId() << " ) " << rr.workerid << " : " << rr.remaining <<std::endl;

            sends.push_back(world.isend(0, TagType::WORK_STATUS, rr.remaining));
            checkQuit(); /// make sure we respond to quits
        }
        resolveSends();
    }

    void checkMessages() {
        while (world.iprobe(0, TagType::WORK)) {
            WorkItem* pwi;
            world.recv(0, TagType::WORK, pwi);
            std::cout << "    Slave " << world.rank() << "  found (" << *pwi << " ) " << std::endl;
            queue.push_back(pwi);
        }
    }

    inline bool checkQuit() {
        if (world.iprobe(0, TagType::QUIT)) {
            std::cout << "    Slave " << world.rank() << "  found QUIT  " << std::endl;
            world.recv(0, TagType::QUIT);
            quit = true;
            return true;
        }
        return false;
    }

    void run() {
        /// take everything from the queue and do the work
        while (!quit) {
            checkMessages(); /// check for more workitems

            dowork(); /// check for work

            if (checkQuit()){ /// check for quit
                resolveSends();
                break;}
            /// for some reason going through this loop too fast causes mpi boost to crash using iprobe
            /// Yield is insufficient
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            std::this_thread::yield();
        }
    }

};


#endif //HI_WORKER_H
