
#ifndef HI_VECTUTIL_H
#define HI_VECTUTIL_H
#include <vector>
#include <list>
#include <iostream>
#include <random>
#include <unordered_set>
#include <numeric>
#include <algorithm>    // std::min

namespace vecutil{

inline std::unordered_set<size_t> pickSet(size_t N, int k, std::mt19937& gen){
    std::unordered_set<size_t> elems;
    for (size_t r = N - k; r < N; ++r) {
        int v = std::uniform_int_distribution<>(0, r)(gen);

        if (!elems.insert(v).second) {
            elems.insert(r);
        }
    }
    return elems;
}

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
inline std::vector<T*> toVecPointers(const size_t N, const size_t C, T *pelems) {
    std::vector<T*> result(N);
    #pragma omp parallel for
    for (size_t i = 0; i < N; ++i) {
//        if (exclude != nullptr && exclude->find(i) != exclude->end())
//            continue;
        result[i] = &pelems[i*C];
    }
    return result;
}

template <typename T>
inline std::vector<T*> randRowPointers(const size_t N, const size_t C, const int k, T *pelems) {
    if (k >= N){
        return toVecPointers(N,C, pelems);
    }
    std::vector<T*> result(k);
    std::random_device rd;
    std::mt19937 gen(rd());
    auto us = pickSet(N,k, gen);
    size_t i=0;
    #pragma omp parallel for
    for (auto iter = us.begin();iter != us.end(); ++iter, ++i){
        result[i] = &pelems[*iter*C];
        assert(result[i] - pelems < N*C);
    }
    return result;
}

template <typename T>
inline std::vector<T*>* randRowPointers(const size_t C, const int k, std::vector<T*>& elems) {
    const size_t N = elems.size();
    assert(k <= N);
    std::vector<T*>* presult = new std::vector<T*>(k);
    std::random_device rd;
    std::mt19937 gen(rd());
    auto us = pickSet(N,k, gen);
    size_t i=0;
    #pragma omp parallel for
    for (auto iter = us.begin();iter != us.end(); ++iter, ++i){
        (*presult)[i] = elems[*iter];
    }
    return presult;
}

template <typename T, typename DIST_TYPE>
inline std::vector<T*> fft(const size_t N, const size_t C,  const int k, T *pelems, DIST_TYPE (*d)(const T*, const T*, const size_t)) {
    if (k >= N){
        return toVecPointers<T>(N, C, pelems);
    }
    std::vector<T*> result;
    T* ppiv = &pelems[0];
    result.emplace_back(ppiv);
    #pragma omp parallel for
    for (size_t i = 1; i < k; ++i) {
        DIST_TYPE dist;
        T max = d(ppiv, &pelems[i * C], C);
        result.emplace_back(&pelems[i * C]);
        T* pval;
        for (size_t j = 0; j < N; ++j) {
            pval = &pelems[j * C];
            dist = d(ppiv, pval, C);
            if (dist > max && ppiv != pval){
                if (std::find(result.begin(), result.end(), pval) != result.end()){
                    continue;}
                max = dist;
                result[i] = pval;
            }
        }
        ppiv = result[i];
    }
    return result;
}

template <typename T, typename DIST_TYPE>
inline std::vector<T*> fft(const size_t N, const size_t C,  const int k, std::vector<T*> elems, DIST_TYPE (*d)(const T*, const T*, const size_t)) {
    if (k >= N){
        return std::vector<T *>(elems);
    }
    std::vector<T*> result;
    T* ppiv = elems[0];
    result.emplace_back(ppiv);
    #pragma omp parallel for
    for (size_t i = 1; i < k; ++i) {
        DIST_TYPE dist;
        T max = d(ppiv, elems[i], C);
        result.emplace_back(elems[i]);
        for (size_t j = 0; j < N; ++j) {
            dist = d(ppiv, elems[j], C);
            if (dist > max && ppiv != elems[j]){
                if (std::find(result.begin(), result.end(), elems[j]) != result.end()){
                    continue;}
                max = dist;
                result[i] = elems[j];
            }
        }
        ppiv = result[i];
    }
    return result;
}


}

#endif //HI_VECTUTIL_H
