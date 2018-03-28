
#ifndef HI_LOADER_HPP
#define HI_LOADER_HPP


#include "../datastructures/data.hpp"

template<typename DATA_CLASS>
class Loader {
public:
    static size_t LoadData(const char* file, std::vector<Data*>* data_arr) {
        return DATA_CLASS::LoadData(file, data_arr);
    }
};


#endif //HI_LOADER_HPP
