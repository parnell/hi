
#ifndef HI_DTYPES_HPP
#define HI_DTYPES_HPP
#include "config.hpp"

#if __DATA_TYPE__ == __VEC_TYPE__
typedef float dtype;
typedef float dist_type;
typedef float itype; /// internal euc data representation


#elif __DATA_TYPE__ == __STRING_TYPE__
typedef char Dat;
typedef int dist_type;
typedef bool itype;

#elif __DATA_TYPE__ == __GRAPH_TYPE__


#endif


#define dtype_f	"%.4f"
#define dtype_inf "%f"

enum IndexGroup {
    IG_SPATIAL, IG_LSH, IG_NONE
};

enum IndexType {
    IT_HI, IT_LSH, IT_KD, IT_NONE
};

enum TagType {
    INIT_WORK, QUIT, ANY, WORK, WORK_STATUS
};


#endif //HI_DTYPES_HPP
