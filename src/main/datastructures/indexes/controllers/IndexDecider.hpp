
#ifndef HI_DECISIONSPLITTER_HPP
#define HI_DECISIONSPLITTER_HPP


#include "../../controllers/DataManager.hpp"
#include "../../../dtypes.hpp"
#include "../../Stat.hpp"
#include "Pivot.hpp"


struct DecideResult{
    IndexGroup indexGroup;
    std::vector<DataManager*> data;

//    std::pair<size_t,std::vector<Dat>>  idxpivot;
    Pivot* ppivot;
    DecideResult() : ppivot(nullptr){}
    DecideResult(Pivot* _pivot) : ppivot(_pivot){}
    ~DecideResult(){
        /** Pivot gets transferred over to HINode which is responsible for cleanup*/
    }
};


class IndexDecider {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
    }

public:
    virtual std::vector<DecideResult*> decide(DataManager *pdata, int maxPivots, float lshVarThreshold, int depth);
    virtual Stat calculateVariance(DataManager *pdata);
};


#endif //HI_DECISIONSPLITTER_HPP
