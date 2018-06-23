
#ifndef HI_HIJOB_HPP
#define HI_HIJOB_HPP

//#include <flann/io/hdf5.h>
#include "../../../rm/Job.hpp"
#include "HIBuildItem.hpp"

namespace hi {
template <typename DM_CLASS>
class HIJob : public Job {
public:
    void addBuildWorkItems(const std::string& filename, int splits) {
        if (splits <= 1){
            addWorkItem(new HIBuildItem<DM_CLASS>(filename, this->id, -1, splits));
        } else {
            for (size_t i = 0; i < splits; ++i) {
                addWorkItem(new HIBuildItem<DM_CLASS>(filename, this->id, i, splits));
            }
        }

    }

};
}

#endif //HI_HIJOB_HPP
