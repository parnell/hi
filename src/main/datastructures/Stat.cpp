
#include "Stat.hpp"
#include <iostream>

void Stat::resize(const size_t size, const size_t D) {
    sum.resize(D);
    mean.resize(D);
    var.resize(D);
}

std::ostream& operator<< (std::ostream& os, const Stat& stat) {
    os << "[Stat " << stat.count << ":" << stat.var[0] << "]";
    return os;
}

std::ostream& operator<< (std::ostream& os, const SingleStat& stat) {
    os << "[Stat " << stat.count << ":" << stat.var << "]";
    return os;
}

void SingleStat::add(const float val) {
    sum+=val;


}
