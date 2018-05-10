
#ifndef HI_LOADBALANCER_HPP
#define HI_LOADBALANCER_HPP

#include <vector>
#include <cstddef>

class LoadBalancer {

    public:

    bool shouldRebalance(std::vector<size_t> work);
};


#endif //HI_LOADBALANCER_HPP
