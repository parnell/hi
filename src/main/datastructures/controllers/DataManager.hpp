
#ifndef HI_DATAMANAGER_HPP
#define HI_DATAMANAGER_HPP

#include <vector>
#include <numeric>
#include "../indexes/controllers/Pivot.hpp"
#include "../indexes/hi/params.hpp"

class DataManager {

public:
    std::vector<size_t> idxs;
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< DataManager >();
        ar & idxs;
        ar & dmtype;
    }

    enum ManagerType {
        DM_EUC, DM_FDNA, UNKNOWN
    };

    ManagerType dmtype = UNKNOWN;

    DataManager() = default;
    explicit DataManager(std::vector<size_t>& idxs): idxs(idxs) {}
    explicit DataManager(size_t rows): idxs(rows) {
        std::iota(idxs.begin(), idxs.end(), 0);
    }
    explicit DataManager(size_t rows, size_t startingIdx): idxs(rows) {
        std::iota(idxs.begin(), idxs.end(), startingIdx);
    }
    virtual ~DataManager() = default;
    ManagerType getType() const { return dmtype;}
    virtual std::vector<Pivot*> pickPivots(int k) {return std::vector<Pivot*>();}


    virtual void sort(Pivot& pivot) {};

    virtual size_t getRows() const {return 0;};

    virtual DataManager *sliceData(size_t _begin, size_t _end) {return nullptr;}

    virtual Stat calculateVariance() {return Stat();}

    virtual Data *getElement(size_t row) {return nullptr;}

    virtual DataManager *new_instance() {return nullptr;}

//    virtual const Dat *operator[](size_t i) const {return nullptr;}
//    virtual Dat *operator[](size_t i) {return nullptr;}

    virtual void scan(Data *pData, int i, hi::HIQueryResults& results) {}

    virtual bool isEuclidean() { return false; }


};
BOOST_CLASS_EXPORT_KEY(DataManager);


#endif //HI_DATAMANAGER_HPP
