
#ifndef HI_MYOMP_HPP
#define HI_MYOMP_HPP

#ifdef ENABLE_OPENMP
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_get_thread_num() { return 0;}
inline omp_int_t omp_get_max_threads() { return 1;}
#endif

#endif //HI_MYOMP_HPP
