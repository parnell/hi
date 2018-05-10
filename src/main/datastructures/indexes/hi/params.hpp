
#ifndef HI_PARAMS_HPP
#define HI_PARAMS_HPP


#include <boost/serialization/access.hpp>
#include <iostream>


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
    }

    int nnodes = 1;
    int nthreads = 1;
    int maxLeafPoints = 5;
    int minPivots = 1;
    int maxPivots = 5;
    float lshVarThreshold = 0.9f;
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

struct HIQueryParams{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & nnodes;
        ar & nthreads;
    }

    int nnodes = 1;
    int nthreads = 1;
};

};

//std::ostream& operator<< (std::ostream& os, const hi::HIBuildResults& hi) {
//    os << "[HIBuildResults nnodes=\t" << hi.nnodes << "\tninternal=" << hi.ninternal <<
//       "\tnleaf=" << hi.nleaf <<"\tnspatial="<<hi.nspatial<<"\tnlsh=" << hi.nlsh << "]";
//    return os;
//}



#endif //HI_PARAMS_HPP
