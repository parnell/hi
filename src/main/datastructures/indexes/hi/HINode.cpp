
//#include <flann/util/matrix.h>
//#include <flann/algorithms/dist.h>
//#include <flann/algorithms/nn_index.h>
//#include <flann/algorithms/kdtree_index.h>
#include "HINode.hpp"
#include "../../../dprint.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "indexwrappers/SpatialWrapper.hpp"
#include "HITree.hpp"

namespace hi {

const unsigned int _M = 500; ///(521) Hash table size
const unsigned int _L = 5; /// Number of hash tables
const unsigned int _S = 100; /// Size of vectors in train
const unsigned int _I = 5;/// Training iterations
const unsigned int _N = 4; /// Binary code bytes, (has to be less than D)

const int _MIN_ROWS_LSH = 100;


HINode::HINode() :parent(nullptr), plsh(nullptr), ppivots(nullptr), leafPoints(nullptr), pchildren(nullptr),
                  is_leaf_(false), pspatial(nullptr), indexType(IT_NONE) {

}

HINode::HINode(HITree *parent): parent(parent), plsh(nullptr), ppivots(nullptr), leafPoints(nullptr), pchildren(nullptr),
                                is_leaf_(false), pspatial(nullptr), indexType(IT_NONE) {

}

void HINode::firstsplit(const HIBuildParams& params, DataManager* pdata) {
    build(params, pdata, 0);
}

void HINode::build(const HIBuildParams& params, DataManager *pdata, int depth) {
//    dprintf("HI::Build()   ---   depth=%d size=%ld\n", depth, pdata->getRows());
    HIBuildResults& br = parent->buildResults;
    br.nnodes += 1;
    if (pdata->getRows() < params.maxLeafPoints){
        createLeaf(params, pdata, depth);
        return;
    }
    br.ninternal += 1;
    const size_t R = pdata->getRows();
    const size_t C = pdata->getCols();
//    dprintf("   HI::Build ---   depth=%d  size=%lu\n", depth, R);

    auto results = parent->pdecider->decide(pdata, params.maxPivots, params.lshVarThreshold, depth);
//    dcoutl("         post decide ");
    for (auto dr : results){
        const IndexGroup group = dr.indexGroup;

        switch (group) {
            case IG_SPATIAL: {
//                dprintf("   HI::Build --- SpatialWrapper  depth=%d  size=%lu\n", depth, pnewdat->getRows());
                if (!ppivots){
                    ppivots = new std::vector<Dat*>();
                }
                ppivots->emplace_back(dr.pivot);
                br.npointsInInternal += 1;
                br.nspatial += 1;
                indexType = IT_KD;
                for (auto pnewdat: dr.data){
                    auto child = new HINode(parent);
                    if (!pchildren){
                        pchildren = new std::vector<HINode*>();
                    }
                    pchildren->push_back(child);
                    child->build(params, pnewdat, depth + 1);
                }
                break;
            }
            case IG_LSH: {
                br.nlsh += 1;
                const auto r = static_cast<unsigned int>(R);
                /** int D=2, int M=500, int L=5, int S = 100, int I = 5*/
                unsigned int M = R > _MIN_ROWS_LSH ? _M : std::min(r, r / 2);
                unsigned int L = _L;
                unsigned int S = R > _MIN_ROWS_LSH ? _S : std::min(r, r / 2);
                unsigned int I = _I;
                unsigned int N = C > _N ? _N : std::min(static_cast<unsigned int>(C), _N);

                indexType = IT_LSH;
//                dprintf("HI::Build --- LSH   size=%lu\n",pnewdat->getRows());
                plsh = new LSHWrapper(LSHTYPE::ITQ, C, M, L, S, I, N);
                plsh->D = static_cast<unsigned int>(pdata->getCols());
                DataManager* pnewdat = dr.data[0];
                br.npointsInLSH += pnewdat->getRows();

                if (plsh->N > pnewdat->getCols())
                    plsh->N = static_cast<unsigned int>(std::max(1, static_cast<int>(pnewdat->getCols()) - 1));

                plsh->hash(pnewdat->getDat(), R, C);
                break;
            }
        }
    }

}

HINode::~HINode() {
    if (plsh)
        delete plsh;

}

void HINode::query(const HIQueryParams& params, int depth) {
    if (plsh){
//        plsh->query();
    }
}

bool HINode::isLeaf() const {
    return is_leaf_;
}

void HINode::setIsLeaf(bool is_leaf_) {
    HINode::is_leaf_ = is_leaf_;
}

IndexType HINode::getIndexType() const {
    return indexType;
}

void HINode::setIndexType(IndexType indexType) {
    HINode::indexType = indexType;
}

LSHWrapper *HINode::getLSH() const {
    return plsh;
}

void HINode::setLSH(LSHWrapper *plsh) {
    HINode::plsh = plsh;
}

void HINode::createLeaf(const HIBuildParams &params, DataManager *pManager, int depth) {
    parent->buildResults.nleaf += 1;
    parent->buildResults.npointsInLeaf += pManager->getRows();

    is_leaf_ = true;
    leafPoints = pManager;
}


}

