
#ifndef HI_STAT_HPP
#define HI_STAT_HPP

#include <vector>

struct Stat {
    std::vector<size_t> sum;
    std::vector<float> mean;
    std::vector<float> var;

};


#endif //HI_STAT_HPP
