
#ifndef HI_MINJOB_HPP
#define HI_MINJOB_HPP

#include "../../dtypes.hpp"
#include <vector>
#include <numeric>
#if P_CPP17
#include <execution>
#endif
#include "../rm/Job.hpp"
#include "../Timer.hpp"


class MinItem : public WorkItem {
    std::vector<int> v;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & boost::serialization::base_object< WorkItem >( *this );
        ar & mysize;
        ar & v;
    }

public:
    MinItem() = default;
    size_t mysize;
    explicit MinItem(size_t size) : mysize(size){
    }

    void work() override{
        Timer t("Doing work");
        v.resize(mysize);
        std::iota(v.begin(), v.end(), 0);

        size_t m = 0;
        std::mutex mtx;
        /// Alas it's still too new, c++17 execution is rough to install on Mac or stampede
#if P_CPP17
        std::for_each(std::execution::par, std::begin(v), std::end(v), [&](int item) {
            std::lock_guard<std::mutex> guard(mtx);
            m+=item*item*item; // correct
        });
#else
        #pragma omp parallel for
        for (auto& item : v) {
//            if (item < m){
//                m = item;
//            }
            m+=item*item*item;
        }
#endif
        std::cout << "searched " << v.size() << "  " << mysize << "  " << m  << std::endl;
    };
};

class MinJob : public Job{
public:
    MinJob(size_t size, int splits){
        for (size_t i = 0; i < splits; ++i) {
            unfinished.push_back(new MinItem(size));
        }
    }
};




#endif //HI_MINJOB_HPP
