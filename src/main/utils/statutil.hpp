
#ifndef HI_VARUTIL_HPP
#define HI_VARUTIL_HPP


#include <complex>
#include <iostream>
#include "../globals.hpp"

namespace varutil {

template <typename T>
double variance(T* p, size_t len, bool isSample=true)
{
    double sq_sum = 0.0;
    std::complex<double> sum = 0.0;
    for (int i = 0; i < len; i++, p++) {
        sum += *p;
        sq_sum += std::norm(*p);
    }
    return isSample ? (sq_sum - std::norm(sum) / len) / (len-1) :(sq_sum - std::norm(sum) / len) / (len);
}

};

#endif //HI_VARUTIL_HPP
