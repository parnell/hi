
#ifndef HI_FDNADATAMANAGER_HPP
#define HI_FDNADATAMANAGER_HPP


#include "MetricDataManager.hpp"

const short DNA_W = 4;
class FDNADataManager : public MetricDataManager {
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(MetricDataManager);
        ar & pdata_arr;
        ar & C;
    }
    std::vector< Data* >* pdata_arr;
    size_t C;
public:
    FDNADataManager() : pdata_arr(nullptr) {
        dmtype = DataManager::ManagerType::DM_FDNA;
    }
    FDNADataManager(std::vector< Data* >* pdata_arr) : pdata_arr(pdata_arr), C(pdata_arr->at(0)->size()){}

    template <typename T>
    EucDataManager<T>* toEuclidean() {
        const size_t R = pdata_arr->size();
        auto& fa = *pdata_arr;
        const size_t nC = C*DNA_W;
        rm::M<T> m(nC, R, true);
        auto a = m.getData();

        for (size_t i = 0; i < R; ++i) {
            auto pdna = static_cast<DNA*>(fa[i]);
            char* c = pdna->getSeq();
            for (size_t j = 0; j < C; ++j) {
                switch(c[j]){
                    case 'a': a[i*nC+j*DNA_W] = 1; break;
                    case 'c': a[i*nC+j*DNA_W+1] = 1; break;
                    case 'g': a[i*nC+j*DNA_W+2] = 1; break;
                    case 't': a[i*nC+j*DNA_W+3] = 1; break;
                    default: break;
                }
            }
        }
        auto r = new EucDataManager<T>(m, R, true);
        r->idxs = idxs;
        return r;
    }



    static FDNADataManager* loadData(std::string filename, size_t only, size_t nsplits);

    size_t size() const { return pdata_arr->size();}
};

BOOST_CLASS_EXPORT_KEY(FDNADataManager);

#endif //HI_FDNADATAMANAGER_HPP
