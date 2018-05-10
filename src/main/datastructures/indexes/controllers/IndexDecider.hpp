
#ifndef HI_DECISIONSPLITTER_HPP
#define HI_DECISIONSPLITTER_HPP


#include "../../controllers/DataManager.hpp"
#include "../../../dtypes.hpp"
#include "../../Stat.hpp"


struct DecideResult{
    IndexGroup indexGroup;
    Dat* pivot;
    std::vector<DataManager*> data;
};


class IndexDecider {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
    }

public:
    virtual std::vector<DecideResult> decide(DataManager *pdata, int maxPivots, float lshVarThreshold, int depth);
    virtual Stat calculateVariance(DataManager *pdata);
};


#endif //HI_DECISIONSPLITTER_HPP
