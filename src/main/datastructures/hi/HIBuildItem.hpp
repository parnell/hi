
#ifndef HI_BUILDITEM_HPP
#define HI_BUILDITEM_HPP


#include "../../globals.hpp"
#include "../rm/WorkItem.hpp"
#include "HIBuild.hpp"
//#include "HI.hpp"
#include <string>
#include <flann/flann.hpp>
#include <flann/io/hdf5.h>
#include <flann/util/matrix.h>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
#include <lshbox/lsh/itqlsh.h>

namespace hi {

class HIBuildItem : public WorkItem {
    std::vector<Dat> items;

    friend class boost::serialization::access;

//    flann::Matrix<Dat> *pdataset;
//    flann::NNIndex<flann::L2<Dat>> *pindex;

        template<class Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & boost::serialization::base_object<WorkItem>(*this);
//        ar & *pdataset;
            ar & items;
        }
public:
    HIBuildItem() : WorkItem() {}

    explicit HIBuildItem(int jobid) : WorkItem(jobid) {}

    HIBuildItem(int jobid, std::vector<Dat> data) : WorkItem(jobid), items(std::move(data)) {}

//    HIBuildItem(int jobid, flann::Matrix<Dat> &dataset) : WorkItem(jobid), pdataset(&dataset) {}

    virtual void work();

    virtual std::string toString() const {
        std::stringstream str;
        str << "[HIBuildItem " << id << ":" << jobid << "]";
        return str.str();
    };


    friend std::ostream &operator<<(std::ostream &os, HIBuildItem const &o) {
        return os << o.toString();
    }
};

}
#endif //HI_BUILDITEM_HPP
