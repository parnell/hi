
#ifndef HI_GLOBAL_INCLUDES_H
#define HI_GLOBAL_INCLUDES_H
#include "datastructures/datatypes/Euc.hpp"
#include "datastructures/datatypes/dna.hpp"
#include "config.hpp"

class DNA;


#if __DATA_TYPE__ == __VEC_TYPE__
typedef float Dat;

#elif __DATA_TYPE__ == __STRING_TYPE__
typedef char Dat;

#elif __DATA_TYPE__ == __GRAPH_TYPE__


#endif


template<typename T>
T sqr(T x) { return x * x; }


#define TIME_MS 1000000
const long long REBALANCE_TIME_NS = TIME_MS * 500;

const size_t PIV_MAX_EVAL_SAMPLES = 5000;
const size_t PIV_MAX_CAND_SAMPLES = 20;


#endif //HI_GLOBAL_INCLUDES_H
