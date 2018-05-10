
#ifndef HI_PIVOTSELECTOR_HPP
#define HI_PIVOTSELECTOR_HPP


#include "../../../globals.hpp"
#include "../../controllers/DataManager.hpp"

class PivotSelector {
public:
    Dat* findPivot(DataManager* pdata, std::vector<Dat*>* pivots);

};


#endif //HI_PIVOTSELECTOR_HPP
