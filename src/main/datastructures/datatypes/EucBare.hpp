
#ifndef HI_EUCBARE_HPP
#define HI_EUCBARE_HPP


#include "../data.hpp"

template <typename T>
struct EucBare : public Data {
public:
    T* p;
    EucBare(T* p) : p(p){}
};


#endif //HI_EUCBARE_HPP
