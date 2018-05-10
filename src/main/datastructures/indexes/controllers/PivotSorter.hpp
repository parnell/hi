
#ifndef HI_PIVOTSORTER_HPP
#define HI_PIVOTSORTER_HPP

#include "../../../globals.hpp"
#include "../../Stat.hpp"

class PivotSorter {
    bool operator () (Dat* i, Dat* j) { return (i<j); }
    std::vector<std::pair<size_t, Dat >> distances;
    Dat* pdata;
    size_t R;
    size_t C;

public:
    PivotSorter(Dat* pdata, const size_t R, const size_t C);
    ~PivotSorter();
    Stat sort(Dat* pivot);

};


#endif //HI_PIVOTSORTER_HPP
