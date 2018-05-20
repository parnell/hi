
#include "HIBuildItem.hpp"
#include "../HI.hpp"
#include "../../../../utils/Timer.hpp"
#include "../../../controllers/EucDataManager.hpp"
//#include <lshbox.h>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>

namespace hi {

//template<class Archive>
//void HIBuildItem::serialize(Archive &ar, const unsigned int version) {
//    ar & boost::serialization::base_object<WorkItem>(*this);
////        ar & *pdataset;
//    ar & items;
//}

std::list<WorkItem*> HIBuildItem::work() {
    std::cout << " Starting load binFile=" << inputBinFile << std::endl;
    auto pdataManager = EucDataManager<itype>::loadData(inputBinFile, 0, 4);
    std::cout <<" HIBuild loaded\tR=" << pdataManager->getRows() <<"\tC=" << pdataManager->getCols() << std::endl;
    HI hi;
    {
        Timer buildTime("HI Build Time");
        hi.build(pdataManager, nranks);
        std::cout << hi << std::endl;
    }
    return NO_WORK;
}

std::string HIBuildItem::toString() const {
    std::stringstream str;
    str << "[HIBuildItem " << id << ":" << jobid << "]";
    return str.str();
}


}