
#include <boost/thread/pthread/thread_data.hpp>
#include "Secondary.hpp"
#include "../../dtypes.hpp"


void Secondary::resolveSends(){
    if (!sends.empty()){
        sends.remove_if([](mpi::request req){ return req.test(); });
    }
}

Secondary::Secondary(int rank) : rank(rank), quit(false) {}

void Secondary::dowork(){
    /// take everything from the queue and do the work
    if (!queue.empty()){ /// Do work
        WorkItem* pwi = queue.front();
        queue.pop_front();
        std::list<WorkItem*> l = pwi->work();
        for (auto& i : l){
            queue.push_back(i);
        }
        ReturnResult rr = ReturnResult(rank, queue.size());
        std::cout << "    Secondary " << rank << "  workleft=" << queue.size()
                  << "  isend (" << pwi->getId() << " ) " << rr.workerid << " : " << rr.remaining <<std::endl;

        sends.push_back(world.isend(0, TagType::WORK_STATUS, rr.remaining));

        checkQuit(); /// make sure we respond to quits
        delete pwi;
    }
    resolveSends();
}

void Secondary::recvMessages() {
    while (world.iprobe(0, TagType::WORK)) {
        std::vector<WorkItem*> pwi;
        world.recv(0, TagType::WORK, pwi);
        std::cout << "    Secondary " << world.rank() << "  found (" << pwi.size() << " ) " << std::endl;
        for (auto& i : pwi){
            queue.push_back(i);
        }
    }
}

bool Secondary::checkQuit() {
    if (world.iprobe(0, TagType::QUIT)) {
        std::cout << "    Secondary " << world.rank() << "  found QUIT  " << std::endl;
        world.recv(0, TagType::QUIT);
        quit = true;
        return true;
    }
    return false;
}

void Secondary::run() {
    /// for some reason going through this loop will often cause iprobe crashes
    while (!quit) {
        recvMessages(); /// check for more workitems

        dowork(); /// do the work

        if (checkQuit()){ /// check for quit
            resolveSends();
            break;}
        boost::this_thread::yield();
    }
}