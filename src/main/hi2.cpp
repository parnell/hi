#include <iostream>
#include <boost/mpi.hpp>
#include <numeric>
#include <thread>
#include <set>
#include <list>
#include <boost/serialization/export.hpp>

namespace mpi = boost::mpi;

class WorkItem {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version) { ar & id; }
    long id;
public:
    WorkItem() : id(-1){}
    WorkItem(int id) : id(id){}
    long getId() const{return id;}
    friend std::ostream& operator<<(std::ostream& os, WorkItem const& o) {return os << o.getId(); }
};
BOOST_CLASS_EXPORT_GUID(WorkItem, "WorkItem");

enum TagType {
    WORK, RESULT, QUIT
};

int main(int argc, char** argv) {
    mpi::environment env;
    mpi::communicator world;
    const int wsize = world.size()-1;
    const int nitems = 4;
    if (world.rank() == 0) {
        std::vector<WorkItem*> items(nitems);
        std::set<int> unfinished;
        for (int j = 0; j < nitems; ++j) {
            items[j] = new WorkItem(j);
            unfinished.insert(j);
        }
        int sendto=1;
        do {
            auto iter = items.begin();
            while (iter != items.end()){
                world.isend((sendto++)%wsize +1, TagType::WORK, *iter);
                items.erase(iter++);
            }
            /// Check for done items
            for (int i =1; i< world.rank();++i ) {
                while (boost::optional<mpi::status> status = world.iprobe(i, TagType::RESULT)) {
                    int completed_work_item;
                    mpi::request req = world.irecv(i, TagType::RESULT, completed_work_item);
                    unfinished.erase(completed_work_item);
                    /// we might add back some more work based on result
                }
            }

            if (unfinished.empty()){
                break;
            }
        } while (true);

    } else {
        std::list<WorkItem*> items;
        WorkItem* pwi;
        while (true) {
            /// Get Work
            while (boost::optional<mpi::status> status= world.iprobe(0, TagType::WORK)) {
                world.recv(0, TagType::WORK, pwi);
                items.push_back(pwi);
            }
            /// Do work
            if (!items.empty()){
                pwi = items.front();
                items.pop_front();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000)); /// simulate some work
                world.isend(0, TagType::RESULT, pwi->getId()); /// send result
                delete pwi;
            }
            /// Check quit
            if (boost::optional<mpi::status> status = world.iprobe(0, TagType::QUIT)) {
                int v;
                world.recv(0, TagType::QUIT, v);
                return true;
            }
            std::this_thread::yield();
        }
    }
    return 0;
}
