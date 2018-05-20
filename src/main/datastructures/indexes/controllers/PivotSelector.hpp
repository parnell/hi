
#ifndef HI_PIVOTSELECTOR_HPP
#define HI_PIVOTSELECTOR_HPP


#include "../../../globals.hpp"
#include "../../controllers/DataManager.hpp"

class PivotSelector {
public:
    Data* findPivot(DataManager* pdata, std::vector<Data*>* pivots);

};


#endif //HI_PIVOTSELECTOR_HPP
