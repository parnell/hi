
#ifndef HI_TIMER_HPP
#define HI_TIMER_HPP


#include <ctime>
#include <chrono>
#include <ratio>
#include <string>
#include <iostream>
#include <iomanip>

using namespace std::chrono;

class Timer {
    std::clock_t c_start;
    high_resolution_clock::time_point t_start;
    std::string name;

public:
    explicit Timer(std::string name) : name(name){
        c_start = std::clock();
        t_start = std::chrono::high_resolution_clock::now();
    }

    void fromStart(std::string optional="", std::string end="\n"){
        std::clock_t c_end = std::clock();
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::fixed << std::setprecision(2)
                    << name << optional << "_walltime="
                    << std::chrono::duration<double, std::milli>(t_end-t_start).count()
                    << " ms"
                    << "\t"
                    << name << optional << "_cputime="
                    << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms"
                    << end;
    }

    ~Timer(){
        fromStart("", "\n");
    }
};


#endif //HI_TIMER_HPP
