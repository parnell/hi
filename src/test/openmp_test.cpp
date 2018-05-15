#include <iostream>
#include "../main/myomp.hpp"

int omp_thread_count() {
    int n = 0;
    #pragma omp parallel reduction(+:n)
    n += 1;
    return n;
}

int main(int argc, char** argv) {
#ifdef ENABLE_OPENMP
    std::cout << "##  OPENMP FOUND ##= "<< std::endl;
#else
    std::cout << "##  OPENMP NOT FOUND ##= "<< std::endl;
#endif
    #pragma omp parallel
    {
        std::cout << "Thread number=" << omp_get_thread_num() <<  "\tmax=" << omp_get_max_threads()<< " " << omp_thread_count()<< std::endl;
    }

    return 0;
}
