
#ifndef HI_VECTUTIL_H
#define HI_VECTUTIL_H
#include <vector>
#include <list>
#include <iostream>
#include <random>
#include <unordered_set>
#include <algorithm>    // std::min

namespace vecutil{

inline std::unordered_set<size_t> pickSet(size_t N, int k, std::mt19937& gen);

inline std::vector<size_t> pick(size_t N, int k) ;

// Functor for deleting pointers in vector.
template<class T> class DeleteVector {
public:
    // Overloaded () operator.
    // This will be called by for_each() function.
    inline bool operator()(T x) const {
        // Delete pointer.
        delete x;
        return true;
    }
};

template <typename T>
inline std::vector<T> split(std::vector<T>& v, const int nparts, const int iteration=0) {
    size_t workper = v.size() / nparts;
    size_t rem = v.size() % nparts;
    auto it = v.begin();
    size_t start = iteration < rem ? workper*iteration : (workper+1)*(rem) + workper*(iteration-rem);
    size_t end = start + (iteration < rem ? workper +1 : workper);
//    std::cout << workper << " i=" <<iteration << "  " << start<< "   " << end << "  " << (end-start) << "      " << end - start << std::endl;
    return std::vector<T>(it + start, it + end);
}

template <typename T>
inline std::vector<T*> randRowPointers(const size_t N, const size_t C, const int k, T *pelems) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<T*> result(k);
    auto us = pickSet(N,k, gen);
    int i=0;
    for (auto iter = us.begin();iter != us.end(); ++iter, ++i){
//        std::cout << " (" << *iter << " :" << &pelems[*iter*C] << " " << pelems[*iter*C] << " ) ";
        result[i] = &pelems[*iter*C];
    }
//    std::cout << std::endl;
    return result;
}


}

#endif //HI_VECTUTIL_H
