
#ifndef HI_HINODE_HPP
#define HI_HINODE_HPP


#include <unordered_map>
#include "../../../globals.hpp"
#include "../../controllers/DataManager.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "../controllers/IndexDecider.hpp"
#include "HITree.hpp"
#include "indexwrappers/SpatialWrapper.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "params.hpp"

namespace hi {

class HITree;
class SpatialWrapper;

class HINode {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & is_leaf_;
        ar & indexType;
        ar & plsh;
        ar & pspatial;
        ar & pchildren;
    }

    HITree* parent;

    bool is_leaf_;
public:
    bool isLeaf() const;

    void setIsLeaf(bool is_leaf_);

    IndexTypes getIndexType() const;

    void setIndexType(IndexTypes indexType);
    std::vector<Dat*>* ppivots;
    DataManager* leafPoints;
    std::vector<HINode*>* pchildren;

private:
    LSHWrapper* plsh;
    SpatialWrapper* pspatial;
public:
    LSHWrapper *getLSH() const;

    void setLSH(LSHWrapper *plsh);

private:

    IndexTypes indexType;

public:
    HINode();
    HINode(HITree* parent);
    ~HINode();
    void firstsplit(const HIBuildParams& params, DataManager* pdata);

    void build(const HIBuildParams& params, DataManager *pdata, int depth);

    void query(const HIQueryParams& params, int depth);

    void createLeaf(const HIBuildParams &params, DataManager *pManager, int depth);
};

}

#endif //HI_HINODE_HPP
