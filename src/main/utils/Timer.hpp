
#ifndef HI_TIMER_HPP
#define HI_TIMER_HPP


#include "../../../../../../../../Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/ctime"
#include "../../../../../../../../Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/chrono"
#include "../../../../../../../../Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/ratio"
#include "../../../../../../../../Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/string"
#include "../../../../../../../../Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/iostream"
#include "../../../../../../../../Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1/iomanip"

using namespace std::chrono;

class Timer {
    std::clock_t c_start;
    high_resolution_clock::time_point t_start;
    high_resolution_clock::time_point t_mark;
    std::string name;
    bool print;

public:
    explicit Timer(std::string name) : name(name) , print(true){
        c_start = std::clock();
        t_start = std::chrono::high_resolution_clock::now();
        t_mark = t_start;
    }
    explicit Timer(std::string name, bool print) : name(name), print(print){
        c_start = std::clock();
        t_start = std::chrono::high_resolution_clock::now();
        t_mark = t_start;
    }

    void fromStart(std::string optional="", std::string end="\n"){
        if (!print){
            return;
        }
        std::clock_t c_end = std::clock();
        auto t_end = std::chrono::high_resolution_clock::now();
        std::cout << std::fixed << std::setprecision(2)
                    << name << optional << "_walltime="
                    << std::chrono::duration<double, std::milli>(t_end-t_start).count()
                    << " (ms)"
                    << "\t"
                    << name << optional << "_cputime="
                    << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " (ms)"
                    << end;
    }
    template <class _Rep, class _Period>
    bool ellapsed(const std::chrono::duration<_Rep, _Period>& duration, bool fromMark=true){
        return (std::chrono::high_resolution_clock::now() - std::chrono::nanoseconds(duration) > t_mark);
    }
    template <class _Rep, class _Period>
    bool ellapsed_and_mark(const std::chrono::duration<_Rep, _Period>& duration, bool fromMark=true){
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
        fromStart("", "\n");
    }
};


#endif //HI_TIMER_HPP
