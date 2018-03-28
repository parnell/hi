
#ifndef HI_HI_H
#define HI_HI_H

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <lshbox.h>
#include "Forest.hpp"
#include "../rm/WorkItem.hpp"


class HI {
    friend class HIBuildItem;
    std::vector<Forest*> hi;

public:

    HI(){

    }
};


#endif //HI_HI_H
