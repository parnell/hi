
#ifndef HI_HITREE_HPP
#define HI_HITREE_HPP


#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "HINode.hpp"
#include "../../controllers/DataManager.hpp"
#include "../controllers/IndexDecider.hpp"
#include "params.hpp"

namespace hi {

class HINode;

/**
 * The DataManager here is the Matrix for all of the data in all the nodes
 */
class HITree {
    friend class HINode;

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & proot;
        ar & buildParams;
        ar & queryParams;
        ar & buildResults;
        ar & queryResults;
    }

    DataManager *pdata;
    IndexDecider *pdecider;
    HINode *proot;

public:
    HIBuildResults buildResults;
    HIQueryResults queryResults;
    HIBuildParams buildParams;
    HIQueryParams queryParams;
    DataManager *getDataManager() const;

    void setDataManager(DataManager *pdata);

    HINode *getRoot() const;

    void setRoot(HINode *proot);

public:
    IndexDecider *getDecider() const;

    void setDecider(IndexDecider *pdecider);


public:
    HITree();
    HITree(DataManager *pdata);

    virtual void build(int depth = 0);
    virtual void query(int depth = 0);
};


}

#endif //HI_HITREE_HPP
