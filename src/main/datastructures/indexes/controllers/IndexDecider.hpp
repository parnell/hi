
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

    std::vector<DecideResult*> decide(DataManager *pdata, int maxPivots, float lshVarThreshold, int depth) {
        const size_t R = pdata->getRows();
//    const size_t C = pdata->getCols();

        Stat stat = pdata->calculateVariance();
//    dprintf(" depth=%d\tvar=%f\tsize=%ld   thresh=%f\n", depth, stat.var[0], R, lshVarThreshold);
        if (stat.var[0] < lshVarThreshold){
//        auto pivots = vecutil::randRowPointers<Dat>(R, C, maxPivots, pdata->getDat());
            auto pivots = pdata->pickPivots(maxPivots);
            std::vector<DecideResult*> res(pivots.size());
            size_t idx = 0;
            std::vector<DataManager*> cuts;
            cuts.emplace_back(pdata);
            for (auto _pivot : pivots){
//            std::cout << "Pre sort" << *pdata << std::endl;
                res[idx] = new DecideResult(_pivot);
                auto &pdr = res[idx];

                auto& pivot = *pdr->ppivot;
                auto& d = pdr->data;

                pdata->sort(pivot);
                pdr->indexGroup = IG_SPATIAL;
                /// Partition
                /// partition pivot index
                auto pivotdat = pdata->sliceData(0,1);
                assert(pivotdat->idxs.size() == 1);
                auto pidx = pivotdat->idxs[0];
                pivot.index = pidx;
                /// split partition regions
                auto *psubdat = pdata->sliceData(1, R / 2 + 1);
                auto *psubdat2 = pdata->sliceData(R / 2 + 1, R);
                pivot.maxl_idx = R / 2;
                pivot.minr_idx= R / 2 + 1;
                d.emplace_back(psubdat);
                d.emplace_back(psubdat2);
                assert(d[0]->getRows() + d[1]->getRows() + 1 == R);
                assert(pdata->idxs.size() == R);
//            assert(pivot.distances.size() == R);
//            assert(pdr->idxpivot.second.size() > 0);
                ++idx;
                delete pivotdat;
            }
            return res;
        } else {
            std::vector<DecideResult*> res(1);
            res[0] = new DecideResult();
            size_t idx = res.size()-1;
            res[idx]->indexGroup = IG_LSH;
            res[idx]->data.emplace_back(pdata);
            return res;
        }
    }
//    virtual Stat calculateVariance(DataManager *pdata);
};


#endif //HI_DECISIONSPLITTER_HPP
