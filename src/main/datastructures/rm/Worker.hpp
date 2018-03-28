
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
public:
    Worker() {
        quit = false;
    }


    ~Worker() {
    }

    void dowork(){
        if (!queue.empty()){ /// Do work
            WorkItem* pwi = queue.front();
            queue.pop_front();
            pwi->work();
//            std::this_thread::sleep_for(std::chrono::microseconds(1000000));
            std::cout << "    Slave " << world.rank() << "  isend (" << pwi->getId() << " ) " << std::endl;
            sends.push_back(world.isend(0, TagType::WORK_STATUS, pwi->getId()));
        }
        sends.remove_if([](mpi::request req){ return req.test(); });
    }

    void checkmessages() {
//        std::cout << "    Slave " << world.rank() << "  checking work" << std::endl;
        boost::optional<mpi::status> status;

        while ((status = world.iprobe(0, TagType::WORK))) {
            WorkItem* pwi;
//            std::cout << "    Slave " << world.rank() << "  waiting data () " << std::endl;
            world.recv(0, TagType::WORK, pwi);
            std::cout << "    Slave " << world.rank() << "  found (" << *pwi << " ) " << std::endl;
            queue.push_back(pwi);
        }
    }

    bool checkquit() {
//        std::cout << "    Slave " << world.rank() << "  check quit" << std::endl;
        boost::optional<mpi::status> status = world.iprobe(0, TagType::QUIT);
        if (status) {
            int v;
            world.recv(0, TagType::QUIT, v);
            std::cout << "    Slave " << world.rank() << "  found QUIT  " << std::endl;
            return true;
        }
        return false;
    }

    void run() {
        /// take everything from the queue and do the work
        while (!quit) {
            checkmessages(); /// check for more workitems

            dowork(); /// check for work

            if (checkquit()){ /// check for quit
                break;}

            std::this_thread::yield();
        }
    }

};


#endif //HI_WORKER_H
