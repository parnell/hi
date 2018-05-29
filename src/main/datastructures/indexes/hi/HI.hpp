
#ifndef HI_HI5_H
#define HI_HI5_H

#include "../../../globals.hpp"
#include "HITree.hpp"
#include "../../controllers/DataManager.hpp"
#include "../../Stat.hpp"
#include "../controllers/IndexDecider.hpp"
#include "../Index.hpp"
#include <string>

namespace hi {
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

    void _build(DataManager *pdata, int max_depth=0);
    void build(DataManager *pdata, size_t nnodes, int max_depth=0);
    void build(const std::string& filename, size_t nnodes, int max_depth=0);

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


    size_t size() const;


};

}

#endif //HI_HI5_H
