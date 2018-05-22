
#ifndef HI_EUCDATAMANAGER_HPP
#define HI_EUCDATAMANAGER_HPP

#include <string>
#include <lshbox/basis.h>
#include <lshbox/metric.h>
#include <lshbox/topk.h>
#include <lshbox/eval.h>
#include <lshbox/matrix.h>
//#include <lshbox.h>
#include "../../globals.hpp"
#include "../rm/M.hpp"
#include "../indexes/controllers/Pivot.hpp"
#include "DataManager.hpp"
#include "../../utils/statutil.hpp"
#include "../../utils/vecutil.hpp"
#include "../datatypes/EucBare.hpp"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>

template<typename T>
class EucDataManager : public DataManager {

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive &ar, const unsigned int version) {
        ar.template register_type< EucDataManager<T> >();
        ar & boost::serialization::base_object<DataManager>(*this);
        ar & m;
        ar & deleteData;
    }
    bool deleteData;
public:
    rm::M<T> m;

public:
    EucDataManager() : deleteData(true) {};

    EucDataManager(T *pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData, std::vector<size_t> idxs) :
            DataManager(idxs),deleteData(deleteData) {
        if (shouldTransfer){
            m.transfer(pdata, rows, cols,deleteData);
        } else {
            m.load(pdata, rows, cols);
        }
    }
    EucDataManager(rm::M<T>& m, size_t R, bool shouldTransfer): DataManager(R), deleteData(true)  {
        if (shouldTransfer){
            this->m.transfer(m.getData(), m.getRows(), m.getCols(), true);
        } else {
            this->m.load(m.getData(), m.getRows(), m.getCols());
        }

    }

    EucDataManager(T* pdata, size_t rows, size_t cols, bool shouldTransfer, bool deleteData, size_t startingIdx):
            DataManager(rows, startingIdx), deleteData(deleteData){
        if (shouldTransfer){
            m.transfer(pdata, rows, cols, deleteData);
        } else {
            m.load(pdata, rows, cols);
        }
    }
    ~EucDataManager() override {
        if (!deleteData){
            m.setData(nullptr); /// this will prevent the natural deletion of the data
        }
    }

    DataManager *sliceData(size_t _begin, size_t _end) override {
//    dprintf("     ### slicing  %ld   %ld    %ld    %p\n", _begin, _end,   _end - _begin, m[_begin]);
        return new EucDataManager<T>(m[_begin], _end - _begin, getCols(), true, false,
                                  std::vector<size_t>(idxs.begin()+_begin, idxs.begin()+_end));
    }

    static EucDataManager* loadData(std::string filename){
        rm::M<T> m;
        m.load(filename);
        auto pd = new EucDataManager<T>(m, m.getRows(), true);
        m.setData(nullptr); /// prevent deletion on exit as we transferred
        return pd;
    }

    static EucDataManager* loadData(std::string filename, size_t only, size_t nsplits){
        rm::M<T> m;
        m.load(filename, only, nsplits);
        auto pd = new EucDataManager<T>(m, m.getRows(), true);
        m.setData(nullptr); /// prevent deletion on exit as we transferred
        return pd;
    }

    size_t getCols() const {
        return m.getDim();
    }


    void print() const {
        m.print();
    }
    void reorder(T* pdata, std::vector<Tup>& tups, size_t R, size_t C)
    {
        for (size_t i=0; i<R; ++i) {
            while (tups[i].sort_index != i) {
                std::swap_ranges(
                        &pdata[tups[i].sort_index*C],
                        &pdata[tups[i].sort_index*C+C],
                        &pdata[tups[tups[i].sort_index].sort_index*C]
//                    &pdata[std::get<0>(idxdist[std::get<0>(idxdist[i])])*C]
                );
                std::swap(tups[i].sort_index, tups[tups[i].sort_index].sort_index);
            }
        }
    }

    void sort(Pivot& pivot) override{
        const size_t R = getRows();
        auto pdistances = pivot.pdistances;
        assert(pdistances->size() == R);

        const size_t C = getCols();

        assert(pivot.ppivot->size() == C);
        auto pivotdat = static_cast<Euc<T>*>(pivot.ppivot);
        auto mdat = m.getData();
        /// Calculate distance to the pivot for all points
        for (size_t i = 0; i < R; ++i) {
            T dist_ = 0;
            for (unsigned j = 0; j != C; ++j) {
                dist_ += sqr( (*pivotdat)[j] - mdat[i*C+j]);
            }
            const float d = std::sqrt(dist_);
//        dprintf(" -- %f   %zu   %zu\n", mdat[i*C], i, idxs[i]);
            (*pdistances)[i] = Tup(idxs[i], d, i);
        }

        std::sort(std::begin(*pdistances ), std::end(*pdistances ), [](auto &left, auto &right) {
            return left.distance < right.distance;
        });
        /// copy over new index order
        for (size_t i=0; i<R; ++i){
            idxs[i] = (*pdistances)[i].oindex;
        }
        /// copy new locations of Dat* array
        reorder(m.getData(), *pdistances, R, C);
    }

    void print_rowp() const {
        m.print_rowp();
    }

    friend std::ostream &operator<<(std::ostream &os, EucDataManager<T> &dm) {
        const size_t R = dm.getRows();
        const size_t C = dm.getCols();
        auto a = dm.getDat();
        os << " [";
        for (size_t __r = 0; __r< R; ++__r){
            os << "\t" << __r << "\t" << dm.idxs[__r] << "[ ";\
        for (size_t __c = 0; __c < C; ++__c) {
                os << a[__r*C+__c] << " ";
            }
            os << "]";
            if (__r != R-1)
                os << std::endl << "  ";
        }
        os << "]" << std::endl;
        return os;
    }
    std::vector<Pivot *> pickPivots(int k) override {
        std::vector<Pivot*> res(k);
        auto r = vecutil::randRowPointers<T>(getRows(), getCols(), k, getDat());;
//    auto r = vecutil::pickSet(getRows(), k, )
        for (size_t i = 0; i < r.size(); ++i) {
            Euc<T>* e = new Euc<T>(r[i], getCols());
            res[i] = new Pivot(e
                    , idxs[r[i]-getDat()],
                               getRows());
        }
        return res;
    }
    void scan(Data *queryPoint, int i, hi::HIQueryResults& qr) override {
//    leafPoints->scan(queryPoint, depth, parent->queryResults);
        auto qp = static_cast<Euc<T>*>(queryPoint);
        const size_t C = getCols();
        for (size_t i = 0; i < getRows(); ++i) {
            ++qr.npointsInLeaf;
            ++qr.distcalcs;
            float d = rm::M<T>::dist(qp->getCoords(), m[i], C);
//        float d = rm::M<Dat>::dist((*this)[i], queryPoint, getCols());
//        dprintf("  adding leaf %lu    %f ,       %f:%f\n", leafPoints->idxs[i], d, leafPoints->m[i][0], leafPoints->m[i][1]);

            qr.add(idxs[i], d);
        }
    }

    T *getDat() const{
        return m.getData();
    }

    size_t getRows() const override{
        return m.getSize();
    }
    const Dat *operator[](size_t i) const{
        return m[i];
    }

    T *operator[](size_t i){
        return m[i];
    }

    T get(size_t r, size_t c){
        return m[r][c];
    }

    T* getRow(size_t r) {return m[r];}

    Data *getElement(size_t row) override{
        return new Euc<T>(m[row], m.getCols());
    }

public:


    DataManager* new_instance() override{
        return new EucDataManager<T>(getDat(), getRows(), getCols(), true, false, this->idxs);
    }

    Stat calculateVariance() override {
        return statutil::calculateVariance(getDat(), getRows(), getCols());
    }
    bool isEuclidean() override{
        return true;
    }
};

BOOST_CLASS_EXPORT_KEY(EucDataManager<float>);
BOOST_CLASS_EXPORT_KEY(EucDataManager<int>);

#endif //HI_EUCDATAMANAGER_HPP
