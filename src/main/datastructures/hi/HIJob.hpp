
#ifndef HI_HIJOB_HPP
#define HI_HIJOB_HPP

#include <flann/flann.hpp>
#include <flann/io/hdf5.h>

#include "../rm/Job.hpp"
namespace hi {
class HIJob : public Job {
    flann::NNIndex<flann::L2<float>> *pindex;
public:


};
}

#endif //HI_HIJOB_HPP
