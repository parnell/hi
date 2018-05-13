
#ifndef HI_PARAMS_HPP
#define HI_PARAMS_HPP


#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>
#include <atomic>
#include <lshbox/topk.h>
#include "../../../globals.hpp"
#include "../../../dprint.hpp"
#include <mutex>
#include <limits>

namespace hi {


struct HIBuildParams{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & nnodes;
        ar & nthreads;
        ar & maxLeafPoints;
        ar & maxPivots;
        ar & minPivots;
        ar & lshVarThreshold;
        ar & maxDepth;
        ar & maxIdx;
    }

    int nnodes = 1;
    int nthreads = 1;
    int maxLeafPoints = 5;
    int minPivots = 1;
    int maxPivots = 5;
    int maxDepth = std::numeric_limits<int>::max();
    float lshVarThreshold = 0.9f;
    size_t maxIdx = -1;
};

struct HIBuildResults{
    friend std::ostream& operator<<(std::ostream& os, const HIBuildResults& hi){
        os << "[HIBuildResults\tnpoints="<<(size_t) hi.npoints <<"\tnnodes=" <<(size_t)  hi.nnodes << "\tninternal=" << (size_t) hi.ninternal <<
           "\tnleaf=" << (size_t) hi.nleaf <<"\tnspatial="<<(size_t) hi.nspatial<<"\tnlsh=" << (size_t) hi.nlsh <<
           "\tnpointsInLeaf=" << (size_t) hi.npointsInLeaf << "\tnpointsInInternal="<<(size_t) hi.npointsInInternal <<
           "\tnpointsInLSH="<<(size_t) hi.npointsInLSH <<"\t]";
        return os;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        /// atomic values aren't serializable, convert to normal size_t
        size_t _nnodes = nnodes ;
        size_t _nlsh = nlsh;
        size_t _nspatial = nspatial;
        size_t _ninternal = ninternal;
        size_t _nleaf = nleaf;
        size_t _npointsInLeaf = npointsInLeaf;
        size_t _npoints = npoints;
        size_t _npointsInInternal = npointsInInternal;
        size_t _npointsInLSH = npointsInLSH;
        ar & _nnodes;
        ar & _nlsh;
        ar & _nspatial;
        ar & _ninternal;
        ar & _nleaf;
        ar & _npointsInLeaf;
        ar & _npoints;
        ar & _npointsInInternal;
        ar & _npointsInLSH;
        if (Archive::is_loading::value) {
            nnodes = _nnodes;
            nlsh = _nlsh;
            nspatial = _nspatial;
            ninternal = _ninternal;
            nleaf = _nleaf;
            npointsInLeaf = _npointsInLeaf;
            npoints = _npoints;
            npointsInInternal = _npointsInInternal;
            npointsInLSH = _npointsInLSH;
        }
    }

    std::atomic<size_t> nnodes{0};
    std::atomic<size_t> nlsh{0};
    std::atomic<size_t> nspatial{0};
    std::atomic<size_t> ninternal{0};
    std::atomic<size_t> nleaf{0};
    std::atomic<size_t> npointsInLeaf{0};
    std::atomic<size_t> npointsInInternal{0};
    std::atomic<size_t> npoints{0};
    std::atomic<size_t> npointsInLSH{0};
};


struct HIQueryResults{
    friend std::ostream& operator<<(std::ostream& os, HIQueryResults& hi){
        os << "[HIQueryResults\tdistcalcs="<< (size_t)hi.distcalcs <<"\tnpoints="<<(size_t) hi.npoints <<
           "\tnnodes=" <<(size_t)  hi.nnodes << "\tninternal=" << (size_t) hi.ninternal <<
           "\tnleaf=" << (size_t) hi.nleaf <<"\tnspatial="<<(size_t) hi.nspatial<<"\tnlsh=" << (size_t) hi.nlsh <<
           "\tnpointsInLeaf=" << (size_t) hi.npointsInLeaf << "\tnpointsInInternal="<<(size_t) hi.npointsInInternal <<
           "\tnpointsInLSH="<<(size_t) hi.npointsInLSH << "\ttopksize="<< hi.topk.getTopk().size()<<"\t]\n";
        for (auto i: hi.getTopK()){
            os << "   [ " << i.first << "  " << i.second << " ]\n" ;
        }
        return os;
    }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        /// atomic values aren't serializable, convert to normal size_t
        size_t _nnodes = nnodes ;
        size_t _nlsh = nlsh;
        size_t _nspatial = nspatial;
        size_t _ninternal = ninternal;
        size_t _nleaf = nleaf;
        size_t _npointsInLeaf = npointsInLeaf;
        size_t _npoints = npoints;
        size_t _npointsInInternal = npointsInInternal;
        size_t _npointsInLSH = npointsInLSH;
        size_t _distcalcs = distcalcs;
        ar & _nnodes;
        ar & _nlsh;
        ar & _nspatial;
        ar & _ninternal;
        ar & _nleaf;
        ar & _npointsInLeaf;
        ar & _npoints;
        ar & _npointsInInternal;
        ar & _npointsInLSH;
        ar & _distcalcs;
        if (Archive::is_loading::value) {
            nnodes = _nnodes;
            nlsh = _nlsh;
            nspatial = _nspatial;
            ninternal = _ninternal;
            nleaf = _nleaf;
            npointsInLeaf = _npointsInLeaf;
            npoints = _npoints;
            npointsInInternal = _npointsInInternal;
            npointsInLSH = _npointsInLSH;
            distcalcs = _distcalcs;
        }
    }
    std::mutex mtx;
    std::atomic<size_t> nnodes{0};
    std::atomic<size_t> nlsh{0};
    std::atomic<size_t> nspatial{0};
    std::atomic<size_t> ninternal{0};
    std::atomic<size_t> nleaf{0};
    std::atomic<size_t> npointsInLeaf{0};
    std::atomic<size_t> npointsInInternal{0};
    std::atomic<size_t> npoints{0};
    std::atomic<size_t> npointsInLSH{0};
    std::atomic<size_t> distcalcs{0};
    lshbox::Topk topk;
    bool genned = false;

    void reset(int k){
        topk.reset(k);
    }
    std::vector<std::pair<float, size_t> > getTopK(){
        if (!genned){
            topk.genTopk();
            genned = true;
        }
        return topk.getTopk();
    }

    void add(size_t idx, float dist){
        topk.push(idx, dist);
    }

    void add(std::pair<float, size_t>& pair) {
        mtx.lock();
        topk.push(pair);
        mtx.unlock();
    }

    void addAll(lshbox::Topk& _topk, std::vector<size_t>& idxs) {
        mtx.lock();
        _topk.genTopk();
        bool hasMappedIdx = !idxs.empty();
        for (auto i : _topk.getTopk()){
            if (hasMappedIdx){
                assert(i.second < idxs.size());
                i.second = idxs[i.second];
            }
            topk.push(i);
        }
        mtx.unlock();
    }
};

struct HIQueryParams{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & nnodes;
        ar & nthreads;
        ar & k;
        ar & _querypoint;
        ar & C;
    }

    int nnodes = 1;
    int nthreads = 1;
    int k = 3;
    size_t C = 0;
    std::vector<Dat> _querypoint;
    Dat* querypoint = nullptr;
    void setQueryPoint(Dat* qp, int C){
        querypoint= new Dat[C];
        for (int i = 0; i < C; ++i) {
            _querypoint.emplace_back(qp[i]);
            querypoint[i] = qp[i];
        }
    }
};

};



#endif //HI_PARAMS_HPP
