
#ifndef HI_GLOBAL_INCLUDES_H
#define HI_GLOBAL_INCLUDES_H
#include "datastructures/datatypes/Euc.hpp"
#include "datastructures/datatypes/dna.hpp"

//typedef Euc<float> Dat;
//typedef Euc<float> Dat;
template<typename T>
T sqr(T x) { return x * x; }

typedef float Dat;
#define TIME_MS 1000000
const long long REBALANCE_TIME_NS = TIME_MS * 500;

//extern std::vector<Data*> vGlobData;

#endif //HI_GLOBAL_INCLUDES_H
