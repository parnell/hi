
#ifndef HI_HI5_H
#define HI_HI5_H

#include "../../../globals.hpp"
#include "HITree.hpp"
#include "../../controllers/DataManager.hpp"
#include "../../Stat.hpp"
#include "../controllers/IndexDecider.hpp"
#include "../Index.hpp"
#include <string>
#include <boost/thread.hpp>

namespace hi {
template <typename DM_CLASS>
class HI  : public plib::Index {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<Index>(*this);
        ar & trees_;
    }

    std::vector<HITree*> trees_;
    IndexDecider* pdecider;

public:
    HI() : pdecider(nullptr){
        pdecider = new IndexDecider();
    }


    void _build(DataManager *pdata, size_t nnodes, unsigned int nthreads, int max_depth, int mynode) {
//    dcoutl("HI::Build");
        HITree* p_tree = new HITree(pdata, mynode);
        trees_.emplace_back(p_tree);
        p_tree->buildParams.nnodes = nnodes;
        p_tree->buildParams.nthreads= nthreads;
        p_tree->build(0);
    }

    void build(DataManager *pdata, size_t nnodes, int max_depth, int mynode) {
        unsigned int nthreads = boost::thread::hardware_concurrency();
        _build(pdata, nnodes, nthreads, max_depth, mynode);
    }

    void build(const std::string& filename, size_t nnodes, int max_depth) {
        unsigned int nthreads = boost::thread::hardware_concurrency();
        #pragma omp parallel for
        for (int i = 0; i < nnodes; ++i) {
            DataManager* pdat = DM_CLASS::loadData(filename, i, nnodes);
            _build(pdat, nnodes, nthreads, max_depth, i);
        }
    }

    size_t size() const {
        size_t s = 0;
        for (auto it = trees_.begin(); it < trees_.end(); ++it) {
            s += (*it)->size();
        }
        return s;
    }

    friend std::ostream &operator<<(std::ostream &os, HI &hi){
        size_t sz = 0;
        for (auto& ptree : hi.getTrees()){
            sz += ptree->getDataManager()->getRows();
        }
        os << "[HI ntrees="<<hi.getTrees().size() << " ]\n";
        for (auto& ptree : hi.getTrees()){
            os << ptree->buildResults << std::endl;
        }
        for (auto& ptree : hi.getTrees()){
            os << ptree->queryResults << std::endl;
        }
        return os;
    }


    std::vector<HITree*> getTrees() {
        return trees_;
    }


};

}

#endif //HI_HI5_H
