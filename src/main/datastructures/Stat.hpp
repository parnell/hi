
#ifndef HI_STAT_HPP
#define HI_STAT_HPP

#include <vector>
#include <iostream>
#include <cstddef>

struct SingleStat {
    friend std::ostream& operator<<(std::ostream&, const SingleStat&);
    size_t count;
    double sum;
    float mean;
    float var;

    void add(float d);
};

struct Stat {
    friend std::ostream& operator<<(std::ostream&, const Stat&);
    Stat(){}
    Stat(const size_t size, const size_t D) : count(size), sum(D), mean(D), var(D){}
    Stat(const size_t D) : count(0), sum(D), mean(D), var(D){}

    size_t count;
    std::vector<double> sum;
    std::vector<float> mean;
    std::vector<float> var;

    void resize(size_t size, size_t D);

};



#endif //HI_STAT_HPP
