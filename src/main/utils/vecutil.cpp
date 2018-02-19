#include "vectutil.hpp"
#include <iostream>

namespace vecutil{


    template <typename T>
    std::vector<T> splitvector(std::vector<T>& v, int nparts){
        int workper = v.size() / nparts;
        int rem = v.size() % nparts;
        std::cout << "per v " << workper << "  rem=" << rem << std::endl;

    }
}

