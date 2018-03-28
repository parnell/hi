
#ifndef HI_DTYPES_HPP
#define HI_DTYPES_HPP

#define P_CPP17 0


typedef float dtype;
#define dtype_f	"%.4f"
#define dtype_inf "%f"

enum IndexTypes {
    HI
};

enum TagType {
    INIT_WORK, QUIT, ANY, WORK, WORK_STATUS
};


#endif //HI_DTYPES_HPP
