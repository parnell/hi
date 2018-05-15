
#ifndef HI_MINJOB_HPP
#define HI_MINJOB_HPP

//#include "../../dtypes.hpp"
#include <vector>
#include <numeric>

#if __cplusplus >= 201701L
#include <execution>
#endif
#include "../rm/Job.hpp"
#include "../../utils/Timer.hpp"
#include "../../myomp.hpp"

class TestWorkItem : public WorkItem {
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
    TestWorkItem() = default;
    size_t mysize;
    explicit TestWorkItem(size_t size) : mysize(size){
    }

    int omp_thread_count() {
        int n = 0;
#pragma omp parallel reduction(+:n)
        n += 1;
        return n;
    }

    virtual std::list<WorkItem*> work() {
        Timer t("Doing work");
        v.resize(mysize);
        std::iota(v.begin(), v.end(), 0);

        size_t m = 0;
        /// Alas it's still too new, c++17 execution is rough to install on Mac or stampede
#if __cplusplus >= 201701L
        std::for_each(std::execution::par, std::begin(v), std::end(v), [&](int item) {
            std::mutex mtx;
            std::lock_guard<std::mutex> guard(mtx);
            m+=item*item*item; // correct
        });
#else

        #pragma omp parallel
        {
            int thread_id = omp_get_thread_num();
            std::cout << "Thread number=" << omp_get_thread_num() <<  "\tmax=" << omp_get_max_threads()<< " " << omp_thread_count()<< std::endl;
        }
        #pragma omp parallel for
//        for (auto& item : v) { // doesn't work with openmp
        for (auto it=v.begin(); it<v.end();++it){
//            m+=item*item*item;
            m += (*it)*(*it)*(*it);
        }
#endif
        std::cout << "searched " << v.size() << "  " << mysize << "  " << m  << std::endl;
        return NO_WORK;
    };


};

class TestJob : public Job{
public:
    TestJob(size_t size, int splits){
        for (size_t i = 0; i < splits; ++i) {
            unfinished.push_back(new TestWorkItem(size));
        }
    }
};




#endif //HI_MINJOB_HPP
