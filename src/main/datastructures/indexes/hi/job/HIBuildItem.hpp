
#ifndef HI_BUILDITEM_HPP
#define HI_BUILDITEM_HPP


#include "../../../../globals.hpp"
#include "../../../rm/WorkItem.hpp"
#include "HIBuild.hpp"
#include "../../../controllers/DataManager.hpp"
#include <string>

namespace hi {

class HIBuildItem : public WorkItem {
    std::vector<Dat> items;

    friend class boost::serialization::access;
    DataManager* pdataManager;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<WorkItem>(*this);
        ar & items;
        ar & inputBinFile;
    }
    std::string inputBinFile;
public:
    HIBuildItem() : WorkItem() {}
    HIBuildItem(std::string inputBinFile, size_t jobid) : WorkItem(jobid), inputBinFile(inputBinFile) {}

//    HIBuildItem(int jobid, std::vector<Dat> data) : WorkItem(jobid), items(std::move(data)) {}

//    HIBuildItem(int jobid, flann::Matrix<Dat> &dataset) : WorkItem(jobid), pdataset(&dataset) {}

    virtual std::list<WorkItem*> work();

    virtual std::string toString() const;


    friend std::ostream &operator<<(std::ostream &os, HIBuildItem const &o) {
        return os << o.toString();
    }
};

}
#endif //HI_BUILDITEM_HPP
