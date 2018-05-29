
#ifndef HI_STAT_HPP
#define HI_STAT_HPP

#include <vector>
#include <iostream>
#include <cstddef>
#include <cfloat>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>

template <typename T>
struct tstat {
    enum PrintFlags{
        COUNT = 1,
        SUM = 2,
        MIN = 4,
        MAX = 8,
        MEAN = 16,
        DEV = 32,
        VAR = 64,
        MIN2 = 128,
        ALL = MIN2 *2 -1
    };

    friend std::ostream& operator<<(std::ostream& os, tstat & stat){
        stat.print(os);
        return os;
    }
    size_t _count;
    T _stat[5];
    bool _dirty_var;
    bool _dirty_dev;
    float _var;
    float _dev;
    std::string name;

    tstat () : _dirty_var(true), _dirty_dev(true), _count(0),
               _stat{0,0,std::numeric_limits<T>::max(),
                     std::numeric_limits<T>::min(),std::numeric_limits<T>::max()}{

    }
    tstat (const char* name) : _dirty_var(true), _dirty_dev(true),  _count(0),
                               _stat{0,0,std::numeric_limits<T>::max(),
                                     std::numeric_limits<T>::min(),std::numeric_limits<T>::max()}, name(name){

    }

    void addStat(T val){
        ++_count;
        _stat[0] += val;
        _stat[1] += val*val;
        /// Update min and second min
        if (val < _stat[2]){
            _stat[4] = _stat[2];
            _stat[2] = val;
        } else if (val < _stat[4] && val != _stat[2]){
            _stat[4] = val;
        }
        _stat[3] = std::max(_stat[3], val);
        _dirty_var=true;
        _dirty_dev = true;
    }

    inline size_t count() const{
        return _count;
    }

    inline T sum() const{
        return _stat[1];
    }

    inline T min() const{
        return _stat[2];
    }

    inline T min2() const{
        return _stat[4];
    }

    inline T max() const{
        return _stat[3];
    }

    inline float mean() const{
        return _stat[0]/_count;
    }

    float dev(){
        if (_dirty_dev){
            _dev = (float) (1.0/_count) * std::sqrt(_count*_stat[1]- _stat[0]*_stat[0]);
            _dirty_dev = false;
        }
        return _dev;
    }

    float var(){
        if (_dirty_var){
            float f = dev();
            _var = f*f;
            _dirty_var = false;
        }
        return _var;
    }

    std::ostream& print(const char* p, std::ostream &out, unsigned flags = ALL) const {
        if (p == nullptr)
            p = "";
        out << "[";
        if (flags & COUNT)
            out << p << "_count=" << _count << "\t";
        if (flags & SUM)
            out << p<<"_sum="<<_stat[0]<<"\t";
        if (flags & MIN)
            out << p<<"_min="<<_stat[2]<<"\t";
        if (flags & MAX)
            out << p<<"_max="<<_stat[3]<<"\t";
        if (flags & MEAN)
            out << p <<"_mean="<<mean() <<"\t";
        if (flags & DEV){
            float dev = (float) (1.0/_count) * std::sqrt(_count*_stat[1]- _stat[0]*_stat[0]);
            out << p <<"_dev="<<dev <<"\t";
        }
        if (flags & VAR){
            float dev = (float) (1.0/_count) * std::sqrt(_count*_stat[1]- _stat[0]*_stat[0]);
            float var = dev*dev;
            out  << p << "_var=" << var <<"\t";
        }
        out << "]";
        return out;
    }

    std::ostream& print(std::ostream &out){
        return print(name.c_str(), out);
    }

    float ratio() {
        if (max() == 0) return 0;
        return (1.0f - (static_cast<float>(min2()) / max()));
//        float f = ( min2() == 0 ? (1.0f - (static_cast<float>(min2())/max()) ) : (1.0f - (static_cast<float>(min2())/max()) )/min2() );
//        std::cout << "   " << min() << "   " <<  min2() << "   " << max() << "           "<< "   " << f << "   "  << count() << std::endl;
//        return min2() == 0 ? (1.0f - (static_cast<float>(min2())/max()) ) : (1.0f - (static_cast<float>(min2())/max()) )/min2();
//        return (1.0f - (static_cast<float>(min2())/max()) )/min2();
    }
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

    float summean(){
        float _sum = 0.0f;
        #pragma om parallel for shared(_sum, mean) reduction(+: _sum)
        for (size_t i = 0; i < mean.size(); ++i) {
            _sum += mean[i];
        }
        return _sum;
    }
    float sumvar(){
        float _sum = 0.0f;
        #pragma om parallel for shared(_sum, var) reduction(+: _sum)
        for (size_t i = 0; i < var.size(); ++i) {
            _sum += var[i];
        }
        return _sum;
    }
    void resize(size_t size, size_t D);


};



#endif //HI_STAT_HPP
