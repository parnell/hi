#include <iostream>
#include <boost/mpi.hpp>
#include <boost/serialization/string.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/mpi/collectives.hpp>  /// for gatherv
#include <numeric>
#include "utils/WorkItem.h"
#include "utils/vecutil.hpp"

namespace mpi = boost::mpi;

void rankf(int rank){
    printf("  RANK=%d\n", rank);
}
typedef std::vector<int>::size_type sizet;

int main(int argc, char** argv) {
    const int tag = 337;
    mpi::environment env;
    mpi::communicator world;
    const int size = world.size();
    std::cout << "rank=" << world.rank() << "\tsize=" << size << std::endl;
    std::vector<WorkItem> items(10);
    WorkItem item;
    std::vector<int> v(100) ;
    std::vector<int> sv(7) ;
    std::iota (std::begin(v), std::end(v), 0);
    std::vector<int> sizes;
    bool hasMoreWork = true;
    std::vector<int> my_data;

    if (world.rank() == 0) {
        for (int i =0; i<size-1;i++) {
            std::vector<int> ret = vecutil::split<int>(v, size-1, i);
            std::cout << "Master     sending " << ret.size() << std::endl;
            world.send(i+1, tag, ret);
            sizes.push_back((int) ret.size());
        }
        std::vector< std::vector<int> > all_numbers;
        std::vector<int> mysend;
        gather(world, mysend, all_numbers, 0);
        for (auto i : all_numbers){
            std::cout << " i = " << i.size() << std::endl;
        }

    } else {
        std::vector<int> ritems;
        WorkItem ritem;
        std::string msg;
        world.recv(0, tag, ritems);
        std::cout << " Slave " << world.rank() << "  ############ , " << ritems.size() << std::endl;

        std::cout.flush();
        gather(world, ritems, 0);
    }
    return 0;
}
