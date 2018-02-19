#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/vector.hpp>
#include "utils/WorkItem.h"

namespace mpi = boost::mpi;

void rankf(int rank){
    printf("  RANK=%d\n", rank);
}

int main(int argc, char** argv) {
    mpi::environment env;
    mpi::communicator world;
    std::cout << "rank=" << world.rank() << "\tsize=" << world.size() << std::endl;
    std::vector<WorkItem> items(10);
    WorkItem item;
    if (world.rank() == 0) {
        world.send(1, 0, items);
        std::string msg;
        world.recv(1, 1, msg);
        std::cout << msg << " back!" << std::endl;
    } else {
        std::vector<WorkItem> ritems;
        WorkItem ritem;
        std::string msg;
        world.recv(0, 0, ritems);
        std::cout << ritem << ", " << ritems.size();
        std::cout.flush();
        world.send(0, 1, ritems.size());
    }
    return 0;
}
