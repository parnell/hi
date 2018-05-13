
//#include <flann/util/matrix.h>
//#include <flann/algorithms/dist.h>
//#include <flann/algorithms/nn_index.h>
//#include <flann/algorithms/kdtree_index.h>
#include "HINode.hpp"
#include "../../../dprint.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "indexwrappers/SpatialWrapper.hpp"
#include "HITree.hpp"
#include "../../rm/M.hpp"
namespace hi {


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
    if (depth >= parent->buildParams.maxDepth)
        return;
    HIBuildResults& br = parent->buildResults;
    br.nnodes += 1;
    if (pdata->getRows() < params.maxLeafPoints){
        createLeaf(params, pdata, depth);
        return;
    }
    br.ninternal += 1;
    const size_t R = pdata->getRows();
    const size_t C = pdata->getCols();
//    dtprintf(depth, "   HI::Build ---   depth=%d  size=%lu\n", depth, R);

    auto results = parent->pdecider->decide(pdata, params.maxPivots, params.lshVarThreshold, depth);
//    dcoutl("         post decide ");
    for (auto dr : results){
        const IndexGroup group = dr.indexGroup;

        switch (group) {
            case IG_SPATIAL: {
//                dprintf("   HI::Build --- SpatialWrapper  depth=%d  size=%lu\n", depth, pnewdat->getRows());
                if (!ppivots){
                    ppivots = new std::vector<std::pair<size_t, std::vector<Dat>>>();
                }
                ppivots->emplace_back(dr.idxpivot);
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
                assert(pchildren != nullptr);
                assert(ppivots != nullptr);
                break;
            }
            case IG_LSH: {
                br.nlsh += 1;
                indexType = IT_LSH;
//                dprintf("HI::Build --- LSH   size=%lu\n",pnewdat->getRows());
                DataManager* pnewdat = dr.data[0];
                plsh = new LSHWrapper(LSHTYPE::ITQ, R, C);
                plsh->idxs = pnewdat->idxs;
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

void HINode::knnquery(Dat* queryPoint, int depth) {
//    for (int i = 0; i < depth; ++i) { std::cout << "    "; } dcoutl(parent->queryParams.querypoint);
    assert(parent->queryParams.querypoint != nullptr);
    if (is_leaf_){
//        for (int i = 0; i < depth; ++i) { std::cout << "    "; } dprintf("   searching leaf depth=%d   size=%lu\n", depth, leafPoints->getRows());
        searchLeaf(queryPoint, depth);
        return;
    }  else if (plsh){
//        dprintf("lsh query %d %lu \n ", parent->queryParams.k, plsh->size());
        auto scan = plsh->query(queryPoint, parent->queryParams.k);
//        dcoutl("here 2");
//        dprintf("   cnt=%d   heapSize=%lu idxsi=%lu \n", scan.cnt(), scan.topk().getTopk().size(), plsh->idxs.size());
//        dcoutl("here 3");
        parent->queryResults.addAll(scan.topk(), plsh->idxs);

//        dcoutl("here 4");
    } else if (pchildren){

        for (auto idxpivot : *ppivots){
            ++parent->queryResults.distcalcs;
            const float d = rm::M<Dat>::dist(idxpivot.second, queryPoint,parent->queryParams.C);
            dprintf("  adding pivot %lu    %f ,       %f:%f\n", idxpivot.first, d, idxpivot.second[0], idxpivot.second[1]);

            parent->queryResults.add(idxpivot.first, d);
        }

//        for (int i = 0; i < depth; ++i) { std::cout << "    "; } dprintf("child searching depth=%d   size=%lu\n", depth, pchildren->size());
        for (auto pchild : *pchildren){
            pchild->knnquery(queryPoint, depth+1);
        }
//        for (int i = 0; i < depth; ++i) { std::cout << "    "; }dprintf("--- depth=%d   size=%lu\n", depth, pchildren->size());
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

void HINode::searchLeaf(Dat* queryPoint, int depth) {
    HIQueryResults& qr = parent->queryResults;

    for (size_t i = 0; i < leafPoints->getRows(); ++i) {
        ++parent->queryResults.distcalcs;
        float d = rm::M<Dat>::dist((*leafPoints)[i], queryPoint, parent->queryParams.C);
//        dprintf("  adding leaf %lu    %f ,       %f:%f\n", leafPoints->idxs[i], d, leafPoints->m[i][0], leafPoints->m[i][1]);

        qr.add(leafPoints->idxs[i], d);
    }

}

std::vector<HINode *> *HINode::getpChildren() const {
    return pchildren;
}

DataManager *HINode::getLeafPoints() const {
    return leafPoints;
}

std::vector<std::pair<size_t, std::vector<Dat> >> *HINode::getPivots() const {
    return ppivots;
}


}

