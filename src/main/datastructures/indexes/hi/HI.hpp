
#ifndef HI_HI5_H
#define HI_HI5_H

#include "../../../globals.hpp"
#include "HITree.hpp"
#include "../../controllers/DataManager.hpp"
#include "../../Stat.hpp"
#include "../controllers/IndexDecider.hpp"
#include "../Index.hpp"

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
    void build(DataManager *pdata, int nnodes, int max_depth=0);
//    ~/src/LSHBOX/include/lshbox.h
    HI() : pdecider(nullptr){
        pdecider = new IndexDecider();
    }


};

}

#endif //HI_HI5_H
