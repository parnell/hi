
#ifndef HI_FILE_LOADER_HPP
#define HI_FILE_LOADER_HPP


#include "dtypes.hpp"

#if __DATA_TYPE__ == __VEC_TYPE__
#include "datastructures/controllers/EucDataManager.hpp"

DataManager* loadFile(const std::string& filename){
    auto pdataManager = EucDataManager<itype>::loadData(filename);
    std::cout << " HIBuild loaded\tR=" << pdataManager->getRows() << "\tC=" << pdataManager->getCols() << std::endl;
    return pdataManager;
}

DataManager* loadFile(const std::string& filename, size_t only, size_t nsplits){
    auto pdataManager = EucDataManager<itype>::loadData(filename, only, nsplits);
    std::cout << " HIBuild loaded\tR=" << pdataManager->getRows() << "\tC=" << pdataManager->getCols() << std::endl;
    return pdataManager;
}


#elif __DATA_TYPE__ == __STRING_TYPE__

DataManager* loadFile(const std::string& filename){
    auto pdataManager = FDNADataManager::loadData(filename);
    std::cout << " HIBuild loaded\tR=" << pdataManager->getRows() << "\tC=" << pdataManager->getCols() << std::endl;
    return pdataManager;
}

DataManager* loadFile(const std::string& filename, size_t only, size_t nsplits){
    auto pdataManager = FDNADataManager::loadData(filename);
    std::cout << " HIBuild loaded\tR=" << pdataManager->getRows() << "\tC=" << pdataManager->getCols() << std::endl;
    return pdataManager;
}

#elif __DATA_TYPE__ == __GRAPH_TYPE__


#endif



#endif //HI_FILE_LOADER_HPP
