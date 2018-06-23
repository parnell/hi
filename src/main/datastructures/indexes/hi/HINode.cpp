
//#include <flann/util/matrix.h>
//#include <flann/algorithms/dist.h>
//#include <flann/algorithms/nn_index.h>
//#include <flann/algorithms/kdtree_index.h>
#include "HINode.hpp"
#include "../../../dprint.hpp"
#include "indexwrappers/LSHWrapper.hpp"
#include "HITree.hpp"
#include "../../rm/M.hpp"
#include "../../../utils/vecutil.hpp"
#include "../../controllers/MetricDataManager.hpp"
#include "../../controllers/FDNADataManager.hpp"

namespace hi {


HINode::HINode() :parent(nullptr), plsh(nullptr), ppivots(nullptr), leafPoints(nullptr), pchildren(nullptr),
                  is_leaf_(false), indexType(IT_NONE) {

}

HINode::HINode(HITree *parent): parent(parent), plsh(nullptr), ppivots(nullptr), leafPoints(nullptr), pchildren(nullptr),
                                is_leaf_(false), indexType(IT_NONE) {

}



HINode::~HINode() {
    if (plsh)
        delete plsh;
    if (leafPoints)
        delete leafPoints;
    if (ppivots){
        for (auto& i: *ppivots)
            delete i;
        delete ppivots;
    }
    if (pchildren){
        for (auto& i: *pchildren)
            delete i;
        delete pchildren;
    }


}

void HINode::knnquery(Data* queryPoint, int depth) {
//    for (int i = 0; i < depth; ++i) { std::cout << "    "; } dcoutl(parent->queryParams.querypoint);
    assert(parent->queryParams.querypoint != nullptr);
    if (is_leaf_){
//        for (int i = 0; i < depth; ++i) { std::cout << "    "; } dprintf("   searching leaf depth=%d   size=%lu\n", depth, leafPoints->getRows());
        searchLeaf(queryPoint, depth);
        return;
    }  else if (plsh){
//        dprintf("lsh query %d %lu \n ", parent->queryParams.k, plsh->size());
        Euc<itype>* pqp = static_cast<Euc<itype>*>(queryPoint);

        auto scan = plsh->query(pqp->getDat(), parent->queryParams.k, parent->queryResults.distcalcs, parent->queryResults.npointsInLSH);
//        dcoutl("here 2");
//        dprintf("   cnt=%d   heapSize=%lu idxsi=%lu \n", scan.cnt(), scan.topk().getTopk().size(), plsh->idxs.size());
//        dcoutl("here 3");
        parent->queryResults.addAll(scan.topk(), plsh->idxs);

//        dcoutl("here 4");
    } else if (pchildren){
        size_t i = 0;
        auto pdm = parent->getDataManager();
        auto& qr = parent->queryResults;
        for (auto pivot : *ppivots){
            ++qr.distcalcs;
            ++qr.npointsInInternal;
            const float dqp = queryPoint->d(pivot->ppivot);
//            dprintf("  adding pivot %lu    %f ,       %f:%f\n", idxpivot.first, d, idxpivot.second[0], idxpivot.second[1]);

            qr.add(pivot->index, dqp);

            /// d(q,p) - radius >= d(p,mid_l)
            const dist_type ld = (*ppivots)[0]->getMaxLeft();
            const dist_type rd = (*ppivots)[0]->getMinRight();
            dist_type max = qr.getMax();
            if ( dqp - max < ld){
                auto pleft = (*pchildren)[0];
                pleft->knnquery(queryPoint, depth+1);
            }
            max = qr.getMax();
            if ( dqp + max > rd){
                auto pright = (*pchildren)[1];
                pright->knnquery(queryPoint, depth+1);
            }
            i++;

        }
//        for (int i = 0; i < depth; ++i) { std::cout << "    "; } dprintf("child searching depth=%d   size=%lu\n", depth, pchildren->size());
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

LSHWrapper<itype> *HINode::getLSH() const {
    return plsh;
}

void HINode::setLSH(LSHWrapper<itype> *plsh) {
    HINode::plsh = plsh;
}

void HINode::searchLeaf(Data* queryPoint, int depth) {
    leafPoints->scan(queryPoint, depth, parent->queryResults);
}

std::vector<HINode *> *HINode::getpChildren() const {
    return pchildren;
}

DataManager *HINode::getLeafPoints() const {
    return leafPoints;
}

std::vector<Pivot*> *HINode::getPivots() const {
    return ppivots;
}

HIBuildParams* HINode::getpBuildParms() {
    return &this->parent->buildParams;
}
HIBuildResults* HINode::getpBuildResults() {
    return &this->parent->buildResults;
}

std::vector<DecideResult *> HINode::decide(DataManager* pdata, const HIBuildParams& parms, int depth) {
//    int maxPivots, float lshVarThreshold, int depth
    return parent->getDecider()->decide(pdata, parms.maxPivots, parms.lshVarThreshold, depth);
}

void HINode::build(const HIBuildParams& params, DataManager *pdata, int depth) {
//    dprintf("HI::Build()   ---   depth=%d size=%ld    ml=%zu\n", depth, pdata->getRows(),   params.maxLeafPoints);
    auto bparms = getpBuildParms();
    if (depth >= bparms->maxDepth)
        return;
    auto pbr = getpBuildResults();
    pbr->nnodes += 1;
    if (pdata->getRows() < params.maxLeafPoints){
        createLeaf(params, pdata, depth);
        return;
    }
    pbr->ninternal += 1;
    const size_t R = pdata->getRows();
//    const size_t C = pdata->getCols();
//    dtprintf(depth, "   HI::Build ---   depth=%d  size=%lu\n", depth, R);

//        auto results = parent->pdecider->decide(pdata, params.maxPivots, params.lshVarThreshold, depth);
    auto results = decide(pdata, *bparms, depth);
//    dcoutl("         post decide ");
    for (auto pdr : results){
        const IndexGroup group = pdr->indexGroup;

        switch (group) {
            case IG_SPATIAL: {
//                dprintf("   HI::Build --- SpatialWrapper  depth=%d  size=%lu\n", depth, pnewdat->getRows());
                if (!ppivots){
                    ppivots = new std::vector<Pivot*>();
                }
                ppivots->emplace_back(pdr->ppivot);
                pbr->npointsInInternal += 1;
                pbr->nspatial += 1;

                indexType = IT_KD;
                for (auto pnewdat: pdr->data){
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
                /// convert metric to euclidean
                pbr->nlsh += 1;
                indexType = IT_LSH;
                //                dprintf("HI::Build --- LSH   size=%lu\n",pnewdat->getRows());
                EucDataManager<itype>* pnewdat;
                bool isEuc = pdr->data[0]->isEuclidean();
                if (isEuc){
                    pnewdat = dynamic_cast<EucDataManager<itype>*>(pdr->data[0]);
                } else { /// This needs to convert metric to euc
                    switch (pdr->data[0]->getType()){
                        case DataManager::ManagerType::DM_FDNA: {
                            FDNADataManager *pdna = dynamic_cast<FDNADataManager *>(pdr->data[0]);
                            pnewdat = pdna->toEuclidean<itype>();
                            break;
                        }
                        default:
                            assert(false);
                    }

                }
                const size_t C = pnewdat->getCols();
                plsh = new LSHWrapper<itype>(LSHTYPE::ITQ, R, C);
                plsh->idxs = pnewdat->idxs;
                pbr->npointsInLSH += pnewdat->getRows();

                if (plsh->N > pnewdat->getCols())
                    plsh->N = static_cast<unsigned int>(std::max(1, static_cast<int>(pnewdat->getCols()) - 1));

                plsh->hash(pnewdat->getDat(), R, C);
                break;
            }
            case IG_NONE:
                BOOST_VERIFY_MSG(false, "IG_NONE case not be handled");
        }
        delete pdr;
    }
}



}



#if COMPILE_TESTS
#include "gtest/gtest.h"

TEST(hi, HINode_test_serialization_blank)
{
    hi::HINode node;
    {
        std::ofstream ofs("HINode_test_serialization.idx");
        boost::archive::text_oarchive oa(ofs);
        oa << node;
    }
    hi::HINode newnode;
    {
        std::ifstream ifs("HINode_test_serialization.idx");
        boost::archive::text_iarchive ia(ifs);
        ia >> newnode;
    }
    EXPECT_EQ(node.getpChildren()== nullptr, true);
    EXPECT_EQ(newnode.getpChildren()== nullptr, true);
}


#endif