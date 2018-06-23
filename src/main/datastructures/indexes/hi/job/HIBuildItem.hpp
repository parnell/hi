
#ifndef HI_BUILDITEM_HPP
#define HI_BUILDITEM_HPP


#include "../../../../globals.hpp"
#include "../../../rm/WorkItem.hpp"
#include "HIBuild.hpp"
#include "../../../controllers/DataManager.hpp"
#include "../../../../utils/Timer.hpp"
#include "../HI.hpp"
#include <string>

namespace hi {

template <typename DM_CLASS>
class HIBuildItem : public WorkItem {
    std::vector<Dat> items;

    friend class boost::serialization::access;
    DataManager* pdataManager;

    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & boost::serialization::base_object<WorkItem>(*this);
        ar & items;
        ar & inputBinFile;
        ar & file_section;
        ar & wsize;
        ar & node;
    }
    std::string inputBinFile;
    int file_section;
    int wsize;
    int node;

public:
    HIBuildItem() : WorkItem() {}
    HIBuildItem(std::string inputBinFile, size_t jobid, int file_section, int wsize ) :
            WorkItem(jobid), inputBinFile(inputBinFile), wsize(wsize), node(-1) {}

    std::list<WorkItem*> work() {
        std::cout << " Starting load binFile=" << inputBinFile << "  " << node << "  " << nranks << std::endl;
        DataManager* pdataManager;
        if (file_section > 1){
            pdataManager = DM_CLASS::loadData(
                    inputBinFile, static_cast<size_t>(file_section), static_cast<size_t>(wsize));
        } else {
            pdataManager = DM_CLASS::loadData(inputBinFile);
        }
        std::cout <<" HIBuild loaded\tR=" << pdataManager->getRows() <<"\t" << std::endl;
        HI<DM_CLASS> hi;
        {
            Timer buildTime("HI Build Time");
            hi.build(pdataManager, wsize, std::numeric_limits<int>::max(), node);
            std::cout << hi << std::endl;
        }
        return NO_WORK;
    }

    std::string toString() const {
        std::stringstream str;
        str << "[HIBuildItem " << id << ":" << jobid << "]";
        return str.str();
    }
    friend std::ostream &operator<<(std::ostream &os, HIBuildItem const &o) {
        return os << o.toString();
    }
};

}
#endif //HI_BUILDITEM_HPP
