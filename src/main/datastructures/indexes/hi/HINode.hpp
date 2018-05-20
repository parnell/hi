
#ifndef HI_HINODE_HPP
#define HI_HINODE_HPP


#include <unordered_map>
#include "../../../globals.hpp"
#include "../../controllers/DataManager.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "../controllers/IndexDecider.hpp"
#include "HITree.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include "params.hpp"
#include "../../controllers/EucDataManager.hpp"

namespace hi {

class HITree;

class HINode {
    friend class boost::serialization::access;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & is_leaf_;
        ar & indexType;
        ar & plsh;
        ar & pchildren;
        ar & parent;
        ar & ppivots;
        ar & leafPoints;
    }

    HITree* parent;
    bool is_leaf_;
    std::vector<Pivot*>* ppivots;
    std::vector<HINode*>* pchildren;

    DataManager* leafPoints;
    LSHWrapper<itype>* plsh;
    IndexType indexType;

public:
    bool isLeaf() const;

    void setIsLeaf(bool is_leaf_);

    IndexType getIndexType() const;

    void setIndexType(IndexType indexType);

private:
public:
    LSHWrapper<itype> *getLSH() const;

    void setLSH(LSHWrapper<itype> *plsh);

private:


public:
    HINode();
    HINode(HITree* parent);
    ~HINode();
    void firstsplit(const HIBuildParams& params, DataManager* pdata){
        build(params, pdata, 0);
    }

    std::vector<HINode*>* getpChildren() const;
    DataManager* getLeafPoints() const;

    HIBuildParams* getpBuildParms();
    HIBuildResults* getpBuildResults();
    std::vector<DecideResult*> decide(DataManager* pdata, const HIBuildParams& parms, int depth);

    void build(const HIBuildParams& params, DataManager *pdata, int depth);

    std::vector<Pivot*>* getPivots() const;
    void knnquery(Data* queryPoint, int depth);


    void createLeaf(const HIBuildParams &params, DataManager *pManager, int depth) {
        auto pbr = getpBuildResults();
        pbr->nleaf += 1;
        pbr->npointsInLeaf += pManager->getRows();

        is_leaf_ = true;
        leafPoints = pManager->new_instance();
    }

    void searchLeaf(Data* queryPoint, int depth);

};

}

#endif //HI_HINODE_HPP
