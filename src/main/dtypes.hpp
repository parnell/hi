
#ifndef HI_DTYPES_HPP
#define HI_DTYPES_HPP


typedef float dtype;
#define dtype_f	"%.4f"
#define dtype_inf "%f"

enum IndexGroup {
    IG_SPATIAL, IG_LSH
};

enum IndexTypes {
    IT_HI, IT_LSH, IT_KD
};

enum TagType {
    INIT_WORK, QUIT, ANY, WORK, WORK_STATUS
};


#endif //HI_DTYPES_HPP
