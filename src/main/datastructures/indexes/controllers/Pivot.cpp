
#include "Pivot.hpp"


Pivot::Pivot(Dat* _ppivot, size_t R, size_t C) :
    pivot(&_ppivot[0], &_ppivot[C]), distances(R){
        assert(!pivot.empty());
        assert(pivot.size() == C);
}

dist_type Pivot::getMaxLeft() const {
    return distances[maxl_idx].distance;
}

dist_type Pivot::getMinRight() const {
    return distances[minr_idx].distance;
}
