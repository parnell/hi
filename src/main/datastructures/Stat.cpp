
#include "Stat.hpp"
#include <iostream>

void Stat::resize(size_t size, size_t D) {
    sum.resize(D);
    mean.resize(D);
    var.resize(D);
}

std::ostream& operator<< (std::ostream& os, const Stat& stat) {
    os << "[Stat " << stat.count << ":" << stat.var[0] << "]";
    return os;
}

