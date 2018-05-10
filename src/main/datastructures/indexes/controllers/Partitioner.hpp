
#ifndef HI_PARTITIONER_HPP
#define HI_PARTITIONER_HPP


#include "../../controllers/DataManager.hpp"
#include "../hi/params.hpp"

class Partitioner {
    std::vector<DataManager*> partition(const hi::HIBuildParams& params, DataManager *pdata, int depth);
};


#endif //HI_PARTITIONER_HPP
