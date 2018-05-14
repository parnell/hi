
#ifndef HI_PIVOTSORTER_HPP
#define HI_PIVOTSORTER_HPP

#include "../../../globals.hpp"
#include "../../Stat.hpp"
#include "Pivot.hpp"
#include <vector>
#include <tuple>

class PivotSorter {
//    std::vector<std::tuple<size_t, dist_type, size_t >> distances;
    Dat* pdata;
    size_t R;
    size_t C;

public:
    PivotSorter(Dat* pdata, const size_t R, const size_t C);
    ~PivotSorter();
    void sort(std::vector<size_t>& idxs, Pivot& pivot);
    void setData(Dat* pdata, size_t R, const size_t C);
    void reorder(Dat* pdata, std::vector<Tup>& tups, size_t R, size_t C);
    Dat* getData() const;
    size_t getR() const;
    size_t getC() const;
};


#endif //HI_PIVOTSORTER_HPP
