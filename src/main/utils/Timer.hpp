
#ifndef HI_TIMER_HPP
#define HI_TIMER_HPP


#include <chrono>
#include <string>
#include <iostream>
#include <ratio>
#include <iomanip> /// for setprecision

using namespace std::chrono;

class Timer {
    std::clock_t c_start;
    high_resolution_clock::time_point t_start;
    high_resolution_clock::time_point t_mark;
    std::string name;
    bool print;
    void _reset(){
        c_start = std::clock();
        t_start = std::chrono::high_resolution_clock::now();
        t_mark = t_start;
    }
public:
    explicit Timer() : print(false){
        _reset();
    }
    explicit Timer(std::string name) : name(name) , print(true){
        _reset();
    }
    explicit Timer(std::string name, bool print) : name(name), print(print){
        _reset();
    }

    void printFromStart(std::string optional="", std::string end="\n"){
        std::clock_t c_end = std::clock();
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::fixed << std::setprecision(2)
                    << name << optional << "\twalltime="
                    << std::chrono::duration<double, std::milli>(t_end-t_start).count()
                    << " (ms)"
                    << "\t"
                    << optional << "\tcputime="
                    << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " (ms)"
                    << end;
    }

    /**
     * Returns true if given time period has ellapsed since last (mark or start)
     * @tparam _Rep
     * @tparam _Period
     * @param duration
     * @param fromMark
     * @return
     */
    template <class _Rep, class _Period>
    bool has_ellapsed(const std::chrono::duration<_Rep, _Period> &duration, bool fromMark = true){
        if (fromMark){
            return (std::chrono::high_resolution_clock::now() - std::chrono::nanoseconds(duration) > t_mark);
        } else {
            return (std::chrono::high_resolution_clock::now() - std::chrono::nanoseconds(duration) > t_start);
        }
    }

    /**
     * Returns time since period has mark or start
     * @tparam _Rep
     * @tparam _Period
     * @param duration
     * @param fromMark
     * @return
     */
    std::chrono::duration<double, std::nano> ellapsed(bool fromMark = true){
        return std::chrono::high_resolution_clock::now() -
               (fromMark ? t_mark : t_start);
    }



    /**
     * Returns true and marks time if given time period has ellapsed since last mark
     * @tparam _Rep
     * @tparam _Period
     * @param duration
     * @param fromMark
     * @return
     */
    template <class _Rep, class _Period>
    bool has_ellapsed_and_mark(const std::chrono::duration<_Rep, _Period> &duration){
        auto now = std::chrono::high_resolution_clock::now();
        if (now - std::chrono::nanoseconds(duration) > t_mark){
            t_mark = now;
            return true;
        }
        return false;
    }

    void mark(){
        t_mark = std::chrono::high_resolution_clock::now();
    }

    ~Timer(){
        if (print){
            printFromStart(std::string(""), std::string("\n"));
        }
    }
};


#endif //HI_TIMER_HPP
